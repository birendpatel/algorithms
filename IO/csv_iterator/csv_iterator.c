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
    printf("~~~~~ CSV_CREATE() FINISHED ~~~~~\n");
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
    printf("~~~~~ CSV_DESTROY() ~~~~~\n\n");
    #endif

    //second test: avoid attempting to free if no loads yet
    //third test: if test is false, csv_next() would have already performed free
    //missing data points to null, but I safely assume free(NULL) is a no-op.
    if (flush_curr && csvfile->curr_row != 0 && csvfile->data_available == true)
    {
        for(size_t i = 0; i < csvfile->total_columns; ++i)
        {
            #if CSV_ITERATOR_DEBUG
            printf("freeing currently loaded row in memory\n");
            #endif

            free(csvfile->data[i]);
        }
    }

    #if CSV_ITERATOR_DEBUG
    printf("freeing struct members and struct csv\n");
    #endif

    fclose(csvfile->file_ptr);
    free(csvfile->column_formats);
    free(csvfile->data); //make sure pointed data gets free'd beforehand
    free(csvfile);

    #if CSV_ITERATOR_DEBUG
    printf("finished freeing memory blocks\n");
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

//PLEASE REFORMAT THIS FUNCTION, WAY TOO LONG, WAY TOO COUPLED.

bool csv_next(struct csv *csvfile)
{
    #if CSV_ITERATOR_DEBUG
    printf("\n\n~~~~~ CSV_NEXT() CALL ~~~~~\n");
    #endif

    assert(csvfile != NULL);
    assert(csvfile->total_columns >= 1);
    assert(csvfile->curr_row >= 0);

    //this clause is triggered after the user has been notified already that
    //there is no data left to read (on the last call that they attempted). This
    //clause will now trigger on all subsequent calls to prevent stage 1 from
    //attempting to free memory blocks which haven't been allocated.
    //there are also three assertion checks below as a measure of extra safety.
    if (csvfile->data_available == false)
    {
        #if CSV_ITERATOR_DEBUG
        printf("user already notified no data left to read, exiting early\n\n");
        #endif
        return false;
    }

    //STAGE 1: if not on very first load, free memory occupied by previous row.
    //missing data points to null, but I safely assume free(NULL) is a no-op.
    if (csvfile->curr_row != 0)
    {
        #if CSV_ITERATOR_DEBUG
        printf("now entering stage 1\n");
        #endif

        assert(csvfile->data_available == true);

        for(size_t i = 0; i < csvfile->total_columns; ++i)
        {
            #if CSV_ITERATOR_DEBUG
            printf("freeing memory held by previous row\n");
            #endif
            free(csvfile->data[i]);
        }
    }

    //STAGE 2: load next row from csv file into memory
    #if CSV_ITERATOR_DEBUG
    printf("now entering stage 2\n");
    #endif

    assert(csvfile->data_available == true);

    //set up a temporary buffer to hold next line in csv
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

    //a bit paranoid to have a third availability check, but it doesn't hurt
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

    #if CSV_ITERATOR_DEBUG
    printf("entering for loop to populate columns\n\n");
    #endif

    //on each loop, load into memory the data from column i of the current row
    for(size_t i = 0; i < csvfile->total_columns; ++i)
    {
        #if CSV_ITERATOR_DEBUG
        printf("now on loop iteration: %d\n", (int) i);
        printf("\tlag points to character: %c\n", *lag);
        #endif

        //replace the next separator with a null character
        char *next_null;
        if ((next_null = strchr(lag, csvfile->sep)))
        {
            #if CSV_ITERATOR_DEBUG
            printf("\tseparator was replaced with null character\n");
            #endif

            *next_null = '\0';
        }

        //track a pointer to this new null character
        lead = strchr(lag, '\0');

        #if CSV_ITERATOR_DEBUG
        printf("\tlead points just before character: %c\n", *(lead+1));
        #endif

        /*
        A missing value may occur in three general places:
            1. in the first column
            2. in a middle column
            3. in the last column

        Each possibility leads to an anomalous and unique lag-lead relationship:
            1. lag and lead point to the same address
            2. lag points to the separator character
            3. lag points to the null character

        Those possibilities would hold at any location in this loop, but at this
        specific location, after strchr() and after setting lead, every single
        possibility leads to lag anomalously pointing to the null character.
        */
        if (*lag == '\0')
        {
            #if CSV_ITERATOR_DEBUG
            printf("\tmissing value at location, moving to next iteration\n");
            #endif

            //void pointer stores NULL
            csvfile->data[i] = NULL;

            //advance lag for the next loop
            lag = lead + 1;

            continue;
        }

        //else, use lead to fetch format for current value and malloc into data
        char *c_ptr;
        int *i_ptr;
        double *d_ptr;

        switch (csvfile->column_formats[i])
        {
            case 'd':
                i_ptr = malloc(sizeof(int));
                *i_ptr = strtol(lag, NULL, 10);
                csvfile->data[i] = i_ptr;
                #if CSV_ITERATOR_DEBUG
                printf("\tswitch found int of value: %d\n", *i_ptr);
                #endif
                break;

            case 'c':
                c_ptr = malloc(1);
                *c_ptr = *lag;
                csvfile->data[i] = c_ptr;
                #if CSV_ITERATOR_DEBUG
                printf("\tswitch found char of value: %c\n", *c_ptr);
                #endif
                break;

            case 'f':
                d_ptr = malloc(sizeof(double));
                *d_ptr = strtod(lag, NULL);
                csvfile->data[i] = d_ptr;
                #if CSV_ITERATOR_DEBUG
                printf("\tswitch found float of value: %g\n", *d_ptr);
                #endif
                break;

            case 's':
                c_ptr = malloc(strlen(lag) + 1);
                strcpy(c_ptr, lag);
                csvfile->data[i] = c_ptr;
                #if CSV_ITERATOR_DEBUG
                printf("\tswitch found int of value: %s\n", c_ptr);
                #endif
                break;
        }

        //advance lag for the next loop
        lag = lead + 1;
        #if CSV_ITERATOR_DEBUG
        printf("\tlag is advanced before lead, points to: %c\n", *lag);
        #endif
    }


    #if CSV_ITERATOR_DEBUG
    printf("now exiting call of csv next\n\n");
    #endif

    ++csvfile->curr_row;
    free(buffer);
    return true;
}

/******************************************************************************/
bool csv_has_next(struct csv *csvfile)
{
    assert(csvfile != NULL);
    assert(csvfile->data_available == true || csvfile->data_available == false);

    return csvfile->data_available;
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
