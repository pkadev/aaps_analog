#ifndef DAC_H__
#define DAC_H__

#include "mspim.h"

void set_current_limit(uint16_t limit);
void set_output_voltage(uint16_t voltage);
uint8_t dac_write(struct spi_dev_t *dev, uint16_t value);

extern struct spi_dev_t dac_I;
extern struct spi_dev_t dac_V;
#endif
