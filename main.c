#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "m128_hal.h"
#include "boot.h"
#include "ipc.h"

/*
 * SW1, aktiv låg, pullup, hittar du på PD2 (INT0/PCINT18)
 * SW2/IRQ, aktiv hög, pulldown, hittar du på PB0 (PCINT0/CLKO/ICP1) OBS SW2/IRQ delas med IRQ dvs du får IRQ på _D också om du trycker på SW2.
 */

volatile uint8_t num_temp_sensors = 0;
struct ipc_packet_t ipc_packet = {0};
volatile uint8_t read_ptr = 0;

ISR(PCINT2_vect) { /*If SW1 is configured as PCINT18 */ }

ISR(INT0_vect) /* SW1 */
{
    //LED_SET(); IRQ_SET();
    //_delay_ms(3);
    //IRQ_CLR(); LED_CLR();
}


ISR(PCINT0_vect) /* SW2 */
{
    /*
     * Only do stuff on one edge and debounce.
     * Button may need longer delay if extremely bouncy.
     */
    //if((SW2_IN & (1<<SW2_PIN)) == (1<<SW2_PIN)) {
    //    _delay_ms(20); //Debound delay
    //    if((SW2_IN & (1<<SW2_PIN)) == (1<<SW2_PIN)) {
    //        LED_SET();
    //        _delay_ms(10);
    //        LED_CLR();
    //    }
    //}
}

int main(void)
{
    if (boot() != AAPS_RET_OK)
        boot_failed(); //Boot has failed

    //spi_send(10);

    print_ipc("This is a welcome message from aaps_a!\n");

    while(1)
    {
        static bool critical_error = false;
        if (!critical_error)
        {
            if (packets_available) {

                ipc_save_packet(&ipc_packet, IPC_PACKET_LEN, read_ptr);
                read_ptr += IPC_PACKET_LEN;
                read_ptr %= IPC_RX_BUF_LEN;

                if (ipc_packet.cmd != 0x01 && ipc_packet.cmd != 0x07)
                    critical_error = true;
            }
        }
        else {
            print_ipc("Critical error\n");
            LED_CLR();
            while(1);
        }
        //ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        //{
        //    SPDR = num_temp_sensors;
        //}
    }

    return 0; //Should never get here
}

