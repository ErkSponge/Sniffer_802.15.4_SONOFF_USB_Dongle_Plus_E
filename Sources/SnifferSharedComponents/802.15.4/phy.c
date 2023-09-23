/****************************************************************************//**
  \file phy.c

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
/******************************************************************************
                   Includes section
******************************************************************************/
#include "stdint.h"
#include "console.h"
#include "phy.h"
#include "string.h"
#include "mac.h"
#include "Hal_Radio.h"

/******************************************************************************
                   Define section
******************************************************************************/
/******************************************************************************
                   Types section
******************************************************************************/
/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
                   Global variables section
******************************************************************************/
/******************************************************************************
                   Local variables section
******************************************************************************/
/******************************************************************************
                   Local function section
******************************************************************************/

/******************************************************************************
                   Global function section
******************************************************************************/
/**************************************************************************//**
\brief Init physical layer
******************************************************************************/
// void PHY_Init( void )
// {
// }

/**************************************************************************//**
\brief PHY enable RX
    Enable reception of messages
    /param[in]     * phy_config              pointer rx configuration
                    phy_config->channel      channel to listen
                    phy_config->rx_callback  function to call on reception of a frame

******************************************************************************/
// void PHY_EnableRx( PhyConfig_t * phy_config )
// {
//     (void) phy_config;
// }

/**************************************************************************//**
\brief Set internal short address used by the PHY filter
    /param[in]     shortAddr        short address to be used for filtering
                                    incoming messages
******************************************************************************/
// void PHY_SetShortAddress( uint16_t shortAddr )
// {
// }

/**************************************************************************//**
\brief Set internal panID used by the PHY filter
    /param[in]     panId      Pan ID  to be used for filtering incoming messages
******************************************************************************/
// void PHY_SetPanID( uint16_t panId )
// {
// }

/**************************************************************************//**
\brief Set internal extended address used by the PHY filter
    /param[in]     extAddr      extended address to be used for filtering
                                incoming messages
******************************************************************************/
// void PHY_SetExtAddr( uint64_t extAddr )
// {
// }

/**************************************************************************//**
\brief Select ZigBee channel
    /param[in]     channel          11 - 26 representing channel number
******************************************************************************/
// void PHY_SetChannel( uint8_t channel )
// {
// }


// /**************************************************************************//**
// \brief Send a frame over the air
//     /param[in]     buffer       pointer to buffer to send
//     /param[in]     size         buffer length
// ******************************************************************************/
// int PHY_SendFrame( uint8_t * buffer, uint8_t size )
// {
// }


/**************************************************************************//**
\brief Phy task
    Retreive received messages and pass to MAC layer
******************************************************************************/
void PHY_Task( void )
{
    PhyRx_t phy_rx;

    //Retreive and process radio packets
    if( HAL_Radio_GetRxPacket( &phy_rx ) == HAL_RADIO_GET_RX_PACKET_SUCCESS )
    {
        MAC_ProcessPhyRx( &phy_rx );
    }
}


// eof phy.c

