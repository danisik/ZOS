#define UID_ITEM_FREE 0
#define MFT_FRAGMENTS_COUNT 1
#define VFS_FILENAME_LENGTH 12
#define DISK_SIZE 10240 //10 KB 
//#define DISK SIZE 10485760 //10 MB
#define CLUSTER_SIZE 256
#define MFT_SIZE_RATIO 0.1
#define DIRECTORY_SIZE 1

#define MAX_LENGTH_OF_COMMAND 100
#define SHELL_CHAR "$"
#define ROOT_CHAR "~"
#define SPLIT_ARGS_CHAR " "
#define ROOT_NAME "root"

#define COPY_FILE "cp"
#define MOVE_FILE "mv"
#define REMOVE_FILE "rm"
#define MAKE_DIRECTORY "mkdir"
#define REMOVE_EMPTY_DIRECTORY "rmdir"
#define PRINT_DIRECTORY "ls"
#define PRINT_FILE "cat"
#define MOVE_TO_DIRECTORY "cd"
#define ACTUAL_DIRECTORY "pwd"
#define MFT_ITEM_INFO "info"
#define HD_TO_PSEUDO "incp"
#define PSEUDO_TO_HD "outcp"
#define LOAD_COMMANDS "load"
#define FILE_FORMATTING "format"
#define DEFRAG "defrag"
#define FULL_INFO "finfo"
#define QUIT "quit"
#define HELP "help"


typedef struct the_bitmap BITMAP;
typedef struct the_mft_fragment MFT_FRAGMENT;
typedef struct the_mft_item MFT_ITEM;
typedef struct the_mft MFT;
typedef struct the_boot_record BOOT_RECORD;
typedef struct the_path PATH;
typedef struct the_vfs VFS;

struct the_bitmap {
	int32_t length;
	unsigned char *data;
};

struct the_mft_fragment {
	int32_t fragment_start_address;     //start adresa
	int32_t fragment_count;             //pocet clusteru ve fragmentu
	int32_t cluster_ID;
};

struct the_mft_item {
	int32_t uid;                                        //UID polozky, pokud UID = UID_ITEM_FREE, je polozka volna
	int32_t parentID;
	int isDirectory;                                    //soubor, nebo adresar (0 soubor, 1 adresar)
	int8_t item_order;                                  //poradi v MFT pri vice souborech, jinak 1
	int8_t item_order_total;                            //celkovy pocet polozek v MFT, jinak 1
	char item_name[12];                                 //8+3 + /0 C/C++ ukoncovaci string znak
	int32_t item_size;                                  //velikost souboru v bytech
	MFT_FRAGMENT fragments[MFT_FRAGMENTS_COUNT]; 	    //fragmenty souboru
};

struct the_mft {
	int32_t size;
	MFT_ITEM **items;
};

struct the_boot_record {
	char signature[9];              //login autora FS
	char volume_descriptor[251];    //popis vygenerovaného FS
	int32_t disk_size;              //celkova velikost VFS
	int32_t cluster_size;           //velikost clusteru
	int32_t cluster_count;          //pocet clusteru
	int32_t mft_start_address;      //adresa pocatku mft
	int32_t bitmap_start_address;   //adresa pocatku bitmapy
	int32_t data_start_address;     //adresa pocatku datovych bloku
	int32_t mft_max_fragment_count; //maximalni pocet fragmentu v jednom zaznamu v mft (pozor, ne souboru)
};

struct the_path {
	char *path;
};

struct the_vfs {
	BOOT_RECORD *boot_record;
	MFT *mft;
	BITMAP *bitmap;
	PATH *actual_path;
	
	char *filename;
	FILE *FILE;	
};


//ntfs.c
void help();

//vfs.c
void vfs_init(VFS **vfs, char *filename, size_t disk_size);
void path_init(VFS **vfs);
void print_vfs(VFS *vfs);
void create_vfs_file(VFS **vfs);

//boot_record.c
void boot_record_init(BOOT_RECORD **boot_record, char *signature, char *volume_descriptor, int32_t disk_size, int32_t cluster_size);
void fread_boot_record(VFS **vfs, FILE *file);
void print_boot_record(BOOT_RECORD *boot_record);

//mft.c
void mft_init(VFS **vfs);
void mft_item_init(VFS **vfs, int uid, int parentID, char *name, int isDirectory, int item_size);
void mft_fragment_init(VFS **vfs, MFT_ITEM **item);
void fread_mft(VFS **vfs, FILE *file);
MFT_ITEM *find_mft_item_by_name(MFT *mft, char *tok);
MFT_ITEM *find_mft_item_by_uid(MFT *mft, int uid);
void print_mft(MFT *mft);

//bitmap.c
void bitmap_init(BITMAP **bitmap, int32_t cluster_count);
void fread_bitmap(VFS **vfs, FILE *file);
void print_bitmap(BITMAP *bitmap);

//commands.c
void copy_file(VFS **vfs, char *tok);
void move_file(VFS **vfs, char *tok);
void remove_file(VFS **vfs, char *tok);
void make_directory(VFS **vfs, char *tok);
void remove_empty_directory(VFS **vfs, char *tok);
void print_directory(VFS *vfs, char *tok);
void print_file(VFS *vfs, char *tok);
void move_to_directory(VFS **vfs, char *tok);
void actual_directory(VFS *vfs);
void mft_item_info(VFS *vfs, char *tok);
void hd_to_pseudo(VFS **vfs, char *tok);
void pseudo_to_hd(VFS **vfs, char *tok);
int load_commands(FILE **file_with_commands, char *tok);
void file_formatting(VFS **vfs, char *tok);
void defrag();
void full_info(VFS *vfs);
void commands_help();

//functions.c
void set_path_to_root(VFS **vfs);
void go_to_parent_folder(VFS **vfs);
int array_length_strtok(char *path);
int index_of_last_digit(char *size);
int get_multiple(char *multiple, int size);
int bitmap_contains_free_cluster(BITMAP *bitmap);
int find_free_cluster(BITMAP **bitmap);
int find_folder_id(MFT *mft, char *path);
void print_folder_content(MFT *mft, int parentID);
int my_atoi(const char* snum);
