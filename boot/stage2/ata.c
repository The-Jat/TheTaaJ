#include <port_io.h>
#include <print.h>
#include <ata.h>

#define ATA_PRIMARY_IO_BASE 0x1F0


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

#define ATA_SR_BSY     0x80

#define ATA_CMD_READ_SECTORS 0x20
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY 0xEC

struct ata_device ata_primary_master = {
    .io_base = 0x1F0,
    .control = 0x3F6,
    .slave = 0
};

struct ata_device ata_primary_slave = {
    .io_base = 0x1F0,
    .control = 0x3F6,
    .slave = 1
};

struct ata_device ata_secondary_master = {
    .io_base = 0x170,
    .control = 0x376,
    .slave = 0
};

struct ata_device ata_secondary_slave = {
    .io_base = 0x170,
    .control = 0x376,
    .slave = 1
};


#define ATA_REG_ALTSTATUS  0x0C

void ata_io_wait(struct ata_device * dev) {
	inb(dev->io_base + ATA_REG_ALTSTATUS);
	inb(dev->io_base + ATA_REG_ALTSTATUS);
	inb(dev->io_base + ATA_REG_ALTSTATUS);
	inb(dev->io_base + ATA_REG_ALTSTATUS);
}

// Function to perform an ATA soft reset
void ata_soft_reset(struct ata_device *dev) {
    // Write the reset bit (SRST) to the Device Control Register
    outb(dev->control, 0x04);
    // Wait for at least 4 microseconds
    ata_io_wait(dev);
    // Clear the reset bit
    outb(dev->control, 0x00);
    
    // Wait for the device to become ready
//    while ((inb(dev->io_base + 7) & 0x80) != 0) {
        // Wait while the BSY (busy) bit is set
//    }

    // Optionally, wait for DRDY (device ready) bit to be set
//    while ((inb(dev->io_base + 7) & 0x40) == 0) {
        // Wait until the DRDY bit is set
//    }
}


int ata_status_wait(struct ata_device * dev, int timeout) {
    int status;
    if (timeout > 0) {
        int i = 0;
        while ((status = inb(dev->io_base + ATA_REG_STATUS)) & ATA_SR_BSY && (i < timeout)) i++;
    } else {
        while ((status = inb(dev->io_base + ATA_REG_STATUS)) & ATA_SR_BSY);
    }
    return status;
}

void detect_device(struct ata_device *dev) {
	ata_soft_reset(dev);
	ata_io_wait(dev);

	// Select the drive: 0xA0 for master, 0xB0 for slave
	outb(dev->io_base + ATA_REG_HDDEVSEL, 0xA0 | dev->slave << 4);
	
	// Wait for the drive to select
	ata_io_wait(dev);
	ata_status_wait(dev, 10000);
	
	// Read the signature bytes
	unsigned char cl = inb(dev->io_base + ATA_REG_LBA1);
	unsigned char ch = inb(dev->io_base + ATA_REG_LBA2);

	// Determine device type based on signature
	if (cl == 0xFF && ch == 0xFF) {
		// No device present
		boot_print("[ATA] No Device Connected to this Channel.\n");
		dev->is_device_connected = 0;
		return;
	}

	if ((cl == 0x00 & ch == 0x00) || (cl == 0x3C && ch == 0xC3)) {
		// Parallel ATA Device \\ Emulated SATA
		// SATA hardware appear as legacy IDE hardware
		boot_print("[ATA] ATA Device Connected to this Channel.\n");
		dev->is_device_connected = 1;
		dev->is_atapi = 0;
		// Send IDENTIFY command
		outb(dev->io_base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
		ata_io_wait(dev);
		
		uint16_t *identify_data = (uint16_t *) &dev->identity;
		for(int i = 0; i< 256; i++) {
			identify_data[i] = inw(dev->io_base + ATA_REG_DATA);
		}
		
		boot_print("Flags: "); boot_print_hex(dev->identity.flags); boot_print("\n");
		boot_print("Serial: "); boot_print_hex(dev->identity.serial); boot_print("\n");
		boot_print("Firmware Revision: "); boot_print_hex(dev->identity.firmware); boot_print("\n");
		return;
	}
	if((cl == 0x14 && ch == 0xEB) || (cl == 0x69 && ch == 0x96)) {
		// ATAPI device
		boot_print("[ATA] ATAPI Device Connected to this Channel.\n");
		dev->is_device_connected = 1;
		dev->is_atapi = 1;
		
		outb(dev->io_base + ATA_REG_HDDEVSEL, 0xA0 | dev->slave << 4);
		ata_io_wait(dev);
		outb(dev->io_base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
		ata_io_wait(dev);
		ata_status_wait(dev, 0);
		unsigned short* identify_data = (unsigned short*) &dev->identity;
		for(int i = 0; i< 256; i++) {
			identify_data[i] = inw(dev->io_base);
		}
		boot_print("[ATAPI] Flags: "); boot_print_hex(dev->identity.flags); boot_print("\n");
		return;
	}
}


void detect_ata_devices(){
	boot_print("[ATA] Detecting Devices\n");
	detect_device(&ata_primary_master);
	detect_device(&ata_primary_slave);
	detect_device(&ata_secondary_master);
	detect_device(&ata_secondary_slave);
	boot_print("[ATA] Detecting Devices Done\n");
	
	//let's read the kernel from the primary channel, master device
	unsigned int lba = 59;
	int sector_count = 26;
	ata_read_sectors(&ata_primary_master, lba, sector_count, 0xB000);
	
}


void ata_read_sectors(struct ata_device *dev, unsigned int lba, int sector_count, unsigned short* buffer) {

	// Select the drive and LBA mode
	outb(dev->io_base + ATA_REG_HDDEVSEL, 0xE0 | (dev->slave << 4) | ((lba >> 24) & 0x0F));
	ata_io_wait(dev);

	// Send the sector count
	outb(dev->io_base + ATA_REG_SECCOUNT0, sector_count);

	// Send the LBA address
	outb(dev->io_base + ATA_REG_LBA0, (unsigned char)lba);
	outb(dev->io_base + ATA_REG_LBA1, (unsigned char) (lba >> 8));
	outb(dev->io_base + ATA_REG_LBA2, (unsigned char) (lba >> 16));

	// Send the Read Command
	outb(dev->io_base + ATA_REG_COMMAND, ATA_CMD_READ_SECTORS);
	ata_io_wait(dev);

	// Read the sector into buffer
	for (int sector = 0; sector< sector_count; sector++){
		ata_status_wait(dev, 10000);
		for (int i = 0; i < 256; i++) {
			((unsigned short *)buffer)[(sector*256) + i] = inw(dev->io_base + ATA_REG_DATA);
		}
	}
}

/*
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
	outb(ATA_PRIMARY_IO_BASE + ATA_REG_COMMAND, ATA_READ_SECTORS);

	// Wait for the device to be ready
	while (!(inb(ATA_PRIMARY_IO_BASE + 7) & 0x08));

	// Read data from the data register
	for (int sector = 0; sector< sector_count; sector++){
		for (int i = 0; i < 256; i++) {
			((unsigned short *)buffer)[(sector*256) + i] = inw(ATA_PRIMARY_IO_BASE);
		}
	}
}
*/

