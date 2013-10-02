#ifndef MAX1168_H_
#define MAX1168_H_

#include <stdint.h>
#include "mspim.h"
#include "boot.h"

#define SPI_DUMMY_BYTE          0xff

enum max1168_channel_t {
    ADC_CH0, //VM30_0       - Main high voltage measurment.
    ADC_CH1, //VM6_1        - Main low voltage measurment.
    ADC_CH2, //CM1_5        - Main current measurment.
    ADC_CH3, //RELAY_S_7    - Measures if main high voltage relya is on?
    ADC_CH4, //VM30S_2      - Sense high voltage measurment.
    ADC_CH5, //VM6_3        - TODO: Not connected in HW? Robert explain!
    ADC_CH6, //CM2_6        - Reference current measurement.
    ADC_CH7, //VM30R_4      - Reference high voltage measurement.
};
enum max1168_clk_t {
    MAX1168_CLK_EXTERNAL,
    MAX1168_CLK_INTERNAL
};

enum max1168_mode_t {
    MAX1168_MODE_16BIT,
    MAX1168_MODE_8BIT
};

struct ADC_t
{
    enum max1168_channel_t channel;
    enum max1168_clk_t clk;
    enum max1168_mode_t mode;
};

aaps_result_t max1168_read_adc(struct ADC_t *adc, uint16_t *adc_result);

#endif
