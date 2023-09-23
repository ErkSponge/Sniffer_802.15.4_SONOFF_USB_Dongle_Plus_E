/****************************************************************************//**
  \file Hal_Console.c

  \brief Hardware abstractiob layer for Console

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

#include "Hal.h"
#include "em_usart.h"
#include "em_cmu.h"
#include "console.h"

/***************************************************************************//**
 * Private defines
 ******************************************************************************/
// //Define used hardware for console output
// #define HAL_CONSOLE_USART               USART0
// #define HAL_CONSOLE_USART_ID            0
// #define HAL_CONSOLE_RX_IRQ              USART0_RX_IRQn
// #define HAL_CONSOLE_USART_CLOCK         cmuClock_USART0

// // USART0 TX on PB01
// #define HAL_CONSOLE_USART_TX_PORT       gpioPortB
// #define HAL_CONSOLE_USART_TX_PIN        1

// // USART0 RX on PB00
// #define HAL_CONSOLE_USART_RX_PORT       gpioPortB
// #define HAL_CONSOLE_USART_RX_PIN        0

/***************************************************************************//**
 * Private types
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

//Define used hardware for console output

// USART0 TX on PB01
#define HAL_CONSOLE_USART_TX_PORT       gpioPortB
#define HAL_CONSOLE_USART_TX_PIN        1

// USART0 RX on PB00
#define HAL_CONSOLE_USART_RX_PORT       gpioPortB
#define HAL_CONSOLE_USART_RX_PIN        0

/***************************************************************************//**
 * Initialize uart
 ******************************************************************************/
void HAL_Console_Init( void )
{
    USART_TypeDef * console_usart = HAL_CONSOLE_USART;
    IRQn_Type irq = USART0_RX_IRQn;

    const USART_InitAsync_TypeDef init = {
        /** Specifies whether TX and/or RX is enabled when initialization is completed. */
        .enable = usartDisable,
        /**
        * USART/UART reference clock assumed when configuring baud rate setup.
        * Set to 0 to use the currently configured reference clock.
        */
        .refFreq = 0,
        /** Desired baud rate. */
        .baudrate = 1000000,
        /** Oversampling used. USART_OVS_TypeDef */
        .oversampling = usartOVS16,
        /** Number of data bits in frame. Notice that UART modules only support 8 or
        * 9 data bits. USART_Databits_TypeDef */
        .databits = usartDatabits8,
        /** Parity mode to use. USART_Parity_TypeDef */
        .parity = usartNoParity,
        /** Number of stop bits to use. USART_Stopbits_TypeDef */
        .stopbits = usartStopbits1,
        #if !defined(_EFM32_GECKO_FAMILY)
        /** Majority Vote Disable for 16x, 8x and 6x oversampling modes. */
        .mvdis = false,             /* Do not disable majority vote. */
        /** Enable USART Rx via PRS. */
        .prsRxEnable = false,
        /** Select PRS channel for USART Rx. (Only valid if prsRxEnable is true). */
        .prsRxCh = 0,
        #endif

        /** Auto CS enabling. */
        .autoCsEnable = false,
        #if (_SILICON_LABS_32B_SERIES > 0)
        /** Auto CS hold time in baud cycles. */
        .autoCsHold = 0,
        /** Auto CS setup time in baud cycles. */
        .autoCsSetup = 0,

        /** Hardware flow control mode. */
        .hwFlowControl = usartHwFlowControlNone,
        #endif
    };

    CMU_ClockEnable(HAL_CONSOLE_USART_CLOCK, true);

    USART_InitAsync(console_usart, &init);

    //Config pins
     GPIO_PinModeSet(HAL_CONSOLE_USART_TX_PORT, HAL_CONSOLE_USART_TX_PIN, gpioModePushPull, 1);
     GPIO_PinModeSet(HAL_CONSOLE_USART_RX_PORT, HAL_CONSOLE_USART_RX_PIN, gpioModeInputPull, 1);

    // RX //
    // Enable Rx
    console_usart->CMD = USART_CMD_RXEN;
    // Wait for Rx to be enabled
    while (!(console_usart->STATUS & USART_STATUS_RXENS)) {
    }
    // Enable Rx route
    GPIO->USARTROUTE[HAL_CONSOLE_USART_ID].RXROUTE =
           (HAL_CONSOLE_USART_RX_PORT << _GPIO_USART_RXROUTE_PORT_SHIFT)
         | (HAL_CONSOLE_USART_RX_PIN << _GPIO_USART_RXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE_SET[HAL_CONSOLE_USART_ID].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN;

    // TX //
    console_usart->CMD = USART_CMD_TXEN;
    // Wait for Tx to be enabled
    while (!(console_usart->STATUS & USART_STATUS_TXENS)) {
    }
    // Enable Tx route
     GPIO->USARTROUTE[HAL_CONSOLE_USART_ID].TXROUTE =
           (HAL_CONSOLE_USART_TX_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
         | (HAL_CONSOLE_USART_TX_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE_SET[HAL_CONSOLE_USART_ID].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN;

    NVIC_DisableIRQ(irq);
    NVIC_ClearPendingIRQ(irq);
    NVIC_SetPriority(irq, 8);    //set uart interrupt priority
    NVIC_EnableIRQ(irq);

    //clear any pending interrupt assuming writing 1 clear state
    console_usart->IF_CLR = USART_IF_RXDATAV;
    //try to enable interrupt
    console_usart->IEN_SET = USART_IEN_RXDATAV;
}

/***************************************************************************//**
 * Transmit a single byte on debug uart
 * If uart tx buffer full, wait for buffer to be available
 ******************************************************************************/
void HAL_Console_Tx_Byte( uint8_t byte )
{
    USART_TypeDef * console_usart = HAL_CONSOLE_USART;
    while( (console_usart->STATUS & USART_STATUS_TXBL) != USART_STATUS_TXBL){};
    console_usart->TXDATA = byte;
}


//using dma - todo
//DMADRV_MemoryPeripheral
//StartTransmitDma

/***************************************************************************//**
 * UART interrupt handler
 ******************************************************************************/
void USART0_RX_IRQHandler( void )
{
    Console_RxByte(HAL_CONSOLE_USART->RXDATA);
}