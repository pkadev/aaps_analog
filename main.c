#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
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
#include "mspim.h"
#include "max1168.h"
#include "1wire.h"

/*
 * SW1, aktiv låg, pullup, hittar du på PD2 (INT0/PCINT18)
 * SW2/IRQ, aktiv hög, pulldown, hittar du på PB0 (PCINT0/CLKO/ICP1) OBS SW2/IRQ delas med IRQ dvs du får IRQ på _D också om du trycker på SW2.
 */

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
	uint8_t cnt;

    if (boot() != AAPS_RET_OK)
        boot_failed();

    write_current_limit(10, 0);
    write_voltage(0x15 , 0x10);

    //print_ipc("[A] Hi!\n");
    //_delay_ms(3000);

    ow_temp_t temp;

    ow_device_t sensor0 =
    { .addr =
      {
        0x28, 0xFD, 0x92, 0x28, 0x01, 0x00, 0x00, 0xD5
      }
    };
    ow_device_t sensor1 =
    { .addr =
      {
        0x28, 0x6F, 0x38, 0x9B, 0x01, 0x00, 0x00, 0x9D
      }
    };

    while(1)
    {
        static bool critical_error = false;
        if (!critical_error)
        {
            if (packets_available)
            {
                cnt = 3;
                ipc_save_packet(&ipc_packet, IPC_PACKET_LEN, read_ptr);
                read_ptr += IPC_PACKET_LEN;
                read_ptr %= IPC_RX_BUF_LEN;

                switch(ipc_packet.cmd)
                {
                    case IPC_CMD_GET_TEMP:
                    {
                        if (ipc_packet.data[1] == 0)
                        {
                            if (ow_read_temperature(&sensor0, &temp) == OW_RET_OK)
                            print_ipc("[A] Ch:%u %u.%u\n", ipc_packet.data[1],
                                      temp.temp, temp.dec);
                        }
                        else if (ipc_packet.data[1] == 1)
                        {
                            if (ow_read_temperature(&sensor1, &temp) == OW_RET_OK)
                            print_ipc("[A] Ch:%u %u.%u\n", ipc_packet.data[1],
                                      temp.temp, temp.dec);
                        }
                    } break;
                    case IPC_CMD_PERIPH_DETECT:
                    {
                        print_ipc("[A] P detect\n");
                    } break;
                    case IPC_CMD_SET_VOLTAGE:
                    {
                        write_voltage(ipc_packet.data[1],
                                      ipc_packet.data[0]);
                    } break;
                    case IPC_CMD_SET_CURRENT_LIMIT:
                    {
                        write_current_limit(ipc_packet.data[1],
                                            ipc_packet.data[0]);
                    } break;
                    case IPC_CMD_GET_VOLTAGE:
                    {
                        int8_t ch = ipc_packet.data[1];
                        if (ch >= ADC_CH0 && ch <= ADC_CH7)
                        {
                            uint16_t adc_val[3];
                            print_ipc("[A] Get voltage a_ch: %u\n",
                                      ipc_packet.data[1]);

                            while(cnt--)
                            {
                                adc_val[cnt] = max1168_read_adc(ch, MAX1168_CLK_EXTERNAL,
                                                           MAX1168_MODE_8BIT);
                            }
                            print_ipc("[A] ADC: %u\n", adc_val[2]);
                            print_ipc("[A] ADC: %u\n", adc_val[1]);
                            print_ipc("[A] ADC: %u\n", adc_val[0]);
                        }
                    } break;
                    case IPC_CMD_SET_RELAY_D:
                    {
                        //print_ipc("[A] Rel_d %u\n", ipc_packet.data[1]);
                        if (ipc_packet.data[1])
                            RELAY_D_SET();
                        else
                            RELAY_D_CLR();
                    } break;
                    case IPC_CMD_SET_RELAY:
                    {
                        //print_ipc("[A] rel\n");
                        if (ipc_packet.data[1])
                            RELAY_SET();
                        else
                            RELAY_CLR();

                    } break;
                    default:
                        print_ipc("[A] Unkn packet 0x%02X\n", ipc_packet.cmd);
                }
            }
        } else {
            print_ipc("[A]Critical error\n");
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

