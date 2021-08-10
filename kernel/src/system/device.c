#include <system/device.h>

static device_t _devices[64];
static uint32_t _current_id = 0;

uint32_t device_add(device_t* device)
{
    _devices[_current_id] = *device;
    _current_id ++;
    return _current_id;
}

const device_t* device_get(uint32_t id)
{
    return &_devices[id];
}

uint32_t device_count()
{
    return _current_id;
}