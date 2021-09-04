/*
 * Termometr_SM.c
 * Created: 02.04.2021 17:01:47
 * Author : Krystian Miszta, Julia Leszczyñska
 */ 

#define F_CPU 16000000
#define SDA_PORT PORTC
#define SDA_PIN 4
#define SCL_PORT PORTC
#define SCL_PIN 5

#include <stdlib.h>
#include <stdio.h>
#include "avr/io.h"
#include <util/delay.h>
#include "hd44780.h"
#include "dhtxx.h"
#include "SoftI2CMaster.h"

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

#pragma region MainMethodes
void bodyTemperature()
{
	lcd_clrscr();
	lcd_puts("Przystaw czolo");
	lcd_goto(0x40);
	lcd_puts("do czujnika");
	LED_off("green");
	LED_off("red");
	_delay_ms(1000);
	
	lcd_clrscr();
	lcd_puts("3 ");
	LED_on("green");
	_delay_ms(1000);
	
	lcd_puts("2 ");
	LED_off("green");
	LED_on("red");
	_delay_ms(1000);
	
	lcd_puts("1");
	LED_off("red");
	LED_on("green");
	_delay_ms(1000);
	getBodyTemperature();
}

void getBodyTemperature()
{
	int dev = 0x5A<<1;
	int data_low = 0;
	int data_high = 0;
	int pec = 0;
	
	i2c_start(dev+I2C_WRITE);
	i2c_write(0x07);

	i2c_rep_start(dev+I2C_READ);
	data_low = i2c_read(false);
	data_high = i2c_read(false);
	pec = i2c_read(true);
	i2c_stop();
	
	double tempFactor = 0.02;
	double tempData = 0x0000;
	int frac;
	
	tempData = (double)(((data_high & 0x007F) << 8) + data_low);
	tempData = (tempData * tempFactor)-0.01;
	
	float celcius = tempData - 273.15;
	
	char bufor[10];
	ftoa(celcius, bufor, 1);
	lcd_clrscr();
	lcd_puts(bufor);
	lcd_puts(" C");
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
#pragma endregion MainMethodes

#pragma region SideMethodes

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

void ftoa(float n, char* res, int afterpoint)
{
	int ipart = (int)n;
	float fpart = n - (float)ipart;
	int i = intToStr(ipart, res, 0);

	if (afterpoint != 0) {
		res[i] = '.';
		fpart = fpart * pow(10, afterpoint);
		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}

int intToStr(int x, char str[], int d)
{
	int i = 0;
	while (x) {
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}
	while (i < d)
	str[i++] = '0';
	
	reverse(str, i);
	str[i] = '\0';
	return i;
}

void reverse(char* str, int len)
{
	int i = 0, j = len - 1, temp;
	while (i < j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++;
		j--;
	}
}
#pragma endregion SideMethodes






