#ifndef CMD_EXEC_H__
#define CMD_EXEC_H__

#include "boot.h"
#include "1wire.h"
#include "ipc.h"

aaps_result_t cmd_exec_get_temp(struct ipc_packet_t *packet);
aaps_result_t cmd_exec_get_adc(struct ipc_packet_t *packet);
aaps_result_t cmd_exec_ctrl_relay(struct ipc_packet_t *packet,
                                  uint8_t relay_id);


#endif
