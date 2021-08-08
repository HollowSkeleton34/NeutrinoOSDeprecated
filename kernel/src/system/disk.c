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


void read_sectors(void* buffer, uint8_t drive, uint64_t lba, uint16_t sectors)
{
    outportb(0x1f2, (uint8_t)(sectors >> 8));
    outportb(0x1f3, (uint8_t)(lba >> 24));
    outportb(0x1f4, (uint8_t)(lba >> 32));
    outportb(0x1f5, (uint8_t)(lba >> 40));
    outportb(0x1f2, (uint8_t)(sectors));
    outportb(0x1f3, (uint8_t)(lba));
    outportb(0x1f4, (uint8_t)(lba >> 8));
    outportb(0x1f5, (uint8_t)(lba >> 16));
    outportb(0x1f6, drive);
    outportb(0x1f7, 0x24);

    while(true)
    {
        uint8_t value = inportb(0x1f7);

        // err or df set
        if(value & 0x21) return;


        if(value & 0x80 && value & 0x08)
        {
            break;
        }
    }
    
}