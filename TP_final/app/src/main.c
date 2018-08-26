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

/* Date: 2018-24-8 */

/*==================[inclusions]=============================================*/

#include "sapi.h"
#include "sapi_esp8266HttpServer.h"
#include "sapi_stdio.h"
#include <string.h>
#include <math.h>
#include "adafruit_fingerprint.h"

/*==================[macros and definitions]=================================*/
// Se deben definir los datos del nombre de la red y la contrasenia.
#define WIFI_NAME        "INVITADOS-CADIEEL"//"WiFi-Arnet-ucx1"
#define WIFI_PASS        "CORDOBA9504"//"NWVPHW7KWC"
// El maximo tiempo que se espera una respuesta del modulo ESP8266
#define WIFI_MAX_DELAY   60000//60000

// El inicio y fin HTML le dan formato al texto enviado por el usuario
#define  BEGIN_USER_LINE "<h3 style=\"text-align: center;\"><strong>"
#define  END_USER_LINE   "</strong></h3>"
#define uartConfig uartInit
//----------------------------------------------------------------------------

 Estado initial_state = ESPERA;

 uint8_t flag=1;
 uint8_t device_reply;

 uint8_t bite = 0,j,aux=0xff,aux2;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

const char HttpWebPageHeader [] =
		"<!DOCTYPE HTML>"
		"<html>"
		"<head><title>EDU-CIAA NXP</title>"
		"<meta http-equiv=\"refresh\" content=\"10\">"
		"</head>"
		"<p style=\"text-align: center;\">&nbsp;</p>"
		"<p style=\"text-align: center;\"><span style=\"color: #0000ff;\"><strong><img src=\"https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcT2moZZ0qZ5Az_Qt4kRv6NsJtIxKJl8phyn0APAyfsshhpvpjPs\" alt=\"Logo\" width=\"135\" height=\"135\" /></strong></span></p>"
		"<h1 style=\"text-align: center;\"><span style=\"color: #0000ff;\"><strong>Servidor Web HTTP<br />ESP8266 - EDU CIAA NXP</strong></span></h1>"
		"<h4 style=\"text-align: center;\"><strong><em>En esta pagina web se muestran datos provenientes de la EDU-CIAA NXP</em></strong><br /><strong><em>enviados cuando un cliente HTTP como esta pagina realiza una peticion al servidor</em></strong></h4>"
		"<p style=\"text-align: center;\">INICIO USER HTML</p>"
		"<p style=\"text-align: center;\">&nbsp;</p>"
		"<p style=\"text-align: center;\">&nbsp;</p>"
//		"<body bgcolor=\"#E2E1E3\"></body>"
		;

const char HttpWebPageEnd [] =
		"<p style=\"text-align: center;\">&nbsp;</p>"
		"<p style=\"text-align: center;\">&nbsp;</p>"
		"<p style=\"text-align: center;\">FIN USER HTML</p>"
		"<hr />"
		"<p style=\"text-align: center;\"><em>Copyright&nbsp;Agustin Bassi -&nbsp;</em><em>Pablo Gomez</em></p>"
		"<p style=\"text-align: center;\">CURSOS INET 2017</p>"
		"<p style=\"text-align: center;\"><a href=\"http://www.proyecto-ciaa.com.ar\">www.proyecto-ciaa.com.ar</a></p>"
		"</html>";

char HttpWebPageBody [200];
char UserId [] ="Principal";
char Space [] ="..........";

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/


/*==================[external functions definition]==========================*/

/* FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET. */
int main(void){
	bool_t error;
	uint8_t counter = 0;
	uint8_t data;
	delay_t wifiDelay;

	uint8_t bandera = 0;
	//-----------------------------------------------------------------------
	// Inicializar la placa
	boardConfig ();
	uartConfig (UART_USB, BAUD_RATE);
	uartConfig (UART_WIFFI, 57600);
	//-----------------------------------------------------------------------------
	stdioPrintf(UART_USB, "\n\rCargando.....");
	   rtc_t rtc;
	   rtc.year = 2016;
	   rtc.month = 7;
	   rtc.mday = 3;
	   rtc.wday = 1;
	   rtc.hour = 13;
	   rtc.min = 17;
	   rtc.sec= 0;

	   bool_t val = 0;
	   uint8_t i = 0;

	   /* Inicializar RTC */
	   val = rtcConfig( &rtc );

	   delay_t delay1s;
	   delayConfig( &delay1s, 1000 );
	   delay(2000);

	   for( i=0; i<10; i++ ){
	      val = rtcRead( &rtc );
	      delay(1000);
	   }
	   rtc.year = 2016;
	   rtc.month = 7;
	   rtc.mday = 3;
	   rtc.wday = 1;
	   rtc.hour = 14;
	   rtc.min = 30;
	   rtc.sec= 0;

	   /* Establecer fecha y hora */
	   val = rtcWrite( &rtc );

	// Envia un mensaje de bienvenida.
	stdioPrintf(UART_USB, "\n\rBienvenido al servidor HTTP Esp8266 con EDU CIAA");
	stdioPrintf(UART_USB, "\n\rLa configuracion puede tardar hasta 1 minuto.");

	error = FALSE;
	// Configura un delay para salir de la configuracion en caso de error.
	delayConfig(&wifiDelay, WIFI_MAX_DELAY);

	// Mientras no termine la configuracion o mientras no pase el tiempo maximo, ejecuta la configuracion.
	// A la configuracion se le pasa nombre y contrasenia de RED
	while (!esp8266ConfigHttpServer(WIFI_NAME, WIFI_PASS) && !error){
		if (delayRead(&wifiDelay)){
			error = TRUE;
		}
	}

	// Avisa al usuario como salio la configuracion
	if (!error){
		stdioPrintf(UART_USB, "\n\rServidor HTTP configurado. IP: %s", esp8266GetIpAddress());
		// Enciende LEDG indicando que el modulo esta configurado.
		gpioWrite(LEDG, TRUE);
	} else {
		stdioPrintf(UART_USB, "\n\rError al configurar servidor HTTP.");
		// Enciende LEDR indicando que el modulo esta en error.
		gpioWrite(LEDR, TRUE);
	}
	/* ------------- REPETIR POR SIEMPRE ------------- */
	while(1) {
		// Si llego una peticion al servidor http
		if (bandera==0){
			esp8266ReadHttpServer();
			// Los datos a enviar a la web deben estar en formato HTML. Notar que
			// BEGIN_USER_LINE y END_USER_LINE solo formatean la cadena a enviar con tags HTML.
			//stdioSprintf(HttpWebPageBody, "%s VALOR CONTADOR: %d - VALOR ADC: %d %s", BEGIN_USER_LINE, counter, 3+counter*2, END_USER_LINE);
			stdioSprintf(HttpWebPageBody, "%s USUARIO:%s %s HORA:%d:%d:%d %s", BEGIN_USER_LINE,UserId,Space,rtc.hour,rtc.min,rtc.sec, END_USER_LINE);
			error = FALSE;
			// Configura un delay para salir de la configuracion en caso de error.
			delayConfig(&wifiDelay, WIFI_MAX_DELAY);

			// Mientras no termine el envio o mientras no pase el tiempo maximo, ejecuta el envio.
			while (!esp8266WriteHttpServer(HttpWebPageHeader, HttpWebPageBody, HttpWebPageEnd) && !error){
				if (delayRead(&wifiDelay)){
					error = TRUE;
				}
			}
			//----------------------------------------------------------------------------------------------------

		      //--------------------------------------------------------------------------------------------------
			// Avisa al usuario como fue el envio
			if (!error){
				stdioPrintf(UART_USB, "\n\rPeticion respondida al cliente HTTP %d.", esp8266GetConnectionId());
				gpioToggle(LEDG);
			} else {
				stdioPrintf(UART_USB, "\n\rPeticion respondida al cliente HTTP %d.", esp8266GetConnectionId());
				gpioToggle(LEDR);
			}

			counter++;
			bandera=1;
		}

		 if( delayRead( &delay1s ) )
		 {
			val = rtcRead( &rtc );
		 }
//------------------------------------------------------------------------------------------------
		   if(initial_state==ESPERA)
		   {
			   if(flag==1)
			   {
				   stdioPrintf(UART_USB,"Ubique el dedo sobre el sensor...\r\n" );
			   	   flag=0;
			   }
			   device_reply=getImage();

			   if(device_reply==FINGERPRINT_OK )
			   {
				   initial_state=SALIDA;
				   flag=1;
				   stdioPrintf(UART_USB,"Imagen Capturada.\r\n" );
				   stdioPrintf(UART_USB,"Remueva el dedo....\r\n" );
				   aux = image2Tz(1);
				   aux2=fingerFastSearch();
				   if(aux2==FINGERPRINT_OK)
				   {
					   stdioPrintf(UART_USB,"Bienvenido MASTER.\r\n" );
					   gpioWrite(LED3,ON);
					   delay(100);
					   gpioWrite(LED3,OFF);
					   bandera=0;
				   }else
				   {
					   stdioPrintf(UART_USB,"Usuario no reconocido.\r\n" );
				   }

			   }else if(device_reply==FINGERPRINT_IMAGEFAIL)
			   {
				   stdioPrintf(UART_USB,"Error en captura\r\n" );
				   initial_state=SALIDA;
				   flag=1;
			   }

		   }
		   if(initial_state==LIMBO)
		   {
			   if(flag==1)
			   {
				   flag=0;
			   }
			   device_reply=getImage();
			   if(device_reply==FINGERPRINT_NOFINGER)
			   {
				   initial_state=TOMA;
				   flag=1;
			   }
		   }
		   if(initial_state==TOMA)
		   {
			   if(flag==1)
			   {
				   stdioPrintf(UART_USB,"Ubique el mismo dedo nuevamente\r\n" );
			   	   flag=0;
			   }
			   device_reply=getImage();

		   }
		   if(initial_state==SALIDA)
		   {
			   if(flag==1)
			   {
				   flag=0;
			   }
			   device_reply=getImage();
			   if(device_reply==FINGERPRINT_NOFINGER)
			   {
				   initial_state=ESPERA;
				   flag=1;
			   }
		   }

//------------------------------------------------------------------------------------------------
	}

	/* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
      por ningun S.O. */
	return 0 ;
}

/*==================[end of file]============================================*/
