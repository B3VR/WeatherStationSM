/*
 * Termometr_SM.c
 *
 * Created: 02.04.2021 17:01:47
 * Author : Krystian Miszta, Julia Leszczyñska
 */ 
#define  F_CPU 16000000

#include <stdlib.h>
#include <stdio.h>
#include "avr/io.h"
#include <util/delay.h>
#include "hd44780.h"
#include "dhtxx.h"




int main(void)
{
	DDRD = 0x30;
	DDRD &= ~(1<<3);
	PORTD |= (1<<3);
	
	lcd_init();
    while(1){		
		
		if (PIND & (1<<3))
		{
			weatherStation();
		}else{
			bodyTemperature();
		}
		
    }
}

void bodyTemperature()
{
	lcd_clrscr();
	lcd_puts("Przystaw czolo");
	lcd_goto(0x40);
	lcd_puts("do czujnika");
	_delay_ms(2000);
	lcd_clrscr();
	lcd_puts("3 ");
	_delay_ms(1000);
	lcd_puts("2 ");
	_delay_ms(1000);
	lcd_puts("1");
	_delay_ms(1000);	
}

void weatherStation()
{
	int temp, hum; 
	char bufor[10];
	dhtxxconvert( DHTXX_DHT11, &PORTD, &DDRD, &PIND, ( 1 << 2 ) );
	_delay_ms(1000);
	dhtxxread( DHTXX_DHT11, &PORTD, &DDRD, &PIND, ( 1 << 2 ), &temp, &hum );
	
	lcd_clrscr();
	lcd_puts("Temp: ");
	itoa(temp/10, bufor, 10);
	lcd_puts(bufor);
	lcd_puts(" C");
	
	itoa(hum/10, bufor,10);
	lcd_goto(64);
	lcd_puts("Wilg: ");
	lcd_puts(bufor);
	lcd_puts(" %");
	checkHumAndTemperature(hum, temp);
}

void checkHumAndTemperature(int hum, int temp)
{
	
	if (hum/10 > 60 || hum/10 < 40 || temp/10 > 25 || temp/10 < 18)
	{
		if ((hum/10 > 60 || hum/10 < 40) && (temp/10 > 25 || temp/10 < 18))
		{
			lcd_goto(15);
			lcd_puts("!");
			lcd_goto(79);
			lcd_puts("!");
		}else if (temp/10 > 25 || temp/10 < 18){
			lcd_goto(15);
			lcd_puts("!");
		}else if (hum/10 > 60 || hum/10 < 40){
			lcd_goto(79);
			lcd_puts("!");
		}
		
		LED_off("green");
		LED_on("red");
	}else
	{
		LED_on("green");
		LED_off("red");
	}
}

void LED_on(char* color)
{
	if(color == "red")
	{
		PORTD |= (1<<5);
	}else if (color == "green")
	{
		PORTD |= (1<<4);
	}
}

void LED_off(char* color)
{
	if(color == "red")
	{
		PORTD &= ~(1<<5);
	}else if (color == "green")
	{
		PORTD &= ~(1<<4);
	}
}




