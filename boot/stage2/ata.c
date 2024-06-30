#include <port_io.h>

#define ATA_PRIMARY_IO_BASE 0x1F0
#define ATA_COMMAND_REGISTER 0x07
#define ATA_READ_SECTORS 0x20

void ata_read_sector() {

	unsigned int lba = 59;
	int sector_count = 26;

	unsigned short *buffer = (unsigned short *)0xB000;

	// Select the drive and head
	outb(ATA_PRIMARY_IO_BASE + 6, 0xE0 | ((lba >> 24) & 0x0F)); 
	// Set the sector count to 1
	outb(ATA_PRIMARY_IO_BASE + 2, 1);  
	// Set the sector number
	outb(ATA_PRIMARY_IO_BASE + 3, lba); 
	// Set the cylinder low
	outb(ATA_PRIMARY_IO_BASE + 4, lba >> 8); 
	// Set the cylinder high
	outb(ATA_PRIMARY_IO_BASE + 5, lba >> 16); 
	// Send the read command
	outb(ATA_PRIMARY_IO_BASE + ATA_COMMAND_REGISTER, ATA_READ_SECTORS);

	// Wait for the device to be ready
	while (!(inb(ATA_PRIMARY_IO_BASE + 7) & 0x08));

	// Read data from the data register
	for (int sector = 0; sector< sector_count; sector++){
		for (int i = 0; i < 256; i++) {
			((unsigned short *)buffer)[(sector*256) + i] = inw(ATA_PRIMARY_IO_BASE);
		}
	}
}
