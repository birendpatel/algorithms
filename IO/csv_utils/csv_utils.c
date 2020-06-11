/*
* author: Biren Patel
* description: CSV utilities
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

/*******************************************************************************
* function prototypes
*******************************************************************************/

void display_instructions(void);
int get_user_code(void);

bool csv_replace_sep(FILE *csvfile, char old, char new);
bool csv_add_missing_commas_tail(FILE *csvfile);

/******************************************************************************/

int main(int argc, char **argv)
{
    //open file with supplied user argument
    FILE *csvfile = fopen(argv[1], "r+");
    if (csvfile == NULL)
    {
        printf("Ensure file exists at location.\nUsage: csv_utils <filename>");
        exit(EXIT_FAILURE);
    }

    //main program loop
    while(true)
    {
        display_instructions();
        switch(get_user_code())
        {
            case 0:
                printf("Program terminated\n");
                fclose(csvfile);
                return EXIT_SUCCESS;

            case 1:
                //replace separator
                printf("selected 1\n");
                break;

            case 2:
                //add missing commas
                printf("selected 2\n");
                break;

            default:
                printf("Program terminated\n");
                fclose(csvfile);
                return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}

/******************************************************************************/

void display_instructions(void)
{
    printf("\n\n     CSV Utilities - Biren Patel - " __DATE__" - "__TIME__);
    printf("\n|-----------------------------------------------------------|");
    printf("\n\n\t\tCodes\tPurpose\n\t\t-----\t-----------------\n");
    printf("\t\t  0  \texit program\n");
    printf("\t\t  1  \treplace separator\n");
    printf("\t\t  2  \tadd missing commas\n");
    printf("\n|-----------------------------------------------------------|");

    printf("\n\nEnter an instruction code: ");
}

/******************************************************************************/

int get_user_code(void)
{
    char buffer[5];
    fgets(buffer, 5, stdin);
    int user_code = (int) strtol(buffer, NULL, 10);
    return user_code;
}

/*******************************************************************************
* function: csv_replace_sep
* purpose: replace all instances of one separator with a different separator
* @ csvfile : pointer to file object
* @ old : the current separator
* @ new : the replacement separator
* returns : true if successful, false otherwise
*******************************************************************************/
bool csv_replace_sep(FILE *csvfile, char old, char new)
{

}

/*******************************************************************************
* function: csv_add_missing_commas_tail
* purpose: add commas for items missing at the tail of each row
* @ csvfile : pointer to file object
* returns : true if successful, false otherwise
*******************************************************************************/
bool csv_add_missing_commas_tail(FILE *csvfile)
{

}
