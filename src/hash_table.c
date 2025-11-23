#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hash_table.h"

#define HT_PRIME_1 151
#define HT_PRIME_2 157

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

    ht->size = 20;
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
        if (item != NULL) {
            ht_del_item(item);
        }
    }
    free(ht->items);
    free(ht);
}

// computing hash of the key
static int ht_hash(char* s, int a, int m) {
    long hash = 0;
    const int string_len = strlen(s);
    for (int i = 0; i < string_len; i++) {
        hash += (long)pow(a, string_len - (i+1)) * s[i];
        hash = hash % m;
    }

    return (int)hash;
}

// double hashing to handle collisions
static int ht_get_hash(char* s, int num_buckets, int attempt) {
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);

    return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}