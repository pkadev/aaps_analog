#include <avr/io.h>
#include "ipc.h"
#include "mspim.h"
#include "m128_hal.h"

static uint8_t mspim_send(uint8_t xfer)
{
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = xfer;
    while(!(UCSR0A & (1<<TXC0)));
    while(!(UCSR0A & (1<<RXC0)));

    return UDR0;
}

static void dac_init(void)
{
    UBRR0 = 5;
    UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
    UCSR0C |= (1<<UMSEL01) | (1<<UMSEL00);
    UCSR0C &= ~(1<<UCPHA0);
    UCSR0C &= ~(1<<UDORD0);
    DDRD |= (1<<PD4) | (1<<PD1);
}

void write_voltage(uint8_t v_high, uint8_t v_low)
{
    dac_init();

    CS_DAC_CLR(CS_DAC_VOLT_PIN);
    mspim_send(v_high);
    mspim_send(v_low);
    CS_DAC_SET(CS_DAC_VOLT_PIN);

}

void write_current_limit(uint8_t i_high, uint8_t i_low)
{
    dac_init();

    CS_DAC_CLR(CS_DAC_STR_PIN);
    mspim_send(i_high);
    mspim_send(i_low);
    CS_DAC_SET(CS_DAC_STR_PIN);

}
