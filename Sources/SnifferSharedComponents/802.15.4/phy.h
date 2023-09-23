/****************************************************************************//**
  \file phy.h

  \brief Phy layer implementation

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
#ifndef _PHY_H
#define _PHY_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include "stdint.h"

/******************************************************************************
                   Define(s) section
******************************************************************************/
#define PHY_PAYLOAD_MAX         128

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum {
    PHY_CHANNEL_11      = 11,
    PHY_CHANNEL_12      = 12,
    PHY_CHANNEL_13      = 13,
    PHY_CHANNEL_14      = 14,
    PHY_CHANNEL_15      = 15,
    PHY_CHANNEL_16      = 16,
    PHY_CHANNEL_17      = 17,
    PHY_CHANNEL_18      = 18,
    PHY_CHANNEL_19      = 19,
    PHY_CHANNEL_20      = 20,
    PHY_CHANNEL_21      = 21,
    PHY_CHANNEL_22      = 22,
    PHY_CHANNEL_23      = 23,
    PHY_CHANNEL_24      = 24,
    PHY_CHANNEL_25      = 25,
    PHY_CHANNEL_26      = 26
}PhyChannel_t;

typedef struct {
    PhyChannel_t channel;            //802.15.4 channel valid
    void(* rxCallback)(uint8_t size, uint8_t * buf);
}PhyConfig_t;

//Phy received frame
typedef struct {
    uint8_t len;
    uint8_t payload[PHY_PAYLOAD_MAX];
    uint8_t lqi;
    int8_t  rssi;
    uint8_t channel;
}PhyRx_t;

/******************************************************************************
                   Global variables section
******************************************************************************/
/******************************************************************************
                   Prototypes section
******************************************************************************/
void PHY_Init( void );

/**************************************************************************//**
\brief Select ZigBee channel
    /param[in]     channel          11 - 26 representing channel number
******************************************************************************/
void PHY_SetChannel( uint8_t channel );

/**************************************************************************//**
\brief Phy Send frame
******************************************************************************/
int PHY_SendFrame( uint8_t * buffer, uint16_t length );

/**************************************************************************//**
\brief Phy task
    Retreive received messages and pass to MAC layer
******************************************************************************/
void PHY_Task( void );

#endif // _PHY_H
