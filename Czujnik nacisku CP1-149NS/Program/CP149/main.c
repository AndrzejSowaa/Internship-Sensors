#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "HD44780.h"


//definicja napiecia referencyjnego
#define VREF 2.56
//definicja ADCIN (wej�cie ADC)
#define ADCIN PC5

int main(void)
{
	char wynik[]="           ";//bufor tekstowy, wyczyszczenie bufora
	float adc;//zmienna do oblicze� napi�cia
	
	

	LCD_Initalize();      //inicjalizacja LCD
	LCD_GoTo(0, 0);      //Ustawienie kursora w pozycji (0,0)
	LCD_WriteText("ADC test: 10 bit");

	//Inicjalizacja ADC
	ADCSRA =   (1<<ADEN) //w��czenie ADC
	|(1<<ADPS0) //ADPS2:0: ustawienie preskalera na 128
	|(1<<ADPS1)
	|(1<<ADPS2);
	

	ADMUX  =    (1<<REFS1) | (1<<REFS0) | (1<<MUX2) | (1<<MUX0);

	
	DDRC &=~ (1<<ADCIN);               //Ustawienie Wej�cia ADC

	for(;;)
	{
		ADCSRA |= (1<<ADSC);	//ADSC: Uruchomienie pojedynczej konwersji

		while(ADCSRA & (1<<ADSC));	//czeka na zako�czenie konwersji

		adc=ADC*VREF/1024;      //przeliczenie warto�ci na napi�cie
		sprintf(wynik,"U=%1.3f [V]",adc);   //konwersja na �a�cuch znakowy
		LCD_GoTo(3, 1);
		LCD_WriteText(wynik);   //Wy�wietlenie wyniku
		_delay_ms(500);
	}
}
