/****************************************************************************//**
  \file Hal_Radio.h

  \brief Hardware abstractiob layer for radio interface

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

#ifndef _HAL_RADIO_
#define _HAL_RADIO_

/******************************************************************************
                    Includes section
******************************************************************************/
#include "phy.h"

/******************************************************************************
                   Types section
******************************************************************************/
typedef enum {
    HAL_RADIO_GET_RX_PACKET_SUCCESS,
    HAL_RADIO_GET_RX_PACKET_NONE,
    HAL_RADIO_GET_RX_PACKET_INVALID_PARAMETER,
}HAL_Radio_GetRxPacket_Result_t;

/***************************************************************************//**
 * Init radio ready for command
 ******************************************************************************/
void HAL_Radio_Init( void );

/***************************************************************************//**
 * Set radio in promiscuous mode (see 802.15.4 for details)
 ******************************************************************************/
void HAL_Radio_InitPromiscuousMode( void );

/***************************************************************************//**
 * Retreive radio incoming messages
 ******************************************************************************/
HAL_Radio_GetRxPacket_Result_t  HAL_Radio_GetRxPacket( PhyRx_t * phy_rx );

/***************************************************************************//**
 * Select channel to use
 *
 * \param[in]   channel   11 - 26
 ******************************************************************************/
void HAL_SetRadioChannel( uint8_t channel );


#endif //_HAL_RADIO_