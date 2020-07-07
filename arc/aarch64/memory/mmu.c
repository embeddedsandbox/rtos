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
#include "mmu_private.h"

uint32_t mmuEntriesPerPage  = 512;
uint32_t mmuPageSize        = 4096;

static tableNode_t * tablePoolHead;
static tableNode_t * tablePoolTail; 

static uint64_t mmuTTBR0;
static uint32_t mmuTTBR0L1TableCount;
static uint64_t mmuTTBR1;
static uint32_t mmuTTBR1L1TableCount;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void mmuSetupMAIR_EL0(void)
{
   #if 0       
    uint32_t    MemoryTypeArray = 0;

    __asm
    (
        "   MRS     X0, MAIR_EL1    \n"
        "   MOV     X1,             \n"
        "   OR      X0, X0, X1      \n"
        "   MSR     MAIR_EL1, X0    \n"
        :
        :
        : X0, X1
    )
    #endif 
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void mmuEnableCacheAndMMU(uint64_t TCR, uint64_t TTBR0, uint64_t TTBR1)
{
#if 0
    __asm
    (
        "   MSR     TTBR0_El1, %0   \n"
        "   MSR     TTBR1_EL1, %1   \n"
        "   MSR     TCR_EL1, %2     \n"
        :
        : "r" (TTBR0), "r" (TTBR1), "r" (TCR)
    );

    __asm
    (
        "   DSB ISH                         \n"
        "   ISB                             \n"
        "   MOV     X0, 0x0000000000001005  \n"  // Enable 
        "   MRS     X1, SCTR_EL1            \n"
        "   OR      X1, X0                  \n"
        "   MSR     SCTR_EL1, X1            \n"
        "   ISB                             \n"
        :
        :
        : X0, X1
    );
#endif
}


//-----------------------------------------------------------------------------
// mmuInitPage
//      fill the page basically the TT equivalent  of NULL 
//-----------------------------------------------------------------------------
void mmuInitPage(uintptr_t page)
{
    // init the PAge out the page
    for (uint32_t idx = 0; idx < mmuEntriesPerPage; idx++)
    {
        ((uintptr_t*)page)[idx] = MMU_TABLE_ENTRY_NOT_PRESENT;
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void mmuFreeTable(uint64_t * page)
{
    tableNode_t * tableNode = (tableNode_t*)page;
    
    tableNode->prev = tableNode;
    tableNode->next = NULL;

    if (tablePoolTail != NULL)
    {
        tablePoolTail->next = tableNode;
    }
    else
    {
        // if tail is NULL we are adding to an empty list
        tablePoolHead = tableNode;
        tablePoolTail = tableNode;
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
uintptr_t  mmuAllocTTPage(void)
{
    uintptr_t allocatedPage = (uintptr_t) NULL;

    if (tablePoolHead != NULL)
    {
        allocatedPage = (uintptr_t)tablePoolHead;        
        tablePoolHead = tablePoolHead->next;

        if (tablePoolHead != NULL)
        {
            tablePoolHead->prev = NULL;
        }

        mmuInitPage(allocatedPage);
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
    uint8_t* nextPage = (uint8_t*)tablePoolStart;

    uint64_t numPages = tablePoolSize / mmuPageSize;

    ASSERT(numPages > 4)

    for (; numPages > 0; numPages--)
    {
        mmuFreeTable((uint64_t*)nextPage);
        nextPage += mmuPageSize;
    }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MmuInit(uintptr_t tablePoolStart, uintptr_t tablePoolSize)
{
    mmuTablePoolInit(tablePoolStart, tablePoolSize);
    mmuTTBR0                = mmuAllocTTPage();
    mmuTTBR0L1TableCount    = 0;

    mmuTTBR1                = mmuAllocTTPage();
    mmuTTBR1L1TableCount    = 0;    
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void MmuMapRange(uintptr_t PageVirtAddr, uintptr_t PagePhysAddress, uintptr_t RegionsSize, uintptr_t pageTypeIndex, uintptr_t attributes)
{
    uint32_t numPages = RegionsSize/mmuPageSize;
    uintptr_t l1Attributes;
    uintptr_t l2Attributes;

    // we are using 
    uintptr_t l1TableAddr;
    uintptr_t l2TableAddr; 
    uintptr_t l3TableAddr;
    uint32_t l1TableIndex = (PageVirtAddr>>30) & 0x1FF;
    uint32_t l2TableIndex = (PageVirtAddr>>21) & 0x1FF;
    uint32_t l3TableIndex = (PageVirtAddr>>12) & 0x1FF;

    // bit 47 determines TTBR0 or TTBR1
    // we are in a 36 bit address setup so we bypass L0
    if (PageVirtAddr & (1LL<<47))
    {
        l1TableAddr     = mmuTTBR1;
        l1Attributes    = MMU_TT1_DESC_LEVEL1_ATTRIB;
        l2Attributes    = MMU_TT1_DESC_LEVEL1_ATTRIB;
    }
    else
    {
        l1TableAddr     = mmuTTBR0;
        l1Attributes    = MMU_TT0_DESC_LEVEL1_ATTRIB;
        l2Attributes    = MMU_TT0_DESC_LEVEL2_ATTRIB;
    }
    
    // At some point would like to use the SW field of the Table Entries
    // to count the number of entries in the table below.  This will allow
    // pages of the table to be unmapped when all the subtables are released.
    // currently I don't have a unmapRegion function so this isn't needed. 
    while ((l1TableIndex < mmuEntriesPerPage) && (numPages > 0 ))
    {
        l2TableAddr = ((uintptr_t*)l1TableAddr)[l1TableIndex];
        
        if (l2TableAddr == MMU_TABLE_ENTRY_NOT_PRESENT)
        {
            l2TableAddr =  mmuAllocTTPage();
            ((uintptr_t*)l1TableAddr)[l1TableIndex] = (l2TableAddr | l1Attributes);
        }
        else
        {
            l2TableAddr &= MMU_TT_ADDRESS_MASK;
        }

        while ((l2TableIndex < mmuEntriesPerPage) && (numPages > 0))
        {
            l3TableAddr =  ((uintptr_t*)l2TableAddr)[l2TableIndex]; 

            if (l3TableAddr == MMU_TABLE_ENTRY_NOT_PRESENT)
            {
                l3TableAddr = mmuAllocTTPage();
                ((uintptr_t*)l2TableAddr)[l2TableIndex] = (l3TableAddr | l2Attributes);
            }
            else
            {
                l3TableAddr &= MMU_TT_ADDRESS_MASK; 
            }
            
            while ((l3TableIndex < mmuEntriesPerPage) && (numPages > 0))
            {
                ((uintptr_t*)l3TableAddr)[l3TableIndex] = (PagePhysAddress & MMU_TT_ADDRESS_MASK) | (attributes & MMU_TT_ATTRIBUTE_MASK);
                PagePhysAddress += mmuPageSize;
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


