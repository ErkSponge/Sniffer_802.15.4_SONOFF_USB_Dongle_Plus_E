/****************************************************************************//**
  \file mac.h

  \brief MAC layer implementation

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

*******************************************************************************/
#ifndef _MAC_H
#define _MAC_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include "phy.h"
#include "stdbool.h"
#include "mac_unpack.h"

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define MAC_BROADCAST_ALL           0xFFFF
#define MAC_BROADCAST_RX_ON         0xFFFE
#define MAC_BROADCAST_ROUTER        0xFFFC

/******************************************************************************
                   Types section
******************************************************************************/

typedef enum {
    MAC_CMD_ASSOCIATION_REQUEST             =   0x01,
    MAC_CMD_ASSOCIATION_RESPONSE            =   0x02,
    MAC_CMD_DISASSOCIATION_NOTIFICATION     =   0x03,
    MAC_CMD_DATA_REQUEST                    =   0x04,
    MAC_CMD_PANID_CONFLICT_NOTIFICATION     =   0x05,
    MAC_CMD_ORPHAN_NOTIFICATION             =   0x06,
    MAC_CMD_BEACON_REQUEST                  =   0x07,
    //More commands exist but are not supported
}MAC_Command_ID_t;

/******************************************************************************
                   Global variables section
******************************************************************************/
/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Init MAC layer
******************************************************************************/
void MAC_Init( void );

/**************************************************************************//**
\brief MAC reception function
******************************************************************************/
void MAC_ProcessPhyRx( PhyRx_t * phy_rx);

/**************************************************************************//**
\brief Get next MAC sequence number
******************************************************************************/
uint8_t MAC_GetNextSequenceNumber( void );

/**************************************************************************//**
\brief callback for MAC pre message process
This callback is raised upon reception of any frame from phy before any
process is done at the MAC layer
returning true will end frame process
returning false normal processing is performed
******************************************************************************/
bool Mac_RxMsgCallbackPreprocessPhyRx( PhyRx_t * phy_rx );

/**************************************************************************//**
\brief callback for MAC pre message process with unpacked data
This callback is raised upon reception of frame that was susccessfully unpacked
returning true will end frame process
returning false will continue with regular frame processing
******************************************************************************/
bool Mac_RxMsgCallbackPreprocessMacUnpacked( MAC_Frame_Unpacked_t * out );

/**************************************************************************//**
\brief callback for MAC post message process with unpacked data
This callback is raised after mac layer unpacked and processed a received frame
It is intended to indicate to upper layer the reception of a valid frame
returning true will end frame process
returning false will continue with regular frame processing
******************************************************************************/
bool Mac_RxMsgCallbackPostprocessMacUnpacked( MAC_Frame_Unpacked_t * out );



#endif // _MAC_H
