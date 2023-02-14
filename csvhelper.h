#include <stdio.h>
#include <string.h>
#include <vector>

#include "head.h"


/** The fixed number of attributes in a record. */
#define num_attributes 100
/** The fixed size of an attribute in bytes. */
#define attribute_len 10
/** The fixed size of a record. */
#define record_size num_attributes*attribute_len
/**
 * Parse the slot number and pid of a record id string.
 */
int parse_record_id(const char* id, int* pid);

/**
 * Print given record as a csv.
 */
void print_record(Record* record);

/**
 * Read records from given file name into a Record vector.
 */
int read_records(const char* file, std::vector<Record*>* records);