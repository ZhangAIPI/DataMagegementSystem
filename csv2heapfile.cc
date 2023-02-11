#include "head.h"

/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file){
    heapfile->file_ptr = file;
    heapfile->page_size = page_size;
    heapfile->free_page_index = 0;
}


/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile){
    assert(heapfile->free_page_index < heapfile->page_size);
    return heapfile->free_page_index++;
}


/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page){
    fseek(heapfile->file_ptr, pid * heapfile->page_size, SEEK_SET);
    fread(page->data, heapfile->page_size, 1, heapfile->file_ptr);
}

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid){
    fseek(heapfile->file_ptr, pid * heapfile->page_size, SEEK_SET);
    fwrite(page->data, heapfile->page_size, 1, heapfile->file_ptr);
}


RecordIterator::RecordIterator(Heapfile *heapfile){
    this->heapfile = heapfile;
    this->curPage = new Page();
    this->curPageID = 0;
    read_page(heapfile, this->curPageID, curPage);
}

Record RecordIterator::next(){
    this->curPageID++;
    read_page(heapfile, this->curPageID, curPage);
}

bool RecordIterator::hasNext(){
    return this->curPageID < this->heapfile->;
}