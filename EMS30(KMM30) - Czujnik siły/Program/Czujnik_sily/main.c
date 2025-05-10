#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#define ADCIN PC5

void uart_transmit(unsigned char data)
{
	while (!( UCSRA & (1<<UDRE)));               
	UDR = data;                                   
}

unsigned char USART_Receive( void )
{
	// Czekaj na odbiór danych 
	while ( !(UCSRA & (1<<RXC)) );
	// Pobierz odebrane dane z bufora
	return UDR;
}

int main()
{
	uint8_t ubrr = 12;
	UBRRL = (unsigned char)ubrr;
	UCSRB |= (1<<RXEN)|(1<<TXEN);
	UCSRC |= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
	char txt[10];

	ADCSRA =   (1<<ADEN) | (1<<ADPS0) | (1<<ADPS2);
	ADMUX  =  (1<<REFS0) | (1<<MUX2) | (1<<MUX0);

	DDRC &=~ (1<<ADCIN);


	while(1)
	{
		uint16_t adc_val;
		float adc_filt;
		uint16_t adc_aver=0;
		float adc_kg = 0;
		for(int j=0; j<100; j++)
		{
			ADCSRA |= (1<<ADSC);

			while(ADCSRA & (1<<ADSC));

			uint16_t adc_val=ADCL;
			adc_val |= (ADCH << 8);
			
			adc_aver+=adc_val;
			if(j==99)
			
			{
				adc_aver = adc_aver / 100;
				if(adc_aver<=516)
				{
					adc_kg = ((adc_aver * (-1)) + 516) / 9.93; 
				}
				else if(adc_aver>516)
				{
					adc_kg = (adc_aver - 516) / 9.93; 
				}

				// Bufory do przechowywania czêœci ca³kowitej i u³amkowej
				int whole_part = (int)adc_kg;
				int fractional_part = (int)((adc_kg - whole_part) * 100); // mno¿ymy przez 100 by uzyskaæ 2 cyfry po przecinku

				// Formatowanie wyniku do tekstu
				char txt[20];
				sprintf(txt, "%d.%02d\r\n", whole_part, fractional_part);
				
				
				for(int i=0; i<5; i++)
				{
					uart_transmit(txt[i]);
					_delay_ms(10);
				}
				adc_aver=0;
			}
			_delay_ms(20);
		}
		_delay_ms(40);
	}
}




/*
   0 kg - 518
 2,5 kg - 505
   5 kg - 478
   7 kg - 454
   9 kg - 430
11,5 kg - 414
*/