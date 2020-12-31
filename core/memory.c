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


#include "mmu.h"
#include "memory.h"

// This rubbish needs to be cleaned up.  We need to be pulling the gTopOfMemory from the list of memory
extern memoryDescriptor_t  platformMemory[];
extern uintptr_t gTopOfMemory;

//------------------------------------------------------------------------------
// initialize the pools of Physical Memory
//    TODO need to change this to receive the memory array as a parameter instead
//    of using a global Varaible
//------------------------------------------------------------------------------
void memoryManagerInit(void)
{
    MmuInit(gTopOfMemory - ((100) * (1024) * (1024)) + 1, ((100) * (1024) * (1024))); 

    uint32_t idx = 0;
    while (platformMemory[idx].memoryBase != -1)
    {
        MmuMapRange(platformMemory[idx].memoryBase, platformMemory[idx].memoryBase, platformMemory[idx].memorySize,platformMemory[idx].memoryType);
        idx++;
    }
}