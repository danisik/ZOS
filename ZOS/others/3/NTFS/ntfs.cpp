
#include <cstdio>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include "ntfs.h"
#include "exception.h"

/*
* __________________________________CONSTRUCTORS______________________________________
*/

/**
* Creates a new file system.
*/
NTFS::NTFS(const string name) : FileSystem(name) {
	this->file = fopen(this->file_name.c_str(), "rb+");
	
	if (this->file == NULL) {
		this->file = fopen(this->file_name.c_str(), "wb+");
		createBoot();
		createMft();
		createBitmap();
		createData();
	}
}

/*
* __________________________________PUBLIC FUNCTIONS______________________________________
*/

/**
* Checks a path validity. Function uses generateNodeID to find file on a file system.
* @param path The path.
* @return true if path exists and is valid.
*/
bool NTFS::isPathValid(const Path &path) const {
	mft_item* item = this->readItem(this->generateNodeID(path.toString()));
	if (item == NULL) {
		return false;
	}
	else {
		delete item;
		return true;
	}
}

/**
* Allocates a new node on a file system.
* @param node The new allocated node.
*/
void NTFS::allocateNode(const Node &node) {
	vector<mft_item> parent_item_vector;
	this->readParentItems(parent_item_vector, node);
	if (parent_item_vector.empty()) {
		throw file_not_found("NTFS ALLOCATENODE: The node could not been alocated. Its parent does not exist.");
	}

	if (this->isPathValid(node.path)) {
		throw alloc_error("NTFS ALLOCATENODE: The node cannot be allocated. It already exists.");
	}

	/* Creates a new node item. */
	mft_item node_item;
	mft_item::createItem(node_item, generateNodeID(node.path.toString()), node.is_directory, SINGLE_ITEM_ORDER, SINGLE_ITEM_ORDER, node.name, 0, 0);

	/* Edits file size in all items. */
	for (vector<mft_item>::iterator it = parent_item_vector.begin(); it != parent_item_vector.end() - 1; it++) {
		it->item_size += sizeof(uint32_t);
	}

	vector<unsigned char> node_item_uid(sizeof(uint32_t));
	memcpy(&node_item_uid.at(0), &node_item.uid, sizeof(uint32_t));

	this->appendData(parent_item_vector.back(), node_item_uid);

	/* Save changes to all items. */
	for (vector<mft_item>::iterator it = parent_item_vector.begin(); it != parent_item_vector.end(); it++) {
		this->editItem(*it);
	}

	this->writeItem(node_item);
}

/**
* Opens a node and set its members. If node does not exist it throws file_not_found exception.
* @param node The node from a file system.
*/
void NTFS::openNode(Node &node) {
	vector<mft_item> items;
	this->readItems(this->generateNodeID(node.path.toString()), items);
	if (items.empty()) {
		throw file_not_found("NTFS OPENNODE: Target does not exist.");
	}

	if (items.front().is_directory != node.is_directory) {
		throw file_not_found("NTFS OPENNODE: Target does not exist.");
	}

	boot_record boot;
	this->readBoot(boot);

	node.size = items.front().item_size;
	for (vector<mft_item>::const_iterator it = items.begin(); it != items.end(); it++) {
		for (uint32_t i = 0; i < it->fragments_count; i++) {
			for (uint32_t j = 0; j < it->fragments[i].cluster_count; j++) {
				node.addData(it->fragments[i].fragment_start_address + j * boot.cluster_size);
			}
		}
	}
}

/**
* Opens a node and set its members.
* @param uid The uid of a node from a file system.
* @param parent Path to a parent of the node.
* @return Found node or null.
*/
Node* NTFS::openNode(const uint32_t uid, const Path &parent) const {
	vector<mft_item> items;
	this->readItems(uid, items);
	if (items.empty()) {
		return NULL;
	}

	boot_record boot;
	this->readBoot(boot);

	Node *node = new Node(Path(items.front().item_name, parent.toString()), items.front().is_directory);
	node->size = items.front().item_size;
	for (vector<mft_item>::const_iterator it = items.begin(); it != items.end(); it++) {
		for (uint32_t i = 0; i < it->fragments_count; i++) {
			for (uint32_t j = 0; j < it->fragments[i].cluster_count; j++) {
				node->addData(it->fragments[i].fragment_start_address + j * boot.cluster_size);
			}
		}
	}

	return node;
}

/**
* Reads data from the given address on a file system. The given vector is cleared before reading.
* @param address The address of data. wrong_address is thrown when the address does not point to a data section.
* @param s_data Smallest data block that was read.
*/
void NTFS::readData(const uint32_t address, vector<unsigned char> &s_data) const {
	boot_record boot;
	this->readBoot(boot);
	if (address < boot.data_start_address || address > boot.disk_size) {
		throw wrong_address("NTFS READDATA: The entered address it not in a data section, " + address);
	}

	fseek(this->file, address, SEEK_SET);
	s_data = vector<unsigned char>(boot.cluster_size);
	fread(&s_data.at(0), 1, boot.cluster_size, this->file);
}

/**
* Writes an existing node into a file system. Second parameter contains data that can be stored on a file system.
* If node has not been alocated function throws alloc_error.
* @param node The node that is going to be written.
* @param data New data holded and stored by the given node.
*/
void NTFS::writeNode(Node &node, const vector<unsigned char> &data) {
	vector<mft_item> items;
	this->readItems(this->generateNodeID(node.path.toString()), items);
	if (items.empty()) {
		throw alloc_error("NTFS WRITENODE: The node has not been alocated.");
	}

	/* Edits file size in all items. */
	for (vector<mft_item>::iterator it = items.begin(); it != items.end() - 1; it++) {
		it->item_size += data.size();
	}

	this->appendData(items.back(), data);

	/* Save changes to all items. */
	for (vector<mft_item>::iterator it = items.begin(); it != items.end(); it++) {
		this->editItem(*it);
	}
}

/**
* Deletes the given node from a file system. Function does not remove data from a file system, just its pointers.
* If node has not been alocated function throws alloc_error. If the node is a directory with children function throws
* delete_error.
* @param node The given node that is going to be deleted.
*/
void NTFS::deleteNode(const Node &node) {
	vector<mft_item> items;
	this->readItems(this->generateNodeID(node.path.toString()), items);
	if (items.empty()) {
		throw alloc_error("NTFS DELETENODE: The node has not been alocated.");
	}

	if (node.is_directory && node.size != 0) {
		throw delete_error("NTFS DELETENODE: Unable to delete directory with files.");
	}

	boot_record boot;
	this->readBoot(boot);

	vector<mft_item> parent_items;
	this->readParentItems(parent_items, node);
	
	bool free = false;
	vector<unsigned char> children;
	for (vector<mft_item>::const_iterator it = parent_items.begin(); it != parent_items.end(); it++) {
		for (uint32_t i = 0; i < it->fragments_count; i++) {
			for (uint32_t j = 0; j < it->fragments[i].cluster_count; j++) {
				vector<unsigned char> temp_children;
				uint32_t cluster_address = it->fragments[i].fragment_start_address + j * boot.cluster_size;
				this->readData(cluster_address, temp_children);
				children.insert(children.end(), temp_children.begin(), temp_children.end());

				uint32_t bitmap_address = boot.bitmap_start_address + (cluster_address - boot.data_start_address) / boot.cluster_size;
				fseek(this->file, bitmap_address, SEEK_SET);
				fwrite(&free, sizeof(bool), 1, this->file);
			}
		}
	}

	/* Deletes almost all parent items. */
	for (vector<mft_item>::iterator it = parent_items.begin() + 1; it != parent_items.end(); it++) {
		this->deleteItem(*it);
	}

	mft_item first_parent_item = parent_items.front();

	children.erase(children.begin() + first_parent_item.item_size , children.end());

	for (int i = 0; i < children.size(); i = i + sizeof(uint32_t)) {
		uint32_t uid;
		memcpy(&uid, &children.at(i), sizeof(uint32_t));
		if (uid == items.front().uid) {
			vector<unsigned char>::iterator start_it = children.begin() + i;
			children.erase(start_it, start_it + sizeof(uint32_t));
		}
	}

	first_parent_item.fragments[MFT_FRAGMENTS_COUNT];
	first_parent_item.fragments_count = 0;
	first_parent_item.item_size = 0;
	first_parent_item.item_order_total = 1;

	this->appendData(first_parent_item, children);
	this->editItem(first_parent_item);

	/* Edits file size in all items. */
	for (vector<mft_item>::iterator it = items.begin(); it != items.end(); it++) {
		this->deleteItem(*it);
	}
}

/**
* Copies the given node to the selected path. If node has not been alocated function throws alloc_error. 
* If the target path does not exist or is not a directory function throws path_not_found.
* @param node The given node that is going to be copied.
* @param path The target path of a copy process.
*/
void NTFS::copyNode(const Node &node, const Path &path) {
	vector<mft_item> items;
	this->readItems(this->generateNodeID(node.path.toString()), items);
	if (items.empty()) {
		throw alloc_error("NTFS COPYNODE: The node has not been alocated.");
	}

	vector<mft_item> target_dir;
	this->readItems(this->generateNodeID(path.toString()), target_dir);
	if (target_dir.empty() || !target_dir.front().is_directory) {
		throw path_not_found("NTFS COPYNODE: Target does not exist or is not a directory.");
	}

	/* Edits file size in all items. */
	for (vector<mft_item>::iterator it = target_dir.begin(); it != target_dir.end() - 1; it++) {
		it->item_size += sizeof(uint32_t);
	}

	vector<unsigned char> content;
	for (vector<uint32_t>::const_iterator it = node.getData().begin(); it != node.getData().end(); it++) {
		vector<unsigned char> temp_content;
		this->readData(*it, temp_content);
		content.insert(content.end(), temp_content.begin(), temp_content.end());
	}

	content.erase(content.end() - (content.size() - node.size), content.end());

	mft_item node_item;
	mft_item::createItem(node_item, generateNodeID(path.toString() + '/' + node.name), 
		items.front().is_directory, 1, 1,
		items.front().item_name, 0, 0);
	this->appendData(node_item, content);

	vector<unsigned char> node_item_uid(sizeof(uint32_t));
	memcpy(&node_item_uid.at(0), &node_item.uid, sizeof(uint32_t));

	this->appendData(target_dir.back(), node_item_uid);

	/* Save changes to all items. */
	for (vector<mft_item>::iterator it = target_dir.begin(); it != target_dir.end(); it++) {
		this->editItem(*it);
	}

	this->writeItem(node_item);
}

/**
* Moves the given node to the selected path. If node has not been alocated function throws alloc_error. 
* If the target path does not exist or is not a directory function throws path_not_found.
* @param node The given node that is going to be moved.
* @param path The target path of a move process.
*/
void NTFS::moveNode(const Node &node, const Path &path) {
	vector<mft_item> items;
	this->readItems(this->generateNodeID(node.path.toString()), items);
	if (items.empty()) {
		throw alloc_error("NTFS MOVENODE: The node has not been alocated.");
	}

	vector<mft_item> target_dir;
	this->readItems(this->generateNodeID(path.toString()), target_dir);
	if (target_dir.empty() || !target_dir.front().is_directory) {
		throw path_not_found("NTFS MOVENODE: Target does not exist or is not a directory.");
	}

	boot_record boot;
	this->readBoot(boot);

	vector<mft_item> parent_items;
	this->readParentItems(parent_items, node);

	vector<unsigned char> children;
	for (vector<mft_item>::const_iterator it = parent_items.begin(); it != parent_items.end(); it++) {
		for (uint32_t i = 0; i < it->fragments_count; i++) {
			for (uint32_t j = 0; j < it->fragments[i].cluster_count; j++) {
				vector<unsigned char> temp_children;
				this->readData(it->fragments[i].fragment_start_address + j * boot.cluster_size, temp_children);
				children.insert(children.end(), temp_children.begin(), temp_children.end());
			}
		}
	}

	/* Deletes almost all parent items. */
	for (vector<mft_item>::iterator it = parent_items.begin() + 1; it != parent_items.end(); it++) {
		this->deleteItem(*it);
	}

	mft_item first_parent_item = parent_items.front();

	children.erase(children.begin() + first_parent_item.item_size, children.end());

	for (int i = 0; i < children.size(); i = i + sizeof(uint32_t)) {
		uint32_t uid;
		memcpy(&uid, &children.at(i), sizeof(uint32_t));
		if (uid == items.front().uid) {
			vector<unsigned char>::iterator start_it = children.begin() + i;
			children.erase(start_it, start_it + sizeof(uint32_t));
		}
	}

	memset(first_parent_item.fragments, 0, sizeof(first_parent_item.fragments));
	first_parent_item.fragments_count = 0;
	first_parent_item.item_size = 0;
	first_parent_item.item_order_total = 1;

	this->appendData(first_parent_item, children);
	this->editItem(first_parent_item);

	for (vector<mft_item>::iterator it = items.begin(); it != items.end(); it++) {
		this->deleteItem(*it);
		it->uid = this->generateNodeID(path.toString() + '/' + node.name);
		this->writeItem(*it);
	}

	vector<unsigned char> node_item_uid(sizeof(uint32_t));
	memcpy(&node_item_uid.at(0), &items.front().uid, sizeof(uint32_t));
	this->appendData(target_dir.back(), node_item_uid);
	this->editItem(target_dir.back());
}

/**
* Data block of files will be placed in one fragment. No spaces between data blocks and files are allowed.
*/
void NTFS::defragment() {
	/* Read boot */
	boot_record boot;
	this->readBoot(boot);

	/* Start address */
	uint32_t data_address = boot.data_start_address;
	mft_item temp_item;

	/* Loop over all mft items */
	for (uint32_t items_index = 0; items_index < boot.mft_item_count; items_index++) {
		fread(&temp_item, sizeof(mft_item), 1, this->file);

		/* Check if item is not free */
		if (temp_item.uid != UID_ITEM_FREE) {

			/* Read all items with uid */
			vector<mft_item> file_items;
			this->readItems(temp_item.uid, file_items);

			/* New fragment that will hold all data */
			mft_fragment new_fragment;
			new_fragment.fragment_start_address = data_address;
			new_fragment.cluster_count = 0;

			/* Iterate over vector of items with read uid */
			for (vector<mft_item>::iterator it = file_items.begin(); it != file_items.end(); it++) {

				/* Loop over all fragments */
				for (uint32_t fragments_index = 0; fragments_index < it->fragments_count; fragments_index++) {

					/* Add number of clusters to the new fragment */
					new_fragment.cluster_count += it->fragments[fragments_index].cluster_count;

					/* Loop over clusters in the old fragments */
					for (uint32_t cluster_index = 0; cluster_index < it->fragments[fragments_index].cluster_count; cluster_index++) {

						/* Check if cluster does not lie on the right address where it should end */
						if (it->fragments[fragments_index].fragment_start_address + cluster_index * boot.cluster_size != data_address) {

							/* Find item that lies on the target address */
							int found_fragment_index;
							mft_item *found_item = this->containsCluster(data_address, found_fragment_index);
							if (found_item != NULL) {

								/* Find a new place for data */
								mft_fragment free_fragment;
								this->findExactFreeSpaceBackward(free_fragment, found_item->fragments[found_fragment_index].cluster_count);

								/* Read data, move them and reset bitmap */
								vector<unsigned char> data_move;
								for (uint32_t data_index = 0; data_index < free_fragment.cluster_count; data_index++) {
									this->readData(found_item->fragments[found_fragment_index].fragment_start_address + data_index * boot.cluster_size, data_move);
									this->writeData(free_fragment.fragment_start_address + data_index * boot.cluster_size, data_move);
									data_move.clear();

									uint32_t bitmap_address = boot.bitmap_start_address + (found_item->fragments[found_fragment_index].fragment_start_address + data_index * boot.cluster_size - boot.data_start_address) / boot.cluster_size;
									this->resetBitmap(bitmap_address);
								}

								/* Edit found fragment */
								found_item->fragments[found_fragment_index] = free_fragment;
								this->editItem(*found_item);
								delete found_item;
							}

							/* Move data to the target address */
							vector<unsigned char> data_move;
							this->readData(it->fragments[fragments_index].fragment_start_address + cluster_index * boot.cluster_size, data_move);
							this->writeData(data_address, data_move);

							/* Reset bitmap on the old position */
							uint32_t bitmap_address = boot.bitmap_start_address + (it->fragments[fragments_index].fragment_start_address + cluster_index * boot.cluster_size - boot.data_start_address) / boot.cluster_size;
							this->resetBitmap(bitmap_address);

							/* Alocate bitmap on the new position */
							this->allocateSpaceInBitmap((boot.bitmap_start_address + data_address - boot.data_start_address) / boot.cluster_size);	
						}

						data_address += boot.cluster_size;
					}
				}
			}

			/* Deletes almost all parent items. */
			for (vector<mft_item>::iterator it = file_items.begin() + 1; it != file_items.end(); it++) {
				this->deleteItem(*it);
			}

			/* Set a single fragment to file item */
			memset(file_items.front().fragments, 0, sizeof(file_items.front().fragments));
			file_items.front().item_order_total = 1;
			file_items.front().fragments_count = 1;
			file_items.front().fragments[0] = new_fragment;

			/* Save changes */
			this->editItem(file_items.front());
		}
	}
}

/**
* Checks size of files and number of data segments per file. If file system is corrupted function throws
* consistency_error.
*/
void NTFS::checkConsistency() {
	boot_record boot;
	this->readBoot(boot);

	mft_item temp_item;

	for (uint32_t i = 0; i < boot.mft_item_count; i++) {
		fread(&temp_item, sizeof(mft_item), 1, this->file);
		uint32_t teoretic_cluster_count = temp_item.item_size % boot.cluster_size > 0 ? temp_item.item_size / boot.cluster_size + 1 : temp_item.item_size / boot.cluster_size;
		uint32_t real_cluster_count = 0;
		for (uint32_t j = 0; j < temp_item.fragments_count; j++) {
			real_cluster_count += temp_item.fragments[j].cluster_count;
		}

		if (teoretic_cluster_count != real_cluster_count) {
			throw consistency_error("NTFS CHECKCONSISTENCY: File system is not consistent.");
		}
	}
}

/*
* __________________________________PROTECTED FUNCTIONS______________________________________
*/

/**
* Writes data into the given address on a file system. It can write just one block of data. Number of written bytes is returned from a function.
* @param address The address of data. Invalid argument is thrown when the address does not point to a data section.
* @param s_data Data that is going to be written.
* @return The number of written bytes.
*/
uint32_t NTFS::writeData(const uint32_t address, const vector<unsigned char> &s_data) {
	boot_record boot;
	this->readBoot(boot);
	if (address < boot.data_start_address || address >= boot.disk_size) {
		throw wrong_address("NTFS WRITEDATA: The entered address it not in a data section, " + address);
	}

	vector<unsigned char> data = s_data;
	data.resize(boot.cluster_size);

	fseek(this->file, address, SEEK_SET);
	fwrite(&data.at(0), 1, boot.cluster_size, this->file);

	return s_data.size() > boot.cluster_size ? boot.cluster_size : s_data.size();
}

/**
* Generates a unique uid from the given path. Function should use some hash function.
* @param path The given path which is going to be transformed to the uid.
* @return The unique 32-bit long uid.
*/
uint32_t NTFS::generateNodeID(const string path) const {
	uint32_t h = 0;
	if (path.size() > 0) {
		for (string::const_iterator it = path.begin(); it != path.end(); it++) {
			h = 31 * h + *it;
		}
	}

	return h;
 }

/*
* __________________________________PRIVATE FUNCTIONS______________________________________
*/

/** 
* Finds an item that contains the given address. Function returns pointer to found item (has to be deleted)
* and index of a fragment that containts the given address.
* @param address The address from a data section.
* @param fragment_index The index of a fragment that contains the address.
* @return The found item.
*/
NTFS::mft_item* NTFS::containsCluster(const uint32_t address, int &fragment_index) {
	boot_record boot;
	this->readBoot(boot);

	mft_item *temp_item = new mft_item;

	for (uint32_t i = 0; i < boot.mft_item_count; i++) {
		fread(temp_item, sizeof(mft_item), 1, this->file);
		if (temp_item->uid != UID_ITEM_FREE) {
			for (uint32_t j = 0; j < temp_item->fragments_count; j++) {
				uint32_t fragment_start = temp_item->fragments[j].fragment_start_address;
				uint32_t cluster_count = temp_item->fragments[j].cluster_count;
				if (address > fragment_start && address < fragment_start + cluster_count * boot.cluster_size) {
					fragment_index = j;
					return temp_item;
				}
			}
		}
	}

	delete temp_item;
	return NULL;
}

/**
* Reads items of a parent of the given node.
* @param parent_items A vector of the items.
* @param node The child node.
*/
void NTFS::readParentItems(vector<mft_item> &parent_items, const Node &node) {
	string node_path = node.path.toString();
	size_t pos = node_path.find_last_of('/');

	/* Checks existence of a parent and if it is a directory */
	string node_parent = node_path.substr(0, pos);
	this->readItems(this->generateNodeID(node_parent), parent_items);
	if (parent_items.empty() || !parent_items.front().is_directory) {
		parent_items.clear();
	}
}

/**
* Appends data to the item. The given item has to be last item of a file. When it would not be 
* last you can expect undefined behavior.
* @param data_holder Last item of the file that will hold data.
*/
void NTFS::appendData(mft_item &data_holder, const vector<unsigned char> &data) {
	boot_record boot;
	this->readBoot(boot);

	uint32_t used_space = data_holder.item_size % boot.cluster_size;
	uint32_t free_space = boot.cluster_size -  used_space;
	data_holder.item_size += data.size();

	if (free_space < data.size() || used_space == 0) {
		vector<unsigned char> data_copy(data);

		if (used_space != 0) {
			uint32_t fragment_start = data_holder.fragments[data_holder.fragments_count - 1].fragment_start_address;
			uint32_t cluster_count = data_holder.fragments[data_holder.fragments_count - 1].cluster_count;
			
			vector<unsigned char> cluster_content;
			this->readData(fragment_start + (cluster_count - 1) * boot.cluster_size, cluster_content);
			memcpy(&cluster_content.at(used_space), &data_copy.at(0), free_space);
			this->writeData(fragment_start + (cluster_count - 1) * boot.cluster_size, cluster_content);

			data_copy = vector<unsigned char>(data_copy.begin() + free_space, data_copy.end());
		}

		/* When a fragment is full it is necessary to create a new one. */
		vector<mft_fragment> free_fragments;
		uint32_t cluster_count = data_copy.size() % boot.cluster_size > 0 ? data_copy.size() / boot.cluster_size + 1 : data_copy.size() / boot.cluster_size;

		this->findFreeSpace(free_fragments, cluster_count); 
		this->addFragments(data_holder, free_fragments);
		for (vector<mft_fragment>::const_iterator it = free_fragments.begin(); it != free_fragments.end(); it++) {
			for (uint32_t i = 0; i < it->cluster_count; i++) {
				this->writeData(it->fragment_start_address + i * boot.cluster_size, vector<unsigned char>(data_copy.begin() + i * boot.cluster_size, data_copy.end()));
			}

			if (data_copy.begin() + cluster_count * boot.cluster_size < data_copy.end()) {
				data_copy = vector<unsigned char>(data_copy.begin() + cluster_count * boot.cluster_size, data_copy.end());
			}
		}
	}
	else {
		/* When a fragment is not full, data is appended. */
		uint32_t fragment_start = data_holder.fragments[data_holder.fragments_count - 1].fragment_start_address;
		uint32_t cluster_count = data_holder.fragments[data_holder.fragments_count - 1].cluster_count;

		vector<unsigned char> cluster_content;
		this->readData(fragment_start + (cluster_count - 1) * boot.cluster_size, cluster_content);
		memcpy(&cluster_content.at(used_space), &data.at(0), data.size());
		this->writeData(fragment_start + (cluster_count - 1) * boot.cluster_size, cluster_content);
	}
}

/**
* Adds fragments to the item. If the item is full function creates a new one.
* @param item The item that will contain fragments.
* @param fragments New fragments.
*/
void NTFS::addFragments(mft_item &item, const vector<mft_fragment> &fragments) {
	for (vector<mft_fragment>::const_iterator it = fragments.begin(); it != fragments.end(); it++) {
		if (item.fragments_count == MFT_FRAGMENTS_COUNT) {
			/* Creates a new item if the last one is full. */
			mft_item new_item;
			mft_item::createItem(new_item, item.uid, item.is_directory, item.item_order + 1, item.item_order_total + 1, string(item.item_name), item.item_size, 0);
			this->addFragments(new_item, vector<mft_fragment>(it, fragments.end()));
			this->writeItem(item);
			break;
		}
		else {
			/* Adds fragment to the last parent item. */
			item.fragments[item.fragments_count] = *it;
			item.fragments_count++;
		}
	}
}

/**
* Finds some free space from the back of a file system. Amount of space is defined through the second 
* argument of the function. Function finds exact sequence of free memory that can keep given amount of space.
* On the other hand findFreeSpace just tries to do so.
* @param space_indexes Indexes to data segment of a file system that can hold given amount of required memory.
* @param space_count Required number of clusters.
*/
void NTFS::findExactFreeSpaceBackward(mft_fragment &free_fragment, const uint32_t space_count) {
	boot_record boot;
	this->readBoot(boot);

	bool value;
	uint32_t counter = 0;
	
	for (uint32_t i = 1; i <= boot.cluster_count; i++) {
		fseek(this->file, boot.data_start_address - i * sizeof(bool), SEEK_SET);
		fread(&value, sizeof(bool), 1, this->file);
		if (!value) {
			counter++;
			if (counter == space_count) {
				uint32_t bitmap_address = boot.data_start_address - i * sizeof(bool);
				uint32_t data_address = boot.data_start_address + (boot.cluster_count - i) * boot.cluster_size;
				this->allocateData(bitmap_address, data_address, counter, free_fragment);
				return;
			}
		}
		else {
			counter = 0;
		}
	}
}

/**
* Finds some free space. Amount of space is defined through the second argument of the function.
* Function tries to find sequence of free memory that can keep given amount of space.
* When it is not possible, function finds free space from different places of a file system.
* @param space_indexes Indexes to data segment of a file system that can hold given amount of required memory.
* @param space_count Required number of clusters.
*/
void NTFS::findFreeSpace(vector<mft_fragment> &space_fragments, const uint32_t space_count) {
	boot_record boot;
	this->readBoot(boot);

	bool value;
	uint32_t counter = 0;
	for (uint32_t i = 0; i < boot.cluster_count; i++) {
		fseek(this->file, boot.bitmap_start_address + i * sizeof(bool), SEEK_SET);
		fread(&value, sizeof(bool), 1, this->file);
		if (!value) {
			counter++;
			if (counter == space_count) {		
				mft_fragment fragment;
				uint32_t bitmap_address = boot.bitmap_start_address + i - counter + 1;
				uint32_t data_address = boot.data_start_address + boot.cluster_size * (i - counter + 1);
				this->allocateData(bitmap_address, data_address, counter, fragment);
				space_fragments.push_back(fragment);
				return;
			}
		}
		else {
			counter = 0;
		}
	}

	counter = 0;

	for (uint32_t i = 0; i < boot.cluster_count; i++) {
		fseek(this->file, boot.bitmap_start_address + i * sizeof(bool), SEEK_SET);
		fread(&value, sizeof(bool), 1, this->file);
		if (!value) {
			counter++;
			if (counter == space_count) {
				mft_fragment fragment;
				uint32_t bitmap_address = boot.bitmap_start_address + i - counter + 1;
				uint32_t data_address = boot.data_start_address + boot.cluster_size * (i - counter + 1);
				this->allocateData(bitmap_address, data_address, counter, fragment);
				space_fragments.push_back(fragment);
				return;
			}
		}
		else if (counter != 0) {
			mft_fragment fragment;
			uint32_t bitmap_address = boot.bitmap_start_address + i - counter + 1;
			uint32_t data_address = boot.data_start_address + boot.cluster_size * (i - counter + 1);
			this->allocateData(bitmap_address, data_address, counter, fragment);
			space_fragments.push_back(fragment);
		}
	}

	space_fragments.clear();
}

/**
* Allocates a new fragment and edites bitmap.
* @param bitmap_address Address of data in bitmap.
* @param data_address Address of data in data segment.
* @param cluster_count Number of allocated clusters.
* @param fragment Allocated fragment.
*/
void NTFS::allocateData(const uint32_t bitmap_address, const uint32_t data_address, const uint32_t cluster_count, mft_fragment &fragment) {
	bool used = true;

	fseek(this->file, bitmap_address, SEEK_SET);
	for (uint32_t i = 0; i < cluster_count; i++) {
		fwrite(&used, sizeof(bool), 1, this->file);
	}

	fragment.fragment_start_address = data_address;
	fragment.cluster_count = cluster_count;
}

/**
* Reads an mft item. Returns a pointer to the read item (has to be deleted).
* @param uid UID of the item.
* @return The pointer to the read item.
*/
NTFS::mft_item* NTFS::readItem(const uint32_t uid) const {
	if (uid == UID_ITEM_FREE) {
		throw uid_error("NTFS READITEM: Cannot read an mft item with UID equals to 0.");
	}

	boot_record boot;
	this->readBoot(boot);

	mft_item *temp_item = new mft_item;
	for (uint32_t i = 0; i < boot.mft_item_count; i++) {
		fread(temp_item, sizeof(mft_item), 1, this->file);
		if (temp_item->uid == uid) {
			return temp_item;
		}
	}

	delete temp_item;
	temp_item = NULL;

	return NULL;
}

/**
* Reads a vector of mft items.
* @param uid UID of the item.
* @param mft_item_vector The vector of read items.
*/
void NTFS::readItems(const uint32_t uid, vector<mft_item> &mft_item_vector) const {
	if (uid == UID_ITEM_FREE) {
		throw uid_error("NTFS READITEMS: Cannot read an mft item with UID equals to 0.");
	}

	boot_record boot;
	this->readBoot(boot);

	mft_item temp_item;
	for (uint32_t i = 0; i < boot.mft_item_count; i++) {
		fread(&temp_item, sizeof(mft_item), 1, this->file);
		if (temp_item.uid == uid) {
			mft_item_vector.push_back(temp_item);
			if (temp_item.item_order_total == mft_item_vector.size()) {
				break;
			}
		}
	}

	if (mft_item_vector.size() == 0) {
		return;
	}

	sort(mft_item_vector.begin(), mft_item_vector.end());
}

/**
* Writes the given mft item into a file system.
* @param item The written item.
*/
void NTFS::writeItem(const mft_item &item) {
	if (item.uid == UID_ITEM_FREE) {
		throw uid_error("NTFS WRITEITEM: Cannot create an mft item with UID equals to 0.");
	}

	boot_record boot;
	this->readBoot(boot);

	mft_item temp_item;
	for (uint32_t i = 0; i < boot.mft_item_count; i++) {
		fread(&temp_item, sizeof(mft_item), 1, this->file);
		if (temp_item.uid == UID_ITEM_FREE) {
			fseek(this->file, -((int)sizeof(mft_item)), SEEK_CUR);
			fwrite(&item, sizeof(mft_item), 1, this->file);
			return;
		}
	}

	throw no_space("NTFS WRITEITEM: Cannot write an mft item, there is not enough space in mft segment.");
}

/**
* Edites the given mft item in a file system.
* @param item The given item.
*/
void NTFS::editItem(const mft_item &item) {
	boot_record boot;
	this->readBoot(boot);

	mft_item temp_item;
	for (uint32_t i = 0; i < boot.mft_item_count; i++) {
		fread(&temp_item, sizeof(mft_item), 1, this->file);
		if (temp_item == item) {
			fseek(this->file, -((int)sizeof(mft_item)), SEEK_CUR);
			fwrite(&item, sizeof(mft_item), 1, this->file);
			return;
		}
	}

	throw file_not_found("NTFS EDITITEM: Cannot edit an mft item, the given item does not exist.");
}

/**
* Removes the given mft item from a file system.
* @param item The given item.
*/
void NTFS::deleteItem(const mft_item &item) {
	if (item.uid == UID_ITEM_FREE) {
		throw uid_error("NTFS DELETEITEM: Cannot delete an mft item with UID equals to 0.");
	}

	boot_record boot;
	this->readBoot(boot);

	mft_item temp_item;
	mft_item new_item;
	mft_item::createItem(new_item, UID_ITEM_FREE, false, SINGLE_ITEM_ORDER, SINGLE_ITEM_ORDER, "", 0, 0);

	for (uint32_t i = 0; i < boot.mft_item_count; i++) {
		fread(&temp_item, sizeof(mft_item), 1, this->file);
		if (temp_item == item) {
			fseek(this->file, -((int)sizeof(mft_item)), SEEK_CUR);
			fwrite(&new_item, sizeof(mft_item), 1, this->file);
			return;
		}
	}

	throw file_not_found("NTFS DELETEITEM: Cannot delete an mft item, the given UID does not exist.");
}

/**
* Creates a new boot section.
*/
void NTFS::createBoot() {
	const uint32_t mft_item_start_address = sizeof(boot_record);
	const uint32_t bitmap_start_address = mft_item_start_address + sizeof(mft_item) * DEFAULT_MFT_COUNT;
	const uint32_t data_start_address = bitmap_start_address + DEFAULT_CLUSTER_COUNT * sizeof(bool);
	const uint32_t disk_size = data_start_address + DEFAULT_CLUSTER_SIZE * DEFAULT_CLUSTER_COUNT;
	
	boot_record boot = {
		"dpoch",
		"",
		disk_size,
		DEFAULT_CLUSTER_SIZE,
		DEFAULT_CLUSTER_COUNT,
		generateNodeID('/' + name),
		DEFAULT_MFT_COUNT,
		mft_item_start_address,
		bitmap_start_address,
		data_start_address
	};

	fseek(this->file, 0, SEEK_SET);
	fwrite(&boot, sizeof(boot), 1, this->file);
}

/**
* Reads a boot section.
* @param boot Boot section.
*/
void NTFS::readBoot(boot_record &boot) const {
	fseek(this->file, 0, SEEK_SET);
	fread(&boot, sizeof(boot_record), 1, this->file);
}

/**
* Creates a new root directory of a file system.
*/
void NTFS::createRoot() {
	mft_item root;
	mft_item::createItem(root, generateNodeID('/' + this->name), true, SINGLE_ITEM_ORDER, SINGLE_ITEM_ORDER, this->name.data(), 0, 0);
	fwrite(&root, sizeof(mft_item), 1, this->file);
}

/**
* Creates a new mft section.
*/
void NTFS::createMft() {
	createRoot();

	mft_item item;
	mft_item::createItem(item, UID_ITEM_FREE, false, SINGLE_ITEM_ORDER, SINGLE_ITEM_ORDER, "", 0, 0);

	for (uint32_t i = 0; i < DEFAULT_MFT_COUNT; i++) {
		fwrite(&item, sizeof(mft_item), 1, this->file);
	}
}

/**
* Creates a new bitmap section.
*/
void NTFS::createBitmap() {
	bool bitmap[DEFAULT_CLUSTER_COUNT] = {0};
	fwrite(bitmap, sizeof(bool), DEFAULT_CLUSTER_COUNT, this->file);
}

/**
* Resets bitmap value on the given address.
* @param address The given address.
*/
void NTFS::resetBitmap(uint32_t address) {
	bool used = false;
	fseek(this->file, address, SEEK_SET);
	fwrite(&used, sizeof(bool), 1, this->file);
}

/**
* Sets cluster as used on the given address.
* @param address The given address.
*/
void NTFS::allocateSpaceInBitmap(uint32_t address) {
	bool used = true;

	fseek(this->file, address, SEEK_SET);
	fwrite(&used, sizeof(bool), 1, this->file);
}

/**
* Creates a new data section.
*/
void NTFS::createData() {
	char data[DEFAULT_CLUSTER_SIZE] = {};
	for (uint32_t i = 0; i < DEFAULT_CLUSTER_COUNT; i++) {
		fwrite(&data[0], sizeof(char), DEFAULT_CLUSTER_SIZE, this->file);
	}
}