/****************************************************************************//**
  \file Hal_Radio.c

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

#include "Hal.h"
#include "rail.h"
#include "rail_ieee802154.h"
#include "printf.h"

/***************************************************************************//**
 * Private defines
 ******************************************************************************/

/***************************************************************************//**
 * Local prototypes
 ******************************************************************************/
static void radioEventHandler(RAIL_Handle_t railHandle,
                              RAIL_Events_t events);


/***************************************************************************//**
 * Local variables
 ******************************************************************************/
static RAIL_Handle_t gRailHandle = NULL;

static RAIL_Config_t railCfg = {  // Must never be const
  .eventsCallback = &radioEventHandler,
  .protocol = NULL,
  .scheduler = NULL,
};

static const RAIL_IEEE802154_Config_t rail154Config = {
  .addresses = NULL,
  .ackConfig = {
    // .enable = true,     // Turn on auto ACK for IEEE 802.15.4.
    .enable = false,     // Turn off auto ACK for IEEE 802.15.4.
    .ackTimeout = 672,  // See note above: 54-12 sym * 16 us/sym = 672 us.
    .rxTransitions = {
      .success = RAIL_RF_STATE_RX,  // Return to RX after ACK processing
      .error = RAIL_RF_STATE_RX,    // Ignored
    },
    .txTransitions = {
      .success = RAIL_RF_STATE_RX,  // Return to RX after ACK processing
      .error = RAIL_RF_STATE_RX,    // Ignored
    },
  },
  .timings = {
    .idleToRx = 100,
    .idleToTx = 100,
    .rxToTx = 192,    // 12 symbols * 16 us/symbol = 192 us
    .txToRx = 192,    // 12 symbols * 16 us/symbol = 192 us
    .rxSearchTimeout = 0, // Not used
    .txToRxSearchTimeout = 0, // Not used
  },
  .framesMask = (RAIL_IEEE802154_ACCEPT_STANDARD_FRAMES | RAIL_IEEE802154_ACCEPT_ACK_FRAMES),
//   .promiscuousMode = false,  // Enable format and address filtering.
  .promiscuousMode = true,  // disable format and address filtering.
  .isPanCoordinator = false,
  .defaultFramePendingInOutgoingAcks = false,
};

static RAIL_RxPacketHandle_t mainPacketHandle;

#define RADIO_TX_FIFO_SIZE      1024        //must be a multiple of 2^n
static uint8_t RadioTxFifo[RADIO_TX_FIFO_SIZE];

static uint8_t MainChannel = 11;        //default channel 11



/***************************************************************************//**
 * Private functions
 ******************************************************************************/
static void radioEventHandler(RAIL_Handle_t railHandle,
                              RAIL_Events_t events)
{
    RAIL_RxPacketHandle_t handle;

    if( (events & RAIL_EVENT_RX_PACKET_RECEIVED) == RAIL_EVENT_RX_PACKET_RECEIVED )
    {
        // Events are generally called from interrupt context
        // to avoid staying/processing in this context we just grab the packet handle
        // and tell rail to keep this packet by calling RAIL_HoldRxPacket
        // Get the handle if not already processing packets
        handle = RAIL_HoldRxPacket(railHandle);
        if ( mainPacketHandle == RAIL_RX_PACKET_HANDLE_INVALID )
        {
            // sl_led_turn_off (&sl_led_led0);
            mainPacketHandle = handle;
        }
    }

    if( (events & ( RAIL_EVENT_RX_SYNC1_DETECT | RAIL_EVENT_RX_SYNC2_DETECT)) != 0 )
    {
        // Hal_Radio_Rx_Started();
        BSP_SetLed();
    }

    if ((events & RAIL_EVENTS_RX_COMPLETION) != 0)
    {
        BSP_ClrLed();
    }
}


/***************************************************************************//**
 * Global functions
 ******************************************************************************/
/***************************************************************************//**
 * Init radio ready for command
 ******************************************************************************/
void HAL_Radio_Init( void )
{
    gRailHandle = RAIL_Init(&railCfg, NULL);

    // Configure the radio and channels for 2.4 GHz IEEE 802.15.4.
    RAIL_IEEE802154_Config2p4GHzRadio(gRailHandle);
    // Initialize the IEEE 802.15.4 configuration using the static configuration above.
    RAIL_IEEE802154_Init(gRailHandle, &rail154Config);
}


/***************************************************************************//**
 * Set radio in promiscuous mode (see 802.15.4 for details)
 ******************************************************************************/
void HAL_Radio_InitPromiscuousMode( void )
{
    RAIL_Status_t status;

//   RAIL_IEEE802154_AddrConfig_t nodeAddress = {
//     { 0xFFFF, 0xFFFF, 0xFFFF },
//     { 0xFFFF, 0xFFFF, 0xFFFF },
//     { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
//       { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
//       { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } }
//   };
//   status = RAIL_IEEE802154_SetAddresses(gRailHandle, &nodeAddress);

    //set RAIL_RX_OPTION_STORE_CRC to obtain CRC
    RAIL_ConfigRxOptions( gRailHandle, RAIL_RX_OPTION_STORE_CRC /*+ RAIL_RX_OPTION_IGNORE_CRC_ERRORS*/, RAIL_RX_OPTION_STORE_CRC /*+ RAIL_RX_OPTION_IGNORE_CRC_ERRORS*/ );

    RAIL_IEEE802154_SetPromiscuousMode(gRailHandle, true );

    RAIL_IEEE802154_AcceptFrames( gRailHandle, (RAIL_IEEE802154_ACCEPT_STANDARD_FRAMES | RAIL_IEEE802154_ACCEPT_ACK_FRAMES));

    // Configures the most useful callbacks and catches a few errors.
    RAIL_ConfigEvents(gRailHandle,
                    RAIL_EVENTS_ALL,
                    RAIL_EVENT_TX_PACKET_SENT
                    | RAIL_EVENTS_RX_COMPLETION
                    | RAIL_EVENT_RX_SYNC1_DETECT
                    | RAIL_EVENT_RX_SYNC2_DETECT);      //Detect begin of a messages

    RAIL_AntennaConfig_t antennaConfig = { 0 };
    antennaConfig.defaultPath = RAIL_ANTENNA_1;
    antennaConfig.ant1PinEn = true;
    // antennaConfig.ant1Loc =
    // antennaConfig.ant1Port =
    // antennaConfig.ant1Pin =

    status = RAIL_ConfigAntenna( gRailHandle, &antennaConfig );

    //configuration for transmission
    RAIL_SetTxFifo( gRailHandle, RadioTxFifo, 0, RADIO_TX_FIFO_SIZE );

    status = RAIL_StartRx( gRailHandle, MainChannel, NULL );

    (void) status;

}


/***************************************************************************//**
 * Process radio incoming messages
 ******************************************************************************/
HAL_Radio_GetRxPacket_Result_t  HAL_Radio_GetRxPacket( PhyRx_t * phy_rx )
{
    RAIL_RxPacketInfo_t packetInfo;
    RAIL_RxPacketDetails_t packetDetails;
    bool packet_received = false;

    if( phy_rx == NULL )
    {
        return HAL_RADIO_GET_RX_PACKET_INVALID_PARAMETER;
    }
    phy_rx->len = 0;

    if( mainPacketHandle != RAIL_RX_PACKET_HANDLE_INVALID )
    {
        RAIL_GetRxPacketInfo(gRailHandle, mainPacketHandle, &packetInfo);
        RAIL_GetRxPacketDetails(gRailHandle, mainPacketHandle, &packetDetails);

        if( (packetInfo.packetBytes < PHY_PAYLOAD_MAX + 1) && (packetInfo.packetBytes != 0) )
        {

            phy_rx->len = packetInfo.packetBytes - 1;
            // RAIL_CopyRxPacket(phy_rx.payload, &packetInfo);      //fisrt byte is the number of bytes
            RAIL_PeekRxPacket ( gRailHandle,                        //RAIL_Handle_t  	railHandle,
                                mainPacketHandle,                   //RAIL_RxPacketHandle_t  	packetHandle,
                                phy_rx->payload,                     //uint8_t *  	pDst,
                                packetInfo.packetBytes - 1,         //uint16_t  	len,
                                1 );                                //uint16_t  	offset
            phy_rx->rssi = packetDetails.rssi;
            phy_rx->lqi = packetDetails.lqi;
            phy_rx->channel = MainChannel;
            packet_received = true;
            // //RAIL_GetChannel() //to retreive channel
            // MAC_ProcessPhyRx( &phy_rx );
        }
        RAIL_ReleaseRxPacket(gRailHandle, mainPacketHandle);
        mainPacketHandle = RAIL_GetRxPacketInfo(gRailHandle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packetInfo);
    }

    if( packet_received )
    {
        return HAL_RADIO_GET_RX_PACKET_SUCCESS;
    }else{
        return HAL_RADIO_GET_RX_PACKET_NONE;
    }
}

/***************************************************************************//**
 * Select channel to use
 *
 * \param[in]   channel   11 - 26
 ******************************************************************************/
void HAL_SetRadioChannel( uint8_t channel )
{
    //Assuming radio is configured for RX operation
    RAIL_Status_t status = RAIL_StartRx( gRailHandle, channel, NULL );
    if( status == RAIL_STATUS_NO_ERROR )
    {
        MainChannel = channel;
    }
}


/***************************************************************************//**
 ******************************************************************************/
