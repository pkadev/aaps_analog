#ifndef IPC_H__
#define IPC_H__
#include <stdint.h>
#include <stdlib.h>

#define SPI_WAIT() while(!(SPSR & (1<<SPIF)))

#define IPC_RX_BUF_LEN 160
#define IPC_DATA_LEN 2
/* IPC Commands */
enum ipc_command_t
{
    IPC_CMD_SUPPORTED_CMDS,
    IPC_CMD_PERIPH_DETECT,
    IPC_CMD_NUM_OF_CMDS,
    IPC_CMD_GET_TEMP,
    IPC_CMD_GET_VOLTAGE,
    IPC_CMD_GET_CURRENT,
    IPC_CMD_DATA_AVAILABLE,
    IPC_CMD_SET_VOLTAGE,
    IPC_CMD_SET_CURRENT_LIMIT,
    IPC_CMD_SET_RELAY_D,
    IPC_CMD_SET_RELAY,
};

struct ipc_packet_t
{
    uint8_t cmd;
    uint8_t len;
    uint8_t data[IPC_DATA_LEN];
    uint8_t crc;
};
extern volatile uint8_t packets_available;
void print_ipc(const char *str, ...);
void ipc_save_packet(struct ipc_packet_t *dst, size_t len, uint8_t read_ptr);
#define IPC_PACKET_LEN sizeof(struct ipc_packet_t)

#endif
