#include <system/memory.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <drivers/vga.h>

// --------------------------------------------------------------
//
// Private Structures
//
// --------------------------------------------------------------

typedef union location
{
    uint64_t Value;
    struct
    {
        uint32_t Low;
        uint32_t High;
    };
} location;

typedef struct e820_desc
{
    location BaseAddr;
    location Length;
    uint32_t Type;
} e820_desc;

typedef struct alloc
{
    struct alloc* PreviousAlloc;
    struct alloc* NextAlloc;
    uint8_t Reserved;
    uint64_t Size; 
} alloc;

// --------------------------------------------------------------
//
// Private Variables
//
// --------------------------------------------------------------

extern uint32_t     __e820_size;
extern e820_desc    __e820_data;
static e820_desc*   __e820_map;

// allocation map
static void* _memoryStart;
static alloc* _currentAlloc;

static char* available = "<available>";
static char* reserved = "<reserved>";

// --------------------------------------------------------------
//
// Private Functions
//
// --------------------------------------------------------------

static void _print_descriptor(const e820_desc* descriptor);

// splits a into two allocs, one of size and the other with the memory leftover
static uint32_t split(alloc* a, uint32_t size);

// first must occur in memory before second and they must be adjacent
static uint32_t merge(alloc* first, alloc* second);

// --------------------------------------------------------------
//
// Private Inlines
//
// --------------------------------------------------------------

static inline bool _memoryIsFree(const e820_desc* memory)
{
    return memory->Type == 1;
}

// --------------------------------------------------------------
//
// Function Definitions
//
// --------------------------------------------------------------

void memory_init()
{
    __e820_map = &__e820_data;
    printf("Loaded %u E820 Regions!\n", __e820_size);

    for(uint32_t i = 0; i < __e820_size; i ++)
    {
        e820_desc* region = __e820_map + i;

        _print_descriptor(region);
    }

    return;
}

// --------------------------------------------------------------
//
// Private Function Definitions
//
// --------------------------------------------------------------

static void _print_descriptor(const e820_desc* descriptor)
{
    printf(
        "Base: 0x%x%08x, Length: 0x%x%08x, Type: %d %s\n",
        descriptor->BaseAddr.High, descriptor->BaseAddr.Low,
        descriptor->Length.High, descriptor->Length.Low,
        descriptor->Type, (descriptor->Type == 1) ? available : reserved);
}

// splits a into two allocs, one of size and the other with the memory leftover
// returns 0 if successful
// returns 1 if the alloc is not large enough for the size
// returns 2 if the alloc is not large enough to be split
static uint32_t split(alloc* a, uint32_t size)
{
    if(a->Size < size) return 1;
    if(a->Size - size < sizeof(alloc)) return 2;

    alloc* next = a->NextAlloc;
    uint32_t split_size = a->Size - size - sizeof(alloc);
    a->NextAlloc = (alloc*)(((uint32_t)a) + sizeof(alloc) + size);
    a->NextAlloc->PreviousAlloc = a;
    a->NextAlloc->NextAlloc = next;
    a->NextAlloc->Reserved = false;
    a->NextAlloc->Size = split_size;
    a->Reserved = true;

    return 0;
}

// first must occur in memory before second and they must be adjacent
// returns 0 if successful
// returns 1 if either alloc is reserved
static uint32_t merge(alloc* first, alloc* second)
{
    if(first->Reserved || second->Reserved) return 1;

    first->Size += second->Size + sizeof(alloc);
    first->NextAlloc = second->NextAlloc;

    return 0;
}
