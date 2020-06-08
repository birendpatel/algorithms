/*
* author: Biren Patel
* description: This API loads a single row at a time into memory from a CSV
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

#include <stdbool.h>

/*******************************************************************************
* client-modifiable parameters
* @ CSV_ITERATOR_BUF_LEN : temp buffer holds raw contents of csv row, in bytes.
*******************************************************************************/
#define CSV_ITERATOR_BUF_LEN 64

/*******************************************************************************
* structure: struct csv
* purpose: client must declare pointer to struct csv
*******************************************************************************/
struct csv;

/*******************************************************************************
* public function: csv_create
* @ filename : path for CSV file
* @ fmt : string specifiying format of a single row in the CSV file
* @ sep : separator between data items in CSV, must be same separator in fmt
* purpose: constructor
*******************************************************************************/
struct csv *csv_create(char* filename, char *fmt, char sep);

/*******************************************************************************
* public function: csv_destroy
* purpose: destructor
* @ csvfile : pointer to struct csv to destroy
* @ flush_curr : if you have not cycled through entire csv until EOF, set this
*   to true to release memory held by the currently loaded csv row.
* note: defaulting flush_curr to true is memory-safe.
*******************************************************************************/
void csv_destroy(struct csv *csvfile, bool flush_curr);

/*******************************************************************************
* public function: csv_next
* purpose: load the next available row from the csv into memory
* @ csvfile : pointer to struct csv
* returns: 1 if successful load, 0 if failure to read next line
*******************************************************************************/
bool csv_next(struct csv *csvfile);

/*******************************************************************************
* public function: csv_has_next
* purpose: indicate if the CSV file still contains rows available to read
* @ csvfile : pointer to struct csv
* returns: 1 if at least one more row is available, 0 otherwise.
*******************************************************************************/
bool csv_has_next(struct csv *csvfile);

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
