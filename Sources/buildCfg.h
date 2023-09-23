/****************************************************************************//**
  \file buildCfg.h

  \brief Build configuration for x project

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
#ifndef _BUILDCFG_H
#define _BUILDCFG_H

/******************************************************************************
                   Define(s) section
******************************************************************************/

#ifdef TARGET_SONOFF_USB_DONGLE_PLUS_E
//Build configuration for SONOFF USB dongle Plus E
//https://sonoff.tech/product/gateway-and-sensors/sonoff-zigbee-3-0-usb-dongle-plus-e/

#include "./Target/Sonoff_USB_Dongle_Plus_E/BSP_Sonoff_USB_Dongle_Plus_E.h"

#endif

/******************************************************************************
                    Includes section
******************************************************************************/
#include "stdint.h"


#endif // _BUILDCFG_H
