/***************************************************************************//**
 @file console.c
  @brief   handle console messages received
           provide print output to a console port

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

******************************************************************************/

#include "console.h"
#include "Hal_Console.h"
#include "jsmn.h"
#include "string.h"
#include "stdlib.h"     //for strtol
#include "Hal.h"

/***************************************************************************//**
 * Private defines
 ******************************************************************************/
#define CONSOLE_RX_BUFFER_SIZE      16

#define MAX_JSON_RX_LENGTH          CONSOLE_RX_BUFFER_SIZE
#define MAX_JSON_RX_TOKEN           8


#define ATTRIBUT_DELIMITER				':'
#define TRAME_DELIMITER					','
#define ARRAY_START_DELIMITER           '['
#define ARRAY_END_DELIMITER             ']'
#define STRING_START_DELIMITER          '\"'
#define STRING_END_DELIMITER            '\"'

/***************************************************************************//**
 * Private types
 ******************************************************************************/
//wait for valid JSON
typedef enum {
    JSON_STEP_OPENING_BRACKET,
    JSON_STEP_CARRIAGE_RETURN,
}JSON_Steps_t;

/***************************************************************************//**
 * Local function protoypes
 ******************************************************************************/
static void console_serialize_json( uint8_t byte );
static void console_process_rx_json( uint8_t * json, uint8_t len );

/***************************************************************************//**
 * Local variables
 ******************************************************************************/
static uint8_t rxFifo[CONSOLE_RX_BUFFER_SIZE];      //Buffer of received byte for console
static uint8_t rxFifoIn = 0;                        //Head index of circular buffer
static uint8_t rxFifoOut = 0;                       //Tail index of circular buffer

static JSON_Steps_t jsonRxStep = JSON_STEP_OPENING_BRACKET;
static uint8_t jsonRxBuffer[MAX_JSON_RX_LENGTH];
static uint8_t jsonRxLength = 0;                    //Count number of bytes received

#define JSON_TX_BUFFER_SIZE  512
static uint8_t jsonTxBuffer[JSON_TX_BUFFER_SIZE];

static const uint8_t HexToAscii[] = {
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
};


/***************************************************************************//**
 * Local functions
 ******************************************************************************/
static void console_process_rx_json( uint8_t * json, uint8_t len );
static void write_json_parameter(uint8_t * buffer, uint16_t *Index, char Attribut, uint8_t* Data, uint8_t Length, const char* Format, uint8_t isInt);

/**************************************************************************//**
\brief Process received JSON
******************************************************************************/
static void console_process_rx_json( uint8_t * json, uint8_t len )
{
    jsmn_parser JsmnParser = { 0 };
    jsmntok_t JsmnTokens[MAX_JSON_RX_TOKEN];
    uint8_t i = 0;

    memset(&JsmnTokens, 0, sizeof(JsmnTokens));
    //Parse JSON payload
    jsmn_init(&JsmnParser);
    jsmn_parse(&JsmnParser, (char *) json, len, JsmnTokens, MAX_JSON_RX_TOKEN);

    //Process JSON
    i = 0;
    while (JsmnTokens[i].type != JSMN_UNDEFINED && i < MAX_JSON_RX_TOKEN)
    {
        if (JsmnTokens[i].type == JSMN_OBJECT)
        {
            i++;
        }
        //JSMN_STRING
        else if (JsmnTokens[i].type == JSMN_STRING)
        {
            switch (json[JsmnTokens[i].start])
            {
            // Channel
            //example: {"C":11}
            case 'C':
                {
                    uint8_t channel;
                    i++;
                    //channel = atoi((char *) &json[JsmnTokens[i].start]);
                    channel = strtol((char *) &json[JsmnTokens[i].start], NULL, 10);
                    if( channel >= 11 && channel <= 26 )
                    {
                        // PHY_SetChannel(channel);
                        HAL_SetRadioChannel(channel);
                    }
                }
                break;
            default:
                break;
            }
            i++;
        }
        else {
            i++;
        }
    }
}

/**************************************************************************//**
\brief Write JSON parameter to buffer
******************************************************************************/
static void write_json_parameter(uint8_t * buffer, uint16_t *Index, char Attribut, uint8_t* Data, uint8_t Length, const char* Format, uint8_t isInt)
{
	buffer[(*Index)++] = '"';
	buffer[(*Index)++] = Attribut;
	buffer[(*Index)++] = '"';
	buffer[(*Index)++] = ATTRIBUT_DELIMITER;
    if( !isInt )
    {
	    buffer[(*Index)++] = '"';
    }
	(*Index) += snprintf((char*)(buffer + (*Index)), Length, Format, Data);
    if( !isInt )
    {
	    buffer[(*Index)++] = '"';
    }
	buffer[(*Index)++] = TRAME_DELIMITER;
}


/***************************************************************************//**
 * Global functions
 ******************************************************************************/

/**************************************************************************//**
\brief Send a buffer content to console
******************************************************************************/
void Console_Init( void )
{
    rxFifoIn = 0;
    rxFifoOut = 0;
    jsonRxLength = 0;
    jsonRxStep = JSON_STEP_OPENING_BRACKET;
    HAL_Console_Init();
}

/**************************************************************************//**
\brief Send a buffer content to console
******************************************************************************/
void Console_Write( uint8_t const * buffer, uint16_t size )
{
    while( size )
    {
        HAL_Console_Tx_Byte ( *buffer++ );
        size--;
    }
}

/**************************************************************************//**
\brief Console reception handler
    Can be called from interrupt context, so keep simple
******************************************************************************/
void Console_RxByte( uint8_t byte )
{
    rxFifo[rxFifoIn] = byte;
    rxFifoIn++;
    if( rxFifoIn >= CONSOLE_RX_BUFFER_SIZE )
    {
        rxFifoIn = 0;
    }
}


/**************************************************************************//**
\brief Process uart reception
******************************************************************************/
void Console_Process_Rx( void )
{
    //Process all byte received
    while( rxFifoIn != rxFifoOut )
    {
        console_serialize_json( rxFifo[rxFifoOut] );
        rxFifoOut++;
        if( rxFifoOut >= CONSOLE_RX_BUFFER_SIZE )
        {
            rxFifoOut = 0;
        }
    }
}

/**************************************************************************//**
\brief Build received JSON buffer
******************************************************************************/
static void console_serialize_json( uint8_t byte )
{
    switch( jsonRxStep )
    {
    //ignore character until start with opening brackets
    case JSON_STEP_OPENING_BRACKET:
        if( byte == '{')
        {
            jsonRxLength = 0;
            jsonRxBuffer[jsonRxLength++] = byte;
            jsonRxStep = JSON_STEP_CARRIAGE_RETURN;
        }
        break;
    //Once opening bracket is received, wait for carriage return
    case JSON_STEP_CARRIAGE_RETURN:
        jsonRxBuffer[jsonRxLength++] = byte;
        if( byte == '\r')
        {
            console_process_rx_json( jsonRxBuffer, jsonRxLength );
            jsonRxLength = 0;
            jsonRxStep = JSON_STEP_OPENING_BRACKET;
        }
        else if( jsonRxLength >= MAX_JSON_RX_LENGTH )
        {
            //Received JSON is too large - drop it!
            jsonRxLength = 0;
            jsonRxStep = JSON_STEP_OPENING_BRACKET;
        }
        break;
    default:
        //do nothing
        break;
    }
}


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
void Console_PhyToJSONV2( PhyRx_t * phy_rx)
{
    uint16_t i = 0;

    memset(jsonTxBuffer, 0, JSON_TX_BUFFER_SIZE );

    jsonTxBuffer[i++] = '{';

    write_json_parameter(jsonTxBuffer, &i, 'L', (uint8_t *)(uint32_t)phy_rx->len, 3+1, "%d", true);
    write_json_parameter(jsonTxBuffer, &i, 'Q', (uint8_t *)(uint32_t)phy_rx->lqi, 3+1, "%d", true);
    write_json_parameter(jsonTxBuffer, &i, 'R', (uint8_t *)(uint32_t)phy_rx->rssi, 3+1, "%d", true);
    write_json_parameter(jsonTxBuffer, &i, 'C', (uint8_t *)(uint32_t)phy_rx->channel, 3+1, "%d", true);
    jsonTxBuffer[i++] = '"';
    jsonTxBuffer[i++] = 'S';
    jsonTxBuffer[i++] = '"';
    jsonTxBuffer[i++] = ATTRIBUT_DELIMITER;
    jsonTxBuffer[i++] = STRING_START_DELIMITER;
    for( uint8_t j = 0; j < phy_rx->len && i < (JSON_TX_BUFFER_SIZE-(4+4+1)); j++ )
    {
        uint8_t rx_byte = phy_rx->payload[j];
        jsonTxBuffer[i++] = HexToAscii[((rx_byte >> 4) & 0x0F)];
        jsonTxBuffer[i++] = HexToAscii[(rx_byte & 0x0F)];
    }
    jsonTxBuffer[i++] = STRING_END_DELIMITER;
    jsonTxBuffer[i++] = '}';
    jsonTxBuffer[i++] = '\n';
    jsonTxBuffer[i++] = '\r';

    Console_Write(jsonTxBuffer, i);
}


