#ifndef MSPIM_H__
#define MSPIM_H__
#include <stdint.h>

void write_voltage(uint8_t v_high, uint8_t v_low);
void write_current_limit(uint8_t i_high, uint8_t i_low);

#endif
