#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "hash.h"

hashtable_t *
create_dictionary(int size)
{
	if(size <= 0) return NULL;

	hashtable_t *hashtable;
	hashtable = calloc(1, sizeof(hashtable_t));
	hashtable->size = size;

	hashtable->table = calloc(size, sizeof(entry_t *));

	return hashtable;
}


int
hash(char *key, hashtable_t *hashtable)
{
        unsigned int hashval;

        for(hashval = 0; *key != '\0'; key++)
                hashval += *key + 31 * hashval;
	
        return hashval % hashtable->size;
}


void
put_hash(hashtable_t *hashtable, char *key, int value)
{
	int index = hash(key, hashtable);
	entry_t *new_entry = NULL;
	entry_t *next = NULL;
	entry_t *last = NULL;
	key_w *new_key = NULL;

	next = hashtable->table[index];
	while(next != NULL && next->key != NULL && strcmp(next->key, key) > 0)
	{
		last = next;
		next = next->next;
	}


	/* we found an existing pair */
	if(next != NULL && next->key != NULL && strcmp(next->key, key)==0)
	{
		next->value = value;
	}
	else
	{
		/* Else we need to insert a new one */
		new_entry = calloc(1, sizeof(entry_t));
		new_entry->key = strdup(key);
		new_entry->value = value;
			
		if(next == hashtable->table[index])
		{
			new_entry->next = next;
			hashtable->table[index] = new_entry;	
		}
		else if(next == NULL)
		{
			last->next = new_entry;
		}	
		else
		{
			new_entry->next = next;
			last->next = new_entry;
		}
		
		new_key = malloc(sizeof(key_w));
		new_key->key = strdup(key);
		new_key->next = hashtable->keys;
		hashtable->keys = new_key;
	}
}

int 
hashtable_get_value(hashtable_t *hashtable, char *key)
{
	int index = hash(key, hashtable);
	
	entry_t *tmp;
	tmp = hashtable->table[index];
	
	
	while(tmp != NULL && tmp->key != NULL && strcmp(key, tmp->key) != 0)
	{
		tmp = tmp->next;
	}
	

	if(tmp == NULL || tmp->key == NULL || strcmp(key, tmp->key) != 0)
		return 0;
	else{ return tmp->value;}
}
	

/*
int
main(int argc, char **argv)
{
	hashtable_t *hashtable = create_dictionary(3);
	put_hash(hashtable, "da", 5);
	put_hash(hashtable, "david", 10);

	put_hash(hashtable, "a", 1);
	put_hash(hashtable, "b", 2);
	put_hash(hashtable, "c", 3);
	put_hash(hashtable, "d", 4);

		
	printf("da %d \n", hashtable_get_value(hashtable, "da"));
	printf("david %d \n", hashtable_get_value(hashtable, "david"));
	printf("a %d \n", hashtable_get_value(hashtable, "a"));
	printf("b %d \n", hashtable_get_value(hashtable, "b"));
	printf("c %d \n", hashtable_get_value(hashtable, "c"));
	printf("d %d \n", hashtable_get_value(hashtable, "d"));

	
	put_hash(hashtable, "a", 4);
	put_hash(hashtable, "b", 3);
	put_hash(hashtable, "c", 2);
	put_hash(hashtable, "d", 1);

		
	printf("a %d \n", hashtable_get_value(hashtable, "a"));
	printf("b %d \n", hashtable_get_value(hashtable, "b"));
	printf("c %d \n", hashtable_get_value(hashtable, "c"));
	printf("d %d \n", hashtable_get_value(hashtable, "d"));

	key_w *key = hashtable->keys;
	while(key != NULL)
	{
		printf("%s \n",  key->key);
		key = key->next;
	}
}*/
