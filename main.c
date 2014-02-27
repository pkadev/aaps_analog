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
uint8_t volatile ilimit_active = 0;
volatile uint8_t clind = 0;
volatile uint8_t clind_steady = 0;
ISR(INT1_vect)
}
    clind_steady = 1;
}

ISR(TIMER1_OVF_vect)
{
    ilimit_active = 3;
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

    TCCR1B |= (1 << CS11) | (1 << CS10);
    if (ow_init() != OW_RET_OK)
        while(1);

    while(1)
    {
        /* Check CLIND */
        switch (read_device_id())
        {
            case 0x0001:

                if (clind_steady)
                {
                    if ((CLIND_IN & (1<<CLIND_PIN)))
                    {
                        core_send_clind(1);
                    }
                    else
                    {
                        core_send_clind(0);
                    }
                    clind_steady = 0;
                }
            break;
            case 0x0002:

                if ((CLIND_IN & (1<<CLIND_PIN)))
                {
                    ilimit_active = 2;
                    TCNT1L = 0xff;
                    TCNT1H = 0x8f;
                }
                if (ilimit_active == 3)
                {
                    ilimit_active = 1;
                    clind = 0;
                    core_send_clind(0);
                    TIMSK1 &= ~(1<<TOIE1);
                }
                if (ilimit_active == 2)
                {
                    TIFR1 |= (1<<TOV1);
                    TIMSK1 = (1<<TOIE1);
                if (!clind)
                {
                    clind = 1;
                    core_send_clind(1);
                }
            break;
        }

    }

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
            //for(;;);
        }
    }
    return 0; //Should never get here
}

