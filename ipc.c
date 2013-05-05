#include <avr/io.h>
#include <util/delay.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <avr/interrupt.h>
#include "ipc.h"
#include "m128_hal.h"

volatile uint8_t ipc_rcv_buf = 0;
volatile uint8_t rx_buf[IPC_RX_BUF_LEN] = {0};
volatile uint8_t write_ptr = 0;

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

void print_ipc(const char *str, ...)
{
    va_list args;
    uint8_t len;
    uint8_t i;
    char printbuffer[80] = {0};

    SPCR &= ~(1<<SPIE);
    va_start (args, str);
    vsprintf (printbuffer, str, args);
    va_end (args);
    len = strlen(printbuffer);
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
        SPDR = ~(printbuffer[i]);
        SPI_WAIT();
    }

    i = SPSR;
    i = SPDR;
    SPCR |= (1<<SPIE);
    /*
     * TODO: This must be here if you print two times in a row.
     * Need to figure out why this is!
     */
    _delay_ms(25);
}

void ipc_save_packet(struct ipc_packet_t *dst, size_t len, uint8_t read_ptr)
{
    //uint8_t i = 0;
    //print_ipc("%u\n", read_ptr);
    //print_ipc("0x%x\n", *(src + read_ptr));
    dst->cmd = *(rx_buf + (read_ptr++ % IPC_RX_BUF_LEN));
    dst->len = *(rx_buf + (read_ptr++ % IPC_RX_BUF_LEN));
    dst->data[0] = *(rx_buf + (read_ptr++ % IPC_RX_BUF_LEN));
    dst->data[1] = *(rx_buf + (read_ptr++ % IPC_RX_BUF_LEN));
    dst->crc = *(rx_buf + (read_ptr++ % IPC_RX_BUF_LEN));

    packets_available--;
    LED_TOGGLE();

}
