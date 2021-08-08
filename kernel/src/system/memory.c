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
static uint64_t totalMemory = 0;

static char* available = "<available>";
static char* reserved = "<reserved>";

// --------------------------------------------------------------
//
// Private Functions
//
// --------------------------------------------------------------

static void _print_descriptor(const e820_desc* descriptor);
static void _reserve_area(uint64_t base, uint64_t length);

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
    _reserve_area(0x7C00, 0xB8400);

    printf("Loaded %u E820 Regions!\n", __e820_size);

    _currentAlloc = NULL;
    alloc* first = NULL;

    for(uint32_t i = 0; i < __e820_size; i ++)
    {
        e820_desc* region = __e820_map + i;

        if(region->Type == 1)
        {
            if(_currentAlloc == NULL)
            {
                _currentAlloc = region->BaseAddr.Value;
                _currentAlloc->NextAlloc = NULL;
                first = _currentAlloc;
            }
            else
            {
                _currentAlloc->NextAlloc = region->BaseAddr.Value;
                _currentAlloc->PreviousAlloc = _currentAlloc;
                _currentAlloc = _currentAlloc->NextAlloc;
            }

            _currentAlloc->Reserved = false;
            _currentAlloc->Size = region->Length.Value - sizeof(alloc);
            totalMemory += _currentAlloc->Size;
        }

        _currentAlloc = first;

        _print_descriptor(region);
    }

    printf("%u Bytes of memory mapped for allocation!\n", totalMemory);

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

// shifts all the maps down the array starting at index
static void _shift_map(int index)
{
    for(uint32_t i = __e820_size; i > index; i --)
    {
        __e820_map[i] = __e820_map[i - 1];
    }
    __e820_size ++;
}

static void _remove_index(int index)
{
    for(uint32_t i = index; i < __e820_size - 1; i ++)
    {
        __e820_map[i] = __e820_map[i + 1];
    }

    __e820_size --;
}

static void _reserve_area(uint64_t base, uint64_t length)
{
    for(uint32_t i = 0; i < __e820_size; i ++)
    {
        e820_desc* region = __e820_map + i;

        // check if this is the region we are editing
        if(region->BaseAddr.Value + region->Length.Value > base)
        {
            // fix the length
            region->Length.Value = base - region->BaseAddr.Value;

            // shift the rest of the map to make room for the new region
            _shift_map(i + 1);

            // get the new regions pointer
            e820_desc* new_region = region + 1;

            // set the regions metadata
            new_region->BaseAddr.Value = base;
            new_region->Length.Value = length;
            new_region->Type = 2;

            // get the next address
            uint64_t next_address = new_region->BaseAddr.Value + new_region->Length.Value;

            // remove any entries the region fully overlaps
            e820_desc* next_region = new_region + 1;
            while(next_address > next_region->BaseAddr.Value + next_region->Length.Value && __e820_size > i + 1)
            {
                _remove_index(i + 2);
            }

            // check if the new region partially overlaps the next region and adjust it
            if(next_address > next_region->BaseAddr.Value)
            {
                next_region->Length.Value -= next_address - next_region->BaseAddr.Value;
                next_region->BaseAddr.Value = next_address;
            }
            // check if there is unmapped memory otherwise
            else if(next_address > next_region->BaseAddr.Value)
            {
                _shift_map(i + 2);
                next_region->BaseAddr.Value = next_address;
                next_region->Length.Value = (next_region + 1)->BaseAddr.Value - next_address;

                // this probably used to be part of the first region, so use that region's type
                next_region->Type = region->Type;
            }
            break;
        }
    }
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
    a->Size = size;

    return 0;
}

// first must occur in memory before second and they must be adjacent
// returns 0 if successful
// returns 1 if either alloc is reserved
// returns 2 if the allocs are non contiguous
static uint32_t merge(alloc* first, alloc* second)
{
    if(first->Reserved || second->Reserved) return 1;
    if(((void*)first) + first->Size + sizeof(alloc) != second) return 2;

    first->Size += second->Size + sizeof(alloc);
    first->NextAlloc = second->NextAlloc;

    return 0;
}

//MALLOC FUNCTION FOR ALLOCATING MEMORY BASED UPON THE SIZE PROVIDED
//NOT TESTED YET
//RETURNS IMMEDIATELY IF SPLIT FUNCITON DOES NOT RETURN A 0

void* malloc(uint64_t size)
{
    uint32_t result = split(_currentAlloc, size);
    
    if (result == 1 || result == 2)
    {
        return 0;
    }
    else
    {
        /*
        alloc* a;
        a->PreviousAlloc = _currentAlloc;
        a->NextAlloc = 0;
        a->Reserved = true;
        a->Size = size;
        
        //&a = &_currentAddress + sizeof(alloc) + _currentAlloc->Size;
        _currentAlloc->PreviousAlloc = a;
        _currentAlloc = a;
        */

       _currentAlloc = _currentAlloc->NextAlloc;

        return ((void*)_currentAlloc->PreviousAlloc) + sizeof(alloc);
    }

    return 0;
}

//GOTS A FEW QUESTIONS
void free(void* pointer)
{
    alloc* block = pointer - sizeof(alloc);

    block->Reserved = false;
}
