#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>
#include "header.h"
#include <ctype.h>
#include <math.h>

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

int my_atoi(const char* snum)
{
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
