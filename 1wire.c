#include <stdbool.h>
#include <stddef.h>
#include "1wire.h"
#include "m128_hal.h"

/*
 * One Wire Command defines
 */
#define OW_CMD55_MATCH_ROM 0x55
#define OW_CMD44_CONV_TEMP 0x44
#define OW_CMDCC_SKIP_ROM 0xCC
#define OW_CMDBE_READ_SCRATCHPAD 0xBE

#define DQ_PORT PORTB
#define DQ_DDR DDRB
#define DQ_PIN 1
#define DQ_IN PINB
#define DQ_MASK (1<<DQ_PIN)
#define DQ_LOW() DQ_PORT&=~(1<<DQ_PIN)
#define DQ_HIGH() DQ_PORT|=(1<<DQ_PIN)

#define CRC8INIT    0x00
#define CRC8POLY    0x18    //0X18 = X^8+X^5+X^4+X^0

#define OW_ROM_BIT_LEN 64
#define THERM_DECIMAL_STEPS_12BIT 625

// global search state
uint8_t LastDiscrepancy;
uint8_t LastFamilyDiscrepancy;
uint8_t LastDeviceFlag;
uint8_t crc8_var;
uint8_t ROM_NO[OW_ROM_BYTE_LEN];

static uint8_t ow_read_bit(void);
static void ow_write_bit(uint8_t bitval);
static void ow_write_byte(uint8_t val);
static uint8_t ow_read_byte(void);
static uint8_t crc8(uint8_t *data_in, uint8_t size);
static uint8_t ow_reset(void);
static uint16_t _round(uint16_t _x);

ow_ret_val_t ow_read_temperature(ow_device_t *ow_device, ow_temp_t *ow_temp)
{
    if (ow_device == NULL || ow_temp == NULL) {
        return OW_RET_FAIL;
    }

    ow_reset();
    ow_write_byte(OW_CMD55_MATCH_ROM);

    for (int8_t i=0; i<OW_ROM_BYTE_LEN; i++) {
        ow_write_byte(ow_device->addr[i]);
    }

    ow_write_byte(OW_CMD44_CONV_TEMP);
    while(ow_read_byte() == 0)
        ;

    ow_scratchpad_t scrpad;

    if (ow_read_scratchpad(ow_device, &scrpad) == OW_RET_OK) {
        ow_temp->temp = ((scrpad.data[1]&0x7) << 4) & 0x7f;
        ow_temp->temp |= (scrpad.data[0] & 0xF0) >> 4;
        ow_temp->dec = _round((scrpad.data[0] & 0x0F) * THERM_DECIMAL_STEPS_12BIT);
        return OW_RET_OK;
    }
    return OW_RET_FAIL;
}

ow_ret_val_t ow_read_scratchpad(ow_device_t *ow_device, ow_scratchpad_t *scratchpad)
{
    ow_reset();
    ow_write_byte(OW_CMD55_MATCH_ROM);
    for (int8_t i = 0; i < OW_ROM_BYTE_LEN; i++) {
        ow_write_byte(ow_device->addr[i]);
    }
    ow_write_byte(OW_CMDBE_READ_SCRATCHPAD);

    for (uint8_t i = 0; i < sizeof(ow_scratchpad_t); i++) {
        scratchpad->data[i] = ow_read_byte();
    }

    if (crc8(scratchpad->data, 8) != scratchpad->data[8]) {
        return OW_RET_FAIL;
    }
    return OW_RET_OK;
}

static uint8_t ow_reset(void)
{
    uint8_t ret_val = 0;
    DQ_DDR |= (1<<DQ_PIN);

    DQ_LOW();
    _delay_us(480);
    DQ_HIGH();
    DQ_DDR &= ~(1<<DQ_PIN);
    _delay_us(60);
    if ((DQ_PIN & DQ_MASK) == 0) {
        ret_val = 1;
    }
    _delay_us(240); 
    return ret_val;
}

static void ow_write_bit(uint8_t bitval)
{
    DQ_DDR |= (1<<DQ_PIN);
    DQ_LOW();
    if(bitval==1) {
        _delay_us(2);
        DQ_HIGH();
    }
    _delay_us(60);
    DQ_HIGH();
}

static void ow_write_byte(uint8_t val)
{
    _delay_us(1);
    uint8_t i;
    uint8_t tmp;
    for (i = 0; i < 8; i++) {
        tmp = val>>i;
        tmp &= 0x01;
        ow_write_bit(tmp);
    }
   _delay_us(100);
}

static uint8_t ow_read_bit(void)
{
    uint8_t val;
    _delay_us(1);
    DQ_DDR |= (1<<DQ_PIN);
    DQ_LOW();
    _delay_us(1);
    DQ_DDR &= ~(1<<DQ_PIN);
    DQ_PORT &= ~(1<<DQ_PIN);
    _delay_us(15);
    val = (DQ_IN & DQ_MASK);
    _delay_us(45);
    return (val>>DQ_PIN);
}

static uint8_t ow_read_byte(void)
{
    uint8_t value = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (ow_read_bit()) {
             value|=0x01<<i;
        }
        _delay_us(60);
    }
    return(value);
}

static uint8_t crc8(uint8_t *data_in, uint8_t size)
{
    uint8_t  crc, bit_counter, data, feedback_bit;
    crc = CRC8INIT;
    for (uint8_t i = 0; i != size; i++) {
        data = data_in[i];

        bit_counter = 8;
        do {
            feedback_bit = (crc ^ data) & 0x01;
            if ( feedback_bit == 0x01 ) {
                crc = crc ^ CRC8POLY;
            }
            crc = (crc >> 1) & 0x7F;
            if ( feedback_bit == 0x01 ) {
                crc = crc | 0x80;
            }
            data = data >> 1;
            bit_counter--;
        } while (bit_counter > 0);
    }
    return crc;
}

static uint16_t _round(uint16_t _x)
{
    uint16_t diff = _x;
    if (_x >= 100) {
        if (_x >= 10000) {
            _x /= 100;
        }
        if (_x < 1000) {
            _x *= 10;
        }
        diff = _x / 100;
        if (_x % 100 >= 50) {
            diff += 1;
        }
    }
    if (_x < 10) {
        diff *= 10;
    }
    return diff;
}
