#ifndef MSPIM_H__
#define MSPIM_H__
#include <stdint.h>

struct spi_dev_t
{
    void (*enable)(void);
    void (*disable)(void);
    void (*init)(void);
    uint8_t (*xfer)(uint8_t tx);
};

uint8_t mspim_send(uint8_t xfer);
void write_current_limit(uint16_t limit);

#endif
