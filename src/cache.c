#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "cache.h"

/**
 * Allocate a cache entry
 */
struct cache_entry *alloc_entry(char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    struct cache_entry *ce = malloc(sizeof(struct cache_entry));

    //the malloc and strcpy is to ensure that you have access to path in case the original is no longer in memory
    ce->path = malloc(strlen(path) + 1);
    strcpy(ce->path, path);

    ce->content_type = malloc(strlen(content_type) + 1);
    strcpy(ce->content_type, content_type);

    ce->content_length = content_length;

    ce->content = malloc(strlen(content) + 1);
    strcpy(ce->content, content);

    ce->prev = ce->next = NULL;

    return ce;
}

/**
 * Deallocate a cache entry
 */
void free_entry(struct cache_entry *entry)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    free(entry->path);
    free(entry->content_type);
    free(entry->content);
    free(entry);
}

/**
 * Insert a cache entry at the head of the linked list
 */
void dllist_insert_head(struct cache *cache, struct cache_entry *ce)
{
    // Insert at the head of the list
    if (cache->head == NULL)
    {
        cache->head = cache->tail = ce;
        ce->prev = ce->next = NULL;
    }
    else
    {
        cache->head->prev = ce;
        ce->next = cache->head;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Move a cache entry to the head of the list
 */
void dllist_move_to_head(struct cache *cache, struct cache_entry *ce)
{
    if (ce != cache->head)
    {
        if (ce == cache->tail)
        {
            // We're the tail
            cache->tail = ce->prev;
            cache->tail->next = NULL;
        }
        else
        {
            // We're neither the head nor the tail
            ce->prev->next = ce->next;
            ce->next->prev = ce->prev;
        }

        ce->next = cache->head;
        cache->head->prev = ce;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Removes the tail from the list and returns it
 * 
 * NOTE: does not deallocate the tail
 */
struct cache_entry *dllist_remove_tail(struct cache *cache)
{
    struct cache_entry *oldtail = cache->tail;

    cache->tail = oldtail->prev;
    cache->tail->next = NULL;

    cache->cur_size--;

    return oldtail;
}

/**
 * Create a new cache
 * 
 * max_size: maximum number of entries in the cache
 * hashsize: hashtable size (0 for default)
 */
struct cache *cache_create(int max_size, int hashsize)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////
    // Allocate space in memory for the cache
    struct cache *cache = malloc(sizeof *cache);

    // Pass NULL to use the default hash function
    cache->index = hashtable_create(hashsize, NULL);

    // Set current head and tail to NULL
    cache->head = cache->tail = NULL;

    cache->max_size = max_size;
    cache->cur_size = 0;

    return cache;
}

void cache_free(struct cache *cache)
{
    struct cache_entry *cur_entry = cache->head;

    hashtable_destroy(cache->index);

    while (cur_entry != NULL)
    {
        struct cache_entry *next_entry = cur_entry->next;

        free_entry(cur_entry);

        cur_entry = next_entry;
    }

    free(cache);
}

/**
 * Store an entry in the cache
 *
 * This will also remove the least-recently-used items as necessary.
 * 
 * NOTE: doesn't check for duplicate cache entries
 */
void cache_put(struct cache *cache, char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    // Allocate a new cache entry with the passed parameters
    struct cache_entry *ce = alloc_entry(path, content_type, content, content_length);

    // Insert the entry at the head of the double-linked list
    dllist_insert_head(cache, ce);

    // Store the entry as well, indexed by the entry's path
    hashtable_put(cache->index, path, ce);

    cache->cur_size += 1;

    // If the cache size is greater than the max size:
    if (cache->cur_size > cache->max_size)
    {

        // Remove the cache entry at the tail fo the linked list
        struct cache_entry *old_ce = dllist_remove_tail(cache);

        // Remove that same entry from the hashtable, using the entry's path and the hashtable_delete function.
        hashtable_delete(cache->index, old_ce->path);

        // Free the cache entry.
        free_entry(old_ce);

        // Ensure the size counter for the number of entries in the cache is correct
        if (cache->cur_size > cache->max_size)
        {
            cache->cur_size -= 1;
        }
    }
}

/**
 * Retrieve an entry from the cache
 */
struct cache_entry *cache_get(struct cache *cache, char *path)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    // Attempt to find the cache entry pointer by path in the hash table

    // if not found, return Null

    if (hashtable_get(cache->index, path) == NULL)
    {

        return NULL;
    }
    else
    {

        // Move the cache entry to the head of the doubly linked list
        dllist_move_to_head(cache, hashtable_get(cache->index, path));

        // Return the cache entry pointer
        return cache->head;
    }
}
