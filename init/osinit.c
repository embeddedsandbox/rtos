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
#include "memory.h"

#include "devices.h"
#include "mmu.h"

#include "int_handler.h"

extern void platformInit(void);
extern void appInit(void);

extern device_t platformDeviceList[];



// These externs are just here for a hack to test 
extern void Gic400SoftwareInterrupt(uint32_t interrupt, uint32_t cpu);
extern void GicIntCtrlrEnableInt(uint32_t interrupt, uint32_t priority, intHandler_t handler, void* context);


// this is here just as a test of the interrupts
void GicSoftIntandler(void * context)
{
    __asm(" B    . \n");        
}

//------------------------------------------------------------------------------
// initialize the pools of Physical Memory
//------------------------------------------------------------------------------
void OsInit(void)
{
    __asm(" B    . \n");
    platformInit();
    memoryManagerInit();

    uint32_t idx = 0;
    while (platformDeviceList[idx].driverInit != NULL)
    {
        if (platformDeviceList[idx].mmio.memoryType != MEM_TYPE_NONE)
        {
            MmuMapRange(platformDeviceList[idx].mmio.memoryBase, platformDeviceList[idx].mmio.memoryBase, platformDeviceList[idx].mmio.memorySize, platformDeviceList[idx].mmio.memoryType);
        }
        if (platformDeviceList[idx].mem.memoryType != MEM_TYPE_NONE)
        {
            MmuMapRange(platformDeviceList[idx].mem.memoryBase, platformDeviceList[idx].mem.memoryBase, platformDeviceList[idx].mem.memorySize, platformDeviceList[idx].mem.memoryType);            
        }

        idx++;
    }
    MmuEnable();

    idx = 0;
    while (platformDeviceList[idx].driverInit != NULL)
    {
        platformDeviceList[idx].driverInit((void*)platformDeviceList[idx].mmio.memoryBase, 0);
        idx++;
    }

#define SGI_0_INT_ID    (0)
    //enable Software Interrupts
    GicIntCtrlrEnableInt(SGI_0_INT_ID, 0, GicSoftIntandler, NULL);

#define CPU_0_ID    (0)

    Gic400SoftwareInterrupt(SGI_0_INT_ID, CPU_0_ID);


    //memoryPoolsInit();
    
//  __ukEventsInit();               // ARC specific code
//  __ukProcessManagementInit();    // SHOULD BE GENERIC
//
//  
//   // process_create(8, idleProc, &__idle_stack__);
//    platform_start_cpus();

    appInit();

//    cpu_id_t id = arc_get_cpu_id();
//    dm_DispatchProcess(id);

    // we should never get here, but we will hang here just in case. 
    while(1)
    {

    }
}
