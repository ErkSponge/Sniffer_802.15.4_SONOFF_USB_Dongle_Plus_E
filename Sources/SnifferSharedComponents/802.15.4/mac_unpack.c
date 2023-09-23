/****************************************************************************//**
  \file mac_unpack.c

  \brief Convert phy buffer to mac general frame unpacked

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
#include "mac_unpack.h"
#include "string.h"
#include "crc.h"

/******************************************************************************
                   Implementations section
******************************************************************************/
#ifndef memcpyLE
#define memcpyLE memcpy
#endif

/******************************************************************************
                   Global variables section
******************************************************************************/
/******************************************************************************
                   Local variables section
******************************************************************************/
typedef enum {
    NOT_PRESENT,
    PRESENT,
}Presence_t;

typedef struct {
    MAC_Addressing_Mode_t   destination_address;
    MAC_Addressing_Mode_t   source_address;
    Presence_t              dest_panid_present;
    Presence_t              source_panid_present;
    MAC_PANID_Compression_t PanID_compression;
}PANID_Compression_Presence_t;

#define MAC_ADDRESSING_PRESENT 0xFF

//See 802.15.4 table 7.2
//PAN ID Compression field value for frame version 0b10
static const PANID_Compression_Presence_t TABLE7_2_PANID_Compression[] =
{
    //destination_address                   //source_address                        //dest_panid_present    //source_panid_present  //PanID_compression
{   MAC_ADDRESSING_MODE_NONE,               MAC_ADDRESSING_MODE_NONE,               NOT_PRESENT,            NOT_PRESENT,            0},
{   MAC_ADDRESSING_MODE_NONE,               MAC_ADDRESSING_MODE_NONE,               PRESENT,                NOT_PRESENT,            1},
{   MAC_ADDRESSING_MODE_SHORT_ADDRESS,      MAC_ADDRESSING_MODE_NONE,               PRESENT,                NOT_PRESENT,            0},
{   MAC_ADDRESSING_MODE_EXTENDED_ADDRESS,   MAC_ADDRESSING_MODE_NONE,               PRESENT,                NOT_PRESENT,            0},
{   MAC_ADDRESSING_MODE_SHORT_ADDRESS,      MAC_ADDRESSING_MODE_NONE,               NOT_PRESENT,            NOT_PRESENT,            1},
{   MAC_ADDRESSING_MODE_EXTENDED_ADDRESS,   MAC_ADDRESSING_MODE_NONE,               NOT_PRESENT,            NOT_PRESENT,            1},
{   MAC_ADDRESSING_MODE_NONE,               MAC_ADDRESSING_MODE_SHORT_ADDRESS,      NOT_PRESENT,            PRESENT,                0},
{   MAC_ADDRESSING_MODE_NONE,               MAC_ADDRESSING_MODE_EXTENDED_ADDRESS,   NOT_PRESENT,            PRESENT,                0},
{   MAC_ADDRESSING_MODE_NONE,               MAC_ADDRESSING_MODE_SHORT_ADDRESS,      NOT_PRESENT,            NOT_PRESENT,            1},
{   MAC_ADDRESSING_MODE_NONE,               MAC_ADDRESSING_MODE_EXTENDED_ADDRESS,   NOT_PRESENT,            NOT_PRESENT,            1},
{   MAC_ADDRESSING_MODE_EXTENDED_ADDRESS,   MAC_ADDRESSING_MODE_EXTENDED_ADDRESS,   PRESENT,                NOT_PRESENT,            0},
{   MAC_ADDRESSING_MODE_EXTENDED_ADDRESS,   MAC_ADDRESSING_MODE_EXTENDED_ADDRESS,   NOT_PRESENT,            NOT_PRESENT,            1},
{   MAC_ADDRESSING_MODE_SHORT_ADDRESS,      MAC_ADDRESSING_MODE_SHORT_ADDRESS,      PRESENT,                PRESENT,                0},
{   MAC_ADDRESSING_MODE_SHORT_ADDRESS,      MAC_ADDRESSING_MODE_EXTENDED_ADDRESS,   PRESENT,                PRESENT,                0},
{   MAC_ADDRESSING_MODE_EXTENDED_ADDRESS,   MAC_ADDRESSING_MODE_SHORT_ADDRESS,      PRESENT,                PRESENT,                0},
{   MAC_ADDRESSING_MODE_SHORT_ADDRESS,      MAC_ADDRESSING_MODE_EXTENDED_ADDRESS,   PRESENT,                NOT_PRESENT,            1},
{   MAC_ADDRESSING_MODE_EXTENDED_ADDRESS,   MAC_ADDRESSING_MODE_SHORT_ADDRESS,      PRESENT,                NOT_PRESENT,            1},
{   MAC_ADDRESSING_MODE_SHORT_ADDRESS,      MAC_ADDRESSING_MODE_SHORT_ADDRESS,      PRESENT,                NOT_PRESENT,            1},
};

#define NB_OF_ELEMENT_PANID_COMPRESSION_0b10    (sizeof(TABLE7_2_PANID_Compression)/sizeof(TABLE7_2_PANID_Compression[0]))


/******************************************************************************
                   Local function section
******************************************************************************/

/******************************************************************************
                   Global function section
******************************************************************************/
/**************************************************************************//**
\brief Unpack a MAC frame
******************************************************************************/
MAC_Unpack_Result_t MAC_Unpack( MAC_Frame_packed_t * in, MAC_Frame_Unpacked_t * out)
{
    uint16_t frame_control;
    uint8_t index = 0;
    bool destination_panid_present = false;
    bool source_panid_present = false;


    if( (in == NULL) ||
        (out == NULL))
    {
        return MAC_UNPACK_PARAMETER_ERROR;
    }

    //Minimum frame size is frame control
    if( in->lenght < MHR_FRAME_CONTROL_SIZE )
    {
        return MAC_UNPACK_FRAME_SIZE_ERROR;
    }

    memset( out, 0, sizeof(MAC_Frame_Unpacked_t));

    //Extract frame control and adjust endianness to host
    frame_control = in->payload[index++];
    frame_control |= (((uint16_t) in->payload[index++]) << 8);

    //Parse frame control
    out->frame_control.frame_Type                   = ((frame_control & MHR_FRAMECONTROL_FRAME_TYPE_MSK) >> MHR_FRAMECONTROL_FRAME_TYPE_SHFT);
    out->frame_control.security_enabled             = ((frame_control & MHR_FRAMECONTROL_SECURITY_ENABLED_MSK) == MHR_FRAMECONTROL_SECURITY_ENABLED) ? MAC_SECURITY_ENABLED : MAC_SECURITY_DISABLED;
    out->frame_control.frame_pending                = ((frame_control & MHR_FRAMECONTROL_FRAME_PENDING_MSK) == MHR_FRAMECONTROL_FRAME_PENDING) ? MAC_FRAME_PENDING_DATA_PENDING : MAC_FRAME_PENDING_NONE;
    out->frame_control.acknowledge_request          = ((frame_control & MHR_FRAMECONTROL_AR_MSK) == MHR_FRAMECONTROL_AR) ? MAC_ACKNOWLEDGE_REQUEST_REQUIRED : MAC_ACKNOWLEDGE_REQUEST_NONE;
    out->frame_control.panid_compression            = ((frame_control & MHR_FRAMECONTROL_PANID_COMPRESSION_MSK) == MHR_FRAMECONTROL_PANID_COMPRESSION) ? MAC_PANID_COMPRESSION_ENABLED : MAC_PANID_COMPRESSION_DISABLED;
    out->frame_control.sequence_number_suppressed   = ((frame_control & MHR_FRAMECONTROL_SEQUENCE_NUMBER_SUPPRESSION_MSK) == MHR_FRAMECONTROL_SEQUENCE_NUMBER_SUPPRESSION) ? MAC_SEQUENCE_NUMBER_SUPPRESSION_ENABLED : MAC_SEQUENCE_NUMBER_SUPPRESSION_DISABLED;
    out->frame_control.ie_present                   = ((frame_control & MHR_FRAMECONTROL_IE_PRESENT_MSK) == MHR_FRAMECONTROL_IE_PRESENT) ? MAC_IE_PRESENT_PRESENT : MAC_IE_PRESENT_NOT_PRESENT;
    out->frame_control.destination_addressing_mode  = ((frame_control & MHR_FRAMECONTROL_DST_ADDR_MODE_MSK) >> MHR_FRAMECONTROL_DST_ADDR_MODE_SHFT);
    out->frame_control.source_addressing_mode       = ((frame_control & MHR_FRAMECONTROL_SRC_ADDR_MODE_MSK) >> MHR_FRAMECONTROL_SRC_ADDR_MODE_SHFT);
    out->frame_control.frame_version                = ((frame_control & MHR_FRAMECONTROL_FRAME_VERSION_MSK) >> MHR_FRAMECONTROL_FRAME_VERSION_SHFT);

    //Frame version 0b11 is unsupported
    if( out->frame_control.frame_version == MAC_FRAME_VERSION_11 )
    {
        return MAC_UNPACK_INVALID_FRAME_VERSION;
    }

    //Reserved, multipurpose, fragment, extended currently not supported
    switch( out->frame_control.frame_Type )
    {
        case MAC_FRAME_TYPE_RESERVED:
        case MAC_FRAME_TYPE_MULTIPURPOSE:
        case MAC_FRAME_TYPE_FRAGMENT:
        case MAC_FRAME_TYPE_EXTENDED:
            return MAC_UNPACK_UNSUPPORTED_FRAME_TYPE;
        default:
            break;
    }

    if( out->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_RESERVED )
    {
        return MAC_UNPACK_INVALID_ADDRESSING_MODE;
    }

    if( out->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_RESERVED )
    {
        return MAC_UNPACK_INVALID_ADDRESSING_MODE;
    }

    if( out->frame_control.security_enabled )
    {
        return MAC_UNPACK_UNSUPPORTED_FEATURE;
    }

    if( out->frame_control.ie_present )
    {
        return MAC_UNPACK_UNSUPPORTED_FEATURE;
    }


    //sequence number present
    if( out->frame_control.sequence_number_suppressed == 0 )
    {
        if( in->lenght < (index + 1) )
        {
            return MAC_UNPACK_FRAME_SIZE_ERROR;
        }
        out->sequence_number = in->payload[index++];
    }

    //PAN ID compression
    //Frame version 0b00 or 0b01
    if( (out->frame_control.frame_version == MAC_FRAME_VERSION_00) ||
        (out->frame_control.frame_version == MAC_FRAME_VERSION_01) )
    {
        //if destination address present
        if((out->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_SHORT_ADDRESS) ||
           (out->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_EXTENDED_ADDRESS))
        {
            destination_panid_present = true;
        }
        //if source adress present
        if((out->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_SHORT_ADDRESS) ||
           (out->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_EXTENDED_ADDRESS))
        {
            source_panid_present = true;
        }

        if( destination_panid_present && source_panid_present && out->frame_control.panid_compression )
        {
            //only destination pan id present
            source_panid_present = false;
        }
    }

    //frame version 0b10
    if( (out->frame_control.frame_version == MAC_FRAME_VERSION_10))
    {
        //applies to Beacon, Data frame, MAC Command, Ack frame
        //consider verifying if frame is one of the above
        for( uint8_t i = 0; i < NB_OF_ELEMENT_PANID_COMPRESSION_0b10; i++ )
        {
            if( ( out->frame_control.destination_addressing_mode    == TABLE7_2_PANID_Compression[i].destination_address ) &&
                ( out->frame_control.source_addressing_mode         == TABLE7_2_PANID_Compression[i].source_address ) &&
                ( out->frame_control.panid_compression              == TABLE7_2_PANID_Compression[i].PanID_compression ) )
            {
                destination_panid_present = TABLE7_2_PANID_Compression[i].dest_panid_present;
                source_panid_present = TABLE7_2_PANID_Compression[i].source_panid_present;
            }
        }
    }

    //Destination PANID
    if( destination_panid_present )
    {
        if( in->lenght < (index + 2) )
        {
            return MAC_UNPACK_FRAME_SIZE_ERROR;
        }
        out->destination_pan_id = in->payload[index++];
        out->destination_pan_id |= (((uint16_t) in->payload[index++]) << 8);
    }

    //Destination address - Short
    if( out->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_SHORT_ADDRESS )
    {
        if( in->lenght < (index + 2) )
        {
            return MAC_UNPACK_FRAME_SIZE_ERROR;
        }
        out->destination_addr.short_addr = in->payload[index++];
        out->destination_addr.short_addr |= (((uint16_t) in->payload[index++]) << 8);
    }

    //Destination address - Extended
    if( out->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_EXTENDED_ADDRESS )
    {
        if( in->lenght < (index + MAC_EXTENDED_ADDR_SIZE) )
        {
            return MAC_UNPACK_FRAME_SIZE_ERROR;
        }
        memcpyLE( out->destination_addr.ext_addr, &in->payload[index], MAC_EXTENDED_ADDR_SIZE );
        index += MAC_EXTENDED_ADDR_SIZE;
    }

    //Source PANID
    if( source_panid_present )
    {
        if( in->lenght < (index + 2) )
        {
            return MAC_UNPACK_FRAME_SIZE_ERROR;
        }
        out->source_pan_id = in->payload[index++];
        out->source_pan_id |= (((uint16_t) in->payload[index++]) << 8);
    }

    //Source address - Short
    if( out->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_SHORT_ADDRESS )
    {
       if( in->lenght < (index + 2) )
        {
            return MAC_UNPACK_FRAME_SIZE_ERROR;
        }
        out->source_addr.short_addr = in->payload[index++];
        out->source_addr.short_addr |= (((uint16_t) in->payload[index++]) << 8);
    }

    //Source address - Extended
    if( out->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_EXTENDED_ADDRESS )
    {
        if( in->lenght < (index + MAC_EXTENDED_ADDR_SIZE) )
        {
            return MAC_UNPACK_FRAME_SIZE_ERROR;
        }
        memcpyLE( out->source_addr.ext_addr, &in->payload[index], MAC_EXTENDED_ADDR_SIZE );
        index += MAC_EXTENDED_ADDR_SIZE;
    }

    //Particular cases - 7.2.1.8 - Destination Addressing Mode field
    //Particular cases - 7.2.1.10 - Source Addressing Mode field
    if( (out->frame_control.frame_version == MAC_FRAME_VERSION_00) ||
        (out->frame_control.frame_version == MAC_FRAME_VERSION_01) )
    {
        if( ( out->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_NONE ) &&
            ( ( out->frame_control.frame_Type == MAC_FRAME_TYPE_DATA) || ( out->frame_control.frame_Type == MAC_FRAME_TYPE_COMMAND ))
        )
        {
            if( out->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_NONE )
            {
                return MAC_UNPACK_INVALID_ADDRESSING_MODE;
            }

            memcpy( &out->destination_pan_id, &out->source_pan_id, sizeof( out->source_pan_id ));
        }

        if( ( out->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_NONE ) &&
            ( ( out->frame_control.frame_Type == MAC_FRAME_TYPE_DATA) || ( out->frame_control.frame_Type == MAC_FRAME_TYPE_COMMAND ))
        )
        {
            if( out->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_NONE )
            {
                return MAC_UNPACK_INVALID_ADDRESSING_MODE;
            }

            memcpy( &out->source_pan_id, &out->destination_pan_id, sizeof( out->destination_pan_id ));
        }


    }

    //Auxiliary Security - header - Not supported rejected above
    //if( out->frame_control.security_enabled )

    //IE - not supported for now
    //if( out->frame_control.ie_present )

    //copy payload content
    if( in->lenght > (index + MAC_FCS_SIZE) )
    {
        out->payload_size = in->lenght - (index + MAC_FCS_SIZE);
        if( out->payload_size > MAC_MAX_PAYLOAD )
        {
            return MAC_UNPACK_FRAME_SIZE_ERROR;
        }
        //Do not change endianness at this point
        memcpy(out->payload, &in->payload[index], out->payload_size);
        index += out->payload_size;
    }

    //FCS - 16 bit
    uint16_t crc_calc;
    uint16_t crc_rcv;

    crc_calc = crcFast( in->payload, (in->lenght - MAC_FCS_SIZE) );
    crc_rcv = in->payload[index++];
    crc_rcv |= (((uint16_t) in->payload[index++]) << 8);
    if( crc_rcv != crc_calc )
    {
        return MAC_UNPACK_FCS_DOES_NOT_MATCH;
    }
    out->fcs.fcs_2_octets = crc_rcv;

    return MAC_UNPACK_SUCCESS;
}


/**************************************************************************//**
\brief Pack a MAC frame
******************************************************************************/
MAC_Pack_Result_t MAC_Pack( MAC_Frame_Unpacked_t * in, MAC_Frame_packed_t * out)
{
    uint16_t frame_control;
    uint8_t index = 0;
    bool destination_panid_present = false;
    bool source_panid_present = false;

    if( in == NULL || out == NULL )
    {
        return MAC_PACK_PARAMETER_ERROR;
    }

    //Clear output frame
    memset( out, 0, sizeof(MAC_Frame_packed_t));

    //Frame version 0b11 is unsupported
    if( in->frame_control.frame_version == MAC_FRAME_VERSION_11 )
    {
        return MAC_PACK_INVALID_FRAME_VERSION;
    }

    //Reserved, multipurpose, fragment, extended currently not supported
    switch( in->frame_control.frame_Type )
    {
        case MAC_FRAME_TYPE_RESERVED:
        case MAC_FRAME_TYPE_MULTIPURPOSE:
        case MAC_FRAME_TYPE_FRAGMENT:
        case MAC_FRAME_TYPE_EXTENDED:
            return MAC_PACK_UNSUPPORTED_FRAME_TYPE;
        default:
            break;
    }

    if( in->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_RESERVED )
    {
        return MAC_PACK_INVALID_ADDRESSING_MODE;
    }

    if( in->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_RESERVED )
    {
        return MAC_PACK_INVALID_ADDRESSING_MODE;
    }

    if( in->frame_control.security_enabled )
    {
        return MAC_PACK_UNSUPPORTED_FEATURE;
    }

    if( in->frame_control.ie_present )
    {
        return MAC_PACK_UNSUPPORTED_FEATURE;
    }

    //Fill frame control
    frame_control  = ((in->frame_control.frame_Type << MHR_FRAMECONTROL_FRAME_TYPE_SHFT) & MHR_FRAMECONTROL_FRAME_TYPE_MSK);
    frame_control |= ((in->frame_control.security_enabled == MAC_SECURITY_DISABLED) ? 0 : MHR_FRAMECONTROL_SECURITY_ENABLED);
    frame_control |= ((in->frame_control.frame_pending == MAC_FRAME_PENDING_NONE) ? 0 : MHR_FRAMECONTROL_FRAME_PENDING);
    frame_control |= ((in->frame_control.acknowledge_request == MAC_ACKNOWLEDGE_REQUEST_NONE) ? 0 : MHR_FRAMECONTROL_AR);
    frame_control |= ((in->frame_control.panid_compression == MAC_PANID_COMPRESSION_DISABLED) ? 0 : MHR_FRAMECONTROL_PANID_COMPRESSION);
    frame_control |= ((in->frame_control.sequence_number_suppressed == MAC_SEQUENCE_NUMBER_SUPPRESSION_DISABLED) ? 0 : MHR_FRAMECONTROL_SEQUENCE_NUMBER_SUPPRESSION);
    frame_control |= ((in->frame_control.ie_present == MAC_IE_PRESENT_NOT_PRESENT) ? 0 : MHR_FRAMECONTROL_IE_PRESENT);
    frame_control |= ((in->frame_control.destination_addressing_mode << MHR_FRAMECONTROL_DST_ADDR_MODE_SHFT) & MHR_FRAMECONTROL_DST_ADDR_MODE_MSK);
    frame_control |= (((uint16_t) in->frame_control.source_addressing_mode << MHR_FRAMECONTROL_SRC_ADDR_MODE_SHFT) & MHR_FRAMECONTROL_SRC_ADDR_MODE_MSK);
    frame_control |= (((uint16_t) in->frame_control.frame_version << MHR_FRAMECONTROL_FRAME_VERSION_SHFT) & MHR_FRAMECONTROL_FRAME_VERSION_MSK);

    out->payload[index++] = (uint8_t) (frame_control & 0xFF);
    out->payload[index++] = (uint8_t) ((uint16_t) (frame_control & 0xFF00) >> 8);


    //sequence number present
    if( in->frame_control.sequence_number_suppressed == 0 )
    {
        out->payload[index++] = in->sequence_number;
    }

    //Addressing fields - depends on frame version
    if( (in->frame_control.frame_version == MAC_FRAME_VERSION_00) ||
        (in->frame_control.frame_version == MAC_FRAME_VERSION_01) )
    {
        //destination address
        if( ( in->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_SHORT_ADDRESS ) ||
            ( in->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_EXTENDED_ADDRESS ))
        {
            destination_panid_present = true;
        }

        //source address
        if( ( in->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_SHORT_ADDRESS ) ||
            ( in->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_EXTENDED_ADDRESS ))
        {
            source_panid_present = true;
            if( in->frame_control.panid_compression != MAC_PANID_COMPRESSION_ENABLED )  //consider evaluate this before and setting interrnally the panid compression field
            {
                source_panid_present = false;
            }
        }
    }

    //frame version 0b10
    if( (in->frame_control.frame_version == MAC_FRAME_VERSION_10))
    {
        //applies to Beacon, Data frame, MAC Command, Ack frame
        if( (in->frame_control.frame_Type == MAC_FRAME_TYPE_BEACON) ||
            (in->frame_control.frame_Type == MAC_FRAME_TYPE_DATA) ||
            (in->frame_control.frame_Type == MAC_FRAME_TYPE_ACK) ||
            (in->frame_control.frame_Type == MAC_FRAME_TYPE_COMMAND) )
        {
            for( uint8_t i = 0; i < NB_OF_ELEMENT_PANID_COMPRESSION_0b10; i++ )
            {
                if( ( in->frame_control.destination_addressing_mode    == TABLE7_2_PANID_Compression[i].destination_address ) &&
                    ( in->frame_control.source_addressing_mode         == TABLE7_2_PANID_Compression[i].source_address ) &&
                    ( in->frame_control.panid_compression              == TABLE7_2_PANID_Compression[i].PanID_compression ) )
                {
                    destination_panid_present = TABLE7_2_PANID_Compression[i].dest_panid_present;
                    source_panid_present = TABLE7_2_PANID_Compression[i].source_panid_present;
                }
            }
        }
    }

    //Copy destination PANID if present
    if( destination_panid_present )
    {
        //Copy destination pan id
        out->payload[index++] = (uint8_t) (in->destination_pan_id & 0xFF);
        out->payload[index++] = (uint8_t) (((uint16_t) in->destination_pan_id & 0xFF00) >> 8);
    }

    //Copy destination short address
    if( in->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_SHORT_ADDRESS )
    {
        //Copy short address
        out->payload[index++] = (uint8_t) (in->destination_addr.short_addr & 0xFF);
        out->payload[index++] = (uint8_t) (((uint16_t) in->destination_addr.short_addr & 0xFF00) >> 8);
    }
    //Copy destination extended address
    else if( in->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_EXTENDED_ADDRESS )
    {
        //Copy long address
        memcpyLE( &out->payload[index], in->destination_addr.ext_addr, MAC_EXTENDED_ADDR_SIZE );
        index += MAC_EXTENDED_ADDR_SIZE;
    }
    else
    {}      //intentionally blank


    //Copy source PANID if present
    if( source_panid_present )
    {
        //Copy source pan id
        out->payload[index++] = (uint8_t) (in->source_pan_id & 0xFF);
        out->payload[index++] = (uint8_t) (((uint16_t) in->source_pan_id & 0xFF00) >> 8);
    }

    if( in->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_SHORT_ADDRESS )
    {
        //Copy short address
        out->payload[index++] = (uint8_t) (in->source_addr.short_addr & 0xFF);
        out->payload[index++] = (uint8_t) (((uint16_t) in->source_addr.short_addr & 0xFF00) >> 8);
    }
    else if( in->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_EXTENDED_ADDRESS )
    {
        //Copy long address
        memcpyLE( &out->payload[index], in->source_addr.ext_addr, MAC_EXTENDED_ADDR_SIZE );
        index += MAC_EXTENDED_ADDR_SIZE;
    }
    else
    {}      //intentionally blank

    //copy payload content
    uint8_t payload_size = in->payload_size;
    if( (payload_size + index + MAC_FCS_SIZE) >= MAC_MAX_PAYLOAD )
    {
        return MAC_PACK_FRAME_SIZE_ERROR;
    }

    //No endianss handling at payload level
    memcpy( &out->payload[index], in->payload, payload_size );
    index += payload_size;

    //FCS - 16 bit
    uint16_t crc_calc;
    crc_calc = crcFast( out->payload, index );
    out->payload[index++] =  (crc_calc & 0xFF);
    out->payload[index++] = ((crc_calc & 0xFF00) >> 8);

    out->lenght = index;

    return MAC_PACK_SUCCESS;

    // //PAN ID compression - to be done prior to call this function
    // //Frame version 0b00 or 0b01
    // if( (in->frame_control.frame_version == MAC_FRAME_VERSION_00) ||
    //     (in->frame_control.frame_version == MAC_FRAME_VERSION_01) )
    // {
    //     //if destination address present
    //     if((in->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_SHORT_ADDRESS) ||
    //        (in->frame_control.destination_addressing_mode == MAC_ADDRESSING_MODE_EXTENDED_ADDRESS))
    //     {
    //         destination_panid_present = true;
    //     }
    //     //if source adress present
    //     if((in->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_SHORT_ADDRESS) ||
    //        (in->frame_control.source_addressing_mode == MAC_ADDRESSING_MODE_EXTENDED_ADDRESS))
    //     {
    //         source_panid_present = true;
    //     }
    //     //if both addresses information present
    //     if( destination_panid_present && source_panid_present )
    //     {
    //         //if same PANID
    //         if( in->destination_pan_id == in->source_pan_id )
    //         {
    //         }
    //         //only destination pan id present
    //         source_panid_present = false;
    //     }
    // }

}



///////////////////////////////////////////////////////////////////////////////
// Unit test for MAC
///////////////////////////////////////////////////////////////////////////////
#ifdef UNIT_TEST_MAC_UNPACK
///////////////////////////////////////////////////////////////////////////////
// Unit test for MAC unpack
///////////////////////////////////////////////////////////////////////////////
static const uint8_t MAC_Unpack_MTO[] = {   0x41, 0x88, 0x00, 0x1e, 0x48, 0xff, 0xff, 0x00, 0x00, 0x09, 0x12, 0xfc, 0xff, 0x00, 0x00, 0x1e,
                                            0x3d, 0x08, 0x85, 0xda, 0xfe, 0xff, 0xd7, 0x6b, 0x08, 0x28, 0x82, 0xd9, 0x9e, 0x00, 0x08, 0x85,
                                            0xda, 0xfe, 0xff, 0xd7, 0x6b, 0x08, 0x00, 0x2b, 0x58, 0xaf, 0xd8, 0x35, 0x52, 0xdd, 0x16, 0x76,
                                            0x4e, 0xc7, 0xdb };

uint8_t MAC_Unpack_UnitTest1( void )
{
    MAC_Unpack_Result_t result;
    MAC_Frame_packed_t in;
    MAC_Frame_Unpacked_t out;

    in.lenght = sizeof(MAC_Unpack_MTO);
    memcpy( in.payload, MAC_Unpack_MTO, in.lenght );

    result = MAC_Unpack( &in, &out );

    if( result == 0 )
    {
        __NOP();
    }

    return true;

}

#endif //UNIT_TEST_MAC_UNPACK



// eof mac_unpack.c

