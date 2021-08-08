#include <stdint.h>
#include <stdbool.h>
#include <system/ports.h>

typedef struct drive
{
    uint32_t Heads;
    uint32_t Tracks;
    uint32_t SPT;
} drive;

extern uint32_t __drive_count;
extern drive __drive_data;

static drive* drives;
static uint32_t _lba_size = 0;

void load_drives()
{
    drives = &__drive_data;
    printf("Loaded %x drives!\n", __drive_count);

    for(uint32_t d = 0; d < __drive_count; d ++)
    {
        drive* drive_data = drives+d;

        _lba_size += 

        printf("Loaded Drive %u: H: %u, T: %u, SPT: %u\n", d + 0x80, drive_data->Heads, drive_data->Tracks, drive_data->SPT);
    }
}


void read_sector(uint8_t drive, uint8_t head, uint8_t track, uint32_t sector)
{
    outportb(0x1f6, 0x80 + drive); // set the drive
}