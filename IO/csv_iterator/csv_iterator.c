/*
* author: Biren Patel
* description: implementation for python-style row iterator from CSV file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "csv_iterator.h"

/*******************************************************************************
* macro: verify_pointer
* purpose: exit if a pointer is null
* @ test : one word name of the test being performed
* @ pointer : pointer returned by some function
*******************************************************************************/

#define VERIFY_POINTER(test, pointer)                                          \
        if (pointer == NULL)                                                   \
        {                                                                      \
            fprintf(stderr, #test " fail: %s in %s\n", __func__, __FILE__);    \
            exit(EXIT_FAILURE);                                                \
        }                                                                      \

/*******************************************************************************
* structure: struct csv
* @ total_columns : total columns in csv file
* @ column_formats : array of data types of each column, encoded as characters
* @ data : array of void pointers to one row of data, one pointer per column.
* purpose: holds CSV metadata
*******************************************************************************/

struct csv
{
    FILE *file_ptr;
    int curr_row;
    int total_columns;
    char *column_formats;
    void *data;
};

/*******************************************************************************
* private function: calc_num_columns
* purpose: determine number of columns based on supplied format string
* @ csvfile : pointer to struct csv
* @ fmt : format string passed by user on contructor
* @ sep : separating chacter passed by user on constructor
* returns: total number of columns
*******************************************************************************/
static int calc_num_columns(struct csv *csvfile, char *fmt, char sep);

/*******************************************************************************
* private function: infer_data_types
* purpose: determine data type of each column based on supplied format string
* @ csvfile : pointer to struct csv
* @ fmt : format string passed by user on contructor
* @ sep : separating chacter passed by user on constructor
* returns: char array where each element supplies format character for a switch
*******************************************************************************/
static char *infer_data_types(struct csv *csvfile, char *fmt, char sep);

/*******************************************************************************
* public functions
*******************************************************************************/

//constructor
struct csv *csv_create(char* filename, char *fmt, char sep)
{
    //pointer to be returned to client
    struct csv *csvfile = malloc(sizeof(struct csv));

    //parse the format string to initialize total_columns
    csvfile->total_columns = calc_num_columns(csvfile, fmt, sep);
    assert(csvfile->total_columns >= 1);

    //parse the format string to initialize column_formats array
    csvfile->column_formats = infer_data_types(csvfile, fmt, sep);
    assert(csvfile->column_formats != NULL);

    //open the csv file
    csvfile->file_ptr = fopen(filename, "r");
    VERIFY_POINTER(fopen, csvfile->file_ptr);

    //allocate memory for array of void pointers representing a single row
    csvfile->data = malloc(csvfile->total_columns * sizeof(void*));
    VERIFY_POINTER(malloc, csvfile->data);

    //set current row index
    csvfile->curr_row = 0;

    #ifdef DEBUG
    printf("csv_create() has finished.\n\n");
    printf("total number of columns: %d\n\n", csvfile->total_columns);

    for(size_t i = 0; i < csvfile -> total_columns; ++i)
    {
        printf("column %d is %c\n", (int) i, csvfile->column_formats[i]);
    }

    printf("\ncurrent row: %d\n", csvfile->curr_row);
    #endif

    return csvfile;
}

//destructor
void csv_destroy(struct csv *csvfile)
{
    fclose(csvfile->file_ptr);
    free(csvfile->column_formats);
    free(csvfile->data); //make sure pointed data gets free'd beforehand
    free(csvfile);
}

//load the next available row from the csv into memory
int csv_next(struct csv *csvfile)
{
    assert(csvfile != NULL);
    assert(csvfile->curr_row >= 0);

    //free previous row from memory, unless this is the first iteration


    //load next row into memory


}

/*******************************************************************************
* private functions
*******************************************************************************/

//determine number of columns
static int calc_num_columns(struct csv *csvfile, char *fmt, char sep)
{
    assert(csvfile != NULL);
    assert(fmt != NULL);
    assert(sep != 0);

    //starts at 1 since no comma after the final specifier
    int total_columns = 1;

    for(char *curr = fmt; *curr != '\0'; ++curr)
    {
        if (*curr == sep) ++total_columns;
    }

    assert(total_columns >= 1);
    return total_columns;
}

//determine data type of each column
static char *infer_data_types(struct csv *csvfile, char *fmt, char sep)
{
    assert(csvfile != NULL);
    assert(fmt != NULL);
    assert(sep != 0);

    //allocate memory to hold data type formats, based on calc_num_columns()
    assert(csvfile->total_columns >= 1);
    char *column_formats = malloc(csvfile->total_columns);
    VERIFY_POINTER(malloc, column_formats);

    //loop through format string and copy over each format specifier
    int loc = 0;

    for(char *curr = fmt; *curr != '\0'; ++curr)
    {
        if (*curr == '%')
        {
            column_formats[loc++] = *(curr + 1);
            assert(loc <= csvfile->total_columns);
        }
    }
    assert(loc == csvfile->total_columns);

    return column_formats;
}
