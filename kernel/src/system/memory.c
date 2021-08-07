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
        uint32_t High;
        uint32_t Low;
    };
} location;

typedef struct address_descriptor
{
    location BaseAddr;
    location Length;
    uint32_t Type;
    uint32_t ExtendedAttributes;
} address_descriptor;

typedef struct e820_table
{
    uint32_t Count;
    address_descriptor* Table;
} e820_table;

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

static address_descriptor _dt_table[16];
static e820_table _e820_table;

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

static e820_table* get_e820();
static void _print_descriptor(const address_descriptor* descriptor);

// splits a into two allocs, one of size and the other with the memory leftover
static uint32_t split(alloc* a, uint32_t size);

// first must occur in memory before second and they must be adjacent
static uint32_t merge(alloc* first, alloc* second);

// --------------------------------------------------------------
//
// Private Inlines
//
// --------------------------------------------------------------

static inline bool _memoryIsFree(const address_descriptor* memory)
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
    e820_table* table = get_e820();

    for(int i = 0; i < table->Count; i ++)
    {
        printf("Region %d | ", i);
        _print_descriptor(&table->Table[i]);
        putc('\n');
    }

    _memoryStart = table->Table[3].BaseAddr.Value;
    _currentAlloc = (alloc*)_memoryStart;
    _currentAlloc->NextAlloc = NULL;
    _currentAlloc->PreviousAlloc = NULL;
    _currentAlloc->Reserved = false;
    _currentAlloc->Size = table->Table[3].Length.Value - sizeof(alloc);

    printf("%u bytes of memory mapped for virtual memory!", _currentAlloc->Size);

    return;
}

// --------------------------------------------------------------
//
// Private Function Definitions
//
// --------------------------------------------------------------

static e820_table* get_e820()
{
    // Bios
    _dt_table[0].BaseAddr.Value = 0x00;
    _dt_table[0].Length.Value = 0x0000000000100000;
    _dt_table[0].Type = 2;
    _dt_table[0].ExtendedAttributes = 0;
    
    // RAM Area 1
    _dt_table[1].BaseAddr.Value = 0x0000000000100000;
    _dt_table[1].Length.Value = 0x0000000000E00000;
    _dt_table[1].Type = 1;
    _dt_table[1].ExtendedAttributes = 0;
    
    // Mapped hardware
    _dt_table[2].BaseAddr.Value = 0x0000000000F00000;
    _dt_table[2].Length.Value = 0x0000000000100000;
    _dt_table[2].Type = 2;
    _dt_table[2].ExtendedAttributes = 0;
    
    // RAM Area 2
    _dt_table[3].BaseAddr.Value = 0x01000000;
    _dt_table[3].Length.Value = 0x00000000BF000000;
    _dt_table[3].Type = 1;
    _dt_table[3].ExtendedAttributes = 0;

    // Mapped devices
    _dt_table[4].BaseAddr.Value = 0xC0000000;
    _dt_table[4].Length.Value =  0x0000000040000000;
    _dt_table[4].Type = 2;
    _dt_table[4].ExtendedAttributes = 0;

    // there usually is an entry for extended memory (past 4gb)
    // but we're in protected mode and we have no idea how much memory
    // the system has

    _e820_table.Count = 5;
    _e820_table.Table = _dt_table;

    return &_e820_table;
}

static void _print_descriptor(const address_descriptor* descriptor)
{
    printf(
        "Base: 0x%x%08x, Length: 0x%x%08x, Type: %d %s",
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

//MALLOC FUNCTION FOR ALLOCATING MEMORY BASED UPON THE SIZE PROVIDED
//NOT TESTED YET
//RETURNS IMMEDIATELY IF SPLIT FUNCITON DOES NOT RETURN A 0

void* malloc(uint64_t size)
{
    uint32_t result = split(currentAlloc, size);
    
    if (result == 1 || result == 2)
    {
        return;
    }
    else
    {
        alloc* a;
        a->PreviousAlloc = _currentAlloc;
        a->NextAlloc = 0;
        a->Reserved = true;
        a->Size = size;
        
        &a = &_currentAddress + sizeof(alloc) + _currentAlloc->Size;
        _currentAlloc->PreviousAlloc = a;
        _currentAlloc = a;
    }
}

//GOTS A FEW QUESTIONS
//void free(alloc* block)
//{
    //alloc* temp;
    
    //while(1)
    //{
        //if (block->
