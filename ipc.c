#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
//#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include "ipc.h"
#include "m128_hal.h"
#include "max1168.h"

#define SPI_WAIT() while(!(SPSR & (1<<SPIF)))

volatile uint8_t ipc_rcv_buf = 0;
volatile uint8_t rx_buf[IPC_RX_BUF_LEN] = {0};
volatile uint8_t write_ptr = 0;
char sendbuffer[40] = {0};

ISR(SPI_STC_vect)
{
    static uint8_t internal_cnt = 0;
    ipc_rcv_buf = SPDR;
    rx_buf[write_ptr++] = SPDR;
    write_ptr %= IPC_RX_BUF_LEN;
    internal_cnt++;

    if (internal_cnt == IPC_PACKET_LEN) {
        packets_available++;
        internal_cnt = 0;
        //LED_TOGGLE();
    }
}

volatile uint8_t packets_available = 0;

void send_ipc_temp(ow_temp_t *temp)
{
    sendbuffer[0] = IPC_DATA_THERMO;
    sendbuffer[1] = temp->temp;
    sendbuffer[2] = temp->dec;
    sendbuffer[3] = '\0';
    print_ipc(sendbuffer);
}

void send_ipc_adc_value(uint16_t adc_value, enum ipc_data_type_t type)
{
    sendbuffer[0] = type;
    sendbuffer[1] = (adc_value >> 8);
    sendbuffer[2] = (adc_value & 0xff);
    sendbuffer[3] = '\0';
    print_ipc(sendbuffer);
}

/*
 * TODO: Find out if we need both signed and unsigned version
 * of this function. Or better send raw data here and use one
 * byte to tell if it's signed or unsigned.
 */
void print_ipc_int(const char *str, unsigned int integer)
{
    char buf[17]; //(sizeof(int)*8+1) All integers fit his on radix=2 systems
    uint8_t str_len = strlen(str);
    uint8_t buf_len;
    sendbuffer[0] = IPC_DATA_ASCII;
    memcpy(sendbuffer+1, str, str_len);

    ltoa(integer, buf, 10);
    buf_len = strlen(buf);

    memcpy(sendbuffer + 1 + str_len, buf, buf_len);
    memset(sendbuffer + 1 + str_len + buf_len, '\n', 1);
    memset(sendbuffer + 1 + str_len + buf_len + 1, 0, 1);

    print_ipc(sendbuffer);
}
void print_ipc(const char *str)
{
    uint8_t len;
    uint8_t i;
    SPCR &= ~(1<<SPIE);
    len = strlen(str);

    /* Put CMD in SPI data buffer */
    SPDR = ~IPC_CMD_DATA_AVAILABLE;

    /* Signal to master that CMD is available */
    IRQ_SET();
    SPI_WAIT();
    IRQ_CLR();

    /* Tell master how many bytes to fetch */
    SPDR = ~len;
    SPI_WAIT();
    for(i = 0; i < len; i++) {
        SPDR = ~(str[i]);
        SPI_WAIT();
    }

    i = SPSR;
    i = SPDR;
    SPCR |= (1<<SPIE);
    /*
     * TODO: This must be here if you print two times in a row.
     * Need to figure out why this is!
     */
    _delay_ms(10);
}

void ipc_save_packet(struct ipc_packet_t *dst, size_t len, uint8_t read_ptr)
{
    dst->cmd = *(rx_buf + (read_ptr++ % IPC_RX_BUF_LEN));
    dst->len = *(rx_buf + (read_ptr++ % IPC_RX_BUF_LEN));
    dst->data[0] = *(rx_buf + (read_ptr++ % IPC_RX_BUF_LEN));
    dst->data[1] = *(rx_buf + (read_ptr++ % IPC_RX_BUF_LEN));
    dst->crc = *(rx_buf + (read_ptr++ % IPC_RX_BUF_LEN));

    packets_available--;
}
