#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

void uart_transmit(unsigned char data)
{
    while (!( UCSRA & (1<<UDRE)));                // wait while register is free
    UDR = data;                                   // load data in the register
}

unsigned char uart_receive()
{
    while ( !(UCSRA & (1<<RXC)) );
    return UDR;
}

void uart_transmit_txt(unsigned char *s)
{
    while(*s)
    {
        uart_transmit(*s);
        s++;
    }
}

int main( void )
{
    ADCSRA =   (1<<ADEN) | (1<<ADPS0) | (1<<ADPS2);
    ADMUX  =    (1<<REFS0) |(1<<MUX2) | (1<<MUX0);

    DDRC &=~ (1<<PC5);

    uint8_t ubrr = 12; //for 9600: 2MHz(12) 4MHz(25)
    UBRRL = (unsigned char)ubrr;
    //UBRRH = (unsigned char)(ubrr>>8);
    UCSRB |= (1<<RXEN)|(1<<TXEN);
    UCSRC |= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
    char txt[] = "Czujnik1:0000\r\n";

    while(1){
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC));

        uint16_t adc_val = ADCL;
        adc_val |= (ADCH << 8);

        sprintf(txt, "Czujnik1:%d\r\n", adc_val);
        uart_transmit_txt(txt);

        _delay_ms(3000);
    }
}