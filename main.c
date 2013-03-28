#include <stddef.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "1wire.h"

/*
 * System defines
 */
#define STATUS_REGISTER         SREG
#define STATUS_REGISTER_IT      SREG_I
/*
 * Defines for Watchdog
 */
#define WD_CTRL_REG             WDTCSR
#define WD_CHANGE_ENABLE        WDCE
#define WD_IT_ENABLE_MASK       WDIE
#define WD_ENABLE               WDE
#define WD_PRESCALER2           WDP2
#define WD_PRESCALER1           WDP1
#define WD_PRESCALER0           WDP0

/*
 * UART0 defines
 */
#define UART_DATA_REG           UDR0
#define UART_BAUD_RATE_REG_LOW  UBRR0L

#define CS_MSK 0x04 //What the hell are you?
#define CLK_MSK 0x20 //What the hell are you?

/* DAC Current defines */
#define CS_DAC_STR_PIN 1
#define CS_DAC_STR_DDR DDRC
#define CS_DAC_STR_PORT PORTC
#define CS_DAC_STR_INIT() (CS_DAC_STR_DDR|=(1<<CS_DAC_STR_PIN))
#define CS_DAC_STR_SET() (CS_DAC_STR_PORT|=(1<<CS_DAC_STR_PIN))
#define CS_DAC_STR_CLR() (CS_DAC_STR_PORT&=~(1<<CS_DAC_STR_PIN))
/* End DAC current defines */

/* DAC Voltage defines */
#define CS_DAC_VOLT_PIN 2
#define CS_DAC_VOLT_DDR DDRC
#define CS_DAC_VOLT_PORT PORTC
#define CS_DAC_VOLT_INIT() (CS_DAC_VOLT_DDR|=(1<<CS_DAC_VOLT_PIN))
#define CS_DAC_VOLT_SET() (CS_DAC_VOLT_PORT|=(1<<CS_DAC_VOLT_PIN))
#define CS_DAC_VOLT_CLR() (CS_DAC_VOLT_PORT&=~(1<<CS_DAC_VOLT_PIN))
/* End DAC Voltage define */

/* Zero STR defines */
#define ZERO_STR_PIN 3
#define ZERO_STR_DDR DDRC
#define ZERO_STR_PORT PORTC
#define ZERO_STR_INIT() (ZERO_STR_DDR |= (1<<ZERO_STR_PIN))
#define ZERO_STR_SET() (ZERO_STR_PORT |= (1<<ZERO_STR_PIN))
#define ZERO_STR_CLR() (ZERO_STR_PORT &= ~(1<<ZERO_STR_PIN))
/* End Zero STR defines */

/* Zero Volt defines */
#define ZERO_VOLT_PIN 4
#define ZERO_VOLT_DDR DDRC
#define ZERO_VOLT_PORT PORTC
#define ZERO_VOLT_INIT() (ZERO_VOLT_DDR |= (1<<ZERO_VOLT_PIN))
#define ZERO_VOLT_SET() (ZERO_VOLT_PORT |= (1<<ZERO_VOLT_PIN))
#define ZERO_VOLT_CLR() (ZERO_VOLT_PORT &= ~(1<<ZERO_VOLT_PIN))
/* End Zero STR defines */

/**/
/* Relay Defines */
#define RELAY_PIN 5
#define RELAY_DDR DDRC
#define RELAY_PORT PORTC
#define RELAY_INIT() (RELAY_DDR|=(1<<RELAY_PIN))
#define RELAY_SET() (RELAY_PORT|=(1<<RELAY_PIN))
#define RELAY_TOGGLE() (RELAY_PORT^=(1<<RELAY_PIN))
#define RELAY_CLR() (RELAY_PORT&=~(1<<RELAY_PIN))
/* End relay defines */

/* Relay Defines */
#define RELAY_D_PIN 7
#define RELAY_D_DDR DDRD
#define RELAY_D_PORT PORTD
#define RELAY_D_INIT() (RELAY_D_DDR|=(1<<RELAY_D_PIN))
#define RELAY_D_SET() (RELAY_D_PORT|=(1<<RELAY_D_PIN))
#define RELAY_D_TOGGLE() (RELAY_D_PORT^=(1<<RELAY_D_PIN))
#define RELAY_D_CLR() (RELAY_D_PORT&=~(1<<RELAY_D_PIN))
/* End Relay Defines */

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
#define SW1_WAIT_UNTILL_RELEASED() while((SW1_IN & (1<<SW1_PIN))!=(1<<SW1_PIN)){}
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

uint16_t voltage = 0;
uint16_t current_limit = 0;

enum modes {
    MODE_1,
    MODE_2,
    MODE_3,
    MODE_4,
    MODE_5,
    NUMBER_OF_MODES
};
volatile uint8_t mode = MODE_1;
volatile uint8_t sw1_pushed = 0;
volatile uint8_t sw2_pushed = 0;
ISR(PCINT2_vect)
{
    //If SW1 is configured as PCINT18
}

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
    if((SW2_IN & (1<<SW2_PIN)) == (1<<SW2_PIN)) {
        _delay_ms(20); //Debounce delay
        while((SW2_IN & (1<<SW2_PIN)) == (1<<SW2_PIN)) {
            //LED_SET();
            //_delay_ms(100);
            //LED_CLR();
            //_delay_ms(100);
        }
        sw2_pushed = 1;
    }
}

static void toggle_mode(void)
{
    mode++;
    mode = mode % NUMBER_OF_MODES;
}

ISR(INT0_vect) /* SW1 */
{
    #define BREAK_TIME 200
    uint8_t delay = 0;
    _delay_ms(30);
    while ((SW1_IN & (1<<SW1_PIN)) != (1<<SW1_PIN)) {
        _delay_ms(10);
        if(delay++ == BREAK_TIME) break;
    }

    if (delay >= BREAK_TIME)
    {
        toggle_mode();
        LED_SET();
        _delay_ms(100);
        LED_CLR();
    } else {
        sw1_pushed = 1;
    }
}

ISR(INT1_vect) //CLIND IRQ
{
    uint8_t j;
    for(j = 0; j < 3; j++) {
        LED_SET();
        _delay_ms(10);
        LED_CLR();
        _delay_ms(50);
    }

}

/* Debug function */
static void enable_ext_irq()
{
    /* Ext IRQ 0 */
    EICRA |= (1<<ISC01);        //IRQ on falling edge
    EIMSK |= (1<<INT0);         //Enable INT0
    /* end Ext IRQ 0 */

    /* Ext IRQ 1 */
    EICRA |= (1<<ISC11) | (1<<ISC10);       //IRQ on rising edge
    EIMSK |= (1<<INT1);                     //Enable INT1
    /* End Ext IRQ 1 */
}

static void enable_pcint18() //This is an alternative to INT0. They are on the same pin
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

#define SPI_WAIT() while((SPSR & (1<<SPIF)) != (1<<SPIF))
static void init_spi(void)
{
    UBRR0 = 150; 
    UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
    UCSR0C |= (1<<UMSEL01) | (1<<UMSEL00) | (1<<UCPOL0);
    DDRD |= (1<<PD4);
}

static uint8_t spi_send(uint8_t xfer)
{
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = xfer;
    return UDR0;
}

int main(void)
{
    _delay_ms(200);

    //SPCR = (1<<SPIE) | (1<<SPE) | (1<<CPHA);
    enable_ext_irq();
    enable_pcint18(); 
    enable_pcint0();
    init_spi();
    /* Init LED pins */
    RELAY_D_INIT();
    RELAY_INIT();
    LED_INIT();
    ZERO_VOLT_INIT();
    ZERO_STR_INIT();

    //IRQ_INIT();
    CS_DAC_STR_INIT();
    CS_DAC_STR_SET();
    CS_DAC_STR_CLR();
    _delay_ms(100);
    CS_DAC_STR_SET();

    /* ZERO DAC is active low */
    ZERO_STR_SET();
    ZERO_VOLT_SET();

    /*
     * Find out how many 1wire devices that are
     * connected to the bus. Blink LED as many times.
     */
    uint8_t num_dev =  ow_num_devices();
    uint8_t i;
    for (i = 0; i < num_dev; i++) {
        LED_SET();
        _delay_ms(200);
        LED_CLR();
        _delay_ms(400);
    }

    /* Enable global interrupts */    
    STATUS_REGISTER |= (1<<STATUS_REGISTER_IT);

    /* Do we need to set MISO as output? */
    DDRB |= (1<<PB4);
#define DAC_STEPS 100
    while(1)
    {
        if(sw1_pushed)
        {
            switch (mode)
            {
                case MODE_1:
                    current_limit += DAC_STEPS;
                    LED_SET();
                    CS_DAC_STR_CLR();
                        spi_send(current_limit & 0x0F);
                        spi_send((current_limit>>8));
                    CS_DAC_STR_SET();
                    _delay_ms(10);
                    LED_CLR();
                break;
                case MODE_2:
                    voltage += DAC_STEPS;
                    RELAY_D_SET();
                    CS_DAC_VOLT_CLR();
                    CS_DAC_VOLT_SET();
                    _delay_ms(10);
                    RELAY_D_CLR();
                break;
                case MODE_3:
                    RELAY_D_SET();
                break;
                case MODE_4:
                    RELAY_SET();
                break;
                case MODE_5:
                    ZERO_VOLT_CLR();
                    _delay_us(1);
                    ZERO_VOLT_SET();
                break;
            }
            sw1_pushed = 0;
        }

        if(sw2_pushed)
        {
            switch (mode)
            {
                case MODE_1:
                    current_limit -= DAC_STEPS;
                    CS_DAC_STR_CLR();
                    /* Add code to send two bytes to DAC_VOLT */
                        spi_send(current_limit & 0x0F);
                        spi_send((current_limit>>8));
                    CS_DAC_STR_SET();
                    LED_SET();
                    _delay_ms(10);
                    LED_CLR();
                    _delay_ms(100);
                    LED_SET();
                    _delay_ms(10);
                    LED_CLR();
                break;
                case MODE_2:
                    voltage -= DAC_STEPS;
                    CS_DAC_VOLT_CLR();
                    /* Add code to send two bytes to DAC_VOLT */
                        spi_send(voltage & 0x0F);
                        spi_send((voltage>>8));
                    CS_DAC_VOLT_SET();
                    RELAY_D_SET();
                    _delay_ms(10);
                    RELAY_D_CLR();
                    _delay_ms(100);
                    RELAY_D_SET();
                    _delay_ms(10);
                    RELAY_D_CLR();
                break;
                case MODE_3:
                    RELAY_D_CLR();
                break;
                case MODE_4:
                    RELAY_CLR();
                break;
                case MODE_5:
                    ZERO_STR_CLR();
                    _delay_us(1);
                    ZERO_STR_SET();
                break;
            }
            sw2_pushed = 0;
        }

        //ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        //{
        //    SPDR = num_temp_sensors;
        //}
    }

    return 0;
}
