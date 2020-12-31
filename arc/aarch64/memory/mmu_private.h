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

//=============================================================================
//  MMU Page Table Desc types 
//=============================================================================
typedef union 
{
    struct 
    {
        uint64_t    descType            :   2;  // [0:1]
        /* Table Descriptors do not have Lower Attributes           */
        /* this is a bit confusing since it says some are inherited */
        uint64_t    memAttrIdx          :   3;  // [2:4]
        uint64_t    notSecure           :   1;  // [5]
        uint64_t    accessPerission     :   2;  // [6:7]
        uint64_t    shareability        :   2;  // [8:9]
        uint64_t    accessedFlag        :   1;  // [10]
        uint64_t    reserved0           :   1;  // [11]
        /*------------------------------------------------*/
        uint64_t    address             :  36;  // [12:47]
        uint64_t    reserved2           :   5;  // [48:52]
        uint64_t    privExecNever       :   1;  // [53]
        uint64_t    unprivExecNever     :   1;  // [54]
        uint64_t    softwareUse         :   4;  // [55:58]
        uint64_t    reserved3           :   5;  // [59:63]
    };
    uint64_t    all;
} mmuDescriptor_t;

typedef  mmuDescriptor_t* mmuPageHandle_t;

#define MMU_DESC_TYPE_TABLE_DESC                (0x3)
#define _MMU_DESC_TYPE_BLOCK_DESC                (0x1)
#define MMU_DESC_TYPE_PAGE_DESC                (0x3)

//#define MMU_DESC_TYPE_L1_L2_BLOCK_DESC          (0x1)
//#define MMU_DESC_TYPE_L3_BLOCK_DESC             (0x1)
#define MMU_DESC_TYPE_INVALID_ENTRY             (0x0)


//------------------------------------------------------------------------------
// Memory Attribute Table Entry EQUATES 
//------------------------------------------------------------------------------
// Index 0  -- Normal Memory Write-Back Read-Write-Allocate 
// Index 1  -- Normal Memory Non-Cacheable (Video Memory or DMA buffers)
// Index 2  -- Device Memory Device Memory
// Index 3  -- Normal Memory Non-Cacheable (Reserved for future use)
// Index 4  -- Normal Memory Non-Cacheable (Reserved for future use)
// Index 5  -- Normal Memory Non-Cacheable (Reserved for future use)
// Index 6  -- Normal Memory Non-Cacheable (Reserved for future use)
// Index 7  -- Normal Memory Non-Cacheable (Reserved for future use)
#define MAIR_EL1_MAP                            ((0x00LL<<16) | (0x44LL<<8) |  (0xFFLL))  //  Note 0x77 should also be Write Back and seems to get us to a fault of 
#define MMU_MAIR_IDX_NORMAL_MEM_CACHEABLE       (0x0)
#define MMU_MAIR_IDX_NORMAL_MEM_NONCACHEABLE    (0x1)
#define MMU_MAIR_IDX_DEVICE_MEM                 (0x2)

#define MMU_DESC_AP_NOACCESS_RW                 (0x0)
#define MMU_DESC_AP_RW_RW                       (0x1)
#define MMU_DESC_AP_NOACCESS_RO                 (0x2)
#define MMU_DESC_AP_RO_RO                       (0x3)

#define MMU_DESC_SHARE_NONE                     (0x0)
#define MMU_DESC_SHARE_OUTER                    (0x2)
#define MMU_DESC_SHARE_INNER                    (0x3)

#define MMU_TABLE_DESC                                      (mmuDescriptor_t){.descType = MMU_DESC_TYPE_TABLE_DESC, .memAttrIdx = MMU_MAIR_IDX_NORMAL_MEM_CACHEABLE,    .notSecure = 0, .accessPerission = MMU_DESC_AP_NOACCESS_RW, .shareability = MMU_DESC_SHARE_INNER, .accessedFlag = 1, .address = 0, .privExecNever = 0, .unprivExecNever = 0, .softwareUse = 0  }
#define MMU_BLOCK_DESC_MEM_PRIVILEGED_SECURE_CACHEABLE      (mmuDescriptor_t){.descType = MMU_DESC_TYPE_PAGE_DESC,  .memAttrIdx = MMU_MAIR_IDX_NORMAL_MEM_CACHEABLE,    .notSecure = 0, .accessPerission = MMU_DESC_AP_NOACCESS_RW, .shareability = MMU_DESC_SHARE_INNER, .accessedFlag = 1, .address = 0, .privExecNever = 0, .unprivExecNever = 0, .softwareUse = 0  }
#define MMU_BLOCK_DESC_MEM_PRIVILEGED_SECURE_NONCACHEABLE   (mmuDescriptor_t){.descType = MMU_DESC_TYPE_PAGE_DESC,  .memAttrIdx = MMU_MAIR_IDX_NORMAL_MEM_NONCACHEABLE, .notSecure = 0, .accessPerission = MMU_DESC_AP_NOACCESS_RW, .shareability = MMU_DESC_SHARE_INNER, .accessedFlag = 1, .address = 0, .privExecNever = 0, .unprivExecNever = 0, .softwareUse = 0  }
#define MMU_BLOCK_DESC_MEM_UNPRIVILEGED_SECURE_CACHEABLE    (mmuDescriptor_t){.descType = MMU_DESC_TYPE_PAGE_DESC,  .memAttrIdx = MMU_MAIR_IDX_NORMAL_MEM_CACHEABLE,    .notSecure = 0, .accessPerission = MMU_DESC_AP_NOACCESS_RW, .shareability = MMU_DESC_SHARE_INNER, .accessedFlag = 1, .address = 0, .privExecNever = 0, .unprivExecNever = 0, .softwareUse = 0  }
#define MMU_BLOCK_DESC_MEM_UNPRIVILEGED_SECURE_NONCACHEABLE (mmuDescriptor_t){.descType = MMU_DESC_TYPE_PAGE_DESC,  .memAttrIdx = MMU_MAIR_IDX_NORMAL_MEM_NONCACHEABLE, .notSecure = 0, .accessPerission = MMU_DESC_AP_NOACCESS_RW, .shareability = MMU_DESC_SHARE_INNER, .accessedFlag = 1, .address = 0, .privExecNever = 0, .unprivExecNever = 0, .softwareUse = 0  }
#define MMU_BLOCK_DESC_DEVICE_MEM                           (mmuDescriptor_t){.descType = MMU_DESC_TYPE_PAGE_DESC,  .memAttrIdx = MMU_MAIR_IDX_DEVICE_MEM,              .notSecure = 0, .accessPerission = MMU_DESC_AP_NOACCESS_RW, .shareability = MMU_DESC_SHARE_NONE,  .accessedFlag = 1, .address = 0, .privExecNever = 1, .unprivExecNever = 1, .softwareUse = 0  }
#define MMU_TABLE_DESC_NOT_PRESENT                          (mmuDescriptor_t){.descType = MMU_DESC_TYPE_INVALID_ENTRY }


//=============================================================================
//  TCR_EL1 EQUATES 
//=============================================================================

typedef union
{
    struct 
    {
        uint64_t    t0sz        : 6;        // [0:5]   TTBR0 number of va address bits 2^64-t0sz
        uint64_t    reserved0   : 2;        // [6:7]
        uint64_t    irgn0       : 2;        // [8:9]   TTBR0 Inner Cacheability (01-- write back allocate)
        uint64_t    orgn0       : 2;        // [10:11] TTBR0 Outer Cacheability (01-- write back allocate)
        uint64_t    sh0         : 2;        // [12:13] TTBR0 Shareability       "Outer Shareable"
        uint64_t    tg0         : 2;        // [14:15] TTBR0 granule size
        uint64_t    t1sz        : 6;        // [16:21] TTBR1 number of va address bits 2^64-t1sz
        uint64_t    asidSelect  : 1;        // [22] Selects TTBR0.ASID or TTBR1.ASID
        uint64_t    epd1        : 1;        // [23]
        uint64_t    irgn1       : 2;        // [24:25] TTBR1 Inner Cacheability (01-- write back allocate)
        uint64_t    orgn1       : 2;        // [26:27] TTBR1 Outer Cacheability (01-- write back allocate)
        uint64_t    sh1         : 2;        // [28:29] TTBR1 shareability
        uint64_t    tg1         : 2;        // [30:31] TTBR1 granule size
        uint64_t    ipsIpa      : 3;        // [32:34] intermediate Physical Address Size
        uint64_t    reserved1   : 1;        // [35]
        uint64_t    asidSize    : 1;        // [36] 0-8bits 1-16bits
        uint64_t    tbi0        : 1;        // [37] 1=top byte used in address calculation
        uint64_t    tbi1        : 1;        // [38] 1=top byte used in Address Calculation
        uint64_t    reserved2   : 25;       // [39:63] 
    };
    uint64_t    all;
} tcrReg_t;

#define TCR_TG0_4K       (0b00)
#define TCR_TG0_16K      (0b10)
#define TCR_TG0_64K      (0b01)
#define TCR_TG1_4K       (0b10)
#define TCR_TG1_16K      (0b01)
#define TCR_TB1_64K      (0b11)

#define TCR_SH_INNER    (0b11)
#define TCR_SH_OUTER    (0b10)
#define TCR_SH_NONE     (0b00)
#define TCP_IPS_32BIT   (0b000)
#define TCP_IPS_36BIT   (0b001)
#define TCP_IPS_40BIT   (0b010)
#define TCP_IPS_42BIT   (0b011)
#define TCP_IPS_44BIT   (0b100)
#define TCR_IPS_48BIT   (0b101)


#define TCR_EL1_DEFAULT (tcrReg_t){.t0sz = 27, .irgn0 = 0b01, .orgn0 = 0b01, .sh0=TCR_SH_INNER, .tg0=TCR_TG0_4K, .t1sz = 27, .asidSize= 0, .epd1 = 0, .irgn1 = 0b01, .orgn1 = 0b01, .sh1=TCR_SH_INNER, .tg1=TCR_TG1_4K, .ipsIpa = TCP_IPS_36BIT, .tbi0=0b00, .tbi1=0b00}

//=============================================================================
//  MMU Table Pool types and defines.   
//=============================================================================

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

typedef struct _tableNode_t tableNode_t;

struct _tableNode_t
{
    tableNode_t *     next;
    tableNode_t *     prev;
};

