#include <drivers/ata.h>
#include <stdint.h>
#include <stdbool.h>
#include <system/ports.h>
#include <stdio.h>
#include <system/sys_kern.h>
#include <system/memory.h>
#include <system/device.h>

// ---------------------------------------------------------------------
//
// Defines
//
// ---------------------------------------------------------------------

#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01

#define ATA_ER_BBK      0x80
#define ATA_ER_UNC      0x40
#define ATA_ER_MC       0x20
#define ATA_ER_IDNF     0x10
#define ATA_ER_MCR      0x08
#define ATA_ER_ABRT     0x04
#define ATA_ER_TK0NF    0x02
#define ATA_ER_AMNF     0x01

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define      ATAPI_CMD_READ       0xA8
#define      ATAPI_CMD_EJECT      0x1B

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

#define IDE_ATA        0x00
#define IDE_ATAPI      0x01
 
#define ATA_MASTER     0x00
#define ATA_SLAVE      0x01

#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

// Channels:
#define      ATA_PRIMARY      0x00
#define      ATA_SECONDARY    0x01
 
// Directions:
#define      ATA_READ      0x00
#define      ATA_WRITE     0x013

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15

// ---------------------------------------------------------------------
//
// Structures
//
// ---------------------------------------------------------------------

typedef struct ata_drive {
	uint8_t drive;
} ata_drive;

// ---------------------------------------------------------------------
//
// Private Variables
//
// ---------------------------------------------------------------------

static bool _ata_pm = false; // Primary master exists?
static bool _ata_ps = false; // Primary slave exists?
static bool _ata_sm = false; // Secondary master exists?
static bool _ata_ss = false; // Secondary slave exists?

static uint8_t* ata_buffer;

// ---------------------------------------------------------------------
//
// Private Function Declarations
//
// ---------------------------------------------------------------------

static void _primary_irq();
static void _secondary_irq();
static void _ata_select_drive(uint8_t bus, uint8_t i);
static bool _ata_identify(uint8_t bus, uint8_t drive);
static void _ata_probe();
static void _ata_read(uint8_t *buf, uint32_t lba, uint32_t numsects, device_t *dev);
static uint8_t _ata_read_one(uint8_t *buf, uint32_t lba, device_t *dev);
static void _ata_poll(uint16_t io);
static void _ata_delay400ns(uint16_t io);

// ---------------------------------------------------------------------
//
// Function Implementations
//
// ---------------------------------------------------------------------

/**
 * @brief installs the ata driver to the irq
 * 
 */
void ata_install()
{
	printf("Loading ATA Drives\n");
	ata_buffer = malloc(512);
	irq_install_handler(ATA_PRIMARY_IRQ, _primary_irq);
	irq_install_handler(ATA_SECONDARY_IRQ, _secondary_irq);
	_ata_probe();
}

// ---------------------------------------------------------------------
//
// Private Function Implementations
//
// ---------------------------------------------------------------------

static void _primary_irq(struct regs* registers)
{
	irq_send_eoi(ATA_PRIMARY_IRQ);
}

static void _secondary_irq(struct regs* registers)
{
	irq_send_eoi(ATA_SECONDARY_IRQ);
}

static void _ata_select_drive(uint8_t bus, uint8_t i)
{
	if(bus == ATA_PRIMARY)
		if(i == ATA_MASTER)
			outportb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xA0);
		else outportb(ATA_PRIMARY_IO + ATA_REG_HDDEVSEL, 0xB0);
	else
		if(i == ATA_MASTER)
			outportb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xA0);
		else outportb(ATA_SECONDARY_IO + ATA_REG_HDDEVSEL, 0xB0);
}

static bool _ata_identify(uint8_t bus, uint8_t drive)
{
	// setup drive and io information
	uint16_t io = 0;
	_ata_select_drive(bus, drive);
	if(bus == ATA_PRIMARY) io = ATA_PRIMARY_IO;
	else io = ATA_SECONDARY_IO;

	// ata requires these registers to be zeroed for identify
	outportb(io + ATA_REG_SECCOUNT0, 0);
	outportb(io + ATA_REG_LBA0, 0);
	outportb(io + ATA_REG_LBA1, 0);
	outportb(io + ATA_REG_LBA2, 0);

	// send identify command and read status
	outportb(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
	uint8_t status = inportb(io + ATA_REG_STATUS);

	if(status)
	{
		// poll until busy is cleared
		while(inportb(io + ATA_REG_STATUS) & ATA_SR_BSY != 0);

_status_read:
		status = inportb(io + ATA_REG_STATUS);

		if(status & ATA_SR_ERR)
			return 0;

		while(!(status & ATA_SR_DRQ)) goto _status_read;
		
		for(int i = 0; i<256; i++)
		{
			*(uint16_t *)(ata_buffer + i*2) = inportw(io + ATA_REG_DATA);
		}
	}
}

static void _ata_probe()
{
	if(_ata_identify(ATA_PRIMARY, ATA_MASTER))
	{
		_ata_pm = true;
		printf("Located Primary Master drive!\n");
	}
}

static void _ata_read(uint8_t *buf, uint32_t lba, uint32_t numsects, device_t *dev)
{
	for(int i = 0; i < numsects; i++)
	{
		ata_read_one(buf, lba + i, dev);
		buf += 512;
	}
}

static void _ata_delay400ns(uint16_t io)
{
	for(int i = 0;i < 4; i++)
		inportb(io + ATA_REG_ALTSTATUS);
}

static void _ata_poll(uint16_t io)
{
	
	for(int i=0; i< 4; i++)
		inportb(io + ATA_REG_ALTSTATUS);

retry:;
	// get drive status
	uint8_t status = inportb(io + ATA_REG_STATUS);
	// if busy retry
	if(status & ATA_SR_BSY) goto retry;
retry2:	status = inportb(io + ATA_REG_STATUS);
	// check for errors
	if(status & ATA_SR_ERR)
	{
		panic("ERR set, device failure!\n");
	}
	// check if drq is set
	if(!(status & ATA_SR_DRQ)) goto retry2;
	// data is ready
	return;
}

static uint8_t _ata_read_one(uint8_t *buf, uint32_t lba, device_t *dev)
{
	// check device we need to output to
	uint8_t drive = ((ata_drive*)(dev->_priv))->drive;
	uint16_t io = 0;
	switch(drive)
	{
		case (ATA_PRIMARY << 1 | ATA_MASTER):
			io = ATA_PRIMARY_IO;
			drive = ATA_MASTER;
			break;
		case (ATA_PRIMARY << 1 | ATA_SLAVE):
			io = ATA_PRIMARY_IO;
			drive = ATA_SLAVE;
			break;
		case (ATA_SECONDARY << 1 | ATA_MASTER):
			io = ATA_SECONDARY_IO;
			drive = ATA_MASTER;
			break;
		case (ATA_SECONDARY << 1 | ATA_SLAVE):
			io = ATA_SECONDARY_IO;
			drive = ATA_SLAVE;
			break;
		default:
			printf("FATAL: unknown drive!\n");
			return 0;
	}

	// get the cmd and slave bit
	uint8_t cmd = (drive==ATA_MASTER?0xE0:0xF0);
	uint8_t slavebit = (drive == ATA_MASTER?0x00:0x01);

	// pass the drive information and read params to the ata regs
	outportb(io + ATA_REG_HDDEVSEL, (cmd | (uint8_t)((lba >> 24 & 0x0F))));
	outportb(io + 1, 0x00);
	outportb(io + ATA_REG_SECCOUNT0, 1);
	outportb(io + ATA_REG_LBA0, (uint8_t)((lba)));
	outportb(io + ATA_REG_LBA1, (uint8_t)((lba) >> 8));
	outportb(io + ATA_REG_LBA2, (uint8_t)((lba) >> 16));
	outportb(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

	// poll the drive to get the status
	// locks up the os if it fails
	_ata_poll(io);

	// read in one sector
	for(int i = 0; i < 256; i++)
	{
		uint16_t data = inportw(io + ATA_REG_DATA);
		*(uint16_t *)(buf + i * 2) = data;
	}
	_ata_delay400ns(io);
	return 1;
}