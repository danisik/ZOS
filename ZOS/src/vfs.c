#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include "header.h"

#define SIGNATURE "Danisik"
#define DESCRIPTOR "pseudoNTFS"

void vfs_init(VFS **vfs, char *filename, size_t disk_size, int formatting) {

	(*vfs) = calloc(1, sizeof(VFS));

	(*vfs) -> filename = calloc(VFS_FILENAME_LENGTH, sizeof(char));

	if (strlen(filename) > 12) {
		strncpy((*vfs) -> filename, filename, 11);
	}
	else strcpy((*vfs) -> filename, filename);
	
	path_init(vfs);

	FILE *test_if_exists = fopen(filename, "rb");
	//if (test_if_exists == NULL || formatting == 1) {
		if (formatting == 0) printf("Data file with name %s not found, creating new\n", filename);

		BOOT_RECORD *boot_record;
		boot_record_init(&boot_record, SIGNATURE, DESCRIPTOR, disk_size, CLUSTER_SIZE);
		(*vfs) -> boot_record = boot_record;

		BITMAP *bitmap;
		bitmap_init(&bitmap, (*vfs) -> boot_record -> cluster_count);
		(*vfs) -> bitmap = bitmap;

		mft_init(vfs);

		create_vfs_file(vfs);
	//}
	/*else {
		printf("Data file with name %s found, filling structures\n", filename);
		fread_boot_record(vfs, test_if_exists);
		fread_mft(vfs, test_if_exists);
		fread_bitmap(vfs, test_if_exists);
	}*/
	
}

void path_init(VFS **vfs) {
	(*vfs) -> actual_path = calloc(1, sizeof(PATH));
	(*vfs) -> actual_path -> path = calloc(PATH_MAX, sizeof(char));
}

void create_vfs_file(VFS **vfs) {
	FILE *file = fopen((*vfs) -> filename, "wb");
	if (file == NULL) {
		printf("File %s not found\n", (*vfs) -> filename);
		return;
	} 
	else {		
        	fwrite((*vfs) -> boot_record, sizeof(BOOT_RECORD), 1, file);
		
		fseek(file, (*vfs) -> boot_record -> mft_start_address, SEEK_SET);
	        fwrite((*vfs) -> mft, sizeof(MFT_ITEM), (size_t) (*vfs) -> mft -> size, file);
		
		fseek(file, (*vfs) -> boot_record -> bitmap_start_address, SEEK_SET);
	        fwrite((*vfs) -> bitmap -> data, sizeof(unsigned char), (size_t) (*vfs) -> bitmap -> length, file);

		fseek(file, (*vfs) -> boot_record -> data_start_address, SEEK_SET);

		fclose(file);
		(*vfs) -> FILE = fopen((*vfs) -> filename, "r+b");	
	}
}

void print_vfs(VFS *vfs) {
	print_boot_record(vfs -> boot_record);
	print_mft(vfs -> mft);
	print_bitmap(vfs -> bitmap);
}
