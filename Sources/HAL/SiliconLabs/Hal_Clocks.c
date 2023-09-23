/****************************************************************************//**
  \file Hal_Clock.c

  \brief Configure clocks for hardware specific

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
#include "em_cmu.h"

/***************************************************************************//**
 * Initialize clocks
 ******************************************************************************/
void HAL_Clocks_Init( void )
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  CMU_HFRCODPLLBandSet(cmuHFRCODPLLFreq_80M0Hz);
#else
  CMU_HFRCOBandSet(SL_DEVICE_INIT_HFRCO_BAND);
#endif

  CMU_ClockEnable(cmuClock_GPIO, true);
}