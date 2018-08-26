/*
 * Copyright 2018.
 * All rights reserved.
 *
 * This file is part sAPI library for microcontrollers.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* Date: 2017-14-11 */

/*==================[inlcusiones]============================================*/

#include "sapi.h"
#include <string.h>
#include <math.h>
#include "adafruit_fingerprint.h"

/*==================[definiciones y macros]==================================*/

//------------------------------------------------------------------------

/*==================[definiciones de datos internos]=========================*/
//static Adafruit_Fingerprint_Packet packet;
//static Adafruit_Fingerprint_Packet packet_in;

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/
void writeStructuredPacket(const Adafruit_Fingerprint_Packet *packet) {
  uartWriteByte( UART_WIFFI,packet->start_code >>8 );
  uartWriteByte( UART_WIFFI,packet->start_code & 0XFF );
  uartWriteByte( UART_WIFFI,packet->address[0] );
  uartWriteByte( UART_WIFFI,packet->address[1] );
  uartWriteByte( UART_WIFFI,packet->address[2] );
  uartWriteByte( UART_WIFFI,packet->address[3] );
  uartWriteByte( UART_WIFFI,packet->type );

  uint16_t wire_length = packet->length+2;//se suma 2 por la presencia de los bytes del checksum

  uartWriteByte( UART_WIFFI,wire_length >> 8 );
  uartWriteByte( UART_WIFFI,wire_length & 0XFF );
  uint16_t sum = ((wire_length)>>8) + ((wire_length)&0xFF) + packet->type;

  for (uint8_t i=0; i< packet->length; i++) {

    uartWriteByte( UART_WIFFI,packet->data[i] );

    sum += packet->data[i];
  }
  uartWriteByte( UART_WIFFI,sum >> 8 );
  uartWriteByte( UART_WIFFI,sum & 0XFF );
  return;
}
//----------------------------------------------------------------------------
void GET_CMD_PACKET(uint8_t commandpacket,uint8_t dat1,uint8_t dat2,uint8_t dat3,uint8_t dat4){
	uint8_t data[5];
	data[0]=commandpacket;
	data[1]=dat1;
	data[2]=dat2;
	data[3]=dat3;
	data[4]=dat4;
   Adafruit_Fingerprint_packet (FINGERPRINT_COMMANDPACKET, sizeof(data), data);
   writeStructuredPacket(&packet);
}

/*==================[declaraciones de funciones internas]====================*/
void Adafruit_Fingerprint_packet (uint8_t type, uint16_t length, uint8_t * data) {

 packet.start_code = FINGERPRINT_STARTCODE;
 packet.type = type;
 packet.length = length;
 packet.address[0] = 0xFF; packet.address[1] = 0xFF;
 packet.address[2] = 0xFF; packet.address[3] = 0xFF;

 if(length<64)
   memcpy(packet.data, data, length);
 else
   memcpy(packet.data, data, 64);
}

uint8_t image2Tz(uint8_t slot) {
  uint8_t data[2] = {FINGERPRINT_IMAGE2TZ,slot};
  Adafruit_Fingerprint_packet (FINGERPRINT_COMMANDPACKET, sizeof(data), data);
  writeStructuredPacket(&packet);
  if(getStructuredPacket(&packet_in,100)==FINGERPRINT_OK)
  {
	  return packet_in.data[0];
  }else
  {
	  return FINGERPRINT_BADPACKET;
  }
}
uint8_t getStructuredPacket(Adafruit_Fingerprint_Packet * packet, uint16_t timeout) {
  uint8_t byte;
  uint16_t idx=0, timer=0;

  while(TRUE) {
    while(!uartRxReady(UART_WIFFI))
    {
      delay(1);
    }
    uartReadByte( UART_WIFFI, &byte );
    switch (idx) {
      case 0:
        if (byte != (FINGERPRINT_STARTCODE >> 8))
	  continue;
        packet->start_code = (uint16_t)byte << 8;
        break;
      case 1:
        packet->start_code |= byte;
        if (packet->start_code != FINGERPRINT_STARTCODE)
	  return FINGERPRINT_BADPACKET;
        break;
      case 2:
      case 3:
      case 4:
      case 5:
        packet->address[idx-2] = byte;
        break;
      case 6:
	packet->type = byte;
	break;
      case 7:
	packet->length = (uint16_t)byte << 8;
	break;
      case 8:
	packet->length |= byte;
	break;
      default:
        packet->data[idx-9] = byte;
        if((idx-8) == packet->length)
          return FINGERPRINT_OK;
        break;
    }
    idx++;
  }
  return FINGERPRINT_BADPACKET;
}
//-----------------------------------------------------------------------
uint8_t getImage(void) {
  uint8_t data[1] = {FINGERPRINT_GETIMAGE};
  Adafruit_Fingerprint_packet (FINGERPRINT_COMMANDPACKET, sizeof(data), data);
  writeStructuredPacket(&packet);
  if(getStructuredPacket(&packet_in,100)==FINGERPRINT_OK)
  {
	  return packet_in.data[0];
  }else
  {
	  return FINGERPRINT_BADPACKET;
  }

}


//-----------------------------------------------------------------------
uint8_t fingerFastSearch(void) {
  // high speed search of slot #1 starting at page 0x0000 and page #0x00A3
  uint8_t data[6]={FINGERPRINT_HISPEEDSEARCH, 0x01, 0x00, 0x00, 0x00, 0xA3};
  Adafruit_Fingerprint_packet (FINGERPRINT_COMMANDPACKET, sizeof(data), data);
  writeStructuredPacket(&packet);
  getStructuredPacket(&packet_in,100);

  uint16_t fingerID = 0xFFFF;
  uint16_t confidence = 0xFFFF;

  fingerID = packet_in.data[1];
  fingerID <<= 8;
  fingerID |= packet_in.data[2];

  confidence = packet_in.data[3];
  confidence <<= 8;
  confidence |= packet_in.data[4];

  return packet_in.data[0];
}
//-------------------------------------------------------------------------
uint8_t createModel(void) {
  uint8_t data[1] = {FINGERPRINT_REGMODEL};
  Adafruit_Fingerprint_packet (FINGERPRINT_COMMANDPACKET, sizeof(data), data);
  writeStructuredPacket(&packet);
  getStructuredPacket(&packet_in,100);

  return packet_in.data[0];
}
//---------------------------------------------------------------------------
uint8_t storeModel(uint16_t location) {

  uint8_t data[4] = {FINGERPRINT_STORE,0X01,(uint8_t)(location >> 8),(uint8_t)(location & 0xFF)};
  Adafruit_Fingerprint_packet (FINGERPRINT_COMMANDPACKET, sizeof(data), data);
  writeStructuredPacket(&packet);
  if(getStructuredPacket(&packet_in,100)==FINGERPRINT_OK)
    {
  	  return packet_in.data[0];
    }else
    {
    	return packet_in.data[0];//coregir esto
    }
}

/*==================[fin del archivo]========================================*/

