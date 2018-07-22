/* Copyright 2018, CHRISTIAN Y.
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

/*
 * Date: 2018-07-06
 */

/*==================[inclusions]=============================================*/

//#include "adc_dac.h"   // <= own header (optional)
#include "sapi.h"        // <= sAPI header
#include "sd_spi.h"   // <= own header (optional)
#include "ff.h"       // <= Biblioteca FAT FS
#include "operaciones.h"
#include "sapi_uart.h"

/*==================[macros and definitions]=================================*/
#define FILENAME "Muestras.txt"

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/


static FATFS fs;           // <-- FatFs work area needed for each volume
static FIL fp;             // <-- File object needed for each open file

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

/*===========================================================================*/

/* FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET. */
int main(void){

   /* ------------- INICIALIZACIONES ------------- */
	static char dataBuff[52];//52
	static char rtcString[22];
	//static char adcString_ch1[6];
	//static char adcString_ch2[6];
	//static char adcString_ch3[6];

   /* Inicializar la placa */
   boardConfig();

   /* Inicializar UART_USB a 115200 baudios */
   uartConfig( UART_USB, 115200 );

   adcConfig( ADC_ENABLE ); /* ADC */

   // SPI configuration
   spiConfig( SPI0 );

   ////////////// Give a work area to the default drive//////////////////////

   UINT nbytes;

   if( f_mount( &fs, "", 0 ) != FR_OK ){
      // If this fails, it means that the function could
      // not register a file system object.
      // Check whether the SD card is correctly connected
   }


   //////////////////////////////////////////////////////////////////////////

   /* Estructura RTC */
   rtc_t rtc;
   rtc.year = 2012;
   rtc.month = 6;
   rtc.mday = 3;
   rtc.wday = 2;
   rtc.hour = 11;
   rtc.min = 11;
   rtc.sec= 0;

   bool_t val = 0;

   /* Inicializar RTC */
   val = rtcConfig( &rtc );

   delay(2000); // El RTC tarda en setear la hora, por eso el delay

   /////////////////////////////////////////////////////////////

   /* Contador */
   uint32_t i = 0;

   /* Variable para almacenar el valor leido del ADC CH1,CH2,CH3 */
   uint16_t muestra = 0;
   uint16_t muestra2 = 0;
   uint16_t muestra3 = 0;

   /* Variables de delays no bloqueantes */
   delay_t delay1;


   /* Inicializar Retardo no bloqueante con tiempo en ms */
   delayConfig( &delay1, 1000 );


   /* ------------- REPETIR POR SIEMPRE ------------- */
   while(1) {

      /* delayRead retorna TRUE cuando se cumple el tiempo de retardo */
      if ( delayRead( &delay1 ) ){

    	 val = rtcRead( &rtc );

    	 showDateAndTime( &rtc,rtcString);

         /* Leo Entradas Analogicas */
         muestra = adcRead( CH1 );
         muestra2 = adcRead( CH2 );
         muestra3 = adcRead( CH3 );



//////////////////////////////////////////////////////////////////////////

         /* Conversión de muestra entera a ascii con base decimal */
         //itoa( muestra, buffer_gen[0], 10 ); /* 10 significa decimal */
         itoa( (int64_t)muestra,buffer_gen[0], 10 ); /* 10 significa decimal */
         itoa( (int64_t)muestra2,buffer_gen[1], 10 ); /* 10 significa decimal */
         itoa( (int64_t)muestra3,buffer_gen[2], 10 ); /* 10 significa decimal */


//////////////////////////////////////////////////////////////////////////
         strncat( buffer_gen[0],",", 1 );
         strncat( dataBuff,buffer_gen[0], strnlen(buffer_gen[0]) );
         strncat( buffer_gen[1], ",", 1 );
         strncat( dataBuff,buffer_gen[1], strnlen(buffer_gen[1]) );
         strncat( buffer_gen[2]," ", 1 );
         strncat( dataBuff,buffer_gen[2], strnlen(buffer_gen[2]) );

         strncat( dataBuff,rtcString, strnlen(rtcString) );

         strncat( dataBuff, "\r\n", 2 );
///////////////////////////////////////////////////////////////////////////


         //Abrir el archivo para escritura////////////////

         if( f_open( &fp, FILENAME, FA_WRITE | FA_OPEN_APPEND ) == FR_OK ){

        	 f_write( &fp,dataBuff,strlen(dataBuff), &nbytes );

            f_close(&fp);
         }
///////////////////////////////////////////////////////////////////////////

      }


   }

   /* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
      por ningun S.O. */
   return 0 ;
}

/*==================[end of file]============================================*/
