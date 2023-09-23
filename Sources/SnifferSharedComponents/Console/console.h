/****************************************************************************//**
  \file console.h

  \brief Console for debug prints and commands

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
#ifndef _CONSOLE_H
#define _CONSOLE_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include "printf.h"
#include "mac.h"

/******************************************************************************
                   Define(s) section
******************************************************************************/
/******************************************************************************
                   Types section
******************************************************************************/
/******************************************************************************
                   Prototypes section
******************************************************************************/
/**************************************************************************//**
\brief Init debug console
******************************************************************************/
void Console_Init( void );

/**************************************************************************//**
\brief Print a single byte to console
******************************************************************************/
void Console_PutByte( uint8_t byte );

/**************************************************************************//**
\brief Send a buffer content to console
******************************************************************************/
void Console_Write( uint8_t const * buffer, uint16_t size );

/**************************************************************************//**
\brief Send a formatted string to console
******************************************************************************/
void Console_Print( const char * count, ...);

/**************************************************************************//**
\brief Console reception handler
******************************************************************************/
void Console_RxByte( uint8_t byte );

/**************************************************************************//**
\brief Process uart reception
******************************************************************************/
void Console_Process_Rx( void );

/**************************************************************************//**
\brief JSON buffer V2
Convert a 802.15.4 phy packet to a JSON message
Payload is concatenated in a hex string to improve message density
where
L = length
Q = LQI
R = RSSI
C = channel
S = string of hexadecimal representation of 802.15.4 packet
Example:
{"L":50,"Q":255,"R":-94,"S":"4188a31e48ffff00000912fcff000001cc0885dafeffd76b0828f6ea32000885dafeffd76b0800295e19cad6ebd84ca2aee2"}
******************************************************************************/
void Console_PhyToJSONV2( PhyRx_t * phy_rx);



#endif // _CONSOLE_H
