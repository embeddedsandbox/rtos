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
#include "uktypes.h"
#include "status.h"

typedef status_t (*fpDiverInit_t)(void * mmio, uint32_t interrupt);
typedef status_t (*fpDriverDevControl_t)(uint32_t ioctl_num, void* arg1);
typedef status_t (*fpDriverWrite_t)(uint8_t* data, uintptr_t size);
typedef status_t (*fpDriverRead_t)(uint8_t* data, uintptr_t size);
typedef status_t (*fpDriverPower_t)(uint32_t command, uint32_t arg);


typedef struct
{
    fpDiverInit_t           init;
    fpDriverPower_t         powerCtrl;
    fpDriverDevControl_t    deviceControl;
    fpDriverWrite_t         write;
    fpDriverRead_t          read;
} _uKDriver_t;



typedef enum
{
    PWR_CMD_SET_POWER_ON,
    PWR_CMD_SET_PWER_OFF,
} powerCommands_t;



typedef enum
{
    EMMC_RESPONSE_NONE,
    EMMC_RESPONSE_48_BITS,
    EMMC_RESPONSE_136_BITS,
} emmcCtrlrResponseType_t;

typedef enum
{ 
    EMMC_TRANSFER_NONE,
    EMMC_TRANSFER_READ_DATA,
    EMMC_TRANSFER_WRITE_DATA,
} emmcCtrlrTransferType_t;




typedef status_t (*fpEmmcCtrllInit)(uint32_t* mmio, uint32_t interrupt, uint32_t blkSize, uint32_t pagesToPreErase);
typedef status_t (*fpEmmcCtrlrSetClockSpeed)(uint32_t clkInHz);
typedef status_t (*fpEmmcCtrlrSetVoltage)(uint32_t voltIn10ths);
typedef status_t (*fpEmmcCtrlrSendCmd)(uint32_t cmd, uint32_t args, uint32_t respType, uint32_t ResponseType, uint32_t TransferType ); // args are [7:0] arg0, [15:8] arg1, [23:16] arg2, [31:24] arg3
typedef status_t (*fpEmmcCtrlrGetResponse)(uint64_t* resp0, uint64_t* resp1);
typedef status_t (*fpEmmcCtrlrSetDataBuffer)(uint8_t* buffer); // this should be a Scatter gather list
typedef status_t (*fpEmmcCtrlrSetBlocksToPreErase)(uint32_t pagesToPreErase);
typedef status_t (*fpInterruptHandler)(void);


typedef struct 
{
    fpEmmcCtrllInit                     init;
    fpEmmcCtrlrSetClockSpeed            setClkSpeed;
    fpEmmcCtrlrSetVoltage               setVoltage;
    fpEmmcCtrlrSendCmd                  sendCmd;
    fpEmmcCtrlrGetResponse              getResp;
    fpEmmcCtrlrSetDataBuffer            setDataBuffer;
    fpEmmcCtrlrSetBlocksToPreErase      setBlocksToPreErase;
    fpInterruptHandler                  interruptHandler;
} emmcController;