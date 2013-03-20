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

#define LED_PIN 5
#define LED_DDR DDRD
#define LED_PORT PORTD

#define LED_INIT() (LED_DDR|=(1<<LED_PIN))
#define LED_SET() (LED_PORT|=(1<<LED_PIN))
#define LED_TOGGLE() (LED_PORT^=(1<<LED_PIN))
#define LED_CLR() (LED_PORT&=~(1<<LED_PIN))

#define CLIND_PIN 3
#define CLIND_PORT PORTD
#define CLIND_DDR DDRD

/* SW1 Defines */
#define SW1_PORT PORTD
#define SW1_DDR DDRD
#define SW1_IN PIND
#define SW1_PIN 2

#define SW1_WAIT_UNTILL_PRESSED() while((SW1_IN & (1<<SW1_PIN))==(1<<SW1_PIN)){}
/* end SW1 defines */

/* SW2 Defines */
#define SW2_PORT PORTB
#define SW2_DDR DDRB
#define SW2_IN PINB
#define SW2_PIN 0

#define SW2_WAIT_UNTILL_PRESSED() while((SW2_IN & (1<<SW2_PIN))==0){}
/* end SW1 defines */

#define IRQ_PIN PD0
#define SPI_CLK PB5
/* 
 * SW1, aktiv låg, pullup, hittar du på PD2 (INT0/PCINT18)
 * SW2/IRQ, aktiv hög, pulldown, hittar du på PB0 (PCINT0/CLKO/ICP1) OBS SW2/IRQ delas med IRQ dvs du får IRQ på _D också om du trycker på SW2.
 */

volatile uint8_t ipc_rcv_buf = 0;

ISR(SPI_STC_vect)
{
    ipc_rcv_buf = SPDR;
    BLED_SET();
    _delay_ms(1);
    BLED_CLR();
}

ISR(PCINT0_vect) /* SW2 */
{
    /*
     * Only do stuff on one edge and debounce.
     * Button may need longer delay if extremely bouncy.
     */
        
    if((SW2_IN & (1<<SW2_PIN)) == (1<<SW2_PIN)) {
        _delay_ms(20); //Debound delay
        if((SW2_IN & (1<<SW2_PIN)) == (1<<SW2_PIN)) {
            LED_SET();
            _delay_ms(10);
            LED_CLR();
            _delay_ms(300);
            LED_SET();
            _delay_ms(10);
            LED_CLR();
        }
    }
}

ISR(PCINT2_vect)
{
    //If SW1 is configured as PCINT18
}

ISR(INT0_vect) /* SW1 */
{
    LED_SET();
    _delay_ms(10);
    LED_CLR();
}

int main(void)
{
    /* Ext IRQ 0 */
    EICRA |= (1<<ISC01);        //IRQ on falling edge
    EIMSK |= (1<<INT0);         //Enable INT0
    /* end Ext IRQ 0 */

    /* PIN change IRQ */
    //PCICR |= (1<<PCIE2);      //Enable PCINT2 (PCINT23..16)
    //PCMSK2 |= (1<<PCINT18);   //Enable PCINT18
    /* End PIN change IRQ */

    /* PIN change IRQ */
    PCICR |= (1<<PCIE0);      //Enable PCINT2 (PCINT23..16)
    PCMSK0 |= (1<<PCINT0);   //Enable PCINT18
    /* End PIN change IRQ */

    SPCR = (1<<SPIE) | (1<<SPE);
    /* Init LED pins */
    BLED_INIT();
    GLED_INIT();
    LED_INIT();

    /* Blink green led as boot indication */

    /* Enable system LED */
    LED_SET();
    
    /* Enable global interrupts */    
    STATUS_REGISTER |= (1<<STATUS_REGISTER_IT);
    
    /* Do we need to set MISO as output? */
    DDRB |= (1<<PB4);

    /* Set I2Q as output */
    //DDRD |= (1<<IRQ_PIN);

    /* Init 2PI Slave */
    while(1)
    {
        if (ipc_rcv_buf > 0x01 && ipc_rcv_buf < 20)
            LED_SET();
        else
            LED_CLR();
    }

    return 0;
}

