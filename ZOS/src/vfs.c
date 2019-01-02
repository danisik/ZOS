#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

void vfs_init(VFS **vfs, char *filename) {
	(*vfs) = calloc(1, sizeof(VFS));
	
	BOOT_RECORD *boot_record;
	boot_record_init(&boot_record, "iNTFS", "My new filesystem", DISK_SIZE, CLUSTER_SIZE);
	(*vfs) -> boot_record = boot_record;

	MFT *mft;
	mft_init(&mft);
	(*vfs) -> mft = mft;

	BITMAP *bitmap;
	bitmap_init(&bitmap, (*vfs) -> boot_record -> cluster_count);
	(*vfs) -> bitmap = bitmap;
	
	if (strlen(filename) > 12) {
		strncpy((*vfs) -> filename, filename, 11);
	}

	//TODO
	(*vfs) -> FILE = NULL;	
	//kontrola zda neni otevreny, pokud ano tak zavrit a znovu otevrit
	//FILE *file = fopen((*vfs) -> FILE, "r+b");
}

void print_vfs(VFS *vfs) {
	print_boot_record(vfs -> boot_record);
	print_mft(vfs -> mft);
	print_bitmap(vfs -> bitmap);
}
