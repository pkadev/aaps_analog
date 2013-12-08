#include "core.h"
#include "boot.h"
#include "ipc.h"
#include "m128_hal.h"
#include "cmd_exec.h"
#include "mspim.h"

aaps_result_t core_handle_ipc_pkt(struct ipc_packet_t *pkt)
{
    aaps_result_t res = AAPS_RET_OK;
    switch(pkt->cmd)
    {
        case IPC_CMD_PERIPH_DETECT:
            //print_ipc("[A] P detect\n");
            break;
        case IPC_CMD_SET_VOLTAGE:
            write_voltage(pkt->data[1], pkt->data[0]);
            break;
        case IPC_CMD_SET_CURRENT_LIMIT:
            write_current_limit(pkt->data[1], pkt->data[0]);
            break;
        case IPC_CMD_GET_ADC:
            cmd_exec_get_adc(pkt);
            break;
        case IPC_CMD_GET_TEMP:
            cmd_exec_get_temp(pkt);
            break;
        case IPC_CMD_SET_RELAY_D:
            cmd_exec_ctrl_relay(pkt, RELAY_D_ID);
            break;
        case IPC_CMD_SET_RELAY:
            cmd_exec_ctrl_relay(pkt, RELAY_ID);
            break;
        default:
            //print_ipc_int("[A] Unknown ipc command 0x", ipc_packet->cmd);
            return AAPS_RET_ERROR_GENERAL;
        break;
    }

   return res;
}

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









