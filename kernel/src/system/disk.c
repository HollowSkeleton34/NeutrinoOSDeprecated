#include <stdint.h>
#include <stdbool.h>

typedef struct drive
{
    uint32_t Heads;
    uint32_t Tracks;
    uint32_t SectorsPerTrack;
} drive;

extern uint32_t __drive_count;
extern drive __drive_data;

static drive* drives;

void load_drives()
{
    drives = &__drive_data;
    printf("Loaded %x drives!\n", __drive_count);

    for(uint32_t d = 0; d < __drive_count; d ++)
    {
        drive* drive_data = drives+d;

        printf("Loaded Drive %u: H: %u, T: %u, SPT: %u\n", d + 0x80, drive_data->Heads, drive_data->Tracks, drive_data->SectorsPerTrack);
    }
}