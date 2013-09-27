#include <avr/io.h>
#include "dac.h"
#include "ipc.h"
#include "m128_hal.h"

static void dac_v_enable(void);
static void dac_v_disable(void);
static void dac_i_enable(void);
static void dac_i_disable(void);
static void dac_init(void);

struct spi_dev_t dac_I =
{
    .enable = dac_v_enable,
    .disable = dac_v_disable,
    .init = dac_init,
    .xfer = mspim_send,
};

struct spi_dev_t dac_V =
{
    .enable = dac_i_enable,
    .disable = dac_i_disable,
    .init = dac_init,
    .xfer = mspim_send,
};

static void dac_init(void)
{
    print_ipc("DAC Init\n");
    UBRR0 = 5;
    UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
    UCSR0C |= (1<<UMSEL01) | (1<<UMSEL00);
    UCSR0C &= ~(1<<UCPHA0);
    UCSR0C &= ~(1<<UDORD0);
    DDRD |= (1<<PD4) | (1<<PD1);
    CS_DAC_VOLT_INIT();
    CS_DAC_STR_INIT();
    CS_DAC_VOLT_SET();
    CS_DAC_STR_SET();
}

static void dac_v_enable(void)
{
    CS_DAC_VOLT_CLR();
}
static void dac_v_disable(void)
{
    CS_DAC_VOLT_SET();
}
static void dac_i_enable(void)
{
    CS_DAC_STR_CLR();
}
static void dac_i_disable(void)
{
    CS_DAC_STR_SET();
}

uint8_t dac_write(struct spi_dev_t *dev, uint16_t value)
{
    print_ipc("dac_write\n");
    dev->enable();
    dev->xfer(value >> 8);
    dev->xfer(value & 0xff);
    dev->disable();
    return 0;
}
