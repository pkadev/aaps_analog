#include "core.h"
#include "boot.h"
#include "ipc.h"
#include "m128_hal.h"
#include "cmd_exec.h"
#include "mspim.h"


aaps_result_t core_send_ipc_adc_value(uint16_t val, uint8_t type, int8_t ch)
{
    /* TODO: Handle errors */
    struct ipc_packet_t pkt;
    uint8_t payload_len = 4;
    uint8_t total_len = payload_len + IPC_PKT_OVERHEAD;
    pkt.len = total_len;
    pkt.cmd = type;
    pkt.data = malloc(payload_len);

    if (pkt.data == NULL)
        return AAPS_RET_ERROR_OUT_OF_MEMORY;

    pkt.data[0] = type;
    pkt.data[1] = ch;
    pkt.data[2] = (val >> 8);
    pkt.data[3] = (val & 0xff);

    pkt.crc = crc8(pkt.data, payload_len);

    if (put_packet_in_tx_buf(&pkt) != AAPS_RET_OK)
    {
        return AAPS_RET_ERROR_GENERAL;
    }

    free(pkt.data);

    IRQ_SET();
    /* TODO: Find out if we need NOP here */
    IRQ_CLR();
    return AAPS_RET_OK;
}

aaps_result_t core_send_ipc_temp(ow_temp_t *temp, uint8_t sensor)
{
    /* TODO: Handle errors */
    struct ipc_packet_t pkt;
    uint8_t payload_len = 3;
    uint8_t total_len = payload_len + IPC_PKT_OVERHEAD;
    pkt.len = total_len;
    pkt.cmd = IPC_DATA_THERMO;
    pkt.data = malloc(payload_len);

    if (pkt.data == NULL)
        return AAPS_RET_ERROR_OUT_OF_MEMORY;

    pkt.data[0] = sensor;
    pkt.data[1] = temp->temp;
    pkt.data[2] = temp->dec;

    pkt.crc = crc8(pkt.data, payload_len);

    if (put_packet_in_tx_buf(&pkt) != AAPS_RET_OK)
    {
        return AAPS_RET_ERROR_GENERAL;
    }

    free(pkt.data);

    IRQ_SET();
    /* TODO: Find out if we need NOP here */
    IRQ_CLR();
    return AAPS_RET_OK;
}

aaps_result_t core_send_clind(uint8_t data)
{
    /* TODO: Handle errors */
    struct ipc_packet_t pkt;
    uint8_t payload_len = 2;
    uint8_t total_len = payload_len + IPC_PKT_OVERHEAD;
    pkt.len = total_len;
    pkt.cmd = IPC_DATA_CLIND;
    pkt.data = malloc(payload_len);

    if (pkt.data == NULL)
        return AAPS_RET_ERROR_OUT_OF_MEMORY;

    pkt.data[0] = data;
    pkt.data[1] = ilimit_active;

    pkt.crc = crc8(pkt.data, payload_len);

    if (put_packet_in_tx_buf(&pkt) != AAPS_RET_OK)
    {
        return AAPS_RET_ERROR_GENERAL;
    }

    free(pkt.data);

    IRQ_SET();
    /* TODO: Find out if we need NOP here */
    IRQ_CLR();
    return AAPS_RET_OK;
}

aaps_result_t core_send_periph_info(void)
{
    /* TODO: Handle errors */
    struct ipc_packet_t pkt;
    uint8_t payload_len = 3;
    uint8_t total_len = payload_len + IPC_PKT_OVERHEAD;
    uint16_t device_id = read_device_id();

    pkt.len = total_len;
    pkt.cmd = IPC_DATA_PERIPH_DETECT;
    pkt.data = malloc(payload_len);

    if (pkt.data == NULL)
        return AAPS_RET_ERROR_OUT_OF_MEMORY;

    pkt.data[0] = device_id << 8;
    pkt.data[1] = device_id & 0xFF;
    pkt.data[2] = ow_get_num_sensors();

    pkt.crc = crc8(pkt.data, payload_len);

    if (put_packet_in_tx_buf(&pkt) != AAPS_RET_OK)
    {
        return AAPS_RET_ERROR_GENERAL;
    }

    free(pkt.data);

    IRQ_SET();
    /* TODO: Find out if we need NOP here */
    IRQ_CLR();
    return AAPS_RET_OK;
}
