#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>
#include "header.h"

VFS *vfs;

FILE *file_with_commands;
int is_used_file = 0;
char filename[100] = "ntfs.dat";

int main(int argc, char *argv[]) {
	
	/*
	if (argc == 2) {
		if (strcmp(argv[1], "-help") == 0) {
			help();
			return 0;
		}
		else vfs_init(&vfs, argv[1]);		
	}
	else {
		help();
		return 0;
	}
	*/

	printf("WELCOME IN PSEUDO-NTFS (iNTFS)");
	vfs_init(&vfs, filename, DISK_SIZE);

	vfs -> bitmap -> data[0] = 1;
	vfs -> bitmap -> data[20] = 1;
	
	char command[MAX_LENGTH_OF_COMMAND];
	char *tok;

	while(1) {
		printf("\n%s%s%s ", ROOT_CHAR, vfs -> actual_path -> path, SHELL_CHAR);
		  	
		if (is_used_file == 0) {
        	    fgets(command, MAX_LENGTH_OF_COMMAND, stdin);
        	}
        	else {
        	    	fgets(command, MAX_LENGTH_OF_COMMAND, file_with_commands);
	
        		if (feof(file_with_commands) == 1) { //check end of file
        			fclose(file_with_commands);
        		        is_used_file = 0;
          		}

	            	printf("%s", command);
        	}
		tok = strtok(command, SPLIT_ARGS_CHAR);

	

		if (strncmp(tok, COPY_FILE, strlen(COPY_FILE)) == 0) {
			copy_file(&vfs, tok); //not implemented
		}
		else if (strncmp(tok, MOVE_FILE, strlen(MOVE_FILE)) == 0) {
			move_file(&vfs, tok); //not implemented
		}
		else if (strncmp(tok, REMOVE_FILE, strlen(REMOVE_FILE)) == 0) {
			remove_file(&vfs, tok); //not implemented 
		}
		else if (strncmp(tok, MAKE_DIRECTORY, strlen(MAKE_DIRECTORY)) == 0) {
			make_directory(&vfs, tok); //not implemented
		}
		else if (strncmp(tok, REMOVE_EMPTY_DIRECTORY, strlen(REMOVE_EMPTY_DIRECTORY)) == 0) {
			remove_empty_directory(&vfs, tok); //not implemented
		}
		else if (strncmp(tok, PRINT_DIRECTORY, strlen(PRINT_DIRECTORY)) == 0) {
			print_directory(vfs, tok); //not implemented
		}
		else if (strncmp(tok, PRINT_FILE, strlen(PRINT_FILE)) == 0) {
			print_file(vfs, tok); //not implemented
		}
		else if (strncmp(tok, MOVE_TO_DIRECTORY, strlen(MOVE_TO_DIRECTORY)) == 0) {
			move_to_directory(&vfs, tok); //need to check if writed directory is ok
		}
		else if (strncmp(tok, ACTUAL_DIRECTORY, strlen(ACTUAL_DIRECTORY)) == 0) {
			actual_directory(vfs);
		}
		else if (strncmp(tok, MFT_ITEM_INFO, strlen(MFT_ITEM_INFO)) == 0) {
			mft_item_info(vfs, tok);
		}
		else if (strncmp(tok, HD_TO_PSEUDO, strlen(HD_TO_PSEUDO)) == 0) {
			hd_to_pseudo(&vfs, tok); //not implemented
		}
		else if (strncmp(tok, PSEUDO_TO_HD, strlen(PSEUDO_TO_HD)) == 0) {
			pseudo_to_hd(&vfs, tok); //not implemented
		}
		else if (strncmp(tok, LOAD_COMMANDS, strlen(LOAD_COMMANDS)) == 0) {
			is_used_file = load_commands(&file_with_commands, tok); 
		}
		else if (strncmp(tok, FILE_FORMATTING, strlen(FILE_FORMATTING)) == 0) {
			file_formatting(&vfs, tok);
		}
		else if (strncmp(tok, DEFRAG, strlen(DEFRAG)) == 0) {
			defrag(); //not implemented
		}
		else if (strncmp(tok, HELP, strlen(HELP)) == 0) {
			commands_help();
		}
		else if (strncmp(tok, FULL_INFO, strlen(FULL_INFO)) == 0) {
			full_info(vfs);
		}
		else if (strncmp(tok, QUIT, strlen(QUIT)) == 0) {
			break;
		}
		else {
			printf("Unknown command\n");
		}
	}
	printf("pseudoNTFS end\n");
}

void help() {
	printf("Help:\n\n");
	printf("First parameter (data filename): iNTFS.dat\n");
	printf("Example: ./ntfs iNTFS.dat\n");	
	return;
}
