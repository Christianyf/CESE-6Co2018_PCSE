/*
 * Copyright 2018
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

/* Date: 2018-9-24 */

#ifndef _ADAFRUIT_FINGERPRINT_
#define _ADAFRUIT_FINGERPRINT_

/*==================[inclusions]=============================================*/

#include "sapi.h"
#include <string.h>
#include <math.h>

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/
#define UART_PC        UART_USB
#define UART_WIFFI     UART_232

#define BAUD_RATE 57600

#define FINGERPRINT_OK 0X00
#define FINGERPRINT_ACKPACKET 0x7
#define FINGERPRINT_NOFINGER 0x02
#define FINGERPRINT_IMAGEFAIL 0x03
#define FINGERPRINT_IMAGEMESS 0x06
#define FINGERPRINT_FEATUREFAIL 0x07
#define FINGERPRINT_INVALIDIMAGE 0x15
#define FINGERPRINT_ENROLLMISMATCH 0x0A
#define FINGERPRINT_BADLOCATION 0x0B
#define FINGERPRINT_FLASHERR 0x18
#define FINGERPRINT_VERIFYPASSWORD 0x13
#define FINGERPRINT_PACKETRECIEVEERR 0x01
#define FINGERPRINT_STARTCODE 0xEF01
#define FINGERPRINT_COMMANDPACKET 0x1
#define FINGERPRINT_GETIMAGE 0x01
#define FINGERPRINT_IMAGE2TZ 0x02
#define FINGERPRINT_REGMODEL 0x05
#define FINGERPRINT_STORE 0x06
#define FINGERPRINT_LOAD 0x07
#define FINGERPRINT_UPLOAD 0x08
#define FINGERPRINT_DELETE 0x0C
#define FINGERPRINT_EMPTY 0x0D
#define FINGERPRINT_HISPEEDSEARCH 0x1B
#define FINGERPRINT_TIMEOUT 0xFF
#define FINGERPRINT_BADPACKET 0xFE
#define FINGERPRINT_TEMPLATECOUNT 0x1D
/*==================[typedef]================================================*/

typedef struct  {

  uint16_t start_code;      ///< "Wakeup" code for packet detection
  uint8_t address[4];       ///< 32-bit Fingerprint sensor address
  uint8_t type;             ///< Type of packet
  uint16_t length;          ///< Length of packet
  uint8_t data[64];         ///< The raw buffer for packet payload

}Adafruit_Fingerprint_Packet;

typedef enum  estado {ESPERA,TOMA,LIMBO,SALIDA} Estado;

/*==================[external data declaration]==============================*/
static Adafruit_Fingerprint_Packet packet;
static Adafruit_Fingerprint_Packet packet_in;

/*==================[external functions declaration]=========================*/
void Adafruit_Fingerprint_packet (uint8_t type, uint16_t length, uint8_t * data);
void writeStructuredPacket(const Adafruit_Fingerprint_Packet *packet);
void GET_CMD_PACKET(uint8_t commandpacket,uint8_t dat1,uint8_t dat2,uint8_t dat3,uint8_t dat4);
void Adafruit_Fingerprint_packet (uint8_t type, uint16_t length, uint8_t * data);
uint8_t image2Tz(uint8_t slot);
uint8_t getStructuredPacket(Adafruit_Fingerprint_Packet * packet, uint16_t timeout);
uint8_t getImage(void);
uint8_t fingerFastSearch(void);
uint8_t getStructuredPacket(Adafruit_Fingerprint_Packet * packet, uint16_t timeout);
uint8_t createModel(void);
uint8_t storeModel(uint16_t location); 

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/*==================[end of file]============================================*/
#endif
