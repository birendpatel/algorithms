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
* structure: struct csv
* purpose: holds CSV metadata
*******************************************************************************/

struct csv
{
    int total_columns;
    char *column_formats;
};

/*******************************************************************************
* private function: handle_fmt_string
* purpose: determine number of columns and their data types
* @ fmt : format string passed by user on contructor
* @ sep : separating chacter passed by user on constructor
*******************************************************************************/
void handle_fmt_string(char *fmt, char sep);

/*******************************************************************************
* public functions
*******************************************************************************/

//constructor
struct csv *csv_create(char* filename, char *fmt, char sep)
{

}

//destructor
void csv_destroy(struct csv *csvfile)
{

}

/*******************************************************************************
* private functions
*******************************************************************************/

//determine number of columns and their data types
void handle_fmt_string(char *fmt, char sep)
{
    assert(fmt != NULL);
    assert(sep != 0);

    //first I need the total number of columns
    //starts at 1 since we dont use comma after the final column specifier
    int total_columns = 1;

    for(char *curr = fmt; *curr != '\0'; ++curr)
    {
        if (*curr == sep)
        {
            ++total_columns;
        }
    }
    assert(total_columns >= 1);

    //now I need to extract the column format specifiers for later use.
    //this is NOT a string
    char *column_formats = malloc(total_columns);
    if (column_formats == NULL)
    {
        fprintf(stderr, "malloc failed in %s in %s", __func__, __FILE__);
        exit(EXIT_FAILURE);
    }

    {
        int loc = 0;

        for(char *curr = fmt; *curr != '\0'; ++curr)
        {
            if (*curr == '%')
            {
                column_formats[loc++] = *(curr + 1);
                assert(loc <= total_columns);
            }
        }
        assert(loc == total_columns);
    }

    //debugging shit
    printf("total columns is %d\n\n", total_columns);
    for(size_t i = 0; i < total_columns; ++i)
        printf("column %d is format %c\n", (int) i, column_formats[i]);
    free(column_formats);
}
