/*
* author: Biren Patel
* description: command line program for CSV utilities
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define BUFFER_LEN 2048

/*******************************************************************************
* function prototypes
*******************************************************************************/

void display_instructions(void);
int get_user_code(void);

bool csv_replace_sep(FILE *csvfile, FILE *newfile);
bool csv_add_missing_commas_tail(FILE *csvfile, FILE *newfile);

/******************************************************************************/

int main(int argc, char **argv)
{
    //open file with supplied user argument
    FILE *csvfile = fopen(argv[1], "r");

    //open file to place new contents
    FILE *newfile = fopen(argv[2], "w");

    if (csvfile == NULL || newfile == NULL)
    {
        printf("Ensure file exists at location.\n");
        printf("Usage: csv_utils <filename> <filename>");
        exit(EXIT_FAILURE);
    }

    //main program loop
    while(true)
    {
        display_instructions();

        switch(get_user_code())
        {
            case 1:
                //replace separator
                csv_replace_sep(csvfile, newfile);
                break;

            case 2:
                //add missing commas
                csv_add_missing_commas_tail(csvfile, newfile);
                break;

            default:
                printf("Program terminated\n");
                fclose(csvfile);
                fclose(newfile);
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
* returns : true if successful, false otherwise
*******************************************************************************/
bool csv_replace_sep(FILE *csvfile, FILE *newfile)
{
    char old, new;
    char buffer[BUFFER_LEN];

    printf("Enter the current separator: ");
    old = getchar();
    while(getchar() != '\n');

    printf("Enter the replacement separator: ");
    new = getchar();
    while(getchar() != '\n');

    while(fgets(buffer, BUFFER_LEN, csvfile))
    {
        char *next_sep = buffer;
        while((next_sep = strchr(buffer, old)))
        {
            *next_sep = new;
        }
        fputs(buffer, newfile);
    }

    return true;
}

/*******************************************************************************
* function: csv_add_missing_commas_tail
* purpose: add commas for items missing at the tail of each row
* @ csvfile : pointer to file object
* returns : true if successful, false otherwise
*******************************************************************************/
bool csv_add_missing_commas_tail(FILE *csvfile, FILE *newfile)
{
    return true;
}
