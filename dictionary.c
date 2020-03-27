// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "dictionary.h"

// Represents number of buckets in a hash table
#define N 26

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Represents a hash table
node *hashtable[N];

// Hashes word to a number between 0 and 25, inclusive, based on its first letter
unsigned int hash(const char *word)
{
    return tolower(word[0]) - 'a';
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Initialize hash table
    for (int i = 0; i < N; i++)
    {
        hashtable[i] = NULL;
    }

    // Open dictionary
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        unload();
        return false;
    }

    // Buffer for a word
    char word[LENGTH + 1];

    // Insert words into hash table
    while (fscanf(file, "%s", word) != EOF)
    {
        // Allocates memmory for a new node
        node *new_node = malloc(sizeof(node));
        if (!new_node)
        {
            unload();
            return false;
        }

        //Stores value from hashed word
        unsigned int hashKey = hash(word);

        //Initializes the node (word, null), and adds it to the hash table at the end of the linked list
        strcpy (new_node->word, word);
        new_node->next = NULL;

        if (hashtable[hashKey] != NULL)
        {
            for (node *ptr = hashtable[hashKey]; ptr != NULL; ptr = ptr->next)
            {
                if (!ptr->next)
                {
                    ptr->next = new_node;
                    break;
                }
            }
        }
        else
        {
            hashtable[hashKey] = new_node;
        }
    }

    // Close dictionary
    fclose(file);

    // Indicate success
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    // Stores number of words and number of NULL values
    int numWords = 0;
    int numNull = 0;

    // Counts the number of elements in the whole hash table, if there's only NULL values, it returns 0, else, the number of words.
    for(int i = 0; i < N; i++)
    {
        node *cursor = hashtable[i];

        if (hashtable[i] == NULL)
        {
            numNull++;
        }

        while (cursor != NULL)
        {
            numWords++;
            cursor = cursor->next;
        }
    }

    if (numNull == 26)
    {
        return 0;
    }
    else
    {
        return numWords;
    }
}

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    node *cursor = hashtable[hash(word)];

    if (!cursor)
    {
        return false;
    }

    else
    {
        while(cursor != NULL)
        {
            if (strcasecmp (cursor->word, word) == 0)
            {
                return true;
            }
            else
            {
                cursor = cursor->next;
            }
        }
    }
return false;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    int numNull = 0;

    for(int i = 0; i < N; i++)
    {
        node *cursor = hashtable[i];

        if (hashtable[i] == NULL)
        {
            numNull++;
        }

        else
        {
            while (cursor != NULL)
            {
                node *tmp = cursor->next;
                free(cursor);
                cursor = tmp;
            }
        }
    }

    if (numNull == 26)
    {
        return false;
    }

    else
    {
        return true;
    }
}
