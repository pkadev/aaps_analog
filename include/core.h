#ifndef CORE_H__
#define CORE_H__

#include <stdint.h>
#include <stdlib.h>
#include "boot.h"
#include "ipc.h"
#include "1wire.h"


struct temperature_t
{
    uint8_t whole;
    uint8_t decimal;
};

aaps_result_t core_handle_ipc_pkt(struct ipc_packet_t *pkt);
aaps_result_t core_send_ipc_adc_value(uint16_t val, uint8_t type, int8_t ch);
aaps_result_t core_send_ipc_temp(ow_temp_t *temp, uint8_t sensor);
#endif

