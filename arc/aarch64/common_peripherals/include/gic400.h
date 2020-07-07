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

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
typedef struct 
{
    volatile uint32_t   ctrl;                                   //  0x0000
    volatile uint32_t   intCtrlrType;                                   //  0x0004
    volatile uint32_t   interfaceId;                            //  0x0008
    volatile uint32_t   reserved0[28];                          //  0x000C - 0x007C
    volatile uint32_t   intGroup[16];                           //  0x0080 - 0x00BC
    volatile uint32_t   reserved1[16];                          //  0x00C0 - 0x00FC
    volatile uint32_t   intSetEnable[32];                       //  0x0100 - 0x017C    
    volatile uint32_t   intClrEnable[32];                       //  0x0180 - 0x01FC
    volatile uint32_t   intSetPend[32];                         //  0x0200 - 0x027C
    volatile uint32_t   intClrPend[32];                         //  0x0280 - 0x02FC
    volatile uint32_t   intIsActive[32];                        //  0x0300 - 0x037C
    volatile uint32_t   intClrActive[32];                       //  0x0380 - 0x03FC
    volatile uint32_t   intPriority[(64*4) - 4];                //  0x0400 - 0x07F8
    volatile uint32_t   reserved3;                              //  0x07FC
    volatile uint32_t   intTargets[(2*64)];                     //  0x0800 - 0x09FC
    volatile uint32_t   reserved4[2*64];                        //  0x0A00 - 0x0BFC
    volatile uint32_t   intConfig[64];                          //  0x0C00 - 0x0CFC
    volatile uint32_t   ppIntStatus;                            //  0x0D00
    volatile uint32_t   spIntStatus[15];                        //  0x0D04 - 0x0D3C 
    volatile uint32_t   impDef[48];                             //  0x0D40 - 0x0DFC
    volatile uint32_t   nonSecureAccessControl[64];             //  0x0E00 - 0x0EFC
    volatile uint32_t   swGenInt;                               //  0x0F00
    volatile uint32_t   reserved5[3];                           //  0x0F04 - 0x0F0C
    volatile uint32_t   swGenIntClearPend[4];                   //  0x0F10 - 0x0F1C
    volatile uint32_t   swGenIntSetPend[4];                     //  0x0F20 - 0x0F2C
    volatile uint32_t   reserved6[40];                          //  0x0F30 - 0x0FCC
    volatile uint32_t   peripheralId4;                          //  0x0FD0
    volatile uint32_t   peripheralId5;                          //  0x0FD4
    volatile uint32_t   peripheralId6;                          //  0x0FD8
    volatile uint32_t   peripheralId7;                          //  0x0FDC
    volatile uint32_t   peripheralId0;                          //  0x0FE0
    volatile uint32_t   peripheralId1;                          //  0x0FE4
    volatile uint32_t   peripheralId2;                          //  0x0FE8
    volatile uint32_t   peripheralId3;                          //  0x0FEC
    volatile uint32_t   componentId0;                           //  0x0FF0
    volatile uint32_t   componentId1;                           //  0x0FF4
    volatile uint32_t   componentId2;                           //  0x0FF8
    volatile uint32_t   componentId3;                           //  0x0FFC

} gic400DistributorMmio_t;


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
typedef struct 
{
    volatile uint32_t   ctrl;                                   //  0x0000
    volatile uint32_t   intPriorityMask;                        //  0x0004
    volatile uint32_t   binaryPointer;                          //  0x0008
    volatile uint32_t   intAcknowledge;                         //  0x000C
    volatile uint32_t   endOfInt;                               //  0x0010
    volatile uint32_t   runningPriority;                        //  0x0014
    volatile uint32_t   highestPendPriority;                    //  0x0018
    volatile uint32_t   aliasBinaryPointer;                     //  0x001C
    volatile uint32_t   aliasedIntAcknowledge;                  //  0x0020
    volatile uint32_t   aliasedEndOfInt;                        //  0x0024
    volatile uint32_t   aliasedHighestPendPriority;             //  0x0028  
    volatile uint32_t   reserved0[41];                          //  0x002C - 0x00CC
    volatile uint32_t   activePriorities;                       //  0x00D0
    volatile uint32_t   reserved1[3];                           //  0x00D4 - 0x00DC
    volatile uint32_t   nonSecureActivePriorities;              //  0x00E0
    volatile uint32_t   reserved2[5];                           //  0x00E4 - 0x00F8
    volatile uint32_t   cpuInterfaceId;                         //  0x00FC
    volatile uint32_t   reserved3[960];                         //  0x0100 - 0x0FFC
    volatile uint32_t   deactivateInt;                          //  0x1000
} gic400CpuMmio_t;

#define GIC_DIST_INTERFACE_ENABLE   (0x00000001)
#define GIC_CPU_INTERFACE_ENABLE    (0x00000001)