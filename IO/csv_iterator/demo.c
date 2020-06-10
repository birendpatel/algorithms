/*
* author: Biren Patel
* description: example use case of the csv iterator data type
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "csv_iterator.h"

int main(void)
{
    //use the format string to tell program how to interpret each row.
    //% notifies program that a format type (d, f, s, or c) is incoming.
    //the separator must be in the format string and must match parameter 3.
    struct csv *file = csv_create("demo.csv", "%d,%f,%s,%c", ',');

    //if you call csv_next(), then it will load the next available row of data
    //into memory. it returns a boolean value to indicate a load was successful.
    //if false, there was nothing left to read. By default, a 1 KiB buffer is
    //used to read each row. Change this macro in the header file if necessary.
    //the buffer is only temporary for the duration of the csv_next() call.
    while(csv_next(file))
    {
        //struct csv stores all data via void pointers.

        //get the void pointer storing data in column 0, and cast to your type
        int *item_1;
        item_1 = (int*) csv_get_ptr(file, 0);

        //if you know a column never has missing data, you can use csv_get to
        //instead get the value directly without the pointer headache. just
        //pass the matching type in the third parameter.
        double item_2;
        item_2 = csv_get(file, 1, double);

        //strings of course will always need a pointer
        char *item_3;
        item_3 = (char*) csv_get_ptr(file, 2);

        //nothing special here, just another example
        char *item_4;
        item_4 = (char*) csv_get_ptr(file, 3);

        //lets print each row to stdout. But there is a catch.
        //missing data is represented by a pointer to NULL.
        //therefore, to avoid accidents, test for NULL before dereferencing
        if (!item_1) printf(" , "); else printf("%d, ", *item_1);
        printf("%g, ", item_2);
        if (!item_3) printf(" , "); else printf("%s, ", item_3);
        if (!item_4) printf(" \n"); else printf("%c\n", *item_4);
    }

    //if there is no data available, another call of csv_next becomes a no-op.
    csv_next(file);

    //destroy the type after you're done. it's safe to set the flush_curr
    //parameter to true, regardless of whether or not there is a current row
    //in memory.
    csv_destroy(file, false);

    return EXIT_SUCCESS;
}
