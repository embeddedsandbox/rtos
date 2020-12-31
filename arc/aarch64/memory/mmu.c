//==============================================================================
// Copyright 2020 Daniel Boals & Michael Boals
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
// THE SOFTWARE.
//==============================================================================

#include "types.h"
#include "mmu.h"
#include "memory.h"
#include "mmu_private.h"

uint32_t mmuEntriesPerPage  = 512;
uint32_t mmuPageSize        = 4096;
static tableNode_t *    tablePoolHead;
static tableNode_t *    tablePoolTail; 
static uint32_t         tablePoolCount;

static uint64_t mmuTTBR0;
static uint32_t mmuTTBR0L1TableCount;
static uint64_t mmuTTBR1;
static uint32_t mmuTTBR1L1TableCount;
//static uint64_t mmuTCR;  

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void mmuSetupTableConfiguration(void)
{
    uint64_t mair_el1   = MAIR_EL1_MAP;
    tcrReg_t tcr_el1    = TCR_EL1_DEFAULT;

    __asm
    (
        "   MSR     MAIR_EL1, %0    \n"        
        :
        : "r" (mair_el1)
    );

    __asm
    (
        "   MSR     TCR_EL1, %0     \n"
        :
        : "r" (tcr_el1)
    );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MmuEnable(void)
{
    uint64_t sctlr_reg;
    __asm
    (
        "   MRS     %0, SCTLR_EL1           \n"
        : "=r" (sctlr_reg)
        :
        :
    );

    sctlr_reg &= ~(0);
    sctlr_reg |= 1 ;//| (1<<2) | (1<<12) ; // turn on the cache with this

    __asm
    (
        "   DSB ISH                         \n"
        "   ISB                             \n"
        "   MSR     SCTLR_EL1, %0           \n"
        "   ISB                             \n"
        :
        : "r" (sctlr_reg)
        : 
    );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void mmuSetTTBR0(void)
{
    uint64_t localTTBR0 = mmuTTBR0 | 1;

    __asm
    (
        "   MSR     TTBR0_El1, %0   \n"
        :
        : "r" (localTTBR0)
    );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void mmuSetTTBR1(void)
{
    uint64_t localTTBR1 = mmuTTBR1 | 1;

    __asm
    (
        "   MSR     TTBR1_EL1, %0   \n"
        :
        : "r" (localTTBR1)
    );
}

//-----------------------------------------------------------------------------
// mmuInitPage
//      fill the page basically the TT equivalent  of NULL 
//-----------------------------------------------------------------------------
void mmuInitPage(mmuPageHandle_t page)
{
    // init the PAge out the page
    for (uint32_t idx = 0; idx < mmuEntriesPerPage; idx++)
    {
        page[idx] = MMU_TABLE_DESC_NOT_PRESENT;
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void mmuFreeTable(mmuPageHandle_t page)
{
    tableNode_t * tableNode = (tableNode_t*)page;
    
    tableNode->prev = tablePoolTail ;
    tableNode->next = NULL;

    if (tablePoolTail != NULL)
    {
        tablePoolTail->next = tableNode;
        tablePoolTail = tableNode;
    }
    else
    {
        // if tail is NULL we are adding to an empty list
        tablePoolHead = tableNode;
        tablePoolTail = tableNode;
    }

    tablePoolCount++;    
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
mmuPageHandle_t  mmuAllocTTPage(void)
{
    mmuPageHandle_t allocatedPage = NULL;

    if (tablePoolHead != NULL)
    {
        allocatedPage = (mmuPageHandle_t)tablePoolHead;        
        tablePoolHead = tablePoolHead->next;

        if (tablePoolHead != NULL)
        {
            tablePoolHead->prev = NULL;
        }
        else
        {
            tablePoolTail = NULL;
        }
        
        mmuInitPage(allocatedPage);
        tablePoolCount--;
    }

    return allocatedPage;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void mmuTablePoolInit(uintptr_t tablePoolStart, uint64_t tablePoolSize)
{
    tablePoolHead = NULL;
    tablePoolTail = NULL;
    tablePoolCount = 0;

    uint8_t* nextPage = (uint8_t*)tablePoolStart;
    uint64_t numPages = tablePoolSize / mmuPageSize;

    ASSERT(numPages > 4)

    for (; numPages > 0; numPages--)
    {
        mmuFreeTable((mmuPageHandle_t)nextPage);
        nextPage += mmuPageSize;
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MmuInit(uintptr_t tablePoolStart, uintptr_t tablePoolSize)
{
    mmuSetupTableConfiguration();

    mmuTablePoolInit(tablePoolStart, tablePoolSize);

    mmuTTBR0                = (uint64_t)mmuAllocTTPage();
    mmuTTBR0L1TableCount    = 0;
    mmuTTBR1                = (uint64_t)mmuAllocTTPage();
    mmuTTBR1L1TableCount    = 0;    

    mmuSetTTBR0();
    mmuSetTTBR1();

}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MmuMapRange(uintptr_t pageVirtAddr, uintptr_t pagePhysAddress, uintptr_t regionsSize, memoryTypes_t regionType)
{
    uint32_t numPages = regionsSize/mmuPageSize;
    mmuDescriptor_t l1Attributes;
    mmuDescriptor_t l2Attributes;
    mmuDescriptor_t l3Attributes;

    // we are using 
    mmuPageHandle_t l1TableAddr;
    mmuPageHandle_t l2TableAddr; 
    mmuPageHandle_t l3TableAddr;

    uint32_t l1TableIndex = (pageVirtAddr>>30) & 0x1FF;
    uint32_t l2TableIndex = (pageVirtAddr>>21) & 0x1FF;
    uint32_t l3TableIndex = (pageVirtAddr>>12) & 0x1FF;

    //
    // TODO Invalidate the VA's in the cache for this range.
    //

    // bit 47 determines TTBR0 or TTBR1
    // we are in a 36 bit address setup so we bypass L0
    if (pageVirtAddr & (1LL<<47))
    {
        l1TableAddr     = (mmuPageHandle_t)mmuTTBR1;
        l1Attributes    = MMU_TABLE_DESC;
        l2Attributes    = MMU_TABLE_DESC;
    }
    else
    {
        l1TableAddr     = (mmuPageHandle_t)mmuTTBR0;
        l1Attributes    = MMU_TABLE_DESC;
        l2Attributes    = MMU_TABLE_DESC;
    }

    switch(regionType)
    {
        case MEM_TYPE_NORMAL_MEMORY:
        {
            l3Attributes = MMU_BLOCK_DESC_MEM_PRIVILEGED_SECURE_CACHEABLE;
            break;
        }

        case MEM_TYPE_NONCACHABLE:
        case MEM_TYPE_VIDEO_MEMORY:
        {
            l3Attributes = MMU_BLOCK_DESC_MEM_PRIVILEGED_SECURE_NONCACHEABLE;
            break;
        }

        case MEM_TYPE_DEVICE_MEMORY:
        {
            l3Attributes = MMU_BLOCK_DESC_DEVICE_MEM;
            break;
        }

        case MEM_TYPE_NONE:
        {
            ASSERT(0);
        }
    }

#if 0  // Idenitity table for debug
    while(numPages > 0)
    {
        l1TableAddr[l1TableIndex] = l3Attributes;
        l1TableAddr[l1TableIndex].address = pagePhysAddress >> 12;

        numPages -= (1*1024*1024*1024)/4096;  // we just mapped 1GB of memory
        l1TableIndex += 1;
        pagePhysAddress += (1*1024*1024*1024);
    }
#endif

    // At some point would like to use the SW field of the Table Entries
    // to count the number of entries in the table below.  This will allow
    // pages of the table to be unmapped when all the subtables are released.
    // currently I don't have a unmapRegion function so this isn't needed. 
    while ((l1TableIndex < mmuEntriesPerPage) && (numPages > 0 ))
    {

        if (l1TableAddr[l1TableIndex].descType == MMU_DESC_TYPE_INVALID_ENTRY)
        {
            l2TableAddr =  mmuAllocTTPage();
            l1TableAddr[l1TableIndex] = l1Attributes;
            l1TableAddr[l1TableIndex].address = (uint64_t)(((uintptr_t)l2TableAddr) >> 12);
        }
        else
        {
            l2TableAddr = (mmuPageHandle_t)(((uintptr_t)l1TableAddr[l1TableIndex].address) << 12);
        }

        while ((l2TableIndex < mmuEntriesPerPage) && (numPages > 0))
        {
            if (l2TableAddr[l2TableIndex].descType == MMU_DESC_TYPE_INVALID_ENTRY)
            {
                l3TableAddr = mmuAllocTTPage();
                l2TableAddr[l2TableIndex] = l2Attributes;
                l2TableAddr[l2TableIndex].address = ((uintptr_t)l3TableAddr) >> 12;
            }
            else
            {
                l3TableAddr = (mmuPageHandle_t)(((uintptr_t)l2TableAddr[l2TableIndex].address) << 12);
            }
            
            while ((l3TableIndex < mmuEntriesPerPage) && (numPages > 0))
            {
                l3TableAddr[l3TableIndex] = l3Attributes;
                l3TableAddr[l3TableIndex].address = pagePhysAddress >> 12;

                pagePhysAddress += mmuPageSize;
                l3TableIndex++;                
                numPages--;
            }
            l3TableIndex = 0;
            l2TableIndex++;
        }
        l2TableIndex = 0;
        l1TableIndex++;
    }   
}


