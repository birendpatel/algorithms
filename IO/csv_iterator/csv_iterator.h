/*
* author: Biren Patel
* description: This API loads a single row at a time into memory from a CSV
* file. The data is converted automatically to the data types that you request
* through a format string. Once a csv type is created, you can then access the
* contents of the next row on demand. Each request will flush the current row
* from memory. The goal of this API is to obtain some functionality and memory
* efficiency similar to a python generator.
*
* note: does not yet handle csv with header
* note: only handles data types int (%d), double (%f), char(%c), and string (%s)
*/

#ifndef CSV_ITERATOR_H
#define CSV_ITERATOR_H

#include <stdbool.h>

/*******************************************************************************
* client-modifiable parameters
* @ CSV_ITERATOR_BUF_LEN : 1 KiB temp buffer holds raw contents of a csv row
* @ CSV_ITERATOR_DEBUG : set to 1 for verbose debugging output to stdout
*******************************************************************************/
#define CSV_ITERATOR_BUF_LEN 1024
#define CSV_ITERATOR_DEBUG 0

/*******************************************************************************
* structure: struct csv
* purpose: client must declare pointer to struct csv
*******************************************************************************/
struct csv;

/*******************************************************************************
* public function: csv_create
* purpose: constructor
* @ filename : path for CSV file
* @ fmt : string specifiying format of a single row in the CSV file
* @ sep : separator between data items in CSV, must be same separator in fmt
* returns: pointer to struct csv
*******************************************************************************/
struct csv *csv_create(char* filename, char *fmt, char sep);

/*******************************************************************************
* public function: csv_destroy
* purpose: destructor
* @ csvfile : pointer to struct csv
* @ flush_curr : if you have not cycled through entire csv until EOF, set this
*                to true to the release memory held by the currently loaded row.
* note: defaulting flush_curr to true is memory-safe.
*******************************************************************************/
void csv_destroy(struct csv *csvfile, bool flush_curr);

/*******************************************************************************
* public function: csv_next
* purpose: load the next available row from the csv into memory
* @ csvfile : pointer to struct csv
* returns: true if successful load, false otherwise
*******************************************************************************/
bool csv_next(struct csv *csvfile);

/*******************************************************************************
* public function: csv_get_ptr
* purpose: get pointer to an item in the current row
* @ csvfile : pointer to struct csv
* @ index : index of item pointer in the struct csv data array
* returns: void pointer to the item at the requested index
* note: csv_get macro is better, but you must use this function if item is a
*       string or if the CSV file contains missing data.
*******************************************************************************/
void *csv_get_ptr(struct csv *csvfile, int index);

/*******************************************************************************
* macro: csv_get
* purpose: wrapper over csv_get_ptr to return data instead of pointer
* @ struct_csv_pointer : pointer to struct csv
* @ index : index of item pointer in the struct csv data array
* @ dtype : data type of item to be returned
* note: do not use if the CSV contains missing data, you could dereference a
*       pointer to NULL. use csv_get_ptr instead and test for NULL before
*       attempting to derefence.
*******************************************************************************/
#define csv_get(struct_csv_pointer, index, dtype)                              \
        *((dtype *) csv_get_ptr(struct_csv_pointer, index))                    \

#endif
