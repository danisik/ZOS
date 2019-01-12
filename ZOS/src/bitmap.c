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

