/*
* author: Biren Patel
* description: implementation for python-style row iterator from CSV file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
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
* @ file_ptr : pointer to stdio FILE type
* @ curr_row : pointer to the current row loaded in memory, 0 before first load
* @ data_available : 0 if no data left to read, 1 otherwise.
* @ sep : character used for row tokenization
* @ total_columns : total columns in csv file
* @ column_formats : array of data types of each column, encoded as characters
* @ data : array of void pointers to one row of data, one pointer per column.
* purpose: holds CSV metadata
*******************************************************************************/

struct csv
{
    FILE *file_ptr;
    int curr_row;
    bool data_available;
    char sep;
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
* private function: csv_destroy_row
* purpose: return the memory blocks held by the current row
* @ csvfile : pointer to struct csv
*******************************************************************************/
static void csv_destroy_row(struct csv *csvfile);

/*******************************************************************************
* private function: csv_convert_reserve
* purpose: convert an area in buffer to a specific data type and reserve memory
* @ csvfile : pointer to struct csv
* @ start_pos : pointer to a starting point in a buffer
* @ col : current column undergoing conversion
* returns: true if successful conversion and allocation, false otherwise.
*******************************************************************************/
static bool csv_convert_reserve(struct csv *csvfile, char *start_pos, int col);

/*******************************************************************************
* public functions
*******************************************************************************/

struct csv *csv_create(char* filename, char *fmt, char sep)
{
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

    //set remaining members
    csvfile->curr_row = 0;
    csvfile->data_available = true;
    csvfile->sep = sep;

    #if CSV_ITERATOR_DEBUG
    printf("\n\n~~~~~ CSV_CREATE() FINISHED ~~~~~\n");
    printf("separator is: %c\n", csvfile->sep);
    printf("total columns is: %d\n", csvfile->total_columns);
    printf("column formats is: %s\n\n", csvfile->column_formats);
    #endif

    return csvfile;
}

/******************************************************************************/

void csv_destroy(struct csv *csvfile, bool flush_curr)
{
    #if CSV_ITERATOR_DEBUG
    printf("\n\n~~~~~ CSV_DESTROY() ~~~~~\n");
    #endif

    //second test: avoid attempting to free if no loads yet
    //third test: if test is false, csv_next() would have already performed free
    if (flush_curr && csvfile->curr_row != 0 && csvfile->data_available == true)
    {
        csv_destroy_row(csvfile);
    }

    fclose(csvfile->file_ptr);
    free(csvfile->column_formats);
    free(csvfile->data); //make sure pointed data gets free'd beforehand
    free(csvfile);

    #if CSV_ITERATOR_DEBUG
    printf("held memory blocks are now free\n");
    #endif
}

/******************************************************************************/

/*
stage 2 in the below function is where the data type inference and missing value
inference happens. Essentially, two pointers to the buffer (lag and lead) play
a game of leapfrog with each other, hopping along points in the buffer at or
just after a separator. lag sets the position of lead, after a terminating
condition, lag leaps over lead to set its own new position and then lead
leaps over lag. this cycle repeats until n items are found or reported missing.
*/

bool csv_next(struct csv *csvfile)
{
    #if CSV_ITERATOR_DEBUG
    printf("\n\n~~~~~ CSV_NEXT() CALL ~~~~~\n");
    #endif

    assert(csvfile != NULL);
    assert(csvfile->total_columns >= 1);
    assert(csvfile->curr_row >= 0);

    //early stopping if user was informed on last call that no data is available
    if (csvfile->data_available == false)
    {
        #if CSV_ITERATOR_DEBUG
        printf("user already notified no data remains, exiting early\n\n");
        #endif
        return false;
    }

    //stage 1: if not on very first load, free memory occupied by previous row.
    if (csvfile->curr_row != 0)
    {
        csv_destroy_row(csvfile);
    }

    //stage 2: set up a temporary buffer to hold next line in csv
    assert(csvfile->data_available == true);
    char *buffer = malloc(CSV_ITERATOR_BUF_LEN);
    VERIFY_POINTER(malloc, buffer)

    //read next line, but if end of CSV then flip flag and return early.
    if (fgets(buffer, CSV_ITERATOR_BUF_LEN, csvfile->file_ptr) == NULL)
    {
        #if CSV_ITERATOR_DEBUG
        printf("no data available to read, exiting early\n\n");
        #endif

        csvfile->data_available = false;
        return false;
    }

    //a final paranoid check for data availability
    assert(csvfile->data_available == true);

    //replace line feed in buffer with a null character
    if (strchr(buffer, '\n'))
    {
        #if CSV_ITERATOR_DEBUG
        printf("read line into buffer, replaced line feed with null char\n");
        #endif

        buffer[strcspn(buffer, "\n")] = '\0';
    }

    //variables used in the following for loop
    char *lag = buffer;
    char *lead;

    //on each loop, load into memory the data from column i of the current row
    for(size_t i = 0; i < csvfile->total_columns; ++i)
    {
        //replace the next separator on or ahead of lag with a null character
        char *next_separator = strchr(lag, csvfile->sep);
        if (next_separator != NULL) *next_separator = '\0';

        //track the lead pointer to this new null character
        lead = strchr(lag, '\0');

        #if CSV_ITERATOR_DEBUG
        printf("now on loop iteration: %d\n", (int) i);
        printf("\tlag points to character: %c\n", *lag);
        printf("\tlead points just before character: %c\n", *(lead+1));
        #endif

        //at this specific point in the loop, after lag and lead have been set,
        //there is a missing value if lag points to the null character.
        if (*lag == '\0')
        {
            #if CSV_ITERATOR_DEBUG
            printf("\tmissing value, moving to next iteration\n");
            #endif

            //NULL indicates that there is a missing value
            csvfile->data[i] = NULL;
            lag = lead + 1;
            continue;
        }

        //no missing data, convert the item data type and reserve memory for it
        bool status;
        status = csv_convert_reserve(csvfile, lag, i);
        assert(status == true);

        //lag leaps over lead to new position before start of next iteration.
        lag = lead + 1;
        assert(lag == lead + 1);
    }

    ++csvfile->curr_row;
    free(buffer);
    return true;
}

/******************************************************************************/

void *csv_get_ptr(struct csv *csvfile, int index)
{
    assert(csvfile != NULL);
    assert(index >= 0 && index < csvfile->total_columns);

    return csvfile->data[index];
}

/*******************************************************************************
* private functions
*******************************************************************************/

static int calc_num_columns(struct csv *csvfile, char *fmt, char sep)
{
    assert(csvfile != NULL);
    assert(fmt != NULL);

    //starts at 1 since no comma after the final specifier
    int total_columns = 1;

    for(char *curr = fmt; *curr != '\0'; ++curr)
    {
        if (*curr == sep) ++total_columns;
    }

    assert(total_columns >= 1);
    return total_columns;
}

/******************************************************************************/

static char *infer_data_types(struct csv *csvfile, char *fmt, char sep)
{
    assert(csvfile != NULL);
    assert(fmt != NULL);

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

/******************************************************************************/

static void csv_destroy_row(struct csv *csvfile)
{
    assert(csvfile != NULL);
    assert(csvfile->data_available == true);

    for(size_t i = 0; i < csvfile->total_columns; ++i)
    {
        #if CSV_ITERATOR_DEBUG
        printf("freeing memory held by previous row\n");
        #endif

        free(csvfile->data[i]);
    }
}

/******************************************************************************/

static bool csv_convert_reserve(struct csv *csvfile, char *start_pos, int col)
{
    bool status = false;

    int *i_ptr;
    char *c_ptr;
    double *d_ptr;

    switch (csvfile->column_formats[col])
    {
        case 'd':
                i_ptr = malloc(sizeof(int));
                VERIFY_POINTER(malloc, i_ptr);
                *i_ptr = strtol(start_pos, NULL, 10);
                csvfile->data[col] = i_ptr;

                #if CSV_ITERATOR_DEBUG
                printf("\tswitch found int of value: %d\n", *i_ptr);
                #endif

                status = true;
                break;

        case 'c':
                c_ptr = malloc(1);
                VERIFY_POINTER(malloc, c_ptr);
                *c_ptr = *start_pos;
                csvfile->data[col] = c_ptr;

                #if CSV_ITERATOR_DEBUG
                printf("\tswitch found char of value: %c\n", *c_ptr);
                #endif

                status = true;
                break;

        case 'f':
                d_ptr = malloc(sizeof(double));
                VERIFY_POINTER(malloc, d_ptr);
                *d_ptr = strtod(start_pos, NULL);
                csvfile->data[col] = d_ptr;

                #if CSV_ITERATOR_DEBUG
                printf("\tswitch found float of value: %g\n", *d_ptr);
                #endif

                status = true;
                break;

        case 's':
                c_ptr = malloc(strlen(start_pos) + 1);
                VERIFY_POINTER(malloc, c_ptr);
                strcpy(c_ptr, start_pos);
                csvfile->data[col] = c_ptr;

                #if CSV_ITERATOR_DEBUG
                printf("\tswitch found int of value: %s\n", c_ptr);
                #endif

                status = true;
                break;
    }

    return status;
}
