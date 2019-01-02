#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

void copy_file() {

	/*
	1) Zkopíruje soubor s1 do umístění s2
	cp s1 s2
	Možný výsledek:
	OK
	FILE NOT FOUND (není zdroj)
	PATH NOT FOUND (neexistuje cílová cesta)
	*/
}

void move_file() {

	/*
	2) Přesune soubor s1 do umístění s2
	mv s1 s2
	Možný výsledek:
	OK
	FILE NOT FOUND (není zdroj)
	PATH NOT FOUND (neexistuje cílová cesta)
	*/
}

void remove_file() {

	/*
	3) Smaže soubor s1
	rm s1
	Možný výsledek:
	OK
	FILE NOT FOUND
	*/
}

void make_directory() {

	/*
	4) Vytvoří adresář a1
	mkdir a1
	Možný výsledek:
	OK
	PATH NOT FOUND (neexistuje zadaná cesta)
	EXIST (nelze založit, již existuje)
	*/
}

void remove_empty_directory() {

	/*
	5) Smaže prázdný adresář a1
	rmdir a1
	Možný výsledek:
	OK
	FILE NOT FOUND (neexistující adresář)
	NOT EMPTY (adresář obsahuje podadresáře, nebo soubory)	
	*/
}

void print_directory() {
	/*
	6) Vypíše obsah adresáře a1
	ls a1
	Možný výsledek:
	-FILE
	+DIRECTORY
	PATH NOT FOUND (neexistující adresář)
	*/
}

void print_file() {

	/*
	7) Vypíše obsah souboru s1
	cat s1
	Možný výsledek:
	OBSAH
	FILE NOT FOUND (není zdroj)
	*/
}

void move_to_directory() {

	/*
	8) Změní aktuální cestu do adresáře a1
	cd a1
	Možný výsledek:
	OK
	PATH NOT FOUND (neexistující cesta)
	*/
}

void actual_directory() {
	
	/*
	9) Vypíše aktuální cestu
	pwd
	Možný výsledek:
	PATH
	*/
}

void mft_item_info() {

	/*
	10) Vypíše informace o souboru/adresáři s1/a1 (v jakých fragmentech/clusterech se nachází),
	uid, …
	info a1/s1
	Možný výsledek:
	NAME – UID – SIZE - FRAGMENTY - CLUSTERY
	FILE NOT FOUND (není zdroj)
	*/
}

void hd_to_pseudo() {

	/*
	11) Nahraje soubor s1 z pevného disku do umístění s2 v pseudoNTFS
	incp s1 s2
	Možný výsledek:
	OK
	FILE NOT FOUND (není zdroj)
	PATH NOT FOUND (neexistuje cílová cesta)
	*/
}

void pseudo_to_hd() {

	/*
	12) Nahraje soubor s1 z pseudoNTFS do umístění s2 na pevném disku
	outcp s1 s2
	Možný výsledek:
	OK
	FILE NOT FOUND (není zdroj)
	PATH NOT FOUND (neexistuje cílová cesta)
	*/
}

void load_commands() {

	/*
	13) Načte soubor z pevného disku, ve kterém budou jednotlivé příkazy a začne je sekvenčně
	vykonávat. Formát je 1 příkaz/1řádek
	load s1
	Možný výsledek:
	OK	
	FILE NOT FOUND (není zdroj)
	*/
}

void file_formatting() {

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
