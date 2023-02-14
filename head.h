#ifndef  __HEAD_H__
#define __HEAD_H__
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <math.h>
typedef const char* V;
typedef std::vector<V> Record;




typedef struct MyPageStruct{
    void *data;
    int page_size;
    int slot_size;
    int free_slot_begin;
} Page;



/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record);

int fixed_len_page_directory_offset(Page *page);
bool is_freeslot(Page* page, int slot);
std::vector<int> fixed_len_page_freeslot_indices(Page *page);
/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf);

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record);


/**
 * Compute the number of bytes required to serialize record
 */
int var_len_sizeof(Record *record);
 
/**
 * Serialize the record using variable record encoding
 */
void var_len_write(Record *record, void *buf);
 
/**
 * Deserialize the `buf` which contains the variable record encoding.
 */
void var_len_read(void *buf, int size, Record *record);

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size);
 
/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page);
 void free_fixed_len_page(Page* page);
/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page);
 
/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r);
 
/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r);
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r);


#endif