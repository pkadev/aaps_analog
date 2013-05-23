#include <avr/io.h>
#include "max1168.h"
#include "mspim.h"
#include <util/delay.h>

/* Hardware dependent defines */
#define MAX1168_CS_DDR          DDRC
#define MAX1168_CS_PIN          PC0
#define MAX1168_CS_PORT         PORTC
#define EOC_PIN                 PIND

#define EOC                     PD6

/* End hardware dependent defines */

#define CS_LOW()    (MAX1168_CS_PORT &= ~(1<<MAX1168_CS_PIN))
#define CS_HIGH()   (MAX1168_CS_PORT |= (1<<MAX1168_CS_PIN))

static void max1168_init(void)
{
    UBRR0 = 15;
    UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
    UCSR0C |= (1<<UMSEL01) | (1<<UMSEL00);
    UCSR0C &= ~(1<<UCPHA0);
    //UCSR0C |= (1<<UCPOL0);

    MAX1168_CS_DDR |= (1<<MAX1168_CS_PIN);
    DDRD |= (1<<PD4) | (1<<PD1);
    CS_HIGH();
}

//static uint8_t max1168_xfer_byte(uint8_t tx)
//{
//    while(!(UCSR0A & (1<<UDRE0)));
//    UDR0 = tx;
//    while(!(UCSR0A & (1<<TXC0)));
//    while(!(UCSR0A & (1<<RXC0)));
//
//    return UDR0;
//}
uint8_t mspim_send2(uint8_t xfer)
{
//    dac_init();
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = xfer;
    while(!(UCSR0A & (1<<TXC0)));
    while(!(UCSR0A & (1<<RXC0)));

    return UDR0;
}

uint16_t max1168_mean(enum max1168_channel ch, uint8_t samples)
{
    uint32_t tmp = 0;
    uint8_t i;
#define MAX_ALLOWED_SAMPLES 10
    if (samples > MAX_ALLOWED_SAMPLES)
        return 0;

    for (i = 0; i < samples; i++) {
       tmp += max1168_read_adc(SPI_DUMMY_BYTE, MAX1168_CLK_EXTERNAL, MAX1168_MODE_8BIT);
    }
    tmp /= samples;
    return (uint16_t)tmp;
}

uint16_t max1168_read_adc(uint8_t reg, enum max1168_clk clk, enum max1168_mode mode)
{
    #define CH2 0x40
    #define CH1 0x20
    #define CH0 0x00
    uint16_t raw_data;
    max1168_init();

    //if (mode == MAX1168_MODE_8BIT)
    //    PORTB &= ~(1<<DSEL);
    //else
    //    PORTB |= (1<<DSEL);

    CS_LOW();
    //_delay_ms(10);
    /* TODO: remove hardcoded channel */
    mspim_send2(CH0 | 0);

    //if (clk == MAX1168_CLK_INTERNAL)
    //    while ((EOC_PIN & (1<<EOC)));

    raw_data = mspim_send2(SPI_DUMMY_BYTE) << 8;
    raw_data |= mspim_send2(SPI_DUMMY_BYTE);
    CS_HIGH();

    return raw_data;
}