#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

void copy_file(VFS **vfs, char *tok) {

	tok = strtok(NULL, " ");
	if (tok == NULL || strlen(tok) == 0) {
		printf("SOURCE NOT DEFINED\n");
		return;
	}


	tok = strtok(NULL, " ");
	if (tok == NULL || strlen(tok) == 0) {
		printf("DESTINATION NOT DEFINED\n");
		return;
	}

	char source[MAX_LENGTH_OF_COMMAND];
	strcpy(source, tok);
	char destination[MAX_LENGTH_OF_COMMAND];
	strcpy(destination, tok);

	/*
	1) Zkopíruje soubor s1 do umístění s2
	cp s1 s2
	Možný výsledek:
	OK
	FILE NOT FOUND (není zdroj)
	PATH NOT FOUND (neexistuje cílová cesta)
	*/
}

void move_file(VFS **vfs, char *tok) {

	tok = strtok(NULL, " ");
	if (tok == NULL || strlen(tok) == 0) {
		printf("SOURCE NOT DEFINED\n");
		return;
	}


	tok = strtok(NULL, " ");
	if (tok == NULL || strlen(tok) == 0) {
		printf("DESTINATION NOT DEFINED\n");
		return;
	}

	char source[MAX_LENGTH_OF_COMMAND];
	strcpy(source, tok);
	char destination[MAX_LENGTH_OF_COMMAND];
	strcpy(destination, tok);

	/*
	2) Přesune soubor s1 do umístění s2
	mv s1 s2
	Možný výsledek:
	OK
	FILE NOT FOUND (není zdroj)
	PATH NOT FOUND (neexistuje cílová cesta)
	*/
}

void remove_file(VFS **vfs, char *tok) {

	tok = strtok(NULL, " ");

	if (tok == NULL || strlen(tok) == 0) {
		printf("FILE NOT DEFINED\n");
		return;
	}

	/*
	3) Smaže soubor s1
	rm s1
	Možný výsledek:
	OK
	FILE NOT FOUND
	*/
}

void make_directory(VFS **vfs, char *tok) {

	tok = strtok(NULL, " ");

	if (tok == NULL || strlen(tok) == 0) {
		printf("NAME OF DIRECTORY NOT DEFINED\n");
		return;
	}

	/*
	4) Vytvoří adresář a1
	mkdir a1
	Možný výsledek:
	OK
	PATH NOT FOUND (neexistuje zadaná cesta)
	EXIST (nelze založit, již existuje)
	*/
}

void remove_empty_directory(VFS **vfs, char *tok) {

	tok = strtok(NULL, " ");

	if (tok == NULL || strlen(tok) == 0) {
		printf("NAME OF DIRECTORY NOT DEFINED\n");
		return;
	}

	/*
	5) Smaže prázdný adresář a1
	rmdir a1
	Možný výsledek:
	OK
	FILE NOT FOUND (neexistující adresář)
	NOT EMPTY (adresář obsahuje podadresáře, nebo soubory)	
	*/
}

void print_directory(VFS *vfs, char *tok) {

	/*
	6) Vypíše obsah adresáře a1
	ls 
	ls a1
	Možný výsledek:
	-FILE
	+DIRECTORY
	PATH NOT FOUND (neexistující adresář)
	*/
}

void print_file(VFS *vfs, char *tok) {

	tok = strtok(NULL, " ");

	if (tok == NULL || strlen(tok) == 0) {
		printf("FILENAME NOT DEFINED\n");
		return;
	}

	/*
	7) Vypíše obsah souboru s1
	cat s1
	Možný výsledek:
	OBSAH
	FILE NOT FOUND (není zdroj)
	*/
}

void move_to_directory(VFS **vfs, char *tok) {
	if ((tok = strtok(NULL, " ")) != NULL) {
		if (strlen(tok) > 1) {
			if (strncmp(tok, "..", 2) == 0) {				
				if (strlen((*vfs) -> actual_path -> path) > 1) {
					go_to_parent_folder(vfs);
				}
			}
			else {
				//TODO kontrola zda adresar existuje		
				set_path_to_root(vfs);	
				if (tok[0] != 47) strcat((*vfs) -> actual_path -> path, "/");
				if (tok[strlen(tok) - 2] == 47) strncat((*vfs) -> actual_path -> path, tok, strlen(tok) - 2);
				else strncat((*vfs) -> actual_path -> path, tok, strlen(tok) - 1);
			}
			return;
		}
		else {
			printf("NAME OF DIRECTORY NOT DEFINED\n");
			return;
		}
	}
	else {
		set_path_to_root(vfs);	
		return;
	}


	/*
	8) Změní aktuální cestu do adresáře a1
	cd ..
	cd a1
	Možný výsledek:
	OK
	PATH NOT FOUND (neexistující cesta)
	*/
}

void actual_directory(VFS *vfs) {
	
	char path[strlen(vfs -> actual_path -> path)];
	if (strlen(vfs -> actual_path -> path) > 0) strcpy(path, vfs -> actual_path -> path);
	else strcpy(path, "/");
	printf("%s", path);
}

void mft_item_info(VFS *vfs, char *tok) {

	tok = strtok(NULL, " ");

	if (tok == NULL || strlen(tok) == 0) {
		printf("SOURCE NAME NOT DEFINED\n");
		return;
	}


	MFT_ITEM *item = find_mft_item(vfs -> mft, tok);

	if (item == NULL) {
		printf("FILE NOT FOUND\n");
		return;
	}

	printf("NAME: %s\n", item -> item_name);
	printf("UID: %d\n", item -> uid);
	printf("SIZE: %d\n", item -> item_size);
	printf("FRAGMENTS: Location - %d; Count - %d\n", item -> fragments -> fragment_start_address, item -> fragments -> fragment_count);
	int cluster_ID = (item -> fragments -> fragment_start_address - vfs -> boot_record -> data_start_address) / vfs -> boot_record -> cluster_size;
	printf("CLUSTERS: ID - %d\n", cluster_ID);
}

void hd_to_pseudo(VFS **vfs, char *tok) {

	tok = strtok(NULL, " ");
	if (tok == NULL || strlen(tok) == 0) {
		printf("SOURCE NOT DEFINED\n");
		return;
	}


	tok = strtok(NULL, " ");
	if (tok == NULL || strlen(tok) == 0) {
		printf("DESTINATION NOT DEFINED\n");
		return;
	}

	char source[MAX_LENGTH_OF_COMMAND];
	strcpy(source, tok);
	char destination[MAX_LENGTH_OF_COMMAND];
	strcpy(destination, tok);

	/*
	11) Nahraje soubor s1 z pevného disku do umístění s2 v pseudoNTFS
	incp s1 s2
	Možný výsledek:
	OK
	FILE NOT FOUND (není zdroj)
	PATH NOT FOUND (neexistuje cílová cesta)
	*/
}

void pseudo_to_hd(VFS **vfs, char *tok) {

	tok = strtok(NULL, " ");
	if (tok == NULL || strlen(tok) == 0) {
		printf("SOURCE NOT DEFINED\n");
		return;
	}


	tok = strtok(NULL, " ");
	if (tok == NULL || strlen(tok) == 0) {
		printf("DESTINATION NOT DEFINED\n");
		return;
	}

	char source[MAX_LENGTH_OF_COMMAND];
	strcpy(source, tok);
	char destination[MAX_LENGTH_OF_COMMAND];
	strcpy(destination, tok);

	/*
	12) Nahraje soubor s1 z pseudoNTFS do umístění s2 na pevném disku
	outcp s1 s2
	Možný výsledek:
	OK
	FILE NOT FOUND (není zdroj)
	PATH NOT FOUND (neexistuje cílová cesta)
	*/
}

int load_commands(FILE **file_with_commands, char *tok) {
	
	tok = strtok(NULL, " \n");
	(*file_with_commands) = fopen(tok, "r");
	if ((*file_with_commands) != NULL) {
		printf("OK\n");       
		return 1;
	}
        else {
		printf("FILE NOT FOUND\n");
		return 0;
	}
}

void file_formatting(VFS **vfs, char *tok) {

	/*
	14) Příkaz provede formát souboru, který byl zadán jako parametr při spuštení programu na
	souborový systém dané velikosti. Pokud už soubor nějaká data obsahoval, budou přemazána.
	Pokud soubor neexistoval, bude vytvořen.
	format 600MB
	Možný výsledek:
	OK
	CANNOT CREATE FILE
	*/
}

void defrag() {
	/*
	Defragmentace (defrag) – pokud login studenta začíná a-i
	Soubory se budou skládat pouze z 1 fragmentu (předpokládáme dostatek volného místa –
	minimálně ve velikosti největšího souboru).
	*/
}

void full_info(VFS *vfs) {
	print_vfs(vfs);
}

void commands_help() {
	printf("Commands:\n");
	printf("%s - Copy file (%s s1 s2)\n", COPY_FILE, COPY_FILE);
	printf("%s - Move file (%s s1 s2)\n", MOVE_FILE, MOVE_FILE);
	printf("%s - Remove file (%s s1)\n", REMOVE_FILE, REMOVE_FILE);
	printf("%s - Make directory (%s s1)\n", MAKE_DIRECTORY, MAKE_DIRECTORY);
	printf("%s - Remove empty directory (%s s1)\n", REMOVE_EMPTY_DIRECTORY, REMOVE_EMPTY_DIRECTORY);
	printf("%s - Print directory (%s s1)\n", PRINT_DIRECTORY, PRINT_DIRECTORY);
	printf("%s - Print file (%s s1)\n", PRINT_FILE, PRINT_FILE);
	printf("%s - Move to directory (%s s1)\n", MOVE_TO_DIRECTORY, MOVE_TO_DIRECTORY);
	printf("%s - Actual directory (%s)\n", ACTUAL_DIRECTORY, ACTUAL_DIRECTORY);
	printf("%s - MFT Item info (%s s1)\n", MFT_ITEM_INFO, MFT_ITEM_INFO);
	printf("%s - Copy file from HD to pseudoNTFS (%s s1 s2)\n", HD_TO_PSEUDO, HD_TO_PSEUDO);
	printf("%s - Copy file from pseudoNTFS to HD (%s s1 s2)\n", PSEUDO_TO_HD, PSEUDO_TO_HD);
	printf("%s - Load commands from file (%s s1)\n", LOAD_COMMANDS, LOAD_COMMANDS);
	printf("%s - Formatting file (%s 600MB)\n", FILE_FORMATTING, FILE_FORMATTING);
	printf("%s - Defragmentation (%s)\n", DEFRAG, DEFRAG);
	printf("%s - Info about pseudoNTFS (%s)\n", FULL_INFO, DEFRAG);
	printf("%s - Available commands (%s)\n", HELP, HELP);
}
