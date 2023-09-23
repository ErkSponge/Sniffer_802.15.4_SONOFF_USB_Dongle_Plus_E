/****************************************************************************//**
  \file BSP_Sonoff_USB_Dongle_Plus_E.h

  \brief Board speicific configuration for SONOFF USB Dongle Plus E

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
#ifndef _BSP_SONOFF_USB_DONGLE_PLUS_E_H
#define _BSP_SONOFF_USB_DONGLE_PLUS_E_H

/******************************************************************************
                   Define(s) section
******************************************************************************/

#define DEBUG_EFM               1
#define EFR32MG21A020F768IM32   1
//#define SL_COMPONENT_CATALOG_PRESENT    1

//define entry point - startup files call _start
#define _start      main

//include config file for printf
#define PRINTF_INCLUDE_CONFIG_H     1

//Ensure bss section is initialized with 0s
#define __STARTUP_CLEAR_BSS


//Define used hardware for console output
#define HAL_CONSOLE_USART               USART0
#define HAL_CONSOLE_USART_ID            0
#define HAL_CONSOLE_RX_IRQ              USART0_RX_IRQn
#define HAL_CONSOLE_USART_CLOCK         cmuClock_USART0

// USART0 TX on PB01
#define HAL_CONSOLE_USART_TX_PORT       gpioPortB
#define HAL_CONSOLE_USART_TX_PIN        1

// USART0 RX on PB00
#define HAL_CONSOLE_USART_RX_PORT       gpioPortB
#define HAL_CONSOLE_USART_RX_PIN        0

#include "em_gpio.h"
#define LED_PORT        gpioPortC
#define LED_PIN         0

/******************************************************************************
                    Includes section
******************************************************************************/

void BSP_Init( void );
void BSP_InitLed( void );
void BSP_SetLed( void );
void BSP_ClrLed( void );

#endif // _BSP_SONOFF_USB_DONGLE_PLUS_E_H
