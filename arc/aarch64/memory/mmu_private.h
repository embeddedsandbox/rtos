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
//  TCR_EL1 EQUATES 
//=============================================================================

#define TCR_EL1_RESERVED            (0x0000000080000000)
#define TCR_EL1_TBI                 (0x0000006000000000)    // Top Byte ignored in Address Calculations
#define TCR_EL1_ASID_SIZE           (0x0000001000000000)    // 16 Bit ASID
#define TCR_EL1_IPS                 (0x0000000100000000)    // 36bit Physical addresses
#define TCR_EL1_TTBR1_GSIZE         (0x0000000000000000)    // 4K Granules for TTBR1_EL1
#define TCR_EL1_TTBR1_SHAREABILITY  (0x0000000030000000)    // inner Shareable  (could use outer shareable)
#define TCR_EL1_TTBR1_CACHEABILITY  (0x000000000A000000)    // Outer write through Inner Write through
#define TCR_EL1_EPD1                (0x0000000000800000)    // Do Not perform a table walk on TTBR1  (for now)
#define TCR_EL1_A1                  (0x0000000000000000)    // ASID is defined by TTBR0.ASID
#define TCR_EL1_T1SZ                (0x00000000001C0000)    // TTRB1 is 36 bit offsets (Need to better understand this before we start using TTBR1)
#define TCR_EL1_TTBR0_GSIZE         (0x0000000000000000)    // 4K Granules for TTBR0_EL1
#define TCR_EL1_TTBR0_SHAREABILITY  (0x0000000000003000)    // inner Shareable  (could use outer shareable)
#define TCR_EL1_TTBR0_CACHEABILITY  (0x0000000000000A00)    // outer write through Inner Write Through
#define TCR_EL1_T0SZ                (0x000000000000001C)    // TTRB0 size is 36 bit offsets

#define TCR_EL1_DEFAULT             (TCR_EL1_RESERVED | TCR_EL1_TBI | TCR_EL1_ASID_SIZE | TCR_EL1_IPS | TCR_EL1_IPS | \
                                     TCR_EL1_TTBR1_GSIZE | TCR_EL1_SHAREABILITY | TCR_EL1_TTBR1_CACHEABILITY | TCR_EL1_EPD1 | \
                                     TCR_EL1_A1 | TCR_EL1_T1SZ | TCR_EL1_TTBR0_GSIZE | TCR_EL1_TTBR0_CACHEABILITY | )

#define L1L2_4KG_BLOCK_DESC_SECURE   (0x0000000000000000)
#define L1L2_4KG_BLOCK_DESC_SECURE   (0x0000000000000000)


//=============================================================================
// Table Entry EQUATES 
//=============================================================================
#define MMU_MDESC_ATTRIB_MASK               (0x0000000FFFFFFC00)
#define MMU_MDESC_ATTRIB_AF                 (1<<10)
#define MMU_MDESC_ATTRIB_SHAREABLE_         (0<<8) // TODO 2 bits
#define MMU_MDESC_ATTRIB_ACCESS_PERM_       (0<<6) // TODO 
#define MMU_MDESC_ATTRIB_SECURE             (0<<5)
#define MMU_MDESC_ATTRIB_NONSECURE          (1<<5)
#define MMU_MDESC_ATTRIB_UXN                (1<<54)
#define MMU_MDESC_ATTRIB_PXN                (1<<53)


#define MMU_DESC_TYPE_TABLE_DESC            0x11
#define MMU_DESC_TYPE_L1_L2_BLOCK_DESC      0x01
#define MMU_DESC_TYPE_L3_BLOCK_DESC         0x10
#define MMU_DESC_TYPE_INVALID_ENTRY         0x00



#define MMU_TABLE_ENTRY_NOT_PRESENT     (0)
#define MMU_TT0_DESC_LEVEL1_ATTRIB      (0)
#define MMU_TT0_DESC_LEVEL2_ATTRIB      (0)
#define MMU_TT1_DESC_LEVEL1_ATTRIB      (0)
#define MMU_TT1_DESC_LEVEL2_ATTRIB      (0)

#define MMU_TT_ADDRESS_MASK             (0x0000000FFFFF000)
#define MMU_TT_ATTRIBUTE_MASK           (0x0000FFF00000FFF)

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

