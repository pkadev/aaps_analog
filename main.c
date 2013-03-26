#include <stddef.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "m128_hal.h"
#include "1wire.h"

#define CS_MSK 0x04 //What the hell are you?
#define CLK_MSK 0x20 //What the hell are you?

//REMOVE
#define RLED_PIN 6
#define RLED_DDR DDRD
#define RLED_PORT PORTD
#define RLED_INIT() (RLED_DDR|=(1<<RLED_PIN))
#define RLED_SET() (RLED_PORT|=(1<<RLED_PIN))
#define RLED_CLR() (RLED_PORT&=~(1<<RLED_PIN))
// END REMOVE

/* DAC Current defines */
#define CS_DAC_STR_PIN 1
#define CS_DAC_STR_DDR DDRC
#define CS_DAC_STR_PORT PORTC
#define CS_DAC_STR_INIT() (CS_DAC_STR_DDR|=(1<<CS_DAC_STR_PIN))
#define CS_DAC_STR_SET() (CS_DAC_STR_PORT|=(1<<CS_DAC_STR_PIN))
#define CS_DAC_STR_CLR() (CS_DAC_STR_PORT&=~(1<<CS_DAC_STR_PIN))

/* End DAC current defines */

#define RELAY_D_PIN 7
#define RELAY_D_DDR DDRD
#define RELAY_D_PORT PORTD

#define RELAY_D_INIT() (RELAY_D_DDR|=(1<<RELAY_D_PIN))
#define RELAY_D_SET() (RELAY_D_PORT|=(1<<RELAY_D_PIN))
#define RELAY_D_TOGGLE() (RELAY_D_PORT^=(1<<RELAY_D_PIN))
#define RELAY_D_CLR() (RELAY_D_PORT&=~(1<<RELAY_D_PIN))

#define LED_PIN 5
#define LED_DDR DDRD
#define LED_PORT PORTD

#define LED_INIT() (LED_DDR|=(1<<LED_PIN))
#define LED_SET() (LED_PORT|=(1<<LED_PIN))
#define LED_TOGGLE() (LED_PORT^=(1<<LED_PIN))
#define LED_CLR() (LED_PORT&=~(1<<LED_PIN))

/* CLIND defines */
#define CLIND_PIN 3
#define CLIND_PORT PORTD
#define CLIND_DDR DDRD

/* SW1 Defines */
#define SW1_PORT PORTD
#define SW1_DDR DDRD
#define SW1_IN PIND
#define SW1_PIN 2

#define SW1_WAIT_UNTILL_PRESSED() while((SW1_IN & (1<<SW1_PIN))==(1<<SW1_PIN)){}
/* end SW1 defines */

/* IRQ Defines */
#define IRQ_PORT PORTB
#define IRQ_DDR DDRB
#define IRQ_IN PINB
#define IRQ_PIN 0
#define IRQ_INIT() (IRQ_DDR|=(1<<IRQ_PIN))
#define IRQ_SET()  (IRQ_PORT|=(1<<IRQ_PIN))
#define IRQ_CLR()  (IRQ_PORT&=~(1<<IRQ_PIN))
/* End IRQ Defines */

/* SW2 Defines */
#define SW2_PORT PORTB
#define SW2_DDR DDRB
#define SW2_IN PINB
#define SW2_PIN 0

#define SW2_WAIT_UNTILL_PRESSED() while((SW2_IN & (1<<SW2_PIN))==0){}
/* end SW1 defines */

#define SPI_CLK PB5
/* 
 * SW1, aktiv låg, pullup, hittar du på PD2 (INT0/PCINT18)
 * SW2/IRQ, aktiv hög, pulldown, hittar du på PB0 (PCINT0/CLKO/ICP1) OBS SW2/IRQ delas med IRQ dvs du får IRQ på _D också om du trycker på SW2.
 */

volatile uint8_t ipc_rcv_buf = 0;
volatile uint8_t num_temp_sensors = 0;

ISR(SPI_STC_vect)
{
    ipc_rcv_buf = SPDR;
    RELAY_D_SET();
    _delay_ms(1);
    RELAY_D_CLR();
}

ISR(PCINT0_vect) /* SW2 */
{
    /*
     * Only do stuff on one edge and debounce.
     * Button may need longer delay if extremely bouncy.
     */
            LED_SET();
            _delay_ms(100);
            LED_CLR();

    //if((SW2_IN & (1<<SW2_PIN)) == (1<<SW2_PIN)) {
    //    _delay_ms(20); //Debound delay
    //    if((SW2_IN & (1<<SW2_PIN)) == (1<<SW2_PIN)) {
    //        LED_SET();
    //        _delay_ms(10);
    //        LED_CLR();
    //        _delay_ms(300);
    //        LED_SET();
    //        _delay_ms(10);
    //        LED_CLR();
    //    }
    //}
}

ISR(PCINT2_vect)
{
    //If SW1 is configured as PCINT18
}

ISR(INT0_vect) /* SW1 */
{
    LED_SET();
    IRQ_SET();
    _delay_ms(3);
    IRQ_CLR();
    LED_CLR();
}

/* Debug function */
static void enable_ext_irq0()
{
    /* Ext IRQ 0 */
    EICRA |= (1<<ISC01);        //IRQ on falling edge
    EIMSK |= (1<<INT0);         //Enable INT0
    /* end Ext IRQ 0 */
}

static void enable_pcint18()
{
    /* PIN change IRQ */
    //PCICR |= (1<<PCIE2);      //Enable PCINT2 (PCINT23..16)
    //PCMSK2 |= (1<<PCINT18);   //Enable PCINT18
    /* End PIN change IRQ */
}

static void enable_pcint0()
{
    /* PIN change IRQ */
    PCICR |= (1<<PCIE0);      //Enable PCINT0 (PCINT7..0)
    PCMSK0 |= (1<<PCINT0);   //Enable PCINT0 - Don't run this when pin is configured as IRQ
    /* End PIN change IRQ */
}

/* IPC Commands */
enum ipc_command_t
{
    IPC_CMD_SUPPORTED_CMDS,
    IPC_CMD_PERIPH_DETECT,
    IPC_CMD_NUM_OF_CMDS,
    IPC_CMD_GET_TEMP,
    IPC_CMD_GET_VOLTAGE,
    IPC_CMD_GET_CURRENT
};
#define SPI_WAIT() while((SPSR & (1<<SPIF)) != (1<<SPIF))
    #define IPC_CMD_DATA_AVAILABLE 0x10
static void print_ipc(const char *str)
{
    uint8_t len = strlen(str);
    SPCR &= ~(1<<SPIE);
    
    /* Put CMD in SPI data buffer */
    SPDR = IPC_CMD_DATA_AVAILABLE;

    /* Signal to master that CMD is available */
    IRQ_SET();
    SPI_WAIT();
    _delay_ms(10); //TODO: Used to debug synchronization. Remove!!
    IRQ_CLR();

    /* Tell master how many bytes to fetch */
    SPDR = len+1;

    _delay_ms(10); //TODO: Used to debug synchronization. Remove!!
    IRQ_SET();
    SPI_WAIT();
    _delay_ms(10); //TODO: Used to debug synchronization. Remove!!
    IRQ_CLR();

    for(uint8_t i = 0; i <= len; i++) {
        SPDR = str[i];
        _delay_ms(10); //TODO: Used to debug synchronization. Remove!!
        IRQ_SET();
        
        SPI_WAIT();
        _delay_ms(10); //TODO: Used to debug synchronization. Remove!!
        IRQ_CLR();
        
    }
 
    SPCR |= (1<<SPIE);
}
static void init_spi(void)
{
    UBRR0 = 150; 
    UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
    UCSR0C |= (1<<UMSEL01) | (1<<UMSEL00) | (1<<UCPOL0);
    DDRD |= (1<<PD4);
}

static uint8_t spi_send(uint8_t xfer)
{
    CS_DAC_STR_CLR();
    _delay_ms(50);
    UDR0 = xfer;
    _delay_ms(50);
    CS_DAC_STR_SET();
    if(UDR0 == 0)
        return 'R';
    return UDR0;
}

int main(void)
{
    _delay_ms(200);

    SPCR = (1<<SPIE) | (1<<SPE) | (1<<CPHA);
    enable_ext_irq0();
    enable_pcint18(); 
    enable_pcint0();
    init_spi();
    /* Init LED pins */
    RELAY_D_INIT();
    LED_INIT();
    RLED_INIT();
    IRQ_INIT();
    CS_DAC_STR_INIT();
    char str[10] = {0};
    /* Enable global interrupts */    
    STATUS_REGISTER |= (1<<STATUS_REGISTER_IT);
    uint8_t data1, data2;    
    /* Do we need to set MISO as output? */
    DDRB |= (1<<PB4);

    str[0] = 'C';
    str[1] = 0;
    while(1)
    {
        spi_send(0xCC);
        spi_send(0x55);
    }
    str[3] = '\0';
    if (data1 != 0)
        str[1] = data1;
    if (data1 != 0)
        str[2] = data2;
    print_ipc(str);
    _delay_ms(10);
_delay_ms(10);
    //print_ipc(str);//    RLED_SET();
 
    while(1)
    {
        if (ipc_rcv_buf == IPC_CMD_PERIPH_DETECT) {
                RLED_SET();
                SPDR = 0xDE;
                IRQ_SET();
                _delay_us(500);
                IRQ_CLR();
                RLED_CLR();
                ipc_rcv_buf = 0;
        }
        
        //ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        //{
        //    SPDR = num_temp_sensors;
        //}
    }

    return 0;
}

