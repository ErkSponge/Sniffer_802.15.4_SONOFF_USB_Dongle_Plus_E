/***************************************************************************//**
 @file main.c
 @brief Acquire 802.15.4 packets, convert to JSON and sends on serial port

SPDX-License-Identifier: MIT

Copyright (c) 2023 Eric St-Onge

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

/******************************************************************************
                   Includes section
******************************************************************************/
#include "Hal.h"
#include "printf.h"
#include "crc.h"
#include "mac.h"
#include "console.h"


/***************************************************************************//**
 * Local defines
 ******************************************************************************/
/***************************************************************************//**
 * Local function protoypes
 ******************************************************************************/
/***************************************************************************//**
 * Local variables
 ******************************************************************************/
/***************************************************************************//**
 * Local functions
 ******************************************************************************/
/***************************************************************************//**
 * Global functions
 ******************************************************************************/
int main(void)
{
    //Init Hardware
    BSP_Init();

    //Init Components
    //Build CRC table
    crcInit();

    //Should be mac enable promiscuous mode
    //MAC_EnablePromiscuousMode();
    HAL_Radio_InitPromiscuousMode();

    // printf("Hello World!");

    //loop forever
    while(1){
        // HAL_Console_Tx_Byte('E');
        PHY_Task();
        Console_Process_Rx();
    }
}


/**************************************************************************//**
\brief Override normal processing done by the MAC layer
returning true will end frame process
returning false normal processing is performed
******************************************************************************/
bool Mac_RxMsgCallbackPreprocessPhyRx( PhyRx_t * phy_rx )
{
    Console_PhyToJSONV2( phy_rx );
    return true;
}

