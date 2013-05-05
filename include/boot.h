#ifndef BOOT_H__
#define BOOT_H__

typedef enum
{
    AAPS_RET_OK,
    AAPS_RET_ERROR_GENERAL,
} aaps_result_t;

aaps_result_t boot(void);
void boot_failed(void);
#endif
