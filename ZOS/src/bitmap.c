#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


void bitmap_init(BITMAP **bitmap, int32_t cluster_count) {
	(*bitmap) = calloc(1, sizeof(BITMAP));
	(*bitmap) -> length = cluster_count;
	(*bitmap) -> data = calloc((*bitmap) -> length, sizeof(unsigned char));
	memset((*bitmap) -> data, 0, (*bitmap) -> length);
}

void fread_bitmap(VFS **vfs, FILE *file) {
	(*vfs) -> bitmap = calloc(1, sizeof(BITMAP));
	(*vfs) -> bitmap -> length = (*vfs) -> boot_record -> cluster_count;
	(*vfs) -> bitmap -> data = calloc((*vfs) -> boot_record -> cluster_count, sizeof(unsigned char));
	fseek(file, (*vfs) -> boot_record -> bitmap_start_address, SEEK_SET);	
	fread((*vfs) -> bitmap -> data, sizeof(char unsigned), (*vfs) -> boot_record -> cluster_count, file);	
}

void print_bitmap(BITMAP *bitmap) {
	printf("\nBitmap:\n----------------\n");
	printf("Length: %d\n", bitmap -> length);
	printf("Clusters (0 - not used, 1 - used):");
	
	int i;
	for (i = 0; i < bitmap -> length; i++) {
		if (i % 8 == 0) printf("\n");		
		printf("%d", bitmap -> data[i]);
	}
	printf("\n");
}

