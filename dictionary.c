// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// Number of buckets in hash table
const unsigned int N = 100000; // Adjusted for better performance


node *table[N]; // table is an array of N pointers to nodes

// Variable to keep track of the number of words loaded
unsigned int word_count = 0;

// Hashes word to a number using djb2 algorithm
unsigned int hash(const char *word)
{
    unsigned long hash = 5381; // the hash value is initialized to 5381 as recommended by the djb2 algorithm
    int c;                     // declare an integer variable to store the ASCII value of a character

    while ((c = tolower(*word++))) // loop through each character in the word and convert it to lowercase
    {
        hash = ((hash << 5) + hash) + c; /* calculate the hash value using the djb2 algorithm
                                            - the hash value is left-shifted by 5 bits and added to the hash value
                                            - the ASCII value of the character is added to the hash value
                                          */
    }

    return hash % N;
}

bool load(const char *dictionary)
{
    for (unsigned int i = 0; i < N; i++) /*
                                            - N is the number of buckets in the hash table, we set it to 100000 at the beginning of the program, we can adjust it for better performance
                                            - we made an array of N pointers at the beginning of the program
                                            - loop through each element in the table array
                                         */
        table[i] = NULL;                  // set each i'th element in the table array to NULL

    FILE *file = fopen(dictionary, "r");  // open the dictionary file in read mode
    if (file == NULL)                     // if the file could not be opened
    {
        fprintf(stderr, "Could not open %s.\n", dictionary); // print an error message
        return false;                                        // return false to indicate an error
    }
    char word[LENGTH + 1];                                   /* - make an array of LENGTH + 1 characters to store the word
                                                                - LENGTH is the maximum length for a word, we set it to 45 at the beginning of the program
                                                                - +1 is for the null terminator
                                                             */
    // Read words from the file one at a time
    while (fscanf(file, "%45s", word) != EOF)                /*
                                                                - fscanf() reads formatted input from a file
                                                                - it has 3 arguments: the file pointer, the format specifier, and the variable to store the input
                                                                - file is the file pointer
                                                                - %45s is the format specifier that reads a string of up to 45 characters
                                                                - %45s means read a string of up to 45 characters or until a whitespace character is encountered, so
                                                                - % means the beginning of the format specifier, 45 is the maximum number of characters to read, and s means read until a whitespace character is encountered.
                                                                - word is the variable to store the word
                                                                - fscanf(file, "%45s", word) reads a word of up to 45 characters from the file and stores it in the word variable
                                                                - how the loop stops:
                                                                - fscanf() returns EOF when it reaches the end of the file
                                                                - the loop stops when fscanf() returns EOF
                                                                - EOF is a constant defined in stdio.h that represents the end of the file
                                                             */
    {
        node *new_node = malloc(sizeof(node));             // allocate memory for a new node to store the word
        if (new_node == NULL)                              // if malloc() returns NULL
        {
            fclose(file);                                  // close the dictionary file
            unload();                                      // unload the dictionary
            return false;                                  // return false to indicate an error
        }
        strcpy(new_node->word, word);                      // copy the word into the new node
        new_node->next = NULL;                             // set the next pointer of the new node to NULL

        unsigned int index = hash(word);                   // hash the word to get the index

        if (table[index] == NULL)                          // if the bucket is empty
        {
            table[index] = new_node;                      // insert the new node into the bucket
        }
        else                                              // if the bucket is not empty
        {
            new_node->next = table[index];                // set the next pointer of the new node to the current head of the linked list
            table[index] = new_node;                      // set the new node as the head of the linked list
        }

        word_count++;                                    // increment the word count
    }                                                    

                                                        /*
                                                          - so in summery, in this while loop:
                                                          - we read a word from the dictionary file
                            
                                                        */

    // Close the dictionary file
    fclose(file);
    return true;
}

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // Create a lowercase copy of the word for case-insensitive comparison
    char lower_word[LENGTH + 1];
    int len = strlen(word);

    if (len > LENGTH)
    {
        return false;
    }

    for (int i = 0; i < len; i++)
    {
        lower_word[i] = tolower(word[i]);
    }
    lower_word[len] = '\0';

    // Hash the lowercase word to get the index
    unsigned int index = hash(lower_word);

    // Traverse the linked list at the hashed index
    node *cursor = table[index];
    while (cursor != NULL)
    {
        if (strcmp(cursor->word, lower_word) == 0)
        {
            return true;
        }
        cursor = cursor->next;
    }

    // Word not found
    return false;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    return word_count;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    // Iterate through each bucket in the hash table
    for (unsigned int i = 0; i < N; i++)
    {
        node *cursor = table[i];
        while (cursor != NULL)
        {
            node *temp = cursor;
            cursor = cursor->next;
            free(temp);
        }
        table[i] = NULL; // Optional: Clear the pointer after freeing
    }
    return true;
}
