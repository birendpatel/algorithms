/*
* author: Biren Patel
* description: API for python-style row iterator from CSV file.
* note: does not yet handle csv with empty fields
* note: does not yet handle csv with header
* note: does not yet handle data types beyond (int, double, char, string)
*/

#ifndef CSV_ITERATOR_H
#define CSV_ITERATOR_H

/*******************************************************************************
* client-modifiable parameters
* @ CSV_ITERATOR_BUF_LEN : temp buffer to hold a csv row, default 64 bytes
*******************************************************************************/
#define CSV_ITERATOR_BUF_LEN 64

/*******************************************************************************
* structure: struct csv
* purpose: client must declare pointer to struct csv
*******************************************************************************/
struct csv;

/*******************************************************************************
* public function: csv_create
* purpose: constructor
*******************************************************************************/
struct csv *csv_create(char* filename, char *fmt, char sep);

/*******************************************************************************
* public function: csv_destroy
* purpose: destructor
* @ csvfile : pointer to struct csv to destroy
*******************************************************************************/
void csv_destroy(struct csv *csvfile);

/*******************************************************************************
* public function: csv_next
* purpose: load the next available row from the csv into memory
* @ csvfile : pointer to struct csv
*******************************************************************************/
void csv_next(struct csv *csvfile);

/*******************************************************************************
* public function: csv_get_ptr
* purpose: access an item from the row currently loaded into memory
* @ csvfile : pointer to struct csv
* @ index : index of item pointer in the struct csv data array
* returns: void pointer to the item at the requested index
* note: in most cases, the csv_get macro would be the preferred wrapper
*******************************************************************************/
void *csv_get_ptr(struct csv *csvfile, int index);

/*******************************************************************************
* macro: csv_get
* purpose: wrapper over csv_get_ptr to return data instead of pointer
* @ struct_csv_pointer : pointer to struct csv
* @ index : index of item pointer in the struct csv data array
* @ dtype : data type of item to be returned.
* note: not suitable for strings, use csv_get_ptr with char* cast.
*******************************************************************************/

#define csv_get(struct_csv_pointer, index, dtype)                              \
        *((dtype *) csv_get_ptr(struct_csv_pointer, index))                    \

#endif
