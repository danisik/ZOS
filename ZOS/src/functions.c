#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>
#include "header.h"
#include <ctype.h>
#include <math.h>

//1 - not same
//0 - same
int compare_two_string(char *string1, char *string2) {
	char s1[strlen(string1)];
	char s2[strlen(string2)];
	
	strcpy(s1, string1);
	strcpy(s2, string2);

	if (strlen(s1) > 0 && s1[strlen(s1) - 1] == '\n') s1[strlen(s1) - 1] = '\0';
	if (strlen(s2) > 0 && s2[strlen(s2) - 1] == '\n') s2[strlen(s2) - 1] = '\0';
	
	if (strlen(s1) != strlen(s2)) return 1;
	else {
		int i;
		for (i = 0; i < strlen(s1); i++) {
			if (s1[i] != s2[i]) return 1;
		}
		return 0;
	}
}

void set_path_to_root(VFS **vfs) {
	memset((*vfs) -> actual_path -> path, 0, PATH_MAX);
}

void go_to_parent_folder(VFS **vfs) {
	int i, length = array_length_strtok((*vfs) -> actual_path -> path);

	char *tok = strtok((*vfs) -> actual_path -> path, "/");
	char path[PATH_MAX];
	memset(path, 0, PATH_MAX);
	
	for (i = 0; i < length - 1; i++) {
		strcat(path, "/");		
		strcat(path, tok);
		tok = strtok(NULL, "/");	
	}

	set_path_to_root(vfs);
	strcpy((*vfs) -> actual_path -> path, path);
}

int array_length_strtok(char *path) {
	char str[PATH_MAX];
	strcpy(str, path);

	char *tok = strtok(str, "/");
	int i = 1;
	while(1) {
		if ((tok = strtok(NULL, "/")) == NULL) break;
		else i++;
	}
	return i;
}

int index_of_last_digit(char *size) {
	char str[strlen(size)];
	strcpy(str, size);
	
	int index = 0;
	while(isdigit(str[index])) {
		index++;
	}

	return index;
}

int get_multiple(char *multiple, int size) {
	int multiple_number = 1;
	char mult[size];
	strncpy(mult, multiple, size);

	switch(size) {
		case 2:
			switch(mult[0]) {
				case 'K':
					multiple_number = 1000;
					break;
				case 'M':
					multiple_number = 1000 * 1000;
					break;
				case 'G':
					multiple_number = 1000 * 1000 * 1000;
					break;
				default:
					printf("Wrong multiple, using only B\n");
					break;
			}
			break;
		default:
			printf("Wrong multiple, using only B\n");
			break;
	}

	return multiple_number;
}

int bitmap_contains_free_cluster(BITMAP *bitmap) {
	int i;
	for (i = 0; i < bitmap -> length; i++) {
		if (bitmap -> data[i] == 0) {
			return 0;
		}
	}
	return 1;
}

int find_free_cluster(BITMAP **bitmap) {
	int i;
	for (i = 0; i < (*bitmap) -> length; i++) {
		if ((*bitmap) -> data[i] == 0) {
			(*bitmap) -> data[i] = 1;
			//TODO fseek set value of this cluster to 1 if is free
			return i;
		}
	}

	return -1;
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

int my_atoi(const char* snum) {
    int idx, strIdx = 0, accum = 0, numIsNeg = 0;
    const unsigned int NUMLEN = (int)strlen(snum);

    if(snum[0] == 0x2d)
        numIsNeg = 1;

    for(idx = NUMLEN - 1; idx >= 0; idx--)
    {
        if(snum[strIdx] >= 0x30 && snum[strIdx] <= 0x39)
            accum += (snum[strIdx] - 0x30) * pow(10, idx);

        strIdx++;
    }

    if(!numIsNeg)
        return accum;
    else
        return accum * -1;
}
