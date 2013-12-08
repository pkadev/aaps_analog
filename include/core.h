#ifndef CORE_H__
#define CORE_H__

#include <stdint.h>
#include <stdlib.h>
#include "boot.h"
#include "ipc.h"


struct temperature_t
{
    uint8_t whole;
    uint8_t decimal;
};

aaps_result_t core_handle_ipc_pkt(struct ipc_packet_t *pkt);
#endif

