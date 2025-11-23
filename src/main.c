#include "hash_table.c"

int main() {
    hash_table* ht = ht_new();
    ht_del_hash_table(ht);
}