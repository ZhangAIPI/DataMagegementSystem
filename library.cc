#include "head.h"
/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record){
    int size = 0;
    for (int i = 0; i < record->size(); i++){
        size += strlen(record->at(i));
    }
    return size;
}

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf){
    int offset = 0;
    for (int i = 0; i < record->size(); i++){
        memcpy((char*)buf + offset, record->at(i), strlen(record->at(i)));
        offset += strlen(record->at(i));
    }
}

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record){
    int offset = 0;
    for (int i = 0; i < record->size(); i++){
        memcpy(record->at(i), (char*)buf + offset, strlen(record->at(i)));
        offset += strlen(record->at(i));
    }
}

/**
 * Compute the number of bytes required to serialize record
 */
int var_len_sizeof(Record *record){
    int size = 0;
    for (int i = 0; i < record->size(); i++){
        size += strlen(record->at(i)) + sizeof(int);
    }
    return size;
}
 
/**
 * Serialize the record using variable record encoding
 */
void var_len_write(Record *record, void *buf){
    int offset = 0;
    for (int i = 0; i < record->size(); i++){
        int len = strlen(record->at(i));
        memcpy((char*)buf + offset, &len, sizeof(int));
        offset += sizeof(int);
        memcpy((char*)buf + offset, record->at(i), len);
        offset += len;
    }
}
 
/**
 * Deserialize the `buf` which contains the variable record encoding.
 */
void var_len_read(void *buf, int size, Record *record){
    int offset = 0;
    for (int i = 0; i < record->size(); i++){
        int len;
        memcpy(&len, (char*)buf + offset, sizeof(int));
        offset += sizeof(int);
        memcpy(record->at(i), (char*)buf + offset, len);
        offset += len;
    }
}



/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size){
    page->data = malloc(page_size);
    page->page_size = page_size;
    page->slot_size = slot_size;
    page->free_slot_begin = 0;
}
 
/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page){
    return page->page_size / page->slot_size;
}
 
/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page){
    return page->page_size / page->slot_size - page->free_slot_begin;
}
 
/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r){
    int slot = fixed_len_page_freeslots(page);
    if (slot == -1){
        return -1;
    }
    write_fixed_len_page(page, slot, r);
    return slot;
}
 
/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r){
    int offset = slot * page->slot_size;
    fixed_len_write(r, (char*)page->data + offset);
}
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r){
    int offset = slot * page->slot_size;
    fixed_len_read((char*)page->data + offset, page->slot_size, r);
}



