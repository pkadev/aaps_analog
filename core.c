#include "core.h"
#include "boot.h"
#include "ipc.h"
#include "m128_hal.h"
#include "cmd_exec.h"

aaps_result_t core_handle_ipc_pkt(struct ipc_packet_t *pkt)
{
    aaps_result_t res = AAPS_RET_OK;
    switch(pkt->cmd)
    {
        case IPC_CMD_PERIPH_DETECT:
            //print_ipc("[A] P detect\n");
            break;
        case IPC_CMD_SET_VOLTAGE:
            //write_voltage(pkt->data[1], pkt->data[0]);
            break;
        case IPC_CMD_SET_CURRENT_LIMIT:
            //write_current_limit(pkt->data[1], pkt->data[0]);
            break;
        case IPC_CMD_GET_ADC:
            cmd_exec_get_adc(pkt);
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








