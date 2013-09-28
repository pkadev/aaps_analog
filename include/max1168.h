#ifndef MAX1168_H_
#define MAX1168_H_

#include <stdint.h>
#include "mspim.h"

#define SPI_DUMMY_BYTE          0xff

enum max1168_channel_t {
    ADC_CH0, //VM30
    ADC_CH1, //VM6
    ADC_CH2,
    ADC_CH3,
    ADC_CH4,
    ADC_CH5,
    ADC_CH6,
    ADC_CH7,
};
enum max1168_clk {
    MAX1168_CLK_EXTERNAL,
    MAX1168_CLK_INTERNAL
};

enum max1168_mode {
    MAX1168_MODE_16BIT,
    MAX1168_MODE_8BIT
};

//uint16_t max1168_mean(enum max1168_channel_t channel, uint8_t samples);
uint16_t max1168_read_adc(enum max1168_channel_t channel, enum max1168_clk clk, enum max1168_mode mode);

extern struct spi_dev_t max1168;
#endif
