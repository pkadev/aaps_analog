#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "m128_hal.h"
#include "boot.h"
#include "mspim.h"
#include "ipc.h"

static void enable_ext_irq();
static void enable_pcint18();
static void enable_pcint0();
static void enable_pcint2();

/* These two should probably go somewhere else */
static void spi_init(void);
/***********************************************/
aaps_result_t boot(void)
{
    /* Enable global interrupts */
    STATUS_REGISTER |= (1<<STATUS_REGISTER_IT);

    ZERO_VOLT_INIT();
    ZERO_STR_INIT();
    IRQ_INIT();
    RELAY_INIT();
    LED_INIT();
    CS_DAC_INIT(CS_DAC_STR_PIN);
    CS_DAC_INIT(CS_DAC_VOLT_PIN);

    ZERO_STR_SET();
    ZERO_VOLT_SET();

    enable_ext_irq();
    enable_pcint2();
    if(0)
    {
        enable_pcint18();
        enable_pcint0();
    }
    spi_init();
    ipc_init();
    aaps_result_t ret = AAPS_RET_OK;
    RELAY_D_INIT();

    return ret;
}

uint16_t read_device_id(void)
{
   return eeprom_read_word(0x00);
}
static void spi_init(void)
{
    //DDRB |= (1<<PB3); //This is MOSI as output?? Remove??
    SPCR = (1<<SPE) | (1<<CPOL);
    /* Do we need to set MISO as output? */
    DDRB |= (1<<PB4);
}

static void enable_pcint2()
{
    /* IRQ for SS on SPI to detect when SPI is busy */
    /* PIN change IRQ */
    PCICR |= (1<<PCIE0);      //Enable PCINT0 (PCINT0..7)
    PCMSK0 |= (1<<PCINT2);   //Enable PCINT2
    /* End PIN change IRQ */
}

void boot_failed(void)
{
    while(1) {
        LED_TOGGLE();
        _delay_ms(250);
    }
}
static void enable_ext_irq()
{
    EICRA &= ~(1<<ISC11);
    EICRA |= (1<<ISC10);        //IRQ on any edge
    EIMSK |= (1<<INT1);
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
