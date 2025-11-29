#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hash_table.h"
#include "prime.c"

#define HT_PRIME_1 163
#define HT_PRIME_2 157
#define HT_INITIAL_BASE_SIZE 53

static hash_item HT_DELETED_ITEM = {NULL, NULL};

// init new hash table item
static hash_item* ht_new_item(const char* k, const char* v) {
    hash_item* hi = malloc(sizeof(hash_item));

    // copy the k, v string from function params
    // and store them in the instance of hash_item struct
    hi->key = strdup(k);
    hi->value = strdup(v);

    return hi;
}

// init new hash table
hash_table* ht_new() {
    hash_table* ht = malloc(sizeof(hash_table));

    ht->size = 53;
    ht->count = 0;
    
    // allocate an array of with ht->size records
    // and each records has size hash_item*
    ht->items = calloc((size_t)ht->size, sizeof(hash_item*));

    return ht;
}

// deleting hash table item
static void ht_del_item(hash_item* hi) {
    free(hi->key);
    free(hi->value);
    free(hi);
} 

// delete item from hash table
void ht_del_hash_table(hash_table* ht) {
    for (int i = 0; i < ht->size; i++) {
        hash_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}

// computing hash of the key
static int ht_hash(const char* s, int a, int m) {
    long hash = 0;
    const int string_len = strlen(s);
    for (int i = 0; i < string_len; i++) {
        hash += (long)pow(a, string_len - (i+1)) * s[i];
    }

    hash = hash % m;

    return (int)hash;
}

// double hashing to handle collisions
static int ht_get_hash(const char* s, int num_buckets, int attempt) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);

    if (hash_b % num_buckets) {
        hash_b = 1;
    }

    return (hash_a + (attempt * hash_b)) % num_buckets;
}

static hash_table* hash_table_new_size(const int base_size) {
    hash_table* ht = malloc(sizeof(hash_table));
    ht->base_size = base_size;

    ht->size = next_prime(ht->base_size);

    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(hash_item*));
    return ht;
}

hash_table* hash_table_new(hash_table* ht) {
    return hash_table_new_size(HT_INITIAL_BASE_SIZE);
}

static void hash_table_resize(hash_table* ht, int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        base_size = HT_INITIAL_BASE_SIZE;
    }

    hash_table* new_ht = hash_table_new_size(base_size);

    for (int i = 0; i < ht->size; i++) {
        hash_item* hi = ht->items[i];

        if (hi != NULL && hi != &HT_DELETED_ITEM) {
            ht_insert(new_ht, hi->key, hi->value);
        }
    }

    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    const int temp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = temp_size;

    hash_item** temp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = temp_items;

    ht_del_hash_table(new_ht);
}

static void ht_resize_up(hash_table* ht) {
    const int new_size = ht->base_size * 2;
    hash_table_resize(ht, new_size);
}

static void ht_resize_down(hash_table* ht) {
    const int new_size = ht->base_size / 2;
    hash_table_resize(ht, new_size);
}

// search fn def
char* ht_search(hash_table* ht, const char* key) {
    // compute hash from key
    int key_index = ht_get_hash(key, ht->size, 0);

    // use hash to look for entry in hash table
    hash_item* hi = ht->items[key_index];
    int i = 1;

    // return value else NULL
    while (hi != NULL) {
        if (hi != &HT_DELETED_ITEM) {
            if (strcmp(hi->key, key) == 0) {
                return hi->value;
            }
        }
        key_index = ht_get_hash(key, ht->size, i);
        hi = ht->items[key_index];
        i++; 
    }
    return NULL;
}

// delete fn def
void ht_delete(hash_table* ht, const char* key) {

    const int load = ht->count * 100 / ht->size;
    if (load < 30) {
        ht_resize_down(ht);
    }

    // find index
    int key_index = ht_get_hash(key, ht->size, 0);
    hash_item* hi = ht->items[key_index];
    int i = 1;

    // delete if available
    while (hi != NULL) {

        // we dont want to check if an index where hash item 
        // has been deleted and marked with null struct
        if (hi != &HT_DELETED_ITEM) {
            if (strcmp(hi->key, key) == 0) {
                ht_del_item(hi);
                // we set value to a sentinel value to prevent
                // breaking of collision chain
                ht->items[key_index] = &HT_DELETED_ITEM;
                ht->count--;
                return;
            }
        }
        key_index = ht_get_hash(key, ht->size, i);
        hi = ht->items[key_index];
        i++;
    }
}

// insert fn def
void ht_insert(hash_table* ht, const char* key, const char* value) {

    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        ht_resize_up(ht);
    }

    // init new item
    hash_item* hi = ht_new_item(key, value);

    // compute hash from key
    int key_index = ht_get_hash(key, ht->size, 0);
    hash_item* curr_hi = ht->items[key_index];
    int i = 1;

    while (curr_hi != NULL) {
        if (curr_hi != &HT_DELETED_ITEM) {
            if (strcmp(curr_hi->key, key) == 0) {
                ht_del_item(curr_hi);
                ht->items[key_index] = hi;
                return;
            }
        }
        key_index = ht_get_hash(key, ht->size, i);
        curr_hi = ht->items[key_index];
        i++;
    }

    ht->items[key_index] = hi;
    ht->count++;
}