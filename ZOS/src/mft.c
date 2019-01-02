#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


void mft_init(MFT **mft) {
	(*mft) = calloc(1, sizeof(MFT));
	(*mft) -> size = DISK_SIZE * MFT_SIZE_RATIO;
	(*mft) -> items = calloc((*mft) -> size, sizeof(MFT_ITEM));

	int i;
	for (i = 0; i < (*mft) -> size; i++) {
		MFT_ITEM item;
		mft_item_init(&item);
		
		(*mft) -> items[i] = item; 
	}
}

void mft_item_init(MFT_ITEM **item) {
	(*item) = calloc(1, sizeof(MFT_ITEM));
	(*item) -> uid = UID_ITEM_FREE;
	(*item) -> isDirectory = -1;                           
	(*item) -> item_order = -1;                        
	(*item) -> item_order_total = -1;              
	(*item) -> item_size = -1;                  
}

void mft_fragment_init() {

}

void print_mft(MFT *mft) {
	printf("\nMFT:\n----------------\n");
	printf("Size: %d B (10%% of disk space)\n", mft -> size);

	//TODO mft items print
}
