#ifndef ONE_WIRE_H__
#define ONE_WIRE_H__

#include <util/delay.h>

#define OW_ROM_BYTE_LEN 8

typedef enum {
    OW_RET_FAIL,
    OW_RET_OK,
    OW_RET_BUSY,
} ow_ret_val_t;

typedef struct {
    uint8_t addr[OW_ROM_BYTE_LEN];
}ow_device_t;

typedef struct {
    uint8_t data[9];
}ow_scratchpad_t;

typedef struct {
    uint8_t temp;
    uint8_t dec;
}ow_temp_t;
ow_device_t *ow_devices;
/* Long green  1-Wire D5 00 00 01 28 92 FD 28 */
/* Short green 1-Wire 9D 00 00 01 9B 38 6F 28 */

 /*
     * Public function for 1-Wire
 */
uint8_t ow_num_devices();
void ow_print_device_addr(ow_device_t *ow_device);
uint8_t crc8(uint8_t *data_in, uint8_t size);
ow_ret_val_t ow_init(void);
ow_device_t *ow_get_sensors(void);
ow_ret_val_t ow_read_temperature(ow_device_t *ow_device, ow_temp_t *ow_temp);
ow_ret_val_t ow_read_scratchpad(ow_device_t *rom, ow_scratchpad_t *scratchpad);
ow_ret_val_t ow_write_scratchpad(ow_device_t *ow_device, ow_scratchpad_t *scratchpad);
ow_ret_val_t ow_convert_temp_async(ow_device_t *ow_device);
ow_ret_val_t get_scratch_pad_async(ow_device_t *ow_device, ow_temp_t *ow_temp);
#endif

