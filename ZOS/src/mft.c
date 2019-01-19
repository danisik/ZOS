#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


void mft_init(MFT **mft) {
	(*mft) = calloc(1, sizeof(MFT));
	(*mft) -> size = 0;
	(*mft) -> items = calloc((*mft) -> size, sizeof(MFT_ITEM));

	mft_item_init(mft, "root", 1, 1);
}

void mft_item_init(MFT **mft, char *name, int isDirectory, int item_size) {

	//kontrola jestli můžu stále vytvářem nové itemy

	(*mft) -> items[(*mft) -> size] = calloc(1, sizeof(MFT_ITEM));
	(*mft) -> items[(*mft) -> size] -> uid = UID_ITEM_FREE;
	(*mft) -> items[(*mft) -> size] -> parentID = UID_ITEM_FREE;
	(*mft) -> items[(*mft) -> size] -> isDirectory = isDirectory;                           
	(*mft) -> items[(*mft) -> size] -> item_order = 1;                        
	(*mft) -> items[(*mft) -> size] -> item_order_total = 1;              
	(*mft) -> items[(*mft) -> size] -> item_size = item_size;        
	strcpy((*mft) -> items[(*mft) -> size] -> item_name, name);
	
	//TODO
	//((*mft) -> items[(*mft) -> size] -> fragments[MFT_FRAGMENTS_COUNT];
	//mft_fragment_init();

	(*mft) -> size++;
}

void mft_fragment_init() {
	//najít volné clustery a zablokovat je
}

void fread_mft(VFS **vfs, FILE *file) {
	(*vfs) -> mft = calloc(1, sizeof(MFT));
	fseek(file, (*vfs) -> boot_record -> mft_start_address, SEEK_SET);
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

void print_mft(MFT *mft) {
	printf("\nMFT:\n----------------\n");
	printf("Items count: %d \n", mft -> size);
	printf("Size: %d \n", 0);
	printf("\nItems (+ directory, - file):\n");
	int i;
	for (i = 0; i < mft -> size; i++) {
		if (mft -> items[i] -> isDirectory == 1) printf("+");
		else printf("-");
		printf("%s\n", mft -> items[i] -> item_name);
	}
}
