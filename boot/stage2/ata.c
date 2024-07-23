#include <port_io.h>
#include <print.h>
#include <ata.h>
#include <mem.h>
#include <ISO9660.h>
#define KERNEL_LOAD_START 0x300000

#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01

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
#define ATA_CMD_PACKET            0xA0
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

int ata_wait(struct ata_device * dev, int advanced) {
	unsigned char status = 0;

	ata_io_wait(dev);

	status = ata_status_wait(dev, -1);

	if (advanced) {
		status = inb(dev->io_base + ATA_REG_STATUS);
		if (status   & ATA_SR_ERR)  return 1;
		if (status   & ATA_SR_DF)   return 1;
		if (!(status & ATA_SR_DRQ)) return 1;
	}

	return 0;
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
		boot_print("Sectors Per Interrupt: "); boot_print_hex(dev->identity.sectors_per_int); boot_print("\n");
		return;
	}
	if((cl == 0x14 && ch == 0xEB) || (cl == 0x69 && ch == 0x96)) {
		// ATAPI device
		boot_print("[ATA] ATAPI Device Connected to this Channel.\n");
		dev->is_device_connected = 1;
/*		dev->is_atapi = 1;
		
		outb(dev->io_base + ATA_REG_HDDEVSEL, 0xA0 | dev->slave << 4);
		ata_io_wait(dev);

		outb(dev->io_base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
		ata_io_wait(dev);

		ata_status_wait(dev, 0);

		unsigned short* identify_data = (unsigned short*) &dev->identity;
		for(int i = 0; i< 256; i++) {
			identify_data[i] = inw(dev->io_base);
		}

		unsigned char* ptr = (unsigned char*)&dev->identity.model;
		for (int i = 0; i < 39; i += 2) {
			unsigned char tmp = ptr[i + 1];
			ptr[i + 1] = ptr[i];
			ptr[i] = tmp;
		}

		boot_print("[ATAPI] Flags: "); boot_ushort_print_hex(dev->identity.flags); boot_print("\n");
		boot_print("[ATAPI] Serial: "); boot_print_hex(dev->identity.serial); boot_print("\n");
		boot_print("[ATAPI] Firmware Revision: "); boot_print_hex(dev->identity.firmware); boot_print("\n");
		boot_print("[ATAPI] Model "); boot_print(ptr); boot_print("\n");
		return;
*/
	dev->is_atapi = 1;

	outb(dev->io_base + 1, 1);
	outb(dev->control, 0);

	outb(dev->io_base + ATA_REG_HDDEVSEL, 0xA0 | dev->slave << 4);
	ata_io_wait(dev);

	outb(dev->io_base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
	ata_io_wait(dev);

	ata_wait(dev, 0);

	uint16_t * buf = (uint16_t *)&dev->identity;

	for (int i = 0; i < 256; ++i) {
		buf[i] = inw(dev->io_base);
	}

	unsigned char * ptr = (unsigned char *)&dev->identity.model;
	for (int i = 0; i < 39; i+=2) {
		unsigned char tmp = ptr[i+1];
		ptr[i+1] = ptr[i];
		ptr[i] = tmp;
	}

	/* Detect medium */
	atapi_command_t command;
	memsetb(&command, 0, sizeof(command));
	command.command_bytes[0] = 0x25;

	uint16_t bus = dev->io_base;

	outb(bus + ATA_REG_FEATURES, 0x00);
	outb(bus + ATA_REG_LBA1, 0x08);
	outb(bus + ATA_REG_LBA2, 0x08);
	outb(bus + ATA_REG_COMMAND, ATA_CMD_PACKET);

	/* poll */
	while (1) {
		unsigned char status = inb(dev->io_base + ATA_REG_STATUS);
		if ((status & ATA_SR_ERR)) goto atapi_error;
		if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRDY)) break;
	}

	for (int i = 0; i < 6; ++i) {
		outw(bus, command.command_words[i]);
	}

	/* poll */
	while (1) {
		unsigned char status = inb(dev->io_base + ATA_REG_STATUS);
		if ((status & ATA_SR_ERR)) goto atapi_error_read;
		if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRDY)) break;
		if ((status & ATA_SR_DRQ)) break;
	}

	uint16_t data[4];

	for (int i = 0; i < 4; ++i) {
		data[i] = inw(bus);
	}

#define htonl(l)  ( (((l) & 0xFF) << 24) | (((l) & 0xFF00) << 8) | (((l) & 0xFF0000) >> 8) | (((l) & 0xFF000000) >> 24))
	uint32_t lba, blocks;;
	memcpyb(&lba, &data[0], sizeof(uint32_t));
	lba = htonl(lba);
	memcpyb(&blocks, &data[2], sizeof(uint32_t));
	blocks = htonl(blocks);

	dev->atapi_lba = lba;
	dev->atapi_sector_size = blocks;

boot_print_hex(dev->atapi_sector_size);
//while(1){}
	return;

atapi_error_read:
	return;

atapi_error:
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

	// read from the ata device which is ATAPI
	if (ata_primary_master.is_atapi) {
		read_ata_device(&ata_primary_master);
	}
	if (ata_primary_slave.is_atapi) {
		read_ata_device(&ata_primary_slave);
	}
	if (ata_secondary_master.is_atapi) {
		read_ata_device(&ata_secondary_master);
	}
	if (ata_secondary_slave.is_atapi) {
		read_ata_device(&ata_secondary_slave);
	}


	while (1);



	//let's read the kernel from the primary channel, master device
	unsigned int lba = 59;
	int sector_count = 26;
	//ata_read_sectors(&ata_primary_master, lba, sector_count, 0xB000);
	
}


void restore_root(void) {
	memcpyb(dir_entry, (iso_9660_directory_entry_t *)&root->root, sizeof(iso_9660_directory_entry_t));

#if 0
	boot_print("Root restored.");
	boot_print("\n Entry len:  "); boot_print_hex( dir_entry->length);
	boot_print("\n File start: "); boot_print_hex( dir_entry->extent_start_LSB);
	boot_print("\n File len:   "); boot_print_hex( dir_entry->extent_length_LSB);
	boot_print("\n");
#endif
}

int _read_12 = 0;
void ata_device_read_sector_atapi(struct ata_device * dev, uint32_t lba, uint8_t * buf) {

	if (!dev->is_atapi) return;


	uint16_t bus = dev->io_base;

_try_again:
	outb(dev->io_base + ATA_REG_HDDEVSEL, 0xA0 | dev->slave << 4);
	ata_io_wait(dev);

	outb(bus + ATA_REG_FEATURES, 0x00);
	outb(bus + ATA_REG_LBA1, dev->atapi_sector_size & 0xFF);
	outb(bus + ATA_REG_LBA2, dev->atapi_sector_size >> 8);
	outb(bus + ATA_REG_COMMAND, ATA_CMD_PACKET);

	/* poll */
	while (1) {
		uint8_t status = inb(dev->io_base + ATA_REG_STATUS);
		if ((status & ATA_SR_ERR)) goto atapi_error_on_read_setup;
		if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break;
	}

	atapi_command_t command;
	command.command_bytes[0] = _read_12 ? 0xA8 : 0x28;
	command.command_bytes[1] = 0;
	command.command_bytes[2] = (lba >> 0x18) & 0xFF;
	command.command_bytes[3] = (lba >> 0x10) & 0xFF;
	command.command_bytes[4] = (lba >> 0x08) & 0xFF;
	command.command_bytes[5] = (lba >> 0x00) & 0xFF;
	command.command_bytes[6] = 0;
	command.command_bytes[7] = 0;
	command.command_bytes[8] = _read_12 ? 0 : 1; /* bit 0 = PMI (0, last sector) */
	command.command_bytes[9] = _read_12 ? 1 : 0; /* control */
	command.command_bytes[10] = 0;
	command.command_bytes[11] = 0;

	for (int i = 0; i < 6; ++i) {
		outw(bus, command.command_words[i]);
	}

	while (1) {
		uint8_t status = inb(dev->io_base + ATA_REG_STATUS);
		if ((status & ATA_SR_ERR)) goto atapi_error_on_read_setup_cmd;
		if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break;
	}

	uint16_t size_to_read = inb(bus + ATA_REG_LBA2) << 8;
	size_to_read = size_to_read | inb(bus + ATA_REG_LBA1);

	inwm(bus,buf,size_to_read/2);

	while (1) {
		uint8_t status = inb(dev->io_base + ATA_REG_STATUS);
		if ((status & ATA_SR_ERR)) goto atapi_error_on_read_setup;
		if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRDY)) break;
	}

	return;

atapi_error_on_read_setup:
	boot_print("error on setup\n");
	return;
atapi_error_on_read_setup_cmd:
	if (_read_12) {
		_read_12 = 0;
		boot_print("trying again\n");
		goto _try_again;
	}
	boot_print("error on cmd\n");
	return;
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


void read_ata_device(struct ata_device * _device) {
	device = _device;
	if (device->atapi_sector_size != 2048) {
		boot_print_hex(device->atapi_sector_size);
		boot_print("\n - bad sector size\n");
		return;
	}

	for (int i = 0x10; i < 0x15; ++i) {
		ata_device_read_sector_atapi(device, i, (uint8_t *)root);
		switch (root->type) {
			case 1:
				root_sector = i;
				goto done;
			case 0xFF:
				return;
		}
	}
	return;
done:
	restore_root();

	// Our kernel is "kernel_entry.bin" however, ISO9660 only allows the idenfier to be
	// of length 11 excluding (.) with 8:3 (8 character for the name without extension,
	// and 3 characters for the extension).
	if (navigate("KERNEL_E.BIN")) {
		boot_print("Found kernel.\n");
		boot_print_hex(dir_entry->extent_start_LSB); boot_print(" ");
		boot_print_hex(dir_entry->extent_length_LSB); boot_print("\n");
		long offset = 0;
		for (int i = dir_entry->extent_start_LSB; i < dir_entry->extent_start_LSB + dir_entry->extent_length_LSB / 2048 + 1; ++i, offset += 2048) {
			ata_device_read_sector_atapi(device, i, (uint8_t *)KERNEL_LOAD_START + offset);
		}
		boot_print("done loading kernel");
		uint32_t kernel_address = 0x300000;
		void (*kernel_entry)(void) = (void(*)(void))0x300000;
		kernel_entry();
		restore_root();
	} else {
		boot_print("Kernel was not found.\n");
	}

	return;
}


