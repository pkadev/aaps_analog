#include <stddef.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "m128_hal.h"

#define CS_MSK 0x04
#define CS_MSK 0x04
#define CLK_MSK 0x20

#define GLED_PIN PD3
#define GLED_DDR DDRD
#define GLED_PORT PORTD

#define GLED_INIT() (GLED_DDR|=(1<<GLED_PIN))
#define GLED_SET() (GLED_PORT|=(1<<GLED_PIN))
#define GLED_TOGGLE() (GLED_PORT^=(1<<GLED_PIN))
#define GLED_CLR() (GLED_PORT&=~(1<<GLED_PIN))

#define BLED_PIN 1
#define BLED_DDR DDRB
#define BLED_PORT PORTB

#define BLED_INIT() (BLED_DDR|=(1<<BLED_PIN))
#define BLED_SET() (BLED_PORT|=(1<<BLED_PIN))
#define BLED_TOGGLE() (BLED_PORT^=(1<<BLED_PIN))
#define BLED_CLR() (BLED_PORT&=~(1<<BLED_PIN))


#define IRQ_PIN PD0
#define SPI_CLK PB5

volatile uint8_t ipc_rcv_buf = 0;

ISR(SPI_STC_vect)
{
    ipc_rcv_buf = SPDR;
    BLED_SET();
    _delay_ms(1);
    BLED_CLR();
}

int main(void)
{
    SPCR = (1<<SPIE) | (1<<SPE);
    /* Init LED pins */
    BLED_INIT();
    GLED_INIT();

    /* Blink green led as boot indication */
    GLED_SET();
    _delay_ms(100);
    GLED_CLR();
    
    /* Enable global interrupts */    
    STATUS_REGISTER |= (1<<STATUS_REGISTER_IT);
    
    /* Do we need to set MISO as output? */
    DDRB |= (1<<PB4);

    /* Set IRQ as output */
    //DDRD |= (1<<IRQ_PIN);

    /* Init SPI Slave */
    while(1)
    {
        if (ipc_rcv_buf > 0x01 && ipc_rcv_buf < 20)
            GLED_SET();
        else
            GLED_CLR();
    }

    return 0;
}

