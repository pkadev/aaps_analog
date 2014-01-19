#ifndef IPC_H__
#define IPC_H__
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "boot.h"

#define IPC_RX_BUF_LEN 62
#define IPC_PKT_OVERHEAD 3  /* Len, cmd, crc */


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
    IPC_CMD_PUT_DATA,
    IPC_CMD_DISPLAY_ADC,
    IPC_CMD_DISPLAY_VOLTAGE,
    IPC_CMD_DISPLAY_CURRENT,
    IPC_CMD_DISPLAY_THERMO,
    IPC_CMD_DISPLAY_DAC,
    IPC_CMD_DISPLAY_POWER,
    IPC_CMD_SET_LED,
};

enum ipc_data_type_t
{
    IPC_DATA_VOLTAGE = 0x7B,
    IPC_DATA_CURRENT = 0x7C,
    IPC_DATA_ASCII = 0x7D,
    IPC_DATA_THERMO = 0x7E,
    IPC_DATA_ENC_CW = 0x7F,
    IPC_DATA_ENC_CCW = 0x80,
    IPC_DATA_ENC_BTN = 0x81,
    IPC_DATA_DAC = 0x82,
    IPC_DATA_ENC_LONGPRESS = 0x83,
    IPC_DATA_ENC_SW0 = 0x84,
    IPC_DATA_ENC_SW1 = 0x85,
    IPC_DATA_ENC_SW2 = 0x86,
    IPC_DATA_CLIND = 0x87,
    IPC_DATA_PERIPH_DETECT = 0x88,
};

struct ipc_packet_t
{
    uint8_t len;
    uint8_t cmd;
    uint8_t crc;
    uint8_t *data;
};

typedef enum
{
    IPC_RET_OK,
    IPC_RET_ERROR_GENERIC,
    IPC_RET_ERROR_BAD_PARAMS,
    IPC_RET_ERROR_TARGET_DEAD,
    IPC_RET_ERROR_RX,
    IPC_RET_ERROR_TX,
    IPC_RET_ERROR_NOT_SUPPORTED,
    IPC_RET_ERROR_PUT_SYNC,
    IPC_RET_ERROR_GET_SYNC,
    IPC_RET_ERROR_PUT_FINALIZE,
    IPC_RET_ERROR_GET_FINALIZE,
    IPC_RET_ERROR_TX_BUF_EMPTY,
    IPC_RET_ERROR_OUT_OF_MEMORY,
    IPC_RET_ERROR_CRC_FAIL,
} ipc_ret_t;

/* IPC primitives functions */
void ipc_init(void);
uint8_t packets_pending();
ipc_ret_t ipc_transfer();
void ipc_reduce_pkts_pending(struct ipc_packet_t *pkt);
aaps_result_t put_packet_in_tx_buf(struct ipc_packet_t *pkt);

#endif
