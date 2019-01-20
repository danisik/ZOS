#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


void mft_init(VFS **vfs) {
	(*vfs) -> mft = calloc(1, sizeof(MFT));
	(*vfs) -> mft -> size = 0;
	(*vfs) -> mft -> items = calloc((*vfs) -> mft -> size, sizeof(MFT_ITEM));

	mft_item_init(vfs, (*vfs) -> mft -> size, -1, ROOT_NAME, 1, DIRECTORY_SIZE);
	mft_item_init(vfs, (*vfs) -> mft -> size, 0, "test", 1, DIRECTORY_SIZE);
	mft_item_init(vfs, (*vfs) -> mft -> size, 0, "tete", 1, DIRECTORY_SIZE);
	mft_item_init(vfs, (*vfs) -> mft -> size, 1, "test2", 1, DIRECTORY_SIZE);
	mft_item_init(vfs, (*vfs) -> mft -> size, 2, "tete2", 1, DIRECTORY_SIZE);
	mft_item_init(vfs, (*vfs) -> mft -> size, 3, "test3", 1, DIRECTORY_SIZE);

}

void mft_item_init(VFS **vfs, int uid, int parentID, char *name, int isDirectory, int item_size) {

	if (bitmap_contains_free_cluster((*vfs) -> bitmap) == 1) {
		printf("Out of memory, cannot create new files\n");
		return;
	} 

	(*vfs) -> mft -> items[(*vfs) -> mft -> size] = calloc(1, sizeof(MFT_ITEM));
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> uid = uid;
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> parentID = parentID;
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> isDirectory = isDirectory;                           
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> item_order = 1;                        
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> item_order_total = 1;              
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> item_size = item_size;        
	strcpy((*vfs) -> mft -> items[(*vfs) -> mft -> size] -> item_name, name);

	mft_fragment_init(vfs, &((*vfs) -> mft -> items[(*vfs) -> mft -> size]));

	(*vfs) -> mft -> size++;
}

void mft_fragment_init(VFS **vfs, MFT_ITEM **item) {
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> fragments -> fragment_start_address = (*vfs) -> boot_record -> mft_start_address + ((*vfs) -> mft -> size * sizeof(MFT_ITEM));
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> fragments -> fragment_count = 1;   

	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> fragments -> cluster_ID = find_free_cluster(&(*vfs) -> bitmap);
}

void fread_mft(VFS **vfs, FILE *file) {
	(*vfs) -> mft = calloc(1, sizeof(MFT));
	fseek(file, (*vfs) -> boot_record -> mft_start_address, SEEK_SET);
}

MFT_ITEM *find_mft_item_by_name(MFT *mft, char *tok) {

	int a = 0;
	while(tok[a] != '\n' && tok[a] != '\0' && tok[a] != 47) {
		a++;
	}
	char compare[a];
	strncpy(compare, tok, a);

	int i;
	for (i = 0; i < mft -> size; i++) {
		if (strcmp(compare, mft -> items[i] -> item_name) == 0) {
			return mft -> items[i];
		}			
	}

	return NULL;
}

MFT_ITEM *find_mft_item_by_uid(MFT *mft, int uid) {
	int i;
	for (i = 0; i < mft -> size; i++) {
		if (mft -> items[i] -> uid == uid) {
			return mft -> items[i];
		}			
	}

	return NULL;
}

MFT_ITEM *get_mft_item_from_path(VFS *vfs, char *tok) {
	MFT_ITEM *item = NULL;

	if (strlen(tok) > 0 && tok[strlen(tok) - 1] == '\n') tok[strlen(tok) - 1] = '\0';
	char temp_path[strlen(vfs -> actual_path -> path) + strlen(tok)];
	strcpy(temp_path, vfs -> actual_path -> path); 
				
	if (tok[0] != 47) strcat(temp_path, "/");
	strcat(temp_path, tok);

	int folder_ID = find_folder_id(vfs -> mft, temp_path);		
	item = find_mft_item_by_uid(vfs -> mft, folder_ID);

	return item;
}

void print_mft(MFT *mft) {
	printf("\nMFT:\n----------------\n");
	printf("Items count: %d (with root)\n", mft -> size);
	printf("Size: %d \n", 0);
	printf("\nItems (+ directory, - file):\n");
	int i;
	for (i = 1; i < mft -> size; i++) {
		if (mft -> items[i] -> isDirectory == 1) printf("+");
		else printf("-");
		printf("%s\n", mft -> items[i] -> item_name);
	}
}
