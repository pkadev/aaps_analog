#ifndef IPC_H__
#define IPC_H__
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "1wire.h"
#include "max1168.h"
#include "boot.h"

#define IPC_RX_BUF_LEN 60
#define IPC_DATA_LEN 2
/* IPC Commands */
enum ipc_command_t
{
    IPC_CMD_SUPPORTED_CMDS,
    IPC_CMD_PERIPH_DETECT,
    IPC_CMD_NUM_OF_CMDS,
    IPC_CMD_GET_TEMP,
    IPC_CMD_GET_ADC,
    IPC_CMD_DATA_AVAILABLE,
    IPC_CMD_SET_VOLTAGE,
    IPC_CMD_SET_CURRENT_LIMIT,
    IPC_CMD_SET_RELAY_D,
    IPC_CMD_SET_RELAY,
};

enum ipc_data_type_t
{
    IPC_DATA_THERMO = 0x7E,
    IPC_DATA_VOLTAGE = 0x7B,
    IPC_DATA_CURRENT = 0x7C,
    IPC_DATA_ASCII = 0x7D,
};

struct ipc_packet_t
{
    uint8_t cmd;
    uint8_t len;
    uint8_t data[IPC_DATA_LEN];
    uint8_t crc;
};
extern volatile uint8_t packets_available;
void print_ipc(const char *str);
aaps_result_t ipc_handle_packet(struct ipc_packet_t *ipc_packet);
void print_ipc_int(const char *str, unsigned int integer);
void send_ipc_temp(ow_temp_t *temp);
void send_ipc_adc_value(uint16_t adc_value, enum ipc_data_type_t type);

#define IPC_PACKET_LEN sizeof(struct ipc_packet_t)

#endif
