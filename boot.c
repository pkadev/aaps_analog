#include <avr/io.h>
#include <util/delay.h>
#include "m128_hal.h"
#include "boot.h"

static void enable_ext_irq0();
static void enable_pcint18();
static void enable_pcint0();

/* These two should probably go somewhere else */
static void init_mspim(void);
static void init_spi(void);
/***********************************************/
aaps_result_t boot(void)
{
    /* Enable global interrupts */
    STATUS_REGISTER |= (1<<STATUS_REGISTER_IT);

    IRQ_INIT();
    RELAY_D_INIT();
    LED_INIT();
    CS_DAC_STR_INIT();
    if(0)
    {
        enable_ext_irq0();
        enable_pcint18();
        enable_pcint0();
    }
    init_spi();
    init_mspim();
    aaps_result_t ret = AAPS_RET_OK;
    return ret;
}
static void init_spi(void)
{
    DDRB |= (1<<PB3); //This is MOSI as output?? Remove??
    SPCR = (1<<SPIE) | (1<<SPE) | (1<<CPOL);
    SPSR = (1<<SPI2X);
    /* Do we need to set MISO as output? */
    DDRB |= (1<<PB4);
}

static void init_mspim(void)
{
    UBRR0 = 150;
    UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
    UCSR0C |= (1<<UMSEL01) | (1<<UMSEL00) | (1<<UCPOL0);
    DDRD |= (1<<PD4);
}
void boot_failed(void)
{
    while(1) {
        LED_TOGGLE();
        _delay_ms(250);
    }
}
static void enable_ext_irq0()
{
    /* Ext IRQ 0 */
    EICRA |= (1<<ISC01);        //IRQ on falling edge
    EIMSK |= (1<<INT0);         //Enable INT0
    /* end Ext IRQ 0 */
}

static void enable_pcint18()
{
    /* PIN change IRQ */
    //PCICR |= (1<<PCIE2);      //Enable PCINT2 (PCINT23..16)
    //PCMSK2 |= (1<<PCINT18);   //Enable PCINT18
    /* End PIN change IRQ */
}

static void enable_pcint0()
{
    /* PIN change IRQ */
    PCICR |= (1<<PCIE0);      //Enable PCINT0 (PCINT7..0)
    PCMSK0 |= (1<<PCINT0);   //Enable PCINT0 - Don't run this when pin is configured as IRQ
    /* End PIN change IRQ */
}
