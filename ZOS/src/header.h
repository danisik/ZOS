#define UID_ITEM_FREE 0
#define MFT_FRAGMENTS_COUNT 32

#define DISK_SIZE 10240 //10 KB 
//#define DISK SIZE 10485760 //10 MB
#define CLUSTER_SIZE 512

#define MFT_SIZE_RATIO 0.1

typedef struct the_bitmap BITMAP;
typedef struct the_mft_fragment MFT_FRAGMENT;
typedef struct the_mft_item MFT_ITEM;
typedef struct the_mft MFT;
typedef struct the_boot_record BOOT_RECORD;
typedef struct the_vfs VFS;

struct the_bitmap {
	int32_t length;
	unsigned char *data;
};

struct the_mft_fragment {
	int32_t fragment_start_address;     //start adresa
	int32_t fragment_count;             //pocet clusteru ve fragmentu
};

struct the_mft_item {
	int32_t uid;                                        //UID polozky, pokud UID = UID_ITEM_FREE, je polozka volna
	int isDirectory;                                    //soubor, nebo adresar (0 soubor, 1 adresar)
	int8_t item_order;                                  //poradi v MFT pri vice souborech, jinak 1
	int8_t item_order_total;                            //celkovy pocet polozek v MFT, jinak 1
	char item_name[12];                                 //8+3 + /0 C/C++ ukoncovaci string znak
	int32_t item_size;                                  //velikost souboru v bytech
	MFT_FRAGMENT fragments[MFT_FRAGMENTS_COUNT]; 	//fragmenty souboru
};

struct the_mft {
	int32_t size;
	MFT_ITEM *items;
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

struct the_vfs {
	BOOT_RECORD *boot_record;
	MFT *mft;
	BITMAP *bitmap;
	
	char *filename;
	FILE *FILE;	
};


//ntfs.c
void help();

//vfs.c
void vfs_init(VFS **vfs, char *filename);
void print_vfs(VFS *vfs);

//boot_record.c
void boot_record_init(BOOT_RECORD **boot_record, char *signature, char *volume_descriptor, int32_t disk_size, int32_t cluster_size);
void print_boot_record(BOOT_RECORD *boot_record);

//mft.c
void mft_init(MFT **mft);
void mft_item_init();
void mft_fragment_init();
void print_mft(MFT *mft);

//bitmap.c
void bitmap_init(BITMAP **bitmap, int32_t cluster_count);
int32_t calculate_length(int32_t cluster_count);
void print_bitmap(BITMAP *bitmap);

//commands.c
void copy_file();
void move_file();
void remove_file();
void make_directory();
void remove_empty_directory();
void print_directory();
void print_file();
void move_to_directory();
void actual_directory();
void mft_item_info();
void hd_to_pseudo();
void pseudo_to_hd();
void load_commands();
void file_formatting();

