#include "sapi.h"        // <= sAPI header
#include "sd_spi.h"   // <= own header (optional)
#include "ff.h"       // <= Biblioteca FAT FS
#include "operaciones.h"

/*===========================================================================*/
//typedef char Buffer[10];

/*============================hola=============================================*/
char* itoa(int64_t value, char* result, int base) {
   // check that the base if valid
   if (base < 2 || base > 36) { *result = '\0'; return result; }

   char* ptr = result, *ptr1 = result, tmp_char;
   int tmp_value;

   do {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
   } while ( value );

   // Apply negative sign
   if (tmp_value < 0) *ptr++ = '-';
   *ptr-- = '\0';
   while(ptr1 < ptr) {
      tmp_char = *ptr;
      *ptr--= *ptr1;
      *ptr1++ = tmp_char;
   }
   return result;
}
/*=============================================================================*/

/* Enviar fecha y hora en formato "DD/MM/YYYY, HH:MM:SS" */
void showDateAndTime( rtc_t * rtc,char *buffer_gen){
	char buff[10];
	buffer_gen[0] = 0;

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->mday), (char*)buff, 10 ); /* 10 significa decimal */
   /* Envio el dia */
   if( (rtc->mday)<10 )
	   strncat( buffer_gen, "0", strlen("0") );
   strncat( buffer_gen, buff, strlen(buff) );
   strncat( buffer_gen, "/", strlen("/") );

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->month), (char*)buff, 10 ); /* 10 significa decimal */
   /* Envio el mes */
   if( (rtc->month)<10 )
	   strncat( buffer_gen, "0", strlen("0") );
   strncat( buffer_gen, buff, strlen(buff) );
   strncat( buffer_gen, "/", strlen("/") );

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->year), (char*)buff, 10 ); /* 10 significa decimal */
   /* Envio el año */
   strncat( buffer_gen, buff, strlen(buff) );
   strncat( buffer_gen, " ", strlen(" ") );


   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->hour), (char*)buff, 10 ); /* 10 significa decimal */
   /* Envio la hora */
   if( (rtc->hour)<10 )
	   strncat( buffer_gen, "0", strlen("0") );
   strncat( buffer_gen, buff, strlen(buff) );
   strncat( buffer_gen, ":", strlen(":") );

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->min), (char*)buff, 10 ); /* 10 significa decimal */
   /* Envio los minutos */
  // uartBuff[2] = 0;    /* NULL */
   if( (rtc->min)<10 )
	   strncat( buffer_gen, "0", strlen("0") );
   strncat( buffer_gen, buff, strlen(buff) );
   strncat( buffer_gen, ":", strlen(":") );

   /* Conversion de entero a ascii con base decimal */
   itoa( (int) (rtc->sec), (char*)buff, 10 ); /* 10 significa decimal */
   /* Envio los segundos */
   if( (rtc->sec)<10 )
	   strncat( buffer_gen, "0", strlen("0") );
   strncat( buffer_gen, buff, strlen(buff) );


}
///////////////////////////////////////////////////////////////////////////////////

