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

ISR(INT1_vect)
{
    print_ipc("CLIND %u\n", (PIND & (1<<PD3)) >> PD3);
    if (PIND & (1<<PD3))
        LED_SET();
    else
        LED_CLR();
}

ISR(INT0_vect) /* SW1 */
{
    //LED_SET(); IRQ_SET();
    //_delay_ms(3);
    //IRQ_CLR(); LED_CLR();
}

static uint8_t mspim_send(uint8_t xfer)
{
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = xfer;
    while(!(UCSR0A & (1<<TXC0)));
    while(!(UCSR0A & (1<<RXC0)));

    return UDR0;
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

    RELAY_SET();
    print_ipc("This is a welcome message from aaps_a!\n");

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
                    case IPC_CMD_SET_VOLTAGE:
                    {
                        uint16_t data = (ipc_packet.data[1] << 8) | ipc_packet.data[0];
                        print_ipc("[AAPS_A] VOLTAGE %u\n", data);
                        print_ipc("[AAPS_A] MSB: 0x%02X LSB: 0x%02X\n",
                                  ipc_packet.data[1], ipc_packet.data[0]);
                        CS_DAC_VOLT_CLR();
                        mspim_send((data >> 8));
                        mspim_send(data & 0xFF);
                        CS_DAC_VOLT_SET();
                    }
                    break;
                    case IPC_CMD_SET_CURRENT_LIMIT:
                    {
                        uint16_t data = (ipc_packet.data[1] << 8) | ipc_packet.data[0];
                        print_ipc("[AAPS_A] I_LIMIT %u\n", data);
                        print_ipc("[AAPS_A] MSB: 0x%02X LSB: 0x%02X\n",
                                  ipc_packet.data[1], ipc_packet.data[0]);
                        uint16_t current_limit = (ipc_packet.data[1] << 8) | ipc_packet.data[0];
                        CS_DAC_STR_CLR();
                        mspim_send((current_limit>>8));
                        mspim_send(current_limit & 0xFF);
                        CS_DAC_STR_SET();
                    }
                    break;
                    default:
                        print_ipc("Unknown packet type: 0x%02X\n", ipc_packet.cmd);
                }
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

