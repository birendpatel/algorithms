/*
* author: Biren Patel
* description: example use case of the csv iterator data type
*/

#include <stdio.h>
#include <stdlib.h>
#include "csv_iterator.h"

int main(void)
{
    struct csv *file = csv_create("demo.csv", "%d,%f,%s,%c", ',');

    int item_1;
    double item_2;
    char *item_3;
    char item_4;

    csv_next(file);
    item_1 = csv_get(file, 0, int);
    printf("%d\n", item_1);

    csv_next(file);
    item_2 = csv_get(file, 1, double);
    printf("%g\n", item_2);

    csv_next(file);
    item_3 = (char *) csv_get_ptr(file, 2);
    printf("%s\n", item_3);

    csv_next(file);
    item_4 = csv_get(file, 3, char);
    printf("%c\n", item_4);

    csv_destroy(file, 1);

    return EXIT_SUCCESS;
}
