#ifndef NTFS_H
#define NTFS_H

#include <cstring>
#include <stdint.h>
#include <string>
#include <vector>
#include "file_system.h"
#include "node.h"

using namespace std;

class NTFS : public FileSystem {
public:
	/**
	* Creates a new file system.
	*/
	NTFS(const string name);

	/**
	* Checks a path validity. Function uses generateNodeID to find file on a file system.
	* @param path The path.
	* @return true if path exists and is valid.
	*/
	bool isPathValid(const Path &path) const;

	/**
	* Allocates a new node on a file system.
	* @param node The new allocated node.
	*/
	void allocateNode(const Node &node);

	/**
	* Opens a node and set its members. If node does not exist it throws file_not_found exception.
	* @param node The node from a file system.
	*/
	void openNode(Node &node);

	/**
	* Opens a node and set its members.
	* @param uid The uid of a node from a file system.
	* @param parent Path to a parent of the node.
	* @return Found node or null.
	*/
	Node* openNode(const uint32_t uid, const Path &parent) const;

	/**
	* Reads data from the given address on a file system. The given vector is cleared before reading.
	* @param address The address of data. wrong_address is thrown when the address does not point to a data section.
	* @param s_data Smallest data block that was read.
	*/
	void readData(const uint32_t address, vector<unsigned char> &s_data) const;

	/**
	* Writes an existing node into a file system. Second parameter contains data that can be stored on a file system.
	* If node has not been alocated function throws alloc_error.
	* @param node The node that is going to be written.
	* @param data New data holded and stored by the given node.
	*/
	void writeNode(Node &node, const vector<unsigned char> &data = vector<unsigned char>());

	/**
	* Deletes the given node from a file system. Function does not remove data from a file system, just its pointers.
	* If node has not been alocated function throws alloc_error. If the node is a directory with children function throws
	* delete_error.
	* @param node The given node that is going to be deleted.
	*/
	void deleteNode(const Node &node);

	/**
	* Copies the given node to the selected path. If node has not been alocated function throws alloc_error.
	* If the target path does not exist or is not a directory function throws path_not_found.
	* @param node The given node that is going to be copied.
	* @param path The target path of a copy process.
	*/
	void copyNode(const Node &node, const Path &path);

	/**
	* Moves the given node to the selected path. If node has not been alocated function throws alloc_error.
	* If the target path does not exist or is not a directory function throws path_not_found.
	* @param node The given node that is going to be moved.
	* @param path The target path of a move process.
	*/
	void moveNode(const Node &node, const Path &path);

	/**
	* Data block of files will be placed in one fragment. No spaces between data blocks and files are allowed.
	*/
	void defragment();

	/**
	* Checks size of files and number of data segments per file. If file system is corrupted function throws
	* consistency_error.
	*/
	void checkConsistency();

protected:

	/**
	* Writes data into the given address on a file system. It can write just one block of data. Number of written bytes is returned from a function.
	* @param address The address of data. Invalid argument is thrown when the address does not point to a data section.
	* @param s_data Data that is going to be written.
	* @return The number of written bytes.
	*/
	uint32_t writeData(const uint32_t address, const vector<unsigned char> &s_data);

	/**
	* Generates a unique uid from the given path. Function should use some hash function.
	* @param path The given path which is going to be transformed to the uid.
	* @return The unique 32-bit long uid.
	*/
	uint32_t generateNodeID(const string path) const;

private:	
	/** Uid of the free item. */
	static const uint32_t UID_ITEM_FREE = 0;
	/** Default value of item order. */
	static const uint16_t SINGLE_ITEM_ORDER = 1;
	/** Default fragments count. */
	static const uint32_t MFT_FRAGMENTS_COUNT = 32;
	/** Default cluster size. */
	static const uint32_t DEFAULT_CLUSTER_SIZE = 512;
	/** Default cluster count. */
	static const uint32_t DEFAULT_CLUSTER_COUNT = 31250;
	/** Default mft count. */
	static const uint32_t DEFAULT_MFT_COUNT = 7000;

	struct boot_record {
		char signature[9];              //login autora FS
		char volume_descriptor[251];    //popis vygenerovaného FS
		uint32_t disk_size;             //celkova velikost VFS v bytech
		uint32_t cluster_size;          //velikost clusteru bytech
		uint32_t cluster_count;         //pocet clusteru
		uint32_t root_dir_uid;			// uid korenoveho adresare
		uint32_t mft_item_count;		// pocet mft itemu
		uint32_t mft_item_start_address;     //adresa pocatku mft (pocet bytu od zacatku souboru)
		uint32_t bitmap_start_address;  //adresa pocatku bitmapy (pocet bytu od zacatku souboru)
		uint32_t data_start_address;    //adresa pocatku datovych bloku (pocet bytu od zacatku souboru)
	};

	struct mft_fragment {
		uint32_t fragment_start_address;     //start adresa
		uint32_t cluster_count;             //pocet clusteru ve fragmentu
	};

	struct mft_item {
		uint32_t uid;                  //UID polozky, pokud UID = UID_ITEM_FREE, je polozka volna
		bool is_directory;              //soubor, nebo adresar
		uint16_t item_order;             //poradi v MFT pokud soubor zabira vice mft itemu
		uint16_t item_order_total;       //celkovy pocet  v MFT
		char item_name[12];            //8+3 + /0 C/C++ ukoncovaci string znak
		uint32_t item_size;            //velikost souboru v bytech
		uint32_t fragments_count;
		mft_fragment fragments[MFT_FRAGMENTS_COUNT]; //fragmenty souboru

		inline bool operator<(const mft_item &cmp_item) const { return item_order < cmp_item.item_order; }
		inline bool operator==(const mft_item &cmp_item) const { return cmp_item.uid == uid && cmp_item.item_order == item_order; }
		static void createItem(mft_item &item, uint32_t uid, bool is_directory, uint16_t item_order, uint16_t item_order_total, string item_name, uint32_t item_size, uint32_t fragments_count) {
			item.uid = uid;
			item.is_directory = is_directory;
			item.item_order = item_order;
			item.item_order_total = item_order_total;
			strncpy(item.item_name, item_name.data(), sizeof(item.item_name));
			item.item_size = item_size;
			item.fragments_count = fragments_count;
		}
	};

	/**
	* Finds an item that contains the given address. Function returns pointer to found item (has to be deleted)
	* and index of a fragment that containts the given address.
	* @param address The address from a data section.
	* @param fragment_index The index of a fragment that contains the address.
	* @return The found item.
	*/
	mft_item* containsCluster(const uint32_t address, int &fragment_index);

	/**
	* Reads items of a parent of the given node.
	* @param parent_items A vector of the items.
	* @param node The child node.
	*/
	void readParentItems(vector<mft_item> &parent_items, const Node &node);

	/**
	* Appends data to the item. The given item has to be last item of a file. When it would not be
	* last you can expect undefined behavior.
	* @param data_holder Last item of the file that will hold data.
	*/
	void appendData(mft_item &data_holder, const vector<unsigned char> &data);

	/**
	* Adds fragments to the item. If the item is full function creates a new one.
	* @param item The item that will contain fragments.
	* @param fragments New fragments.
	*/
	void addFragments(mft_item &item, const vector<mft_fragment> &fragments);

	/**
	* Finds some free space from the back of a file system. Amount of space is defined through the second
	* argument of the function. Function finds exact sequence of free memory that can keep given amount of space.
	* On the other hand findFreeSpace just tries to do so.
	* @param space_indexes Indexes to data segment of a file system that can hold given amount of required memory.
	* @param space_count Required number of clusters.
	*/
	void findExactFreeSpaceBackward(mft_fragment &free_fragment, const uint32_t space_count);

	/**
	* Finds some free space. Amount of space is defined through the second argument of the function.
	* Function tries to find sequence of free memory that can keep given amount of space.
	* When it is not possible, function finds free space from different places of a file system.
	* @param space_indexes Indexes to data segment of a file system that can hold given amount of required memory.
	* @param space_count Required number of clusters.
	*/
	void findFreeSpace(vector<mft_fragment> &space_fragments, const uint32_t cluster_count);

	/**
	* Allocates a new fragment and edites bitmap.
	* @param bitmap_address Address of data in bitmap.
	* @param data_address Address of data in data segment.
	* @param cluster_count Number of allocated clusters.
	* @param fragment Allocated fragment.
	*/
	void allocateData(const uint32_t bitmap_address, const uint32_t data_address, const uint32_t cluster_count, mft_fragment &fragment);

	/**
	* Reads an mft item. Returns a pointer to the read item (has to be deleted).
	* @param uid UID of the item.
	* @return The pointer to the read item.
	*/
	mft_item* readItem(const uint32_t uid) const;

	/**
	* Reads a vector of mft items.
	* @param uid UID of the item.
	* @param mft_item_vector The vector of read items.
	*/
	void readItems(const uint32_t uid, vector<mft_item> &mft_item_vector) const;

	/**
	* Writes the given mft item into a file system.
	* @param item The written item.
	*/
	void writeItem(const mft_item &item);

	/**
	* Edites the given mft item in a file system. Function checks uid and item_order.
	* @param item The given item.
	*/
	void editItem(const mft_item &item);

	/**
	* Removes the given mft item from a file system. Function checks uid and item_order.
	* @param item The given item.
	*/
	void deleteItem(const mft_item &item);

	/**
	* Creates a new boot section.
	*/
	void createBoot();

	/**
	* Reads a boot section.
	* @param boot Boot section.
	*/
	void readBoot(boot_record &boot) const;

	/**
	* Creates a new root directory of a file system.
	*/
	void createRoot();

	/**
	* Creates a new mft section.
	*/
	void createMft();

	/**
	* Creates a new bitmap section.
	*/
	void createBitmap();

	/**
	* Resets bitmap value on the given address.
	* @param address The given address.
	*/
	void resetBitmap(uint32_t address);

	/**
	* Sets cluster as used on the given address.
	* @param address The given address.
	*/
	void allocateSpaceInBitmap(uint32_t address);

	/**
	* Creates a new data section.
	*/
	void createData();
};

#endif // !NTFS_H



