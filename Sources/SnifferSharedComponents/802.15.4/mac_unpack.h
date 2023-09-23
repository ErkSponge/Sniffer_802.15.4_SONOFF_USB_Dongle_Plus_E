/****************************************************************************//**
  \file mac_unpack.h

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
#ifndef _MAC_UNPACK_H
#define _MAC_UNPACK_H

/******************************************************************************
                    Includes section
******************************************************************************/
#include "stdint.h"
#include "stdbool.h"

/******************************************************************************
                   Define(s) section
******************************************************************************/
//Define the maximum payload a MAC frame can support - actually lower than 128
#define     MAC_MAX_PAYLOAD         (128)

#define     MAC_FRAME_MAX_SIZE      (MAC_MAX_PAYLOAD + 0)

#define     MAC_EXTENDED_ADDR_SIZE  (8)

#define     MAC_FCS_SIZE            (2)             //uint16_t

/******************************************************************************
                   Types section
******************************************************************************/
typedef union {
    uint8_t ext_addr[MAC_EXTENDED_ADDR_SIZE];
    uint64_t long_addr;
    uint16_t short_addr;
}MAC_Addr_t;

//MAC - Frame Control
#define MHR_FRAME_CONTROL_SIZE                          (2)

//MAC - Frame Control - Frame Type - bit0 - bit2
#define MHR_FRAMECONTROL_FRAME_TYPE_MSK                 (0x07)
#define MHR_FRAMECONTROL_FRAME_TYPE_SHFT                (0x00)
#define MHR_FRAMECONTROL_FRAME_TYPE_BEACON              (0x00)
#define MHR_FRAMECONTROL_FRAME_TYPE_DATA                (0x01)
#define MHR_FRAMECONTROL_FRAME_TYPE_ACK                 (0x02)
#define MHR_FRAMECONTROL_FRAME_TYPE_MAC_COMMAND         (0x03)
#define MHR_FRAMECONTROL_FRAME_TYPE_RESERVED            (0x04)
#define MHR_FRAMECONTROL_FRAME_TYPE_MULTIPURPOSE        (0x05)
#define MHR_FRAMECONTROL_FRAME_TYPE_FRAGMENT_OR_FRAK    (0x06)
#define MHR_FRAMECONTROL_FRAME_TYPE_EXTENDED            (0x07)

//MAC - Frame Control - Security Enabled - bit 3
#define MHR_FRAMECONTROL_SECURITY_ENABLED_SHFT          (3)
#define MHR_FRAMECONTROL_SECURITY_ENABLED_MSK           ((0X01) << MHR_FRAMECONTROL_SECURITY_ENABLED_SHFT)
#define MHR_FRAMECONTROL_SECURITY_ENABLED               ((0X01) << MHR_FRAMECONTROL_SECURITY_ENABLED_SHFT)

//MAC - Frame Control - Frame Pending - bit 4
#define MHR_FRAMECONTROL_FRAME_PENDING_SHFT             (4)
#define MHR_FRAMECONTROL_FRAME_PENDING_MSK              ((0X01) << MHR_FRAMECONTROL_FRAME_PENDING_SHFT)
#define MHR_FRAMECONTROL_FRAME_PENDING                  ((0X01) << MHR_FRAMECONTROL_FRAME_PENDING_SHFT)

//MAC - Frame Control - AR (acknowledge Request) - bit 5
#define MHR_FRAMECONTROL_AR_SHFT                        (5)
#define MHR_FRAMECONTROL_AR_MSK                         ((0X01) << MHR_FRAMECONTROL_AR_SHFT)
#define MHR_FRAMECONTROL_AR                             ((0X01) << MHR_FRAMECONTROL_AR_SHFT)

//MAC - Frame Control - PAN ID Compression - bit 6
#define MHR_FRAMECONTROL_PANID_COMPRESSION_SHFT         (6)
#define MHR_FRAMECONTROL_PANID_COMPRESSION_MSK          ((0x01) << MHR_FRAMECONTROL_PANID_COMPRESSION_SHFT)
#define MHR_FRAMECONTROL_PANID_COMPRESSION              ((0x01) << MHR_FRAMECONTROL_PANID_COMPRESSION_SHFT)

//MAC - Frame Control - Reserved - bit 7
#define MHR_FRAMECONTROL_RESERVED_BIT7_SHFT             (7)
#define MHR_FRAMECONTROL_RESERVED_BIT7_MSK              ((0x01) << MHR_FRAMECONTROL_RESERVED_BIT7_SHFT)
#define MHR_FRAMECONTROL_RESERVED_BIT7                  ((0x01) << MHR_FRAMECONTROL_RESERVED_BIT7_SHFT)

//MAC - Frame Control - Sequence Number Suppression - bit 8
#define MHR_FRAMECONTROL_SEQUENCE_NUMBER_SUPPRESSION_SHFT   (8)
#define MHR_FRAMECONTROL_SEQUENCE_NUMBER_SUPPRESSION_MSK    ((0x01) << MHR_FRAMECONTROL_SEQUENCE_NUMBER_SUPPRESSION_SHFT)
#define MHR_FRAMECONTROL_SEQUENCE_NUMBER_SUPPRESSION        ((0x01) << MHR_FRAMECONTROL_SEQUENCE_NUMBER_SUPPRESSION_SHFT)

//MAC - Frame Control - IE Present - bit 9
#define MHR_FRAMECONTROL_IE_PRESENT_SHFT                (9)
#define MHR_FRAMECONTROL_IE_PRESENT_MSK                 ((0x01) << MHR_FRAMECONTROL_IE_PRESENT_SHFT)
#define MHR_FRAMECONTROL_IE_PRESENT                     ((0x01) << MHR_FRAMECONTROL_IE_PRESENT_SHFT)

//MAC - Frame Control - Destination Addressing mode - bit 10-11
#define MHR_FRAMECONTROL_DST_ADDR_MODE_SHFT             (10)
#define MHR_FRAMECONTROL_DST_ADDR_MODE_MSK              ((0x03) << MHR_FRAMECONTROL_DST_ADDR_MODE_SHFT)
#define MHR_FRAMECONTROL_ADDR_MODE_NOT_PRESENT_Val      (0x00UL)
#define MHR_FRAMECONTROL_ADDR_MODE_RESERVED_Val         (0x01UL)
#define MHR_FRAMECONTROL_ADDR_MODE_SHORT_ADDR_Val       (0x02UL)
#define MHR_FRAMECONTROL_ADDR_MODE_EXT_ADDR_Val         (0x03UL)
#define MHR_FRAMECONTROL_DST_ADDR_MODE_NOT_PRESENT      ((MHR_FRAMECONTROL_ADDR_MODE_NOT_PRESENT_Val) << MHR_FRAMECONTROL_DST_ADDR_MODE_SHFT)
#define MHR_FRAMECONTROL_DST_ADDR_MODE_RESERVED         ((MHR_FRAMECONTROL_ADDR_MODE_RESERVED_Val)    << MHR_FRAMECONTROL_DST_ADDR_MODE_SHFT)
#define MHR_FRAMECONTROL_DST_ADDR_MODE_SHORT_ADDR       ((MHR_FRAMECONTROL_ADDR_MODE_SHORT_ADDR_Val)  << MHR_FRAMECONTROL_DST_ADDR_MODE_SHFT)
#define MHR_FRAMECONTROL_DST_ADDR_MODE_EXT_ADDR         ((MHR_FRAMECONTROL_ADDR_MODE_EXT_ADDR_Val)    << MHR_FRAMECONTROL_DST_ADDR_MODE_SHFT)

//MAC - Frame Control - Frame Version - bit 12-13
#define MHR_FRAMECONTROL_FRAME_VERSION_SHFT             (12)
#define MHR_FRAMECONTROL_FRAME_VERSION_MSK              ((0x03) << MHR_FRAMECONTROL_FRAME_VERSION_SHFT)

//MAC - Frame Control - Source Addressing mode - bit 14-15
#define MHR_FRAMECONTROL_SRC_ADDR_MODE_SHFT             (14)
#define MHR_FRAMECONTROL_SRC_ADDR_MODE_MSK              ((0x03) << MHR_FRAMECONTROL_SRC_ADDR_MODE_SHFT)
#define MHR_FRAMECONTROL_SRC_ADDR_MODE_NOT_PRESENT      ((MHR_FRAMECONTROL_ADDR_MODE_NOT_PRESENT_Val) << MHR_FRAMECONTROL_SRC_ADDR_MODE_SHFT)
#define MHR_FRAMECONTROL_SRC_ADDR_MODE_RESERVED         ((MHR_FRAMECONTROL_ADDR_MODE_RESERVED_Val)    << MHR_FRAMECONTROL_SRC_ADDR_MODE_SHFT)
#define MHR_FRAMECONTROL_SRC_ADDR_MODE_SHORT_ADDR       ((MHR_FRAMECONTROL_ADDR_MODE_SHORT_ADDR_Val)  << MHR_FRAMECONTROL_SRC_ADDR_MODE_SHFT)
#define MHR_FRAMECONTROL_SRC_ADDR_MODE_EXT_ADDR         ((MHR_FRAMECONTROL_ADDR_MODE_EXT_ADDR_Val)    << MHR_FRAMECONTROL_SRC_ADDR_MODE_SHFT)


typedef enum {
    MAC_FRAME_TYPE_BEACON,
    MAC_FRAME_TYPE_DATA,
    MAC_FRAME_TYPE_ACK,
    MAC_FRAME_TYPE_COMMAND,
    MAC_FRAME_TYPE_RESERVED,
    MAC_FRAME_TYPE_MULTIPURPOSE,
    MAC_FRAME_TYPE_FRAGMENT,
    MAC_FRAME_TYPE_EXTENDED,
}MAC_Frame_Type_t;

typedef enum {
    MAC_SECURITY_DISABLED,
    MAC_SECURITY_ENABLED
}MAC_Security_Enabled_t;

typedef enum {
    MAC_FRAME_PENDING_NONE,
    MAC_FRAME_PENDING_DATA_PENDING
}MAC_Frame_Pending_t;

typedef enum {
    MAC_ACKNOWLEDGE_REQUEST_NONE,
    MAC_ACKNOWLEDGE_REQUEST_REQUIRED
}MAC_Acknowledge_Request_t;

typedef enum {
    MAC_PANID_COMPRESSION_DISABLED,
    MAC_PANID_COMPRESSION_ENABLED
}MAC_PANID_Compression_t;

typedef enum {
    MAC_SEQUENCE_NUMBER_SUPPRESSION_DISABLED,       //Sequence number present
    MAC_SEQUENCE_NUMBER_SUPPRESSION_ENABLED         //Sequence number suppressed
}MAC_Sequene_Number_Suppression_t;

typedef enum {
    MAC_IE_PRESENT_NOT_PRESENT,
    MAC_IE_PRESENT_PRESENT
}MAC_IE_Present_t;

typedef enum {
    MAC_ADDRESSING_MODE_NONE,
    MAC_ADDRESSING_MODE_RESERVED,
    MAC_ADDRESSING_MODE_SHORT_ADDRESS,
    MAC_ADDRESSING_MODE_EXTENDED_ADDRESS
}MAC_Addressing_Mode_t;

typedef enum {
    MAC_FRAME_VERSION_00,
    MAC_FRAME_VERSION_01,
    MAC_FRAME_VERSION_10,
    MAC_FRAME_VERSION_11
}MAC_Frame_Version_t;

//Frame control - Unpack
typedef struct {
    MAC_Frame_Type_t            frame_Type;
    MAC_Security_Enabled_t      security_enabled;
    MAC_Frame_Pending_t         frame_pending;
    MAC_Acknowledge_Request_t   acknowledge_request;
    MAC_PANID_Compression_t     panid_compression;
    MAC_Sequene_Number_Suppression_t    sequence_number_suppressed;
    MAC_IE_Present_t            ie_present;
    MAC_Addressing_Mode_t       destination_addressing_mode;
    MAC_Frame_Version_t         frame_version;
    MAC_Addressing_Mode_t       source_addressing_mode;
}MAC_Frame_Control_t;

typedef union {
    uint16_t    fcs_2_octets;
    uint32_t    fcs_4_octets;
}MAC_FCS_t;


//MAC frame - umpack
typedef struct {
    MAC_Frame_Control_t     frame_control;
    uint8_t                 sequence_number;
    uint16_t                destination_pan_id;
    MAC_Addr_t              destination_addr;
    uint16_t                source_pan_id;
    MAC_Addr_t              source_addr;
    // MAC_Security_Header_t   auxiliary_security_header;      //not suppoorted for now
    // MAC_IE_Header_t         ie_header;                      //not suppoorted for now
    // uint8_t                 ie_payload[];                   //not suppoorted for now
    uint8_t                 payload[MAC_MAX_PAYLOAD];
    uint8_t                 payload_size;
    MAC_FCS_t               fcs;
}MAC_Frame_Unpacked_t;


typedef struct {
    uint8_t lenght;
    uint8_t payload[MAC_FRAME_MAX_SIZE];
}MAC_Frame_packed_t;

typedef enum {
    MAC_UNPACK_SUCCESS,
    MAC_UNPACK_PARAMETER_ERROR,
    MAC_UNPACK_FRAME_SIZE_ERROR,
    MAC_UNPACK_UNSUPPORTED_FEATURE,
    MAC_UNPACK_INVALID_ADDRESSING_MODE,
    MAC_UNPACK_INVALID_FRAME_VERSION,
    MAC_UNPACK_UNSUPPORTED_FRAME_TYPE,
    MAC_UNPACK_FCS_DOES_NOT_MATCH,
}MAC_Unpack_Result_t;


typedef enum {
    MAC_PACK_SUCCESS,
    MAC_PACK_PARAMETER_ERROR,
    MAC_PACK_FRAME_SIZE_ERROR,
    MAC_PACK_UNSUPPORTED_FEATURE,
    MAC_PACK_INVALID_ADDRESSING_MODE,
    MAC_PACK_INVALID_FRAME_VERSION,
    MAC_PACK_UNSUPPORTED_FRAME_TYPE,
    MAC_PACK_FCS_DOES_NOT_MATCH,
}MAC_Pack_Result_t;



/******************************************************************************
                   Global variables section
******************************************************************************/
/******************************************************************************
                   Prototypes section
******************************************************************************/
MAC_Unpack_Result_t MAC_Unpack( MAC_Frame_packed_t * in, MAC_Frame_Unpacked_t * out);

MAC_Pack_Result_t MAC_Pack( MAC_Frame_Unpacked_t * in, MAC_Frame_packed_t * out);



///////////////////////////////////////////////////////////////////////////////
//  Unit tests
///////////////////////////////////////////////////////////////////////////////
uint8_t MAC_Unpack_UnitTest1( void );

#endif // _MAC_UNPACK_H
