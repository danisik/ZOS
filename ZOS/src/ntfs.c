#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header.h"

VFS *vfs;

int main(int argc, char *argv[]) {
	vfs_init(&vfs, argv[1]);

	/*
	if (argc == 2) {
		vfs_init(&vfs, argv[1]);
	}
	else {
		help();
	}
	*/
}

void help() {
	printf("Help:\n\n");
	printf("First parameter (data filename): iNTFS.dat\n");
	printf("Example: ./ntfs iNTFS.dat\n");	
	return;
}
