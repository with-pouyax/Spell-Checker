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

// Hash table
node *table[N];

// Variable to keep track of the number of words loaded
unsigned int word_count = 0;

// Hashes word to a number using djb2 algorithm
unsigned int hash(const char *word)
{
    unsigned long hash = 5381;
    int c;

    while ((c = tolower(*word++)))
    {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash % N;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // Initialize the hash table to NULL
    for (unsigned int i = 0; i < N; i++)
    {
        table[i] = NULL;
    }

    // Open the dictionary file
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", dictionary);
        return false;
    }

    char word[LENGTH + 1];

    // Read words from the file one at a time
    while (fscanf(file, "%45s", word) != EOF)
    {
        // Create a new node for each word
        node *new_node = malloc(sizeof(node));
        if (new_node == NULL)
        {
            fclose(file);
            unload();
            return false;
        }

        // Copy the word into the node
        strcpy(new_node->word, word);
        new_node->next = NULL;

        // Hash the word to obtain a hash value
        unsigned int index = hash(word);

        // Insert the node into the hash table at the hashed index
        if (table[index] == NULL)
        {
            table[index] = new_node;
        }
        else
        {
            new_node->next = table[index];
            table[index] = new_node;
        }

        // Increment the word count
        word_count++;
    }

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
