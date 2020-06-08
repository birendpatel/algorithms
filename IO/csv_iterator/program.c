#include "csv_iterator.h"

int main(void)
{
    struct csv *data = csv_create("data.csv", "%d,%f,%s", ',');

    csv_next(data);
    csv_next(data);
    csv_next(data);
    csv_next(data);

    csv_destroy(data);
}
