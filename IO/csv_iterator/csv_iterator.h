/*
* author: Biren Patel
* description: API for python-style row iterator from CSV file.
*
* This API allows you to load a single row at a time into memory from a CSV
* file. The data is converted automatically to the data types that you request
* through a format string. See demo.c for an example. You can then access the
* contents of the current row, or flush it from memory and immediately load
* the next row. The goal of this API is to obtain some functionality and memory
* efficiency similar to a python generator.
*
* note: does not yet handle csv with missing fields
* note: does not yet handle csv with header
* note: does not yet handle data types beyond (int, double, char, string)
*/

#ifndef CSV_ITERATOR_H
#define CSV_ITERATOR_H

/*******************************************************************************
* client-modifiable parameters
* @ CSV_ITERATOR_BUF_LEN : a temporary buffer to hold the raw contents of a
*   single csv row. This temporary buffer only occupies memory during the
*   csv_next() call. The default is 64 bytes.
* @ CSV_ITERATOR_DEBUG : display debugging results in stdout
*******************************************************************************/
#define CSV_ITERATOR_BUF_LEN 64
#define CSV_ITERATOR_DEBUG 0

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
* @ flush_curr : if you have not cycled through entire csv until EOF, set this
*   to 1 to release memory held by the currently loaded csv row.
*******************************************************************************/
void csv_destroy(struct csv *csvfile, int flush_curr);

/*******************************************************************************
* public function: csv_next
* purpose: load the next available row from the csv into memory
* @ csvfile : pointer to struct csv
* returns: 1 if successful load, 0 if failure
*******************************************************************************/
int csv_next(struct csv *csvfile);

/*******************************************************************************
* public function: csv_get_ptr
* purpose: access an item from the row currently loaded into memory
* @ csvfile : pointer to struct csv
* @ index : index of item pointer in the struct csv data array
* returns: void pointer to the item at the requested index
* note: csv_get macro is better, unless you need a pointer or item is string
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
