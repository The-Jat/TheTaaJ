#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define SECTOR_SIZE 2048

// Primary Volume Descriptor (PVD) structure
typedef struct __attribute__((packed)) {
    uint8_t type;                        // Offset 0, 1 byte
    char id[5];                          // Offset 1, 5 bytes
    uint8_t version;                     // Offset 6, 1 byte
    uint8_t unused1;                     // Offset 7, 1 byte
    char system_id[32];                  // Offset 8, 32 bytes
    char volume_id[32];                  // Offset 40, 32 bytes
    uint8_t unused2[8];                  // Offset 72, 8 bytes
    uint32_t volume_space_size_le;       // Offset 80 (little-endian), 4 bytes
    uint32_t volume_space_size_be;       // Offset 84 (big-endian), 4 bytes
    uint8_t unused3[32];                 // Offset 88, 32 bytes
    uint16_t volume_set_size_le;         // Offset 120 (little-endian), 2 bytes
    uint16_t volume_set_size_be;         // Offset 122 (big-endian), 2 bytes
    uint16_t volume_sequence_number_le;  // Offset 124 (little-endian), 2 bytes
    uint16_t volume_sequence_number_be;  // Offset 126 (big-endian), 2 bytes
    uint16_t logical_block_size_le;      // Offset 128 (little-endian), 2 bytes
    uint16_t logical_block_size_be;      // Offset 130 (big-endian), 2 bytes
    uint32_t path_table_size_le;         // Offset 132 (little-endian), 4 bytes
    uint32_t path_table_size_be;         // Offset 136 (big-endian), 4 bytes
    uint32_t type_l_path_table;          // Offset 140, 4 bytes
    uint32_t opt_type_l_path_table;      // Offset 144, 4 bytes
    uint32_t type_m_path_table;          // Offset 148, 4 bytes
    uint32_t opt_type_m_path_table;      // Offset 152, 4 bytes
    uint8_t root_directory_record[34];   // Offset 156, 34 bytes
    char volume_set_id[128];             // Offset 190, 128 bytes
    char publisher_id[128];              // Offset 318, 128 bytes
    char data_preparer_id[128];          // Offset 446, 128 bytes
    char application_id[128];            // Offset 574, 128 bytes
    char copyright_file_id[37];          // Offset 702, 37 bytes
    char abstract_file_id[37];           // Offset 739, 37 bytes
    char bibliographic_file_id[37];      // Offset 776, 37 bytes
    char creation_date[17];              // Offset 813, 17 bytes
    char modification_date[17];          // Offset 830, 17 bytes
    char expiration_date[17];            // Offset 847, 17 bytes
    char effective_date[17];             // Offset 864, 17 bytes
    uint8_t file_structure_version;      // Offset 881, 1 byte
    uint8_t unused4;                     // Offset 882, 1 byte
    uint8_t application_data[512];       // Offset 883, 512 bytes
    uint8_t unused5[653];                // Offset 1395, 653 bytes
} PVD;

// Directory Record structure
typedef struct __attribute__((packed)) {
    uint8_t length;                      // Offset 0, 1 byte
    uint8_t ext_attr_length;             // Offset 1, 1 byte
    uint32_t extent_location_le;         // Offset 2 (little-endian), 4 bytes
    uint32_t extent_location_be;         // Offset 6 (big-endian), 4 bytes
    uint32_t data_length_le;             // Offset 10 (little-endian), 4 bytes
    uint32_t data_length_be;             // Offset 14 (big-endian), 4 bytes
    uint8_t recording_date[7];           // Offset 18, 7 bytes
    uint8_t file_flags;                  // Offset 25, 1 byte
    uint8_t file_unit_size;              // Offset 26, 1 byte
    uint8_t interleave_gap_size;         // Offset 27, 1 byte
    uint16_t volume_sequence_number_le;  // Offset 28 (little-endian), 2 bytes
    uint16_t volume_sequence_number_be;  // Offset 30 (big-endian), 2 bytes
    uint8_t file_id_length;              // Offset 32, 1 byte
    char file_id[];                      // Offset 33, Variable
} DirectoryRecord;

// Function to read the Primary Volume Descriptor
void read_pvd(FILE *iso_file, PVD *pvd) {
    fseek(iso_file, 16 * SECTOR_SIZE, SEEK_SET); // Seek to sector 16
    fread(pvd, sizeof(PVD), 1, iso_file);        // Read the PVD
}

// Function to print the Primary Volume Descriptor
void print_pvd(PVD *pvd) {
    printf("Volume ID: %.32s\n", pvd->volume_id);
    printf("Volume Space Size (LE): %u\n", pvd->volume_space_size_le);
    printf("Volume Space Size (BE): %u\n", pvd->volume_space_size_be);
}

// Function to read a sector from the ISO file
void read_sector(FILE *iso_file, uint32_t sector, void *buffer) {
    fseek(iso_file, sector * SECTOR_SIZE, SEEK_SET);
    fread(buffer, SECTOR_SIZE, 1, iso_file);
}

// Function to print directory entries
void print_directory_entries(FILE *iso_file, uint8_t *root_directory_record) {
    uint32_t extent_location = *((uint32_t *)(root_directory_record + 2));
    uint32_t data_length = *((uint32_t *)(root_directory_record + 10));
    printf("Root Directory Data Length: %u\n", data_length);
    uint8_t buffer[SECTOR_SIZE];

    for (uint32_t i = 0; i < data_length; i += SECTOR_SIZE) {
        read_sector(iso_file, extent_location + (i / SECTOR_SIZE), buffer);
        uint8_t *ptr = buffer;

        while (ptr < buffer + SECTOR_SIZE && ptr[0] != 0) {
            DirectoryRecord *record = (DirectoryRecord *)ptr;
            printf("File ID Length: %u\n", record->file_id_length);
            printf("File ID: %.*s\n", record->file_id_length, record->file_id);
            printf("Extent Location (LE): %u\n", record->extent_location_le);
            printf("Data Length (LE): %u\n", record->data_length_le);
            printf("File Flags: %u\n", record->file_flags);
            ptr += record->length;
        }
    }
}

void search_file_in_root_directory(FILE *iso_file, uint8_t *root_directory_record, char file_name[]) {
	printf("------[search_file_in_root_directory]------\n");
	uint32_t extent_location = *((uint32_t *)(root_directory_record + 2));
	uint32_t data_length = *((uint32_t *)(root_directory_record + 10));
	printf("Root Directory Data Length: %u\n", data_length);
	uint8_t buffer[SECTOR_SIZE];

	for (uint32_t i = 0; i < data_length; i += SECTOR_SIZE) {
		read_sector(iso_file, extent_location + (i / SECTOR_SIZE), buffer);
        
		DirectoryRecord* record = (DirectoryRecord*) buffer;

		while ((uint8_t)record < buffer + SECTOR_SIZE) {

			if(record->length == 0) break; // End of directory

			char name[record->file_id_length + 1];
			memcpy(name, record->file_id, record->file_id_length);
			name[record->file_id_length] = '\0';
			if(strcmp(name, file_name) == 0) {
				// File found
				printf("File found.\n");
				printf("Extent Location (LE): %u\n", record->extent_location_le);
				printf("Data Length (LE): %u\n", record->data_length_le);
				break;
			}
			// Move to the next record
			record = (DirectoryRecord*) ((uint8_t*) record + record->length);
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <iso_file>\n", argv[0]);
		return 1;
	}

	FILE *iso_file = fopen(argv[1], "rb");
	if (!iso_file) {
		perror("Error opening ISO file");
		return 1;
	}

	PVD pvd;
	read_pvd(iso_file, &pvd);
	print_pvd(&pvd);
	print_directory_entries(iso_file, pvd.root_directory_record);
	
	// search for file in root directory
	char file_name[] = "AB.TXT;1";
	search_file_in_root_directory(iso_file, pvd.root_directory_record, file_name);

	fclose(iso_file);
	return 0;
}

