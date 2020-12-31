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

#include "gic400.h"
#include "gic400_internal.h"
#include "int_handler.h"

static gic400DistributorMmio_t*    gic400DistributorMmio;
static gic400CpuMmio_t*            gic400CpuMmio;

static InterruptHandler_t           vectorTable[256];

//-----------------------------------------------------------------------------
//  This needs to be run on all CPUs as the GIC has a unique CPU interface 
//  For each processor. 
//-----------------------------------------------------------------------------
status_t Gic400CpuInit(void* gicMmio, /*UNUSED*/ uint32_t interrupt)
{    
    // set the address of the CPU structure. 
    gic400CpuMmio = (gic400CpuMmio_t*)gicMmio;

    gic400CpuMmio->intPriorityMask     = 0xFF;
    gic400CpuMmio->ctrl                = 1;

    return STATUS_SUCCESS;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
status_t Gic400DistributorInit(void* gicMmio, /*UNUSED*/ uint32_t interrupt)
{
    gic400DistributorMmio  = (gic400DistributorMmio_t*)((uint8_t*)gicMmio);

    uint32_t numInterrupts;

    numInterrupts = 32*((gic400DistributorMmio->intCtrlrType & 0x1F)+1);


    // disable all interrupts then clear any pending and active interrupts
    // then let the OS re-enable them as it needs to.

    // Disable all interrupts
    gic400DistributorMmio->intClrEnable[0]            = 0x0000FFFF;
    gic400DistributorMmio->intClrActive[0]            = 0x0000FFFF;
    gic400DistributorMmio->intClrPend[0]              = 0x0000FFFF;


    // disable and clear all interrupts
    for (int idx = 1; idx < numInterrupts/32 ; idx++)
    {
        gic400DistributorMmio->intGroup[idx]                  = 0;    // move all interrupts to group 0
        gic400DistributorMmio->nonSecureAccessControl[idx]    = 0;    // make all interrupts secure
        gic400DistributorMmio->intClrEnable[idx]              = 0xFFFFFFFF;
        gic400DistributorMmio->intClrActive[idx]              = 0xFFFFFFFF;
        gic400DistributorMmio->intClrPend[idx]                = 0xFFFFFFFF;

    }

    // set the priority for each interrupt
    // the default will be priority level 127
    // this puts the all in the middle of the map
    // Group 0 interrupts should always be 0x00-0x7F
    // Group 1 interrupts should always be 0x80-0xFF
    for (int idx = 0; idx < numInterrupts/4; idx++)
    {
        gic400DistributorMmio->intPriority[idx]    = 0x7F7F7F7F;   
    }

    // route all interrupts to all CPUs
    // first 8 regsiters are RO because they are
    // CPU Private
    for (int idx = 8; idx < numInterrupts/4; idx++)
    {
        gic400DistributorMmio->intTargets[idx]    = 0x0F0F0F0F;
    }

    // configure all of the interrupts as level
    for (int idx = 2; idx < numInterrupts/2; idx++)
    {
            // for now set them all to level sensitive
        gic400DistributorMmio->intConfig[idx]     = 0xAAAAAAAA;
    }

    return STATUS_SUCCESS;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void GicIntCtrlrEnableInt(uint32_t interrupt, uint32_t priority, intHandler_t handler, void* context)
{
    vectorTable[interrupt] = (InterruptHandler_t){.handler = handler, .context = context};


    gic400DistributorMmio->ctrl = 0;              // disable the gic to do the initialization
    gic400DistributorMmio->intSetEnable[interrupt/32] = 1<<(interrupt & 0x1F);
    *((uint8_t*)(&gic400DistributorMmio->intPriority[interrupt/4]) + (interrupt & 0b11)) = priority;
    gic400DistributorMmio->ctrl = 1;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
// need to make this a weak symbol so it isn't the symbol that forces this file
// pulled in
void InterruptDispatcher(void)
{
    uint32_t interrupt = gic400CpuMmio->intAcknowledge;

    if ((interrupt & 0x3FF ) < 1022 )  // TODO Clean up Magic Number
    {
        vectorTable[interrupt].handler(vectorTable[interrupt].context);
        gic400CpuMmio->endOfInt = interrupt;
    }
    else
    {
        /* SPURIOUS INTERRUPT  NOTHING FOR THIS CPU TO DO */
    }
    
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
// need to make this a weak symbol so it isn't the symbol that forces this file
// pulled in
void Gic400SoftwareInterrupt(uint32_t interrupt, uint32_t cpu)
{
    gic400DistributorMmio->swGenInt =   ((1 << cpu) << 16) | interrupt;
}



