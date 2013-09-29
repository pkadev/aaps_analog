#include "cmd_exec.h"
#include "m128_hal.h"

#define THERMO_SENSOR_0     0x00
#define THERMO_SENSOR_1     0x01

static bool is_current_meas(enum max1168_channel_t ch)
{
    if (ch == ADC_CH2 || ch == ADC_CH6)
        return true;
    else
        return false;
}

aaps_result_t cmd_exec_get_temp(struct ipc_packet_t *packet)
{
    /*
     * TODO: Add const qualifier on ow_devise. The address
     * of the them can never change.
     */
    ow_temp_t temp;
    ow_device_t sensor0 =
    { .addr = { 0x28, 0xFD, 0x92, 0x28, 0x01, 0x00, 0x00, 0xD5 } };
    ow_device_t sensor1 =
    { .addr = { 0x28, 0x6F, 0x38, 0x9B, 0x01, 0x00, 0x00, 0x9D } };

    if (packet->data[1] == THERMO_SENSOR_0)
        ow_read_temperature(&sensor0, &temp);
    else if (packet->data[1] == THERMO_SENSOR_1)
        ow_read_temperature(&sensor1, &temp);

    send_ipc_temp(&temp);
    return AAPS_RET_OK;
}

aaps_result_t cmd_exec_get_adc(struct ipc_packet_t *packet)
{
    uint8_t type;
    uint16_t adc_val[3];
	uint8_t num_meas = 3;
    int8_t ch = packet->data[1];

    if (ch >= ADC_CH0 && ch <= ADC_CH7)
    {
        while(num_meas--) {
            adc_val[num_meas] = max1168_read_adc(ch, MAX1168_CLK_EXTERNAL,
                                                 MAX1168_MODE_8BIT);
        }

        type = is_current_meas(ch) ? IPC_DATA_CURRENT : IPC_DATA_VOLTAGE;

        send_ipc_adc_value(adc_val[2], type);
        send_ipc_adc_value(adc_val[1], type);
        send_ipc_adc_value(adc_val[0], type);
    }
    return AAPS_RET_OK;
}

aaps_result_t cmd_exec_ctrl_relay(struct ipc_packet_t *packet,
                                  uint8_t relay_id)
{
    if (relay_id == RELAY_D_ID)
    {
        if (packet->data[1])
            RELAY_D_SET();
        else
            RELAY_D_CLR();
    }
    else if (relay_id == RELAY_ID)
    {
        if (packet->data[1])
            RELAY_SET();
        else
            RELAY_CLR();
    }

    return AAPS_RET_OK;
}
