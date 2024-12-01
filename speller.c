// Implements a spell-checker

#include <ctype.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "dictionary.h"

// Undefine any definitions
#undef calculate
#undef getrusage

// Default dictionary
#define DICTIONARY "dictionaries/large" // the default dictionary is large

// Prototype
double calculate(const struct rusage *b, const struct rusage *a);

int main(int argc, char *argv[])
{
                            /*
                             - here in this program, we read 2 or 3 arguments from the command line
                             - argv[0] is the name of the program
                             - argv[1] is the name of the dictionary file
                             - argv[2] is the name of the text file
                             - the goal of the program is to check text file for misspelled words using the dictionary file and report the number of misspelled words
                            */
    struct rusage before;
    struct rusage after;
    double time_load;
    double time_check;
    double time_size;
    double time_unload;
    char *dictionary;

    if (argc != 2 && argc != 3) // if the number of arguments is not 2 or 3
    {
        printf("Usage: ./speller [DICTIONARY] text\n");
        return 1;
    }
    time_load = 0.0;
    time_check = 0.0;
    time_size = 0.0;
    time_unload = 0.0;
    dictionary = (argc == 3) ? argv[1] : DICTIONARY; // if the number of arguments is 3, set dictionary to argv[1], else set it to DICTIONARY
    getrusage(RUSAGE_SELF, &before); /* 
                                       - getrusage() is a system call that gets the resource usage of the calling process
                                       - system call means that it is a request for the operating system to do something on behalf of the process
                                       - resource usage means the amount of resources that the process is using
                                       - getrusage() receives two arguments: the first is the resource to be queried, and the second is a pointer to a struct rusage
                                       - RUSAGE_SELF is a macro that specifies that the resource usage of the calling process should be queried
                                       - queried means that the resource usage is being asked for.
                                       - RUSAGE_SELF is a constant that is used to specify the resource usage of the calling process
                                       - &before is the address of the struct that will store the resource usage before the operation
                                       - by before the operation, it means before the operation of loading the dictionary
                                       -getrusage(RUSAGE_SELF, &before) gets the resource usage of the calling process before the operation of loading the dictionary and stores it in the struct before
                                     */

    bool loaded = load(dictionary); /*
                                        - load() is a function that loads a dictionary into memory
                                        - dictionary is a pointer to a string that contains the path to the dictionary file
                                        - load(dictionary) loads the dictionary file into memory
                                        - loaded is a boolean variable that stores whether the dictionary was loaded successfully
                                        - if the dictionary was loaded successfully, loaded is set to true, else it is set to false
                                    */
    getrusage(RUSAGE_SELF, &after); // now we get the resource usage of the calling process after the operation of loading the dictionary and store it in the struct after
    if (!loaded)                    // if the dictionary was not loaded successfully
    {
        printf("Could not load %s.\n", dictionary); // print an error message
        return 1;                                   // return 1 to indicate an error
    }
    time_load = calculate(&before, &after);         /*
                                                      - calculate() is a function that calculates the time between two resource usage structs
                                                      - it receives two arguments: the resource usage struct before the operation and the resource usage struct after the operation
                                                      - it returns the number of seconds between the two resource usage structs
                                                      - time_load is a double variable that stores the time it took to load the dictionary
                                                      - calculate(&before, &after) calculates the time it took to load the dictionary and stores it in time_load
                                                    */
    char *text = (argc == 3) ? argv[2] : argv[1];   // if the number of arguments is 3, set text to argv[2], else set it to argv[1]
    FILE *file = fopen(text, "r");                  // open the text file in read mode and store the file pointer in file
    if (file == NULL)                              // if the file could not be opened
    {
        printf("Could not open %s.\n", text);      // print an error message
        unload();                                  /*
                                                        - unload() is a function that unloads the dictionary from memory
                                                        - it returns a boolean value that indicates whether the dictionary was unloaded successfully
                                                        - unload() unloads the dictionary from memory
                                                        - if we do not unload the dictionary, we will have a memory leak
                                                   */
        return 1;                                   // return 1 to indicate an error
    }
    printf("\nMISSPELLED WORDS\n\n");               // print a message to indicate that the misspelled words will be printed below

    int     index = 0;
    int     misspellings = 0;
    int     words = 0;
    char    word[LENGTH + 1];                        // +1 for null terminator

    char c;                                          // character to read from file
    while (fread(&c, sizeof(char), 1, file))         // read one character at a time from the file
    {
        if (isalpha(c) || (c == '\'' && index > 0))  // if the character is a letter or an apostrophe and the index is greater than 0
        {
            word[index] = c;                         // store the character in the word array
            index++;                                 // increment the index
            if (index > LENGTH)                      // if the index is greater than the maximum length of a word
            {
                while (fread(&c, sizeof(char), 1, file) && isalpha(c)); /*
                                                                            - while the character is a letter, read the next character
                                                                            - this is to consume the rest of the word
                                                                            - we do not want to check the spelling of a word that is longer than the maximum length
                                                                            - we are ignoring words that are longer than the maximum length
                                                                            - this is similar to how MS Word ignores words that are too long
                                                                            - we dont have i++ here because we are not storing the characters in the word array
                                                                            - the while loop will keep reading characters until it reaches a non-letter character
                                                                        */

                //after consuming the rest of the word, we prepare for the next word
                index = 0; // by setting the index to 0
            }
        }
        else if (isdigit(c)) // if the character is a digit
        {
            while (fread(&c, sizeof(char), 1, file) && isalnum(c)); // consume the rest of the word until we reach a non alphanumeric character
                                                                    // alphanumeric means a letter or a digit
            // we prepare for the next word
            index = 0; // by setting the index to 0
        }
        else if (index > 0) // if the character is not a letter, digit, or apostrophe and the index is greater than 0
        {
            // it means we have reached the end of a word
            word[index] = '\0'; // so we add a null terminator to the word
            words++;            // increment the number of words
            getrusage(RUSAGE_SELF, &before); // get the resource usage before checking the spelling of the word  
            bool misspelled = !check(word);  // check if the word is misspelled
            getrusage(RUSAGE_SELF, &after);  // get the resource usage after checking the spelling of the word
            time_check += calculate(&before, &after); // calculate the time it took to check the spelling of the word and add it to time_check
            if (misspelled) // if the word is misspelled
            {
                printf("%s\n", word); // print the word
                misspellings++;      // increment the number of misspelled words
            }
            // Prepare for next word
            index = 0; // by setting the index to 0
        }
    }
    if (ferror(file)) /*
                        - ferror() is a function that checks if an error occurred while reading from a file
                        - it returns a non-zero value if an error occurred, else it returns 0
                      */
    {
        fclose(file); // close the file
        printf("Error reading %s.\n", text); // print an error message
        unload(); // unload the dictionary
        return 1; // return 1 to indicate an error
    }
    fclose(file); // close the file
    getrusage(RUSAGE_SELF, &before); // get the resource usage before determining the size of the dictionary
    unsigned int n = size();         // determine the size of the dictionary and store it in n
    getrusage(RUSAGE_SELF, &after);  // get the resource usage after determining the size of the dictionary
    time_size = calculate(&before, &after);  // calculate the time it took to determine the size of the dictionary and store it in time_size
    getrusage(RUSAGE_SELF, &before);         // get the resource usage before unloading the dictionary
    bool unloaded = unload();                // unload the dictionary and store whether it was unloaded successfully in unloaded
    getrusage(RUSAGE_SELF, &after);          // get the resource usage after unloading the dictionary
    if (!unloaded)                         // if the dictionary was not unloaded successfully
    {
        printf("Could not unload %s.\n", dictionary); // print an error message
        return 1; // return 1 to indicate an error
    }
    time_unload = calculate(&before, &after);  // calculate the time it took to unload the dictionary and store it in time_unload
    printf("\nWORDS MISSPELLED:     %d\n", misspellings); // print the number of misspelled words
    printf("WORDS IN DICTIONARY:  %d\n", n);              // print the number of words in the dictionary
    printf("WORDS IN TEXT:        %d\n", words);          // print the number of words in the text
    printf("TIME IN load:         %.2f\n", time_load);    // print the time it took to load the dictionary
    printf("TIME IN check:        %.2f\n", time_check);   // print the time it took to check the spelling of the words
    printf("TIME IN size:         %.2f\n", time_size);    // print the time it took to determine the size of the dictionary
    printf("TIME IN unload:       %.2f\n", time_unload);  // print the time it took to unload the dictionary
    printf("TIME IN TOTAL:        %.2f\n\n",  
           time_load + time_check + time_size + time_unload); // print the total time it took to load, check, determine the size, and unload the dictionary

    return 0; // return 0 to indicate success
}

double calculate(const struct rusage *b, const struct rusage *a)
{
    if (b == NULL || a == NULL) // if either of the resource usage structs is NULL
    {
        return 0.0; // return 0.0
    }
    else // if both resource usage structs are not NULL
    {
        return ((((a->ru_utime.tv_sec * 1000000 + a->ru_utime.tv_usec) -
                  (b->ru_utime.tv_sec * 1000000 + b->ru_utime.tv_usec)) +
                 ((a->ru_stime.tv_sec * 1000000 + a->ru_stime.tv_usec) -
                  (b->ru_stime.tv_sec * 1000000 + b->ru_stime.tv_usec)))
                / 1000000.0); // calculate the time between the two resource usage structs and return it
    }
}
