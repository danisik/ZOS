#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


void bitmap_init(BITMAP **bitmap, int32_t cluster_count) {
	(*bitmap) = calloc(1, sizeof(BITMAP));
	(*bitmap) -> length = calculate_length(cluster_count);
	(*bitmap) -> data = calloc((*bitmap) -> length, sizeof(unsigned char));
}

int32_t calculate_length(int32_t cluster_count) {
    int32_t length = cluster_count / 8;

    if (cluster_count % 8 != 0) {
        length++;
    }

    return length;
}

void print_bitmap(BITMAP *bitmap) {
	printf("\nBitmap:\n----------------\n");
	printf("Length: %d\n", bitmap -> length);
	printf("Clusters (0 - not used, 1 - used):");
	
	int i;
	for (i = 0; i < bitmap -> length; i++) {
		if (i % 50 == 0) printf("\n");
		printf("%d", bitmap -> data[i]);
	}
	printf("\n");
}

