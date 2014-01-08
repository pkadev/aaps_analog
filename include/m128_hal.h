#ifndef M128_HAL_H__
#define M128_HAL_H__
#include <avr/io.h>
/* TODO: Rename this file to m48 or something */
/*
 * AVR specific defines
 */


/*
 * System defines
 */
#define STATUS_REGISTER         SREG
#define STATUS_REGISTER_IT      SREG_I

#define RELAY_D_ID              0x00
#define RELAY_ID                0x01
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


/* CLIND defines */
#define CLIND_PIN 3
#define CLIND_IN PIND
#define CLIND_PORT PORTD
#define CLIND_DDR DDRD

/* SW1 Defines */
#define SW1_PORT PORTD
#define SW1_DDR DDRD
#define SW1_IN PIND
#define SW1_PIN 2
#define SW1_WAIT_UNTILL_PRESSED() while((SW1_IN & (1<<SW1_PIN))==(1<<SW1_PIN)){}
/* end SW1 defines */

/* SW2 Defines */
#define SW2_PORT PORTB
#define SW2_DDR DDRB
#define SW2_IN PINB
#define SW2_PIN 0
#define SW2_WAIT_UNTILL_PRESSED() while((SW2_IN & (1<<SW2_PIN))==0){}
/* end SW2 defines */

/* DAC defines */
#define CS_DAC_STR_PIN 3
#define CS_DAC_VOLT_PIN 4
#define DAC_DDR DDRC
#define DAC_PORT PORTC

#define CS_DAC_INIT(pin) (DAC_DDR|=(1<<pin))
#define CS_DAC_SET(pin) (DAC_PORT|=(1<<pin))
#define CS_DAC_CLR(pin) (DAC_PORT&=~(1<<pin))
/* End DAC defines */

/* Zero STR defines */
#define ZERO_STR_PIN 1
#define ZERO_STR_DDR DDRC
#define ZERO_STR_PORT PORTC
#define ZERO_STR_INIT() (ZERO_STR_DDR |= (1<<ZERO_STR_PIN))
#define ZERO_STR_SET() (ZERO_STR_PORT |= (1<<ZERO_STR_PIN))
#define ZERO_STR_CLR() (ZERO_STR_PORT &= ~(1<<ZERO_STR_PIN))
/* End Zero STR defines */

/* Zero Volt defines */
#define ZERO_VOLT_PIN 2
#define ZERO_VOLT_DDR DDRC
#define ZERO_VOLT_PORT PORTC
#define ZERO_VOLT_INIT() (ZERO_VOLT_DDR |= (1<<ZERO_VOLT_PIN))
#define ZERO_VOLT_SET() (ZERO_VOLT_PORT |= (1<<ZERO_VOLT_PIN))
#define ZERO_VOLT_CLR() (ZERO_VOLT_PORT &= ~(1<<ZERO_VOLT_PIN))
/* End Zero STR defines */

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

/* IRQ Defines */
#define IRQ_PORT PORTB
#define IRQ_DDR DDRB
#define IRQ_IN PINB
#define IRQ_PIN 0
#define IRQ_INIT() (IRQ_DDR|=(1<<IRQ_PIN))
#define IRQ_SET()  (IRQ_PORT|=(1<<IRQ_PIN))
#define IRQ_CLR()  (IRQ_PORT&=~(1<<IRQ_PIN))
/* End IRQ Defines */
/*
 * LED defines
 */
#define LED_PIN 5
#define LED_DDR DDRD
#define LED_PORT PORTD

#define LED_INIT() (LED_DDR|=(1<<LED_PIN))
#define LED_SET() (LED_PORT|=(1<<LED_PIN))
#define LED_TOGGLE() (LED_PORT^=(1<<LED_PIN))
#define LED_CLR() (LED_PORT&=~(1<<LED_PIN))

#endif
