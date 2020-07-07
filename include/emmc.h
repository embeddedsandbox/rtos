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




typedef enum
{
    CMD0    = 0x40 + 0,         // Software Reset                                       R1 Response
    CMD1    = 0x40 + 1,         // Initiate Initialization Process                      R1 Response
    CMD8    = 0x40 + 8,         // SDC V2 only Voltage Check  3 args                    R7 Response
    CMD9    = 0x40 + 9,         // Read CSD Register                                    R1 Response
    CMD10   = 0x40 + 10,        // Read CID Register                                    R1 Response
    CMD12   = 0x40 + 12,        // Stop To Read data                                    R1b Response
    CMD16   = 0x40 + 16,        // Change R/W Block Size                                R1 Response
    CMD17   = 0x40 + 17,        // Read Block                                           R1 Response
    CMD18   = 0x40 + 18,        // Read Multiple Blocks                                 R1 Response
    CMD23   = 0x40 + 23,        // MMC only Number of blocks for next multi RW          R1
    ACMD23  = 0x40 + 23,        // SDC only Number of blocks to pre erase for next W    R1
    CMD24   = 0x40 + 24,        // Write Block                                          R1
    CMD25   = 0x40 + 25,        // Write Multi Block                                    R1
    ACMD41  = 0x40 + 41,        // SDC Only Initiate Initialization Process             R1
    CMD55   = 0x40 + 55,        // Leading Command for ACMD<XX>                         R1
    CMD58   = 0x40 + 59,        // Read OCR                                             

} emmcCommands_t;


