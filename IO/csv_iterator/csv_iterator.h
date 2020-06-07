/*
* author: Biren Patel
* description: API for python-style row iterator from CSV file.
*/

#ifndef CSV_ITERATOR_H
#define CSV_ITERATOR_H

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
* returns: 1 if successful, 0 if failure
*******************************************************************************/
int csv_next(struct csv *csvfile);

#endif
