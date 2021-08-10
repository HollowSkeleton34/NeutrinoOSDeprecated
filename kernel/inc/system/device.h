#ifndef __SYSTEM_DEVICE
#define __SYSTEM_DEVICE

#include <stdint.h>

typedef enum __device_type
{
    DEVICE_UNKNOWN = 0,
    DEVICE_DRIVE_BLOCK = 1,
    DEVICE_DRIVE_CHAR = 2
} device_type;

typedef struct __device_t
{
    char name[32];
    uint32_t uuid;
    device_type dtype;
    uint8_t (*read)(uint8_t* buffer, uint32_t offset, uint32_t len, void* device);
    uint8_t (*write)(uint8_t* buffer, uint32_t offset, uint32_t len, void* device);
    void* _priv;
} device_t;

uint32_t device_add(device_t* device);
const device_t* device_get(uint32_t id);
uint32_t device_count();


#endif