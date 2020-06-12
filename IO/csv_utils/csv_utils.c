/*
* author: Biren Patel
* description: command line program for CSV utilities, quick and dirty CSV file
* cleanup operations for files that can risk unsafe overwrites. emphasis on
* unsafe - I'm just trying to do my C work faster here!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define BUFFER_LEN 2048
#define TEMP_FILENAME "_csv_utils_tempfile_.csv"

FILE *csvfile;
char *filename;

/*******************************************************************************
* function prototypes
*******************************************************************************/

void display_instructions(void);
int get_user_code(void);
void swap_files(void);
void csv_replace_sep(void);
void csv_add_missing_commas_tail(void);

/******************************************************************************/

int main(int argc, char **argv)
{
    //open file with supplied user argument
    filename = argv[1];
    csvfile = fopen(filename, "r");

    if (csvfile == NULL)
    {
        printf("Ensure file exists at location.\n");
        printf("Usage: csv_utils <filename>");
        exit(EXIT_FAILURE);
    }

    display_instructions();

    //main program loop
    while(true)
    {
        printf("\n\n>>> Enter an instruction code: [ ]\b\b");

        switch(get_user_code())
        {
            case 1:
                //show instructions again
                display_instructions();
                break;

            case 2:
                //replace separator
                csv_replace_sep();
                break;

            case 3:
                //add missing commas
                csv_add_missing_commas_tail();
                break;

            default:
                printf(">>> Program terminated\n");
                fclose(csvfile);
                return EXIT_SUCCESS;
        }
    }

    return EXIT_SUCCESS;
}

/******************************************************************************/

void display_instructions(void)
{
    system("cls");
    printf("\n\n     CSV Utilities - Biren Patel - " __DATE__" - "__TIME__);
    printf("\n|-----------------------------------------------------------|");
    printf("\n\n\t\tCodes\tPurpose\n\t\t-----\t-----------------\n");
    printf("\t\t  0  \texit program\n");
    printf("\t\t  1  \tclear screen\n");
    printf("\t\t  2  \treplace separator\n");
    printf("\t\t  3  \tadd missing commas\n");
    printf("\n|-----------------------------------------------------------|");
}

/******************************************************************************/

int get_user_code(void)
{
    char buffer[5];
    fgets(buffer, 5, stdin);
    int user_code = (int) strtol(buffer, NULL, 10);
    return user_code;
}

/******************************************************************************/

void swap_files(void)
{
    fclose(csvfile);
    remove(filename);
    rename(TEMP_FILENAME, filename);
    csvfile = fopen(filename, "r");

    assert(csvfile != NULL);
}

/*******************************************************************************
* function: csv_replace_sep
* purpose: replace all instances of one separator with a different separator
* @ csvfile : pointer to file object
* returns : true if successful, false otherwise
*******************************************************************************/
void csv_replace_sep(void)
{
    FILE *newfile = fopen(TEMP_FILENAME, "w");

    char old, new;
    char buffer[BUFFER_LEN];

    printf(">>> Enter the current separator: [ ]\b\b");
    old = getchar();
    while(getchar() != '\n');

    printf(">>> Enter the replacement separator: [ ]\b\b");
    new = getchar();
    while(getchar() != '\n');

    if (new == old)
    {
        return;
    }

    while(fgets(buffer, BUFFER_LEN, csvfile))
    {
        char *next_sep = buffer;
        while((next_sep = strchr(buffer, old)))
        {
            *next_sep = new;
        }
        fputs(buffer, newfile);
    }

    fclose(newfile);
    swap_files();
    printf(">>> requested changes are complete.\n");
}

/*******************************************************************************
* function: csv_add_missing_commas_tail
* purpose: add commas for items missing at the tail of each row
* @ csvfile : pointer to file object
* returns : true if successful, false otherwise
*******************************************************************************/
void csv_add_missing_commas_tail(void)
{
    //in user file find the maximum commas that currently exist on single line
    int max_commas = 0;
    char buffer[BUFFER_LEN];

    while(fgets(buffer, BUFFER_LEN, csvfile))
    {
        int curr_commas = 0;

        for(char *curr = buffer; *curr != '\0'; ++curr)
        {
            if (*curr == ',') ++curr_commas;
        }

        if (curr_commas > max_commas) max_commas = curr_commas;
    }

    //start again at the top and add x amount of commas if missing
    rewind(csvfile);
    FILE *newfile = fopen(TEMP_FILENAME, "w");

    while(fgets(buffer, BUFFER_LEN, csvfile))
    {

        int curr_commas = 0;

        for(char *curr = buffer; *curr != '\0'; ++curr)
        {
            if (*curr == ',') ++curr_commas;
        }

        int missing_commas = max_commas - curr_commas;

        if (missing_commas > 0)
        {
            if (strchr(buffer, '\n'))
            {
                buffer[strcspn(buffer, "\n")] = '\0';
            }

            char comma_str[missing_commas + 2];
            for(size_t i = 0; i < missing_commas + 1; ++i)
            {
                comma_str[i] = (i == missing_commas ? '\n' : ',');
            }
            comma_str[missing_commas + 1] = '\0';

            strcat(buffer, comma_str);
        }

        fputs(buffer, newfile);
    }

    fclose(newfile);
    swap_files();
    printf(">>> requested changes are complete.\n");
    printf(">>> %d columns now exist in file.\n", max_commas + 1);
}
