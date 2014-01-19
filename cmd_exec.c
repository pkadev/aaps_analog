#include "core.h"
#include "cmd_exec.h"
#include "m128_hal.h"
#include "max1168.h"

static bool is_current_meas(enum max1168_channel_t ch)
{
    if (ch == ADC_CH2 || ch == ADC_CH6)
        return true;
    else
        return false;
}

aaps_result_t cmd_exec_get_temp(struct ipc_packet_t *packet)
{
    /*
     * TODO: Add const qualifier on ow_devise. The address
     * of the them can never change.
     */
    ow_temp_t temp;
    ow_ret_val_t ret = IPC_RET_ERROR_GENERIC;
    ret = get_scratch_pad_async(&(ow_get_sensors()[packet->data[0]]), &temp);

    ow_convert_temp_async(&(ow_get_sensors()[packet->data[0]]));
    if (ret != OW_RET_OK)
        return AAPS_RET_ERROR_GENERAL;
    else
    {
        core_send_ipc_temp(&temp, packet->data[0]);
        return AAPS_RET_OK;
    }
}

aaps_result_t cmd_exec_get_adc(struct ipc_packet_t *packet)
{
    uint8_t type;
    uint16_t adc_val;
    aaps_result_t res = AAPS_RET_ERROR_GENERAL;
    int8_t ch = packet->data[0];

    struct ADC_t adc =
    {
        .channel = ch,
        .clk = MAX1168_CLK_EXTERNAL,
        .mode = MAX1168_MODE_8BIT,
    };

    res = max1168_read_adc(&adc, &adc_val);

    if (res == AAPS_RET_OK) {
        type = is_current_meas(ch) ? IPC_DATA_CURRENT : IPC_DATA_VOLTAGE;
        core_send_ipc_adc_value(adc_val, type, ch);
    }
    return res;
}

aaps_result_t cmd_exec_ctrl_relay(struct ipc_packet_t *packet,
                                  uint8_t relay_id)
{
    if (relay_id == RELAY_D_ID)
    {
        if (packet->data[0])
            RELAY_D_SET();
        else
            RELAY_D_CLR();
    }
    else if (relay_id == RELAY_ID)
    {
        if (packet->data[0])
            RELAY_SET();
        else
            RELAY_CLR();
    }

    return AAPS_RET_OK;
}

static aaps_result_t resp_periph_detec(struct ipc_packet_t *packet)
{
    core_send_periph_info();
    return AAPS_RET_OK;
}
aaps_result_t core_handle_ipc_pkt(struct ipc_packet_t *pkt)
{
    aaps_result_t res = AAPS_RET_OK;
    switch(pkt->cmd)
    {
        case IPC_CMD_PERIPH_DETECT:
            resp_periph_detec(pkt);
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
