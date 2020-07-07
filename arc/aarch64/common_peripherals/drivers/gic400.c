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


static gic400DistributorMmio_t*    gicDustrubutorMmio;
static gic400CpuMmio_t*            gicCpuMmio;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void initGicCpu(void)
{    
    gicCpuMmio->intPriorityMask     = 0xFF;
    gicCpuMmio->ctrl                = 1;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void initGic(gicDustrubutorMmio* gicDistAddr, gic400CpuMmio_t* gicCpuAddr)
{
    gicDustrubutorMmio  = gicDistAddr;
    gicCpuMmio          = gicCpuAddr;

    uint32_t numInterrupts;
    (gic400DistributorMmio_t*) intCtrlrMmio;

    numInterrupts = 32*((gicDustrubutorMmio->intCtrlrType & 0x1F)+1);


    // disable all interrupts then clear any pending and active interrupts
    // then let the OS re-enable them as it needs to.

    // Disable all interrupts
    gicDustrubutorMmio->intClrEnable[0]            = 0x0000FFFF;
    gicDustrubutorMmio->intClrActive[0]            = 0x0000FFFF;
    gicDustrubutorMmio->intClrPend[0]              = 0x0000FFFF;


    // disable and clear all interrupts
    for (int idx = 1; idx < numInterrupts/32 ; idx++)
    {
        gicDustrubutorMmio->intGroup[idx]                  = 0;    // move all interrupts to group 0
        gicDustrubutorMmio->nonSecureAccessControl[idx]    = 0;    // make all interrupts secure
        gicDustrubutorMmio->intClrEnable[idx]              = 0xFFFFFFFF;
        gicDustrubutorMmio->intClrActive[idx]              = 0xFFFFFFFF;
        gicDustrubutorMmio->intClrPend[idx]                = 0xFFFFFFFF;

    }

    // set the priority for each interrupt
    // the default will be priority level 127
    // this puts the all in the middle of the map
    // Group 0 interrupts should always be 0x00-0x7F
    // Group 1 interrupts should always be 0x80-0xFF
    for (int idx = 0; idx < numInterrupts/4; idx++)
    {
        gicDustrubutorMmio->intPriority[idx]    = 0x7F7F7F7F;   
    }

    // route all interrupts to all CPUs
    // first 8 regsiters are RO because they are
    // CPU Private
    for (int idx = 8; idx < numInterrupts/4; idx++)
    {
        gicDustrubutorMmio->intTargets[idx]    = 0x0F0F0F0F;
    }

    // configure all of the interrupts as level
    for (int idx = 2; idx < numInterrupts/2; idx++)
    {
            // for now set them all to level sensitive
        gicDustrubutorMmio->intConfig[idx]     = 0xAAAAAAAA;
    }

    initGicCpu();
}


void intCtrlrEnableInt(uint32_t irq, uint32_t priority)
{
    gicDistMmio->ctrl = 0;              // disable the gic to do the initialization
      gic_distributor->interrupt_set_enable[irq/32] = 1<<(irq & 0x1F);
      gic_distributor_enable();
}
