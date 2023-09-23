/****************************************************************************//**
  \file mac.c

  \brief mac layer implementation

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
#include "mac.h"
#include "mac_unpack.h"
#include "console.h"
#include "phy.h"
#include "string.h"

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
\brief Init MAC layer
******************************************************************************/
void MAC_Init( void )
{
}

/**************************************************************************//**
\brief MAC reception function
******************************************************************************/
void MAC_ProcessPhyRx( PhyRx_t * phy_rx)
{
    //unpack frame for processing
    MAC_Frame_packed_t in;
    MAC_Frame_Unpacked_t out;
    MAC_Unpack_Result_t result;

    if( phy_rx == NULL )
    {
        return;
    }

    // Console_Print("RX msg\n\r");
    // // uint8_t len;
    // // uint8_t payload[PHY_PAYLOAD_MAX];
    // // uint8_t lqi;
    // // uint8_t rssi;

    // Console_Print("len: %d\n\r", phy_rx->len);
    // Console_Print("lqi: %d\n\r", phy_rx->lqi);
    // Console_Print("rssi: %d\n\r", phy_rx->rssi);

    // for( uint8_t i = 0; i < phy_rx->len; i++ ){
    //     if( (i % 16) == 0 && i != 0 ){
    //         Console_Print("\n\r");
    //     }
    //     Console_Print("%02x ", phy_rx->payload[i]);
    // }
    // Console_Print("\n\r");


    //Callback used to process any incoming frame
    //if returns true, further processing is discarded
    if( Mac_RxMsgCallbackPreprocessPhyRx( phy_rx ) )
    {
        return;
    }

    in.lenght = phy_rx->len;
    memcpy( in.payload, phy_rx->payload, ( in.lenght < PHY_PAYLOAD_MAX ) ? in.lenght :  PHY_PAYLOAD_MAX );
    result = MAC_Unpack( &in, &out );
    if( result != MAC_UNPACK_SUCCESS )
    {
        //error unpacking
        return;
    }

    //Call pre-processing callback with unpacked frame
    //Pass a pointer to received unpacked message
    //if returns true, further processing is discarded
    if( Mac_RxMsgCallbackPreprocessMacUnpacked( &out ) )
    {
        return;
    }

    switch( out.frame_control.frame_Type )
    {
        case MAC_FRAME_TYPE_BEACON:
        case MAC_FRAME_TYPE_DATA:
        case MAC_FRAME_TYPE_ACK:
        case MAC_FRAME_TYPE_COMMAND:
        case MAC_FRAME_TYPE_RESERVED:
        case MAC_FRAME_TYPE_MULTIPURPOSE:
        case MAC_FRAME_TYPE_FRAGMENT:
        case MAC_FRAME_TYPE_EXTENDED:
            break;
    }

    //Call post-processing callback with unpacked frame
    //Pass a pointer to received unpacked message
    //if returns true, further processing is discarded
    //Typical usage is to notify upper layer of received message
    if( Mac_RxMsgCallbackPostprocessMacUnpacked( &out) )
    {
        return;
    }
}


/**************************************************************************//**
\brief Get next MAC sequence number
******************************************************************************/
uint8_t MAC_GetNextSequenceNumber( void )
{
    static uint8_t sequence_number;
    return sequence_number++;
}


/**************************************************************************//**
\brief Send Beacon Request
******************************************************************************/
int MAC_Tx_BeaconRequest( void )
{
    MAC_Frame_Unpacked_t frame;
    MAC_Frame_packed_t frame_packed;
    memset( &frame, 0, sizeof(frame) );

    frame.frame_control.acknowledge_request         = MAC_ACKNOWLEDGE_REQUEST_NONE;
    frame.frame_control.destination_addressing_mode = MAC_ADDRESSING_MODE_SHORT_ADDRESS;
    frame.frame_control.frame_pending               = MAC_FRAME_PENDING_NONE;
    frame.frame_control.frame_Type                  = MAC_FRAME_TYPE_COMMAND;
    frame.frame_control.frame_version               = MAC_FRAME_VERSION_00;
    frame.frame_control.ie_present                  = MAC_IE_PRESENT_NOT_PRESENT;
    frame.frame_control.panid_compression           = MAC_PANID_COMPRESSION_DISABLED;
    frame.frame_control.security_enabled            = MAC_SECURITY_DISABLED;
    frame.frame_control.sequence_number_suppressed  = MAC_SEQUENCE_NUMBER_SUPPRESSION_DISABLED;
    frame.frame_control.source_addressing_mode      = MAC_ADDRESSING_MODE_NONE;

    frame.destination_addr.short_addr = MAC_BROADCAST_ALL;
    frame.destination_pan_id = MAC_BROADCAST_ALL;

    frame.payload_size = 1;
    frame.payload[0] = MAC_CMD_BEACON_REQUEST;
    frame.sequence_number = MAC_GetNextSequenceNumber();

    if( MAC_PACK_SUCCESS != MAC_Pack( &frame, &frame_packed ))
    {
        return -1;
    }

    return( PHY_SendFrame( frame_packed.payload, frame_packed.lenght ));
}

/**************************************************************************//**
\brief Send Association Request
******************************************************************************/
int MAC_TxAssociationRequest( uint16_t dest_pan, uint16_t dest_shortID )
{
    MAC_Frame_Unpacked_t frame;
    MAC_Frame_packed_t frame_packed;
    memset( &frame, 0, sizeof(frame) );

    frame.frame_control.acknowledge_request            = MAC_ACKNOWLEDGE_REQUEST_REQUIRED;
    frame.frame_control.destination_addressing_mode    = MAC_ADDRESSING_MODE_SHORT_ADDRESS;
    frame.frame_control.frame_pending                  = MAC_FRAME_PENDING_NONE;
    frame.frame_control.frame_Type                     = MAC_FRAME_TYPE_COMMAND;
    frame.frame_control.frame_version                  = MAC_FRAME_VERSION_00;
    frame.frame_control.ie_present                     = MAC_IE_PRESENT_NOT_PRESENT;
    frame.frame_control.panid_compression              = MAC_PANID_COMPRESSION_DISABLED;
    frame.frame_control.security_enabled               = MAC_SECURITY_DISABLED;
    frame.frame_control.sequence_number_suppressed     = MAC_SEQUENCE_NUMBER_SUPPRESSION_DISABLED;
    frame.frame_control.source_addressing_mode         = MAC_ADDRESSING_MODE_EXTENDED_ADDRESS;

    frame.frame_control.destination_addressing_mode     = MAC_ADDRESSING_MODE_SHORT_ADDRESS;
    frame.destination_addr.short_addr                   = dest_shortID;
    frame.destination_pan_id                            = dest_pan;

    frame.frame_control.source_addressing_mode          = MAC_ADDRESSING_MODE_EXTENDED_ADDRESS;
    frame.source_addr.long_addr                         = 0xCAFEBABEDEADBEEF;
    // frame.source_addr.long_addr                         = GetOwnExtendedAddress();

    frame.payload_size = 1;
    frame.payload[0] = MAC_CMD_ASSOCIATION_REQUEST;
    frame.sequence_number = MAC_GetNextSequenceNumber();

    if( MAC_PACK_SUCCESS != MAC_Pack( &frame, &frame_packed ))
    {
        return -1;
    }

    return (PHY_SendFrame( frame_packed.payload, frame_packed.lenght ));
}

/**************************************************************************//**
\brief callback for MAC pre message process
This callback is raised upon reception of any frame from phy before any
process is done at the MAC layer
returning true will end frame process
returning false will continue with regular frame processing
******************************************************************************/
bool __attribute__((weak)) Mac_RxMsgCallbackPreprocessPhyRx( PhyRx_t * phy_rx )
{
    (void) phy_rx;
    return false;
}

/**************************************************************************//**
\brief callback for MAC pre message process with unpacked data
This callback is raised upon reception of frame that was susccessfully unpacked
returning true will end frame process
returning false will continue with regular frame processing
******************************************************************************/
bool __attribute__((weak)) Mac_RxMsgCallbackPreprocessMacUnpacked( MAC_Frame_Unpacked_t * out )
{
    (void) out;
    return false;
}

/**************************************************************************//**
\brief callback for MAC post message process with unpacked data
This callback is raised after mac layer unpacked and processed a received frame
It is intended to indicate to upper layer the reception of a valid frame
returning true will end frame process
returning false will continue with regular frame processing
******************************************************************************/
bool __attribute__((weak)) Mac_RxMsgCallbackPostprocessMacUnpacked( MAC_Frame_Unpacked_t * out )
{
    (void) out;
    return false;
}




// ///////////////////////////////////////////////////////////////////////////////
// // Unit test for MAC
// ///////////////////////////////////////////////////////////////////////////////


// eof mac.c

