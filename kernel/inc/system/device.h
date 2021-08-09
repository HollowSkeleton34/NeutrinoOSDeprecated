#ifndef __SYSTEM_DEVICE
#define __SYSTEM_DEVICE

#include <stdint.h>

typedef enum __device_type
{
    DEVICE_UNKNOWN = 0,
    DEVICE_CHAR = 1,
    DEVICE_BLOCK = 2
} device_type;

typedef struct __device_t
{
    char* name;
    uint32_t uuid;
    device_type dtype;
    uint8_t (*read)(uint8_t* buffer, uint32_t offset, uint32_t len, void* device);
    uint8_t (*write)(uint8_t* buffer, uint32_t offset, uint32_t len, void* device);
    void* _priv;
} device_t;



#endif