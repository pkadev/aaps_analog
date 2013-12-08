#include <avr/io.h>
#include <avr/interrupt.h>
#include "m128_hal.h"
#include "boot.h"
#include "ipc.h"
#include "mspim.h"
#include "max1168.h"
#include "cmd_exec.h"
#include "core.h"

struct ipc_packet_t ipc_packet = {0};

ISR(PCINT2_vect) { /*If SW1 is configured as PCINT18 */ }

ISR(INT1_vect)
{
 //   print_ipc_int("CLIND ",(PIND & (1<<PD3)) >> PD3);
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
    //print_ipc_int("[A] Hi from channel id: ", 1);

    struct ipc_packet_t ipc_pkt;


    while(1)
    {
        /* Handle IPC traffic */
        if (ipc_transfer(&ipc_pkt) == IPC_RET_OK)
        {
            if(packets_pending())
            {
                core_handle_ipc_pkt(&ipc_pkt);
                /*
                 * Maybe this should be called from
                 * the core_handle_ipc_pKt() funciton
                 */
                ipc_reduce_pkts_pending(&ipc_pkt);
            }
        }
        else
        {
            /*TODO: Add error handling */
            //lcd_write_string("IPC Error");
            while(1);
        }

                    RELAY_D_SET();

    }
    return 0; //Should never get here
}

