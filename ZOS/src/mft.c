#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


void mft_init(MFT **mft) {
	(*mft) = calloc(1, sizeof(MFT));
	(*mft) -> size = 0;
	(*mft) -> items = calloc((*mft) -> size, sizeof(MFT_ITEM));

	mft_item_init(mft, "root", 1, 1);

	//TODO
	//MFT_FRAGMENT fragments[MFT_FRAGMENTS_COUNT];
	//mft_fragment_init();
}

void mft_item_init(MFT **mft, char *name, int isDirectory, int item_size) {
	(*mft) -> items[(*mft) -> size] = calloc(1, sizeof(MFT_ITEM));
	(*mft) -> items[(*mft) -> size] -> uid = UID_ITEM_FREE;
	(*mft) -> items[(*mft) -> size] -> parentID = UID_ITEM_FREE;
	(*mft) -> items[(*mft) -> size] -> isDirectory = isDirectory;                           
	(*mft) -> items[(*mft) -> size] -> item_order = 1;                        
	(*mft) -> items[(*mft) -> size] -> item_order_total = 1;              
	(*mft) -> items[(*mft) -> size] -> item_size = item_size;        
	strcpy((*mft) -> items[(*mft) -> size] -> item_name, name);
	(*mft) -> size++;
}

void mft_fragment_init() {

}

void print_mft(MFT *mft) {
	printf("\nMFT:\n----------------\n");
	printf("Size: %d B (10%% of disk space)\n", mft -> size);
}

MFT_ITEM *find_mft_item(MFT *mft, char *tok) {

	int i;
	for (i = 0; i < mft -> size; i++) {
		if (strncmp(tok, mft -> items[i] -> item_name, strlen(mft -> items[i] -> item_name)) == 0) {
			return mft -> items[i];
		}			
	}

	return NULL;
}
