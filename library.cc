#include "head.h"
#include <cstring>
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
        memcpy((void *)record->at(i), (char*)buf + offset, strlen(record->at(i)));
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
        memcpy((void*)record->at(i), (char*)buf + offset, len);
        offset += len;
    }
}

int fixed_len_page_directory_offset(Page *page) {
    // Calculate the byte offset where the directory starts.
    return page->page_size - ceil((floor((float)page->page_size/(float)page->slot_size))/8);
}

unsigned char* get_directory(Page* page){
    return (unsigned char*)page->data + fixed_len_page_directory_offset(page);
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
 

bool is_freeslot(Page* page, int slot){
    unsigned char directory = *(get_directory(page) + slot / 8);
    return (directory & (1 << (slot % 8))) == 0;
}

/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page) {
    return (fixed_len_page_directory_offset(page))/page->slot_size;
}
/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page){
    return fixed_len_page_freeslot_indices(page).size();
}
 
/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r){
    unsigned char* directory_offset = get_directory(page);

    //Iterate slots directory to find a free one.
    for(int i = 0; i < fixed_len_page_capacity(page); i++){
        if(i > 0 && i%8 == 0)
            directory_offset++;

        unsigned char directory = *directory_offset;

        if(directory >> (i%8) == 0){
            //Write record to page.
            fixed_len_write(r, ((char*)page->data) + i*page->slot_size);

            //Update directory.
            directory |= 1 << (i%8);
            memcpy(directory_offset, &directory, 1);
            return i;
        }
    }

    //Reached here means we didn't find any free slots.
    return -1;
}
 
/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r){
    //Check that slot is in valid space.
    if(slot >= fixed_len_page_capacity(page))
        return;

    //Get byte position of slot in the directory.
    unsigned char* directory_offset = get_directory(page);
    directory_offset += slot/8;

    //Update directory, set as written.
    unsigned char directory = *directory_offset;
    directory |= 1 << (slot%8);
    memcpy(directory_offset, &directory, 1);

    //Write record to slot.
    unsigned char* slot_ptr = ((unsigned char*)page->data) + page->slot_size*slot;
    fixed_len_write(r, slot_ptr);
}
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r){
    //Check that slot is in valid space.
    if(slot >= fixed_len_page_capacity(page))
        return;

    //It is up to the caller to make sure the requested slot is actually not empty.
    char* slot_ptr = (char*)page->data + (page->slot_size * slot);
    fixed_len_read(slot_ptr, page->slot_size, r);
}

void free_fixed_len_page(Page* page){
    //Free data buffer.
    free(page->data);
    page->data = 0;
    page->page_size = 0;
}

std::vector<int> fixed_len_page_freeslot_indices(Page *page) {
    std::vector<int> freeslots;

    //Get directory.
    unsigned char* directory_offset = get_directory(page);

    //Loop over directory to see which records are free.
    for(int i = 0; i < fixed_len_page_capacity(page); i++) {
        if(i > 0 && i%8 == 0)
            directory_offset++;

        unsigned char directory = *directory_offset;

        if((directory & (1 << (i%8))) == 0){
            freeslots.push_back(i);
        }
    }

    return freeslots;
}
