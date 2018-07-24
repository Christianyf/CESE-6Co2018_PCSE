/* 
 * All rights reserved.
 *
 * This file is part of sAPI Library.
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
 */

/*==================[inlcusiones]============================================*/

#include "sapi.h"        // <= Biblioteca sAPI
#include <string.h>
#include <math.h>

/*==================[definiciones y macros]==================================*/

#define UART_PC        UART_USB
#define UART_BLUETOOTH UART_232

#define BAUD_RATE 115200

#define ADDRESS_MAG  0x0D  //direccion magnetometro

/*==================[definiciones de datos internos]=========================*/

DEBUG_PRINT_ENABLE
CONSOLE_PRINT_ENABLE

static int16_t qmc5883l_x_raw;
static int16_t qmc5883l_y_raw;
static int16_t qmc5883l_z_raw;

static char buffer[10];

uint8_t registroAleer;

int16_t xValue;
int16_t yValue;
int16_t zValue;

uint8_t x_MSB, x_LSB;
uint8_t y_MSB, y_LSB;
uint8_t z_MSB, z_LSB;

uint8_t dataToReadBuffer;

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/



/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void )
{
   // ---------- CONFIGURACIONES ------------------------------
	float angulo;
	// Inicializar y configurar la plataforma
   boardConfig();

   /* Inicializar QMC5883L */

   QMC5883L_config_t qmc5883L_initValue;

   qmc5883lPrepareDefaultInit( &qmc5883L_initValue );

   qmc5883L_initValue.mode = QMC5883L_continuous_measurement;

   qmc5883lInit( qmc5883L_initValue );



   // Inicializar UART_USB para conectar a la PC
   debugPrintConfigUart( UART_PC, 9600 );/////////////////////
   debugPrintlnString( "UART_PC configurada." );

   // Inicializar UART_232 para conectar al modulo bluetooth
   consolePrintConfigUart( UART_BLUETOOTH, 9600 );
   debugPrintlnString( "UART_BLUETOOTH para modulo Bluetooth configurada." );

   uint8_t data = 0;


   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE ) {

	   qmc5883lRead( &qmc5883l_x_raw, &qmc5883l_y_raw, &qmc5883l_z_raw );
	   angulo = atan2( qmc5883l_y_raw,qmc5883l_x_raw ); /*valor en radianes*/
	   angulo = angulo * 180 / 3.14; /*Convertir a grados*/

	   sprintf(buffer,"%d,%d,%d",qmc5883l_x_raw,qmc5883l_y_raw,qmc5883l_z_raw);

      // Si leo un dato de una UART lo envio a al otra (bridge)
      if( uartReadByte( UART_PC, &data ) ) {
         uartWriteByte( UART_BLUETOOTH, data );
      }
      if( uartReadByte( UART_BLUETOOTH, &data ) ) {
         if( data == 'h' ) {
            gpioWrite( LEDB, ON );
         }
         if( data == 'l' ) {
            gpioWrite( LEDB, OFF );
         }
         uartWriteByte( UART_PC, data );
      }


      uartWriteString( UART_BLUETOOTH, buffer );

      delay(1000);
   }

   // NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
   // directamenteno sobre un microcontroladore y no es llamado por ningun
   // Sistema Operativo, como en el caso de un programa para PC.
   return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

bool_t hm10bleTest( int32_t uart )
{
   uartWriteString( uart, "AT\r\n" );
   return waitForReceiveStringOrTimeoutBlocking( uart,
                                                 "OK\r\n", strlen("OK\r\n"),
                                                 50 );
}



/*==================[fin del archivo]========================================*/
