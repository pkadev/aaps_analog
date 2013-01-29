#include <stddef.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "m128_hal.h"
#define LED_PIN 1
#define CS_MSK 0x04
#define CS_MSK 0x04
#define CLK_MSK 0x20

#define GLED_PIN PD3
#define GLED_DDR DDRD
#define GLED_PORT PORTD
#define GLED_INIT() (GLED_DDR|=(1<<GLED_PIN))
#define GLED_SET() (GLED_PORT|=(1<<GLED_PIN))
#define GLED_CLR() (GLED_PORT&=~(1<<GLED_PIN))

#define IRQ_PIN PD0
#define SPI_CLK PB5
ISR(SPI_STC_vect)
{
    volatile char val = SPSR;
    volatile char vali = SPDR;
    SPCR = (1<<SPIE) | (1<<SPE) | (1<<MSTR);
    val++;
    vali++;
    PORTB |= (1<<LED_PIN);
    //PORTD |= (1<<IRQ_PIN);
    _delay_ms(5);
    PORTB &= ~(1<<LED_PIN);
    //PORTD &= ~(1<<IRQ_PIN);
}

int main(void)
{
    GLED_INIT();
    GLED_SET();
    _delay_ms(5000);
    GLED_CLR();
    
    STATUS_REGISTER |= (1<<STATUS_REGISTER_IT);
    
    /* Set LED as output */
    DDRB |= (1<<LED_PIN);
    
    /* Set CS and CLK as input */
    //DDRB &= ~(CS_MSK);
    //DDRB &= ~(1<<SPI_CLK);
    //PORTB &= ~(CS_MSK);
    //PORTB |= (1<<SPI_CLK);

    /* Set IRQ as output */
    DDRD |= (1<<IRQ_PIN);

    /* Init SPI Slave */
    SPCR = (1<<SPIE) | (1<<SPE);// | (1<<MSTR);
    //SPDR = 0xCC;
    while(1)
    {
        GLED_SET();
        _delay_ms(100);
        GLED_CLR();
        _delay_ms(5000);
    }

    return 0;
}

