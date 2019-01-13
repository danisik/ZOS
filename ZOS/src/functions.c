#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>
#include "header.h"

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
