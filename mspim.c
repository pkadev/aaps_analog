#include <avr/io.h>
#include "ipc.h"
#include "mspim.h"
#include "m128_hal.h"

static void dac_init(void)
{
    UBRR0 = 5;
    UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
    UCSR0C |= (1<<UMSEL01) | (1<<UMSEL00);
    UCSR0C &= ~(1<<UCPHA0);
    UCSR0C &= ~(1<<UDORD0);
    DDRD |= (1<<PD4) | (1<<PD1);
}

void write_current_limit(uint16_t limit)
{
    dac_init();

    CS_DAC_VOLT_CLR();
    mspim_send((limit >> 8));
    mspim_send(limit & 0xFF);
    CS_DAC_VOLT_SET();

}
uint8_t mspim_send(uint8_t xfer)
{
    dac_init();
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = xfer;
    while(!(UCSR0A & (1<<TXC0)));
    while(!(UCSR0A & (1<<RXC0)));

    return UDR0;
}

