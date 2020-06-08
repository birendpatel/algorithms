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
* @ CSV_ITERATOR_BUF_LEN : temp buffer used to hold one csv row, default 1 KiB
*******************************************************************************/
#define CSV_ITERATOR_BUF_LEN 1024

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

#endif
