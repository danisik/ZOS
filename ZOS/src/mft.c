#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


void mft_init(VFS **vfs) {
	(*vfs) -> mft = calloc(1, sizeof(MFT));
	(*vfs) -> mft -> size = 0;
	(*vfs) -> mft -> items = calloc((*vfs) -> mft -> size, sizeof(MFT_ITEM));

	mft_item_init(vfs, (*vfs) -> mft -> size, -1, ROOT_NAME, 1, DIRECTORY_SIZE);

	//TODO smazat ve výsledném programu, pouze pro testovací účely
	
	mft_item_init(vfs, (*vfs) -> mft -> size, 0, "test", 1, DIRECTORY_SIZE);
	mft_item_init(vfs, (*vfs) -> mft -> size, 0, "tete", 1, DIRECTORY_SIZE);
	mft_item_init(vfs, (*vfs) -> mft -> size, 1, "test2", 1, 9000);
	mft_item_init(vfs, (*vfs) -> mft -> size, 2, "tete2", 1, DIRECTORY_SIZE);
	mft_item_init(vfs, (*vfs) -> mft -> size, 3, "test3", 1, DIRECTORY_SIZE);
	
}

void mft_item_init(VFS **vfs, int uid, int parentID, char *name, int isDirectory, int item_size) {

	if (bitmap_contains_free_cluster((*vfs) -> bitmap) == 1) {
		printf("Out of memory, cannot create new files\n");
		return;
	} 

	(*vfs) -> mft -> items = realloc((*vfs) -> mft -> items, ((*vfs) -> mft -> size + 1) * sizeof(MFT_ITEM));

	(*vfs) -> mft -> items[(*vfs) -> mft -> size] = calloc(1, sizeof(MFT_ITEM));
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> uid = uid;
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> parentID = parentID;
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> isDirectory = isDirectory;                           
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> item_order = 1;                        
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> item_order_total = 1;              
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> item_size = item_size;        
	strcpy((*vfs) -> mft -> items[(*vfs) -> mft -> size] -> item_name, name);
	(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> fragments_created = 0;

	int cluster_count = item_size / CLUSTER_SIZE;
	if ((item_size % CLUSTER_SIZE) != 0) cluster_count++;
	mft_fragment_init(vfs, cluster_count);	

	(*vfs) -> mft -> size++;

	fwrite_mft(vfs);
	fwrite_mft_item(vfs);
}

void mft_fragment_init(VFS **vfs, int cluster_count) {
	int i = 0;	
	int already_setted = 0;
	while(1) {
		(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> fragments[i] = calloc(1, sizeof(MFT_FRAGMENT));
		struct the_fragment_temp *temp = find_free_cluster(&(*vfs) -> bitmap, cluster_count - already_setted);	
		if (temp -> start_cluster_ID == -1 && temp -> count == 0 && temp -> successful == 0) break;
		already_setted += temp -> count;
	
		(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> fragments[i] -> fragment_count = temp -> count;
		(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> fragments[i] -> start_cluster_ID = temp -> start_cluster_ID;
		(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> fragments[i] -> fragment_start_address = (*vfs) -> boot_record -> data_start_address + 1 + temp -> start_cluster_ID * CLUSTER_SIZE;

		(*vfs) -> mft -> items[(*vfs) -> mft -> size] -> fragments_created++;
		if (temp -> successful == 1) break; 

		i++;
	}

	if (cluster_count != already_setted) {
		//TODO kontrola zda jsme obsadili všechny potřebné clustery, pokud ne, vyhodit out of memory			
		//free vytvořený item
		printf("smazat item");
	}

	//TODO fwrite fragmenty ??

	fwrite_bitmap(vfs);
}

void fread_mft(VFS **vfs, FILE *file) {
	(*vfs) -> mft = calloc(1, sizeof(MFT));
	fseek(file, (*vfs) -> boot_record -> mft_start_address, SEEK_SET);
	fread((*vfs) -> mft, sizeof(MFT), 1, file);
	
	(*vfs) -> mft -> items = calloc((*vfs) -> mft -> size, sizeof(MFT_ITEM));
	int i;
	for (i = 0; i < (*vfs) -> mft -> size; i++) {
		(*vfs) -> mft -> items[i] = calloc(1, sizeof(MFT_ITEM));
		fseek(file, (*vfs) -> boot_record -> mft_start_address + sizeof(MFT) + 1 + i*sizeof(MFT_ITEM), SEEK_SET);
		fread((*vfs) -> mft -> items[i], sizeof(MFT_ITEM), 1, file); 
	}
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

void remove_directory(VFS **vfs, int uid) {
	int i;
	for (i = 0; i < (*vfs) -> mft -> size; i++) {
		if ((*vfs) -> mft -> items[i] -> uid == uid) {	

			//TODO upravit fragmenty
			//(*vfs) -> bitmap -> data[(*vfs) -> mft -> items[i] -> fragments -> start_cluster_ID] = 0;
			(*vfs) -> mft -> items[i] = (*vfs) -> mft -> items[(*vfs) -> mft -> size - 1];
			(*vfs) -> mft -> items = realloc((*vfs) -> mft -> items, ((*vfs) -> mft -> size - 1) * sizeof(MFT_ITEM));
			(*vfs) -> mft -> size--;


			fwrite_mft(vfs);
			fwrite_mft_item(vfs);
			fwrite_bitmap(vfs);
			return;
		}
	}
}

size_t get_size_of_items(MFT *mft) {
	size_t size = 0;
	int i;
	
	for(i = 0; i < mft -> size; i++) {
		size += mft -> items[i] -> item_size;
	}

	return size;
}

int find_folder_id(MFT *mft, char *path) {
	char actual_path[strlen(path)];
	strcpy(actual_path, path);	

	int i;
	int actual_parentID = 0; //id of root
	int folder_ID = -1;
	int exit = 0;
	char *tok = strtok(actual_path, "/");
	while(1) {
		for (i = 0; i < mft -> size; i++) {
			if (strcmp(tok, mft -> items[i] -> item_name) == 0) {
				if (mft -> items[i] -> parentID == actual_parentID) {
					if (mft -> items[i] -> isDirectory == 0) {
						printf("%s is file, not folder!\n", tok);
						exit = 1;							
						break;
					}
					else {
						folder_ID = mft -> items[i] -> uid;
						actual_parentID = folder_ID;
						break;
					}
				}
			} 
			folder_ID = -1;
		}
		if (folder_ID == -1) break;
		if (exit == 1) break;
		tok = strtok(NULL, "/");
		if (tok == NULL) break;
	}
	return folder_ID;
}

void print_folder_content(MFT *mft, int parentID) {
	int i;
	for (i = 1; i < mft -> size; i++) {		
		if (mft -> items[i] -> parentID == parentID) {
			if (mft -> items[i] -> isDirectory == 1) printf("+"); 
			else printf("-");

			printf("%s\n", mft -> items[i] -> item_name);
		}
	}
}

int is_folder_empty(MFT *mft, int folderID) {
	int i;
	for (i = 0; i < mft -> size; i++) {
		if (mft -> items[i] -> parentID == folderID) return 1;
	}
	
	return 0;
}

void fwrite_mft(VFS **vfs) {
	fseek((*vfs) -> FILE, (*vfs) -> boot_record -> mft_start_address, SEEK_SET);
	fwrite((*vfs) -> mft, sizeof(MFT), 1, (*vfs) -> FILE);
	fseek((*vfs) -> FILE, (*vfs) -> boot_record -> mft_start_address, SEEK_SET);
}

void fwrite_mft_item(VFS **vfs) {
	fseek((*vfs) -> FILE, (*vfs) -> boot_record -> mft_start_address + sizeof(MFT) + 1 + ((*vfs) -> mft -> size - 1)*sizeof(MFT_ITEM), SEEK_SET);
	fwrite((*vfs) -> mft -> items[(*vfs) -> mft -> size - 1], sizeof(MFT_ITEM), 1, (*vfs) -> FILE);
	fseek((*vfs) -> FILE, (*vfs) -> boot_record -> mft_start_address + sizeof(MFT) + 1 + (*vfs) -> mft -> size*sizeof(MFT_ITEM), SEEK_SET);
}

void print_mft(MFT *mft) {
	printf("\nMFT:\n----------------\n");
	printf("Items count: %d\n", mft -> size);
	printf("Size: %lu \n", get_size_of_items(mft));
	printf("\nItems (+ directory, - file):\n");
	int i;
	for (i = 0; i < mft -> size; i++) {
		if (mft -> items[i] -> isDirectory == 1) printf("+");
		else printf("-");
		printf("%s\n", mft -> items[i] -> item_name);
	}
}
