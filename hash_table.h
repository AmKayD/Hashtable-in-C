#ifndef HASH_TABLE_H
#define HASH_TABLE_H

typedef struct {
    char* key;
    char* value;
} ht_item;

typedef struct{
    //int base_size;
    int size_index;
    int size;
    int count;
    ht_item** items;
} ht_hash_table;

//static ht_item* ht_new_item(const char* key, const char* value);
ht_hash_table* ht_new();
//static void ht_del_item(ht_item* i);
void ht_del_hash_table(ht_hash_table* ht);
// static int get_hash(const char* string, const int num_buckets, const int attempts);
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table *ht, const char* key);
#endif

