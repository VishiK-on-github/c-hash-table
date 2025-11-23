typedef struct {
    char* key;
    char* value;
} hash_item;

typedef struct {
    int size;
    int count;
    hash_item** items; // TODO: what is this?
} hash_table;

// hash table functions
void ht_insert(hash_table* ht, const char* key, const char* value);
char* ht_search(hash_table* ht, const char* key);
void ht_delete(hash_table* ht, const char* key);