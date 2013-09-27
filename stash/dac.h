#ifndef DAC_H__
#define DAC_H__

#include "mspim.h"

struct spi_dev_t
{
    void (*enable)(void);
    void (*disable)(void);
    void (*init)(void);
    uint8_t (*xfer)(uint8_t tx);
};
void set_current_limit(uint16_t limit);
void set_output_voltage(uint16_t voltage);
uint8_t dac_write(struct spi_dev_t *dev, uint16_t value);

extern struct spi_dev_t dac_I;
extern struct spi_dev_t dac_V;
#endif
