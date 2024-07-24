#include <ISO9660.h>

int root_sector = 0;
iso_9660_volume_descriptor_t * root = (iso_9660_volume_descriptor_t *)((uint8_t *)0x20000);
iso_9660_directory_entry_t * dir_entry = (iso_9660_directory_entry_t *)((uint8_t *)0x20800);
uint8_t * mod_dir = (uint8_t *)0x21000;
uint8_t * dir_entries = (uint8_t *)(0x30000);
struct ata_device * device = 0;


int navigate(char * name) {
	memsetb(dir_entries, 2048, 0xA5);
	boot_print("reading from sector ");
	boot_print_hex(dir_entry->extent_start_LSB);
	boot_print("\n");
	ata_device_read_sector_atapi(device, dir_entry->extent_start_LSB, dir_entries);
	ata_device_read_sector_atapi(device, dir_entry->extent_start_LSB+1, dir_entries + 2048);
	ata_device_read_sector_atapi(device, dir_entry->extent_start_LSB+2, dir_entries + 4096);

	long offset = 0;
	while (1) {
		iso_9660_directory_entry_t * dir = (iso_9660_directory_entry_t *)(dir_entries + offset);
		if (dir->length == 0) {
			if (offset < dir_entry->extent_length_LSB) {
				offset += 1; // this->block_size - ((uintptr_t)offset % this->block_size);
				goto try_again;
			}
			break;
		}
		if (!(dir->flags & FLAG_HIDDEN)) {
			char file_name[dir->name_len + 1];
			memcpyb(file_name, dir->name, dir->name_len);
			file_name[dir->name_len] = 0;
			char * s = strchr(file_name,';');
			if (s) {
				*s = '\0';
			}
#if 1//0
			boot_print("Found a file: ");
			boot_print(" Name: ");
			boot_print(file_name); boot_print("\n");
#endif
			if (!strcmp(file_name, name)) {
				memcpyb(dir_entry, dir, sizeof(iso_9660_directory_entry_t));
				return 1;
			}
		}
		offset += dir->length;
try_again:
		if ((long)(offset) > dir_entry->extent_length_LSB) break;
	}

	return 0;
}
