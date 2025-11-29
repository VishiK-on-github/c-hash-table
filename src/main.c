#include "hash_table.c"
#include <stdio.h>

int main() {
    hash_table* ht = ht_new();
    printf("%s\n", ht_search(ht, "a"));
    printf("%d\n", ht->count);
    ht_delete(ht, "a");
    ht_del_hash_table(ht);
}