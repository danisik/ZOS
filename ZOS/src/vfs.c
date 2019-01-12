#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>
#include "header.h"

#define SIGNATURE "Danisik"
#define DESCRIPTOR "pseudoNTFS"

void vfs_init(VFS **vfs, char *filename) {
	(*vfs) = calloc(1, sizeof(VFS));
	
	BOOT_RECORD *boot_record;
	boot_record_init(&boot_record, SIGNATURE, DESCRIPTOR, DISK_SIZE, CLUSTER_SIZE);
	(*vfs) -> boot_record = boot_record;

	MFT *mft;
	mft_init(&mft);
	(*vfs) -> mft = mft;

	BITMAP *bitmap;
	bitmap_init(&bitmap, (*vfs) -> boot_record -> cluster_count);
	(*vfs) -> bitmap = bitmap;
	
	(*vfs) -> filename = calloc(VFS_FILENAME_LENGTH, sizeof(char));
	if (strlen(filename) > 12) {
		strncpy((*vfs) -> filename, filename, 11);
	}
	else strcpy((*vfs) -> filename, filename);

	//TODO
	(*vfs) -> FILE = NULL;	

	path_init(vfs);
	create_vfs_file(vfs);
}

void path_init(VFS **vfs) {
	(*vfs) -> actual_path = calloc(1, sizeof(PATH));
	(*vfs) -> actual_path -> path = calloc(PATH_MAX, sizeof(char));
	strcat((*vfs) -> actual_path -> path, "~");
}

void create_vfs_file(VFS **vfs) {
	FILE *file = fopen((*vfs) -> filename, "wb");
	if (file == NULL) {
		printf("File %s not found\n", (*vfs) -> filename);
		return;
	} 
	else {		

		//TODO načíst hodnoty z už vytvořeného 
        	fwrite((*vfs) -> boot_record, sizeof(BOOT_RECORD), 1, file);
	        fwrite((*vfs) -> mft -> items, sizeof(MFT_ITEM), (size_t) (*vfs) -> mft -> size, file);
	        fwrite((*vfs) -> bitmap -> data, sizeof(unsigned char), (size_t) (*vfs) -> bitmap -> length, file);

	        char zeros[(*vfs) -> boot_record -> cluster_size];
	        memset(zeros, 0, (size_t) (*vfs) -> boot_record -> cluster_size);
	
		int i;
	        for (i = 0; i < (*vfs) -> boot_record -> cluster_count; ++i) {
	            fwrite(zeros, (size_t) (*vfs) -> boot_record -> cluster_size, 1, file);
	        }
		fclose(file);
		(*vfs) -> FILE = fopen((*vfs) -> filename, "r+b");	
	}
}

void print_vfs(VFS *vfs) {
	print_boot_record(vfs -> boot_record);
	print_mft(vfs -> mft);
	print_bitmap(vfs -> bitmap);
}
