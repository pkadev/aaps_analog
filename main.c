#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "m128_hal.h"
#include "boot.h"
#include "ipc.h"
#include "mspim.h"
#include "max1168.h"
#include "cmd_exec.h"

struct ipc_packet_t ipc_packet = {0};
volatile uint8_t read_ptr = 0;

ISR(PCINT2_vect) { /*If SW1 is configured as PCINT18 */ }

ISR(INT1_vect)
{
    print_ipc_int("CLIND ",(PIND & (1<<PD3)) >> PD3);
    if (PIND & (1<<PD3))
        LED_SET();
    else
        LED_CLR();
}


int main(void)
{
    if (boot() != AAPS_RET_OK)
        boot_failed();

    /*
     * Read channel if from eeprom? Or say hello with type
     * of peripheral?
     */
    print_ipc_int("[A] Hi from channel id: ", 1);

    while(1)
    {
        static bool critical_error = false;
        if (!critical_error)
        {
            if (packets_available)
            {
                ipc_save_packet(&ipc_packet, IPC_PACKET_LEN, read_ptr);
                read_ptr += IPC_PACKET_LEN;
                read_ptr %= IPC_RX_BUF_LEN;

                switch(ipc_packet.cmd)
                {
                    case IPC_CMD_GET_TEMP:
                        cmd_exec_get_temp(&ipc_packet);
                        break;
                    case IPC_CMD_PERIPH_DETECT:
                        print_ipc("[A] P detect\n");
                        break;
                    case IPC_CMD_SET_VOLTAGE:
                        write_voltage(ipc_packet.data[1], ipc_packet.data[0]);
                        break;
                    case IPC_CMD_SET_CURRENT_LIMIT:
                        write_current_limit(ipc_packet.data[1], ipc_packet.data[0]);
                        break;
                    case IPC_CMD_GET_ADC:
                        cmd_exec_get_adc(&ipc_packet);
                        break;
                    case IPC_CMD_SET_RELAY_D:
                        cmd_exec_ctrl_relay(&ipc_packet, RELAY_D_ID);
                        break;
                    case IPC_CMD_SET_RELAY:
                        cmd_exec_ctrl_relay(&ipc_packet, RELAY_ID);
                        break;
                    default:
                        print_ipc_int("[A] Unknown ipc command 0x", ipc_packet.cmd);
                        read_ptr = 0;
                }
            }
        } else {
            print_ipc("[A]Critical error\n");
            LED_CLR();
            while(1);
        }
    }
    return 0; //Should never get here
}

