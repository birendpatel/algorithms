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
    char sep[2];
    int total_columns;
    char *column_formats;
    void **data;
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

    //set current row index and separator, separator is string for strtok
    csvfile->curr_row = 0;
    csvfile->sep[0] = sep;
    csvfile->sep[1] = '\0';

    #ifdef CSV_ITERATOR_DEBUG
    printf("csv_create() has finished.\n\n");
    printf("total number of columns: %d\n\n", csvfile->total_columns);

    for(size_t i = 0; i < csvfile -> total_columns; ++i)
    {
        printf("column %d is %c\n", (int) i, csvfile->column_formats[i]);
    }

    printf("\ncurrent row: %d\n", csvfile->curr_row);
    printf("separator: %s\n", csvfile->sep);
    #endif

    return csvfile;
}

//destructor
void csv_destroy(struct csv *csvfile)
{
    #ifdef CSV_ITERATOR_DEBUG
    printf("\n\nreleasing all memory blocks.\n");
    #endif

    fclose(csvfile->file_ptr);
    free(csvfile->column_formats);
    free(csvfile->data); //make sure pointed data gets free'd beforehand
    free(csvfile);
}

//load the next available row from the csv into memory
void csv_next(struct csv *csvfile)
{
    assert(csvfile != NULL);
    assert(csvfile->total_columns >= 1);
    assert(csvfile->curr_row >= 0);

    //STAGE 1: free memory occupied by previous row, if not on first pass
    if (csvfile->curr_row != 0)
    {
        for(size_t i = 0; i < csvfile->total_columns; ++i)
        {
            free(csvfile->data[i]);
        }
    }

    //STAGE 2: load next row from csv file into memory

    //set up a temporary buffer to hold next line in csv
    char *buffer = malloc(CSV_ITERATOR_BUF_LEN);
    VERIFY_POINTER(malloc, buffer);

    //read the next line
    char *current_line = fgets(buffer, CSV_ITERATOR_BUF_LEN, csvfile->file_ptr);
    VERIFY_POINTER(fgets, current_line);
    #ifdef CSV_ITERATOR_DEBUG
    printf("\n\nloaded next line\n");
    #endif

    for(size_t i = 0; i < csvfile->total_columns; ++i)
    {
        #ifdef CSV_ITERATOR_DEBUG
        printf("Searching for item %d\n", (int) i+1);
        #endif

        char *item;

        //get data from column i of current row, if clause is for strtok arg 1
        if (i == 0)
        {
            item = strtok(current_line, csvfile->sep);
        }
        else
        {
            item = strtok(NULL, csvfile->sep);
        }
        VERIFY_POINTER(tokenization, item);

        //decide on format for current item and malloc into csvfile data
        int *i_ptr;
        double *d_ptr;
        char *c_ptr;

        switch (csvfile->column_formats[i])
        {
            case 'd':
                i_ptr = malloc(sizeof(int));
                *i_ptr = strtol(item, NULL, 10);
                csvfile->data[i] = i_ptr;

                #ifdef CSV_ITERATOR_DEBUG
                printf("found int\n");
                #endif

                break;

            case 'c':
                i_ptr = malloc(sizeof(int));
                *i_ptr = strtol(item, NULL, 10);
                csvfile->data[i] = i_ptr;

                #ifdef CSV_ITERATOR_DEBUG
                printf("found char\n");
                #endif

                break;

            case 'f':
                d_ptr = malloc(sizeof(double));
                *d_ptr = strtod(item, NULL);
                csvfile->data[i] = d_ptr;

                #ifdef CSV_ITERATOR_DEBUG
                printf("found double\n");
                #endif

                break;

            case 's':
                c_ptr = malloc(strlen(item) + 1);
                strcpy(c_ptr, item);
                csvfile->data[i] = c_ptr;

                #ifdef CSV_ITERATOR_DEBUG
                printf("found string\n");
                #endif

                break;
        }
    }

    //advance current row so we know to execute stage 1 on the next call
    ++csvfile->curr_row;

    //release memory block occupied by buffer to keep resource demands low
    free(buffer);
}

//access an item from the row currently loaded into memory
void *curr_row_get_item(struct csv *csvfile, int index)
{
    assert(csvfile != NULL);
    assert(index >= 0 && index < csvfile->total_columns);

    return csvfile->data[index];
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
