#ifndef MAX1168_H_
#define MAX1168_H_

#include <stdint.h>
#include "mspim.h"

#define SPI_DUMMY_BYTE          0xff

enum max1168_channel {
    CH0,
    CH1
};
enum max1168_clk {
    MAX1168_CLK_EXTERNAL,
    MAX1168_CLK_INTERNAL
};

enum max1168_mode {
    MAX1168_MODE_16BIT,
    MAX1168_MODE_8BIT
};

uint16_t max1168_mean(enum max1168_channel ch, uint8_t samples);
uint16_t max1168_read_adc(uint8_t reg, enum max1168_clk clk, enum max1168_mode mode);

extern struct spi_dev_t max1168;
#endif
