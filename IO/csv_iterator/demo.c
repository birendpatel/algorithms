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
    //use the format string to tell program how to interpret each row
    struct csv *file = csv_create("demo.csv", "%d,%f,%s,%c", ',');

    //we will use these variables to hold data in the currently loaded row.
    //item 3 is of course a string, hence the char pointer.
    int item_1;
    double item_2;
    char *item_3;
    char item_4;

    //make sure there is data available to read, csv_next() returns false if not.
    //csv_has_next() has lazy evaluation, it doesn't know there is no data
    //available to read until csv_next() actually attempts a read, so you cannot
    //use it here in the while test in place of csv_next().
    while(csv_next(file))
    {
        //extract the data into our variables.
        item_1 = csv_get(file, 0, int);
        item_2 = csv_get(file, 1, double);
        item_4 = csv_get(file, 3, char);

        //csv_get_ptr could be used for all items, but it would be a nuisance
        //to have to cast the void pointer and perform indirection. csv_get()
        //does that for you if you provide the data type in the third parameter.
        //strings must call csv_get_ptr()
        item_3 = csv_get_ptr(file, 2);

        //print current row to stdout to take a look at our results
        printf("%d, ", item_1);
        printf("%g, ", item_2);
        printf("%s, ", item_3);
        printf("%c \n", item_4);
    }

    //don't forget to destroy the type after you're done.
    //it's safe to set the flush_curr parameter to true, regardless of whether
    //or not there is a current row in memory.
    csv_destroy(file, false);

    return EXIT_SUCCESS;
}
