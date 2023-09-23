/****************************************************************************//**
  \file BSP_Sonoff_USB_Dongle_Plus_E.c

  \brief Board support package for Sonoff USB Dongle Plus E

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
#include "BSP_Sonoff_USB_Dongle_Plus_E.h"
#include "em_chip.h"
#include "Hal.h"
#include "application_properties.h"

/******************************************************************************
                   Implementations section
******************************************************************************/
/******************************************************************************
                   Global variables section
******************************************************************************/
//Interface with the booloader requires an ApplicationProperties_t structure
__attribute__((used)) const ApplicationProperties_t sl_app_properties = {
  .magic = APPLICATION_PROPERTIES_MAGIC,
  .structVersion = 0,
  .signatureType = APPLICATION_SIGNATURE_NONE,
  .signatureLocation = 0xFFFFFFFFU,
  .app = {
    .type = APPLICATION_TYPE_ZIGBEE,
    .version = 0,
    .capabilities = 0,
    .productId = {0},
  },
  .longTokenSectionAddress = (uint8_t *)0,
};

/******************************************************************************
                   Local variables section
******************************************************************************/

/******************************************************************************
                   Local function section
******************************************************************************/


/******************************************************************************
                   Global function section
******************************************************************************/
void BSP_Init( void )
{
    //Call SiLabs errata handler
    CHIP_Init();

    //Init Clocks
    HAL_Clocks_Init();

    //Init Console
    HAL_Console_Init();

    //Init Radio - does not enable
    HAL_Radio_Init();

    BSP_InitLed();

}

void BSP_InitLed( void )
{
    GPIO_PinModeSet(LED_PORT,
                    LED_PIN,
                    gpioModePushPull,
                    1);
}

void BSP_SetLed( void )
{
    GPIO_PinOutClear(LED_PORT, LED_PIN);
}

void BSP_ClrLed( void )
{
    GPIO_PinOutSet(LED_PORT, LED_PIN);
}

// eof BSP_Sonoff_USB_Dongle_Plus_E.c

