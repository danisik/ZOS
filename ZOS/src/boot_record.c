#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


void boot_record_init(BOOT_RECORD **boot_record, char *signature, char *volume_descriptor, int32_t disk_size, int32_t cluster_size) {	
	(*boot_record) = calloc(1, sizeof(BOOT_RECORD));
	if (strlen(signature) > 9) {
		strncpy((*boot_record) -> signature, signature, 8);
	}
	else strcpy((*boot_record) -> signature, signature);  
	if (strlen(signature) > 251) {
		strncpy((*boot_record) -> volume_descriptor, volume_descriptor, 250);
	}
	else strcpy((*boot_record) -> volume_descriptor, volume_descriptor);     
       
	(*boot_record) -> disk_size = disk_size;
	(*boot_record) -> cluster_size = cluster_size;
	(*boot_record) -> cluster_count = disk_size / cluster_size;

	//TODO
	//(*boot_record) -> mft_start_address;      //adresa pocatku mft
	//(*boot_record) -> bitmap_start_address;   //adresa pocatku bitmapy
	//(*boot_record) -> data_start_address;     //adresa pocatku datovych bloku

	(*boot_record) -> mft_max_fragment_count = MFT_FRAGMENTS_COUNT; 
}

void print_boot_record(BOOT_RECORD *boot_record) {
	printf("\nBoot record:\n----------------\n");
	printf("Signature: %s\n", boot_record -> signature);
	printf("Volume descriptor: %s\n", boot_record -> volume_descriptor);
	printf("Disk size: %d B\n", boot_record -> disk_size);
	printf("Cluster size: %d B\n", boot_record -> cluster_size);
	printf("Cluster count: %d\n", boot_record -> cluster_count);
	
	//TODO
	//printf("MFT start address: %d\n", boot_record -> mft_start_address);
	//printf("BITMAP start address: %d\n", boot_record -> bitmap_start_address);
	//printf("DATA start address: %d\n", boot_record -> data_start_address);
	
	printf("MFT max fragment count: %d\n", boot_record -> mft_max_fragment_count);
}
