#include<stdlib.h>
#include<string.h>
#include<math.h>

#include "hash_table.h"
#include "prime.h"
#include "xmalloc.h"

const int HT_PRIME_1 = 151;
const int HT_PRIME_2 = 163;

const int HT_INITIAL_BASE_SIZE = 500;

static ht_item HT_DELETED_ITEM = {NULL, NULL};

static ht_item* ht_new_item(const char* key, const char* value){

    ht_item* item = xmalloc(sizeof(ht_item));
    item->key = strdup(key);
    item->value = strdup(value);
    return item;
}

static void ht_del_item(ht_item* i){
    free(i->key);
    free(i->value);
    free(i);
}

ht_hash_table* ht_new_sized(const int size_index){

    ht_hash_table *ht = xmalloc(sizeof(ht_hash_table));

    ht->size_index = size_index;

    const int base_size = HT_INITIAL_BASE_SIZE << ht->size_index;
    ht->size = next_prime(base_size);
    ht->count = 0;
    ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));

    return ht;
}

ht_hash_table* ht_new(){

    // ht_hash_table* ht = (ht_hash_table*)malloc(sizeof(ht_hash_table));

    // ht->size = 53;
    // ht->count = 0;
    // ht->items = calloc((size_t)ht->size, sizeof(ht_item*)); 
    // are you sure?!! about the pointer


    return ht_new_sized(0);
}



void ht_del_hash_table(ht_hash_table* ht){
    for(int i = 0; i < ht->size; i++){
        ht_item* item = ht->items[i];
        if(item != NULL && item != &HT_DELETED_ITEM)
            ht_del_item(item);
    }

    free(ht->items);
    free(ht);
}

static void ht_resize(ht_hash_table* ht, const int direction){
    const int new_size_index = ht->size_index + direction;
    if(new_size_index < 0) 
        return;
        //dont resize

    ht_hash_table* new_ht = ht_new_sized(new_size_index);
    for(int i = 0; i < ht->size; i++){
        ht_item* item = ht->items[i];
        if(item != NULL && item != &HT_DELETED_ITEM){
            ht_insert(new_ht, item->key, item->value);
        }
    }

    ht->size_index = new_ht->size_index;
    ht->count = new_ht->count;

    const int temp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = temp_size;

    ht_item** temp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = temp_items;

    ht_del_hash_table(new_ht);

}

// static void ht_resize_up(ht_hash_table* ht){
//     const int new_base_size = ht->base_size * 2;
//     ht_resize(ht, new_base_size);
// }

// static void ht_resize_down(ht_hash_table* ht){
//     const int new_base_size = ht->base_size / 2;
//     ht_resize(ht, new_base_size);
// }

static int ht_hash(const char* string, const int prime, const int buckets){
    long hash = 0;
    const int len_s = strlen(string);
    for(int i = 0; i < len_s; i++){
        hash += (long)pow(prime, len_s - (i+1)) * string[i];
        hash = hash % buckets;
        
    }
    return (int)hash;
}

static int ht_get_hash(const char* string, const int num_buckets, const int attempts){
    const int hash_a = ht_hash(string, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(string, HT_PRIME_2, num_buckets);

    int hash = (hash_a + (attempts * (hash_b+1))) % num_buckets;
    return hash;
}


void ht_insert(ht_hash_table* ht, const char* key, const char* value){

    const int load = ht->count * 100 / ht->size;
    if(load > 70){
        ht_resize(ht, 1);
    }

    
    // while(ht->items[index] != NULL && ht->items[index] != &HT_DELETED_ITEM){
    //     index = ht_get_hash(key, ht->size, i);
    //     i++;
    // }
    
    //we need to deal with duplicate keys as well
    ht_item* item = ht_new_item(key, value);
    int index = ht_get_hash(key, ht->size, 0);
    int i = 1;

    ht_item* cur_item = ht->items[index];
    while(cur_item != NULL && cur_item != &HT_DELETED_ITEM){
        if(strcmp(cur_item->key, item->key) == 0){
            ht_del_item(cur_item);
            ht->items[index] = item;
            return;
        }
        index = ht_get_hash(item->key, ht->size, i);
        cur_item = ht->items[index];
        i++;
    }
    
    //found an empty bucket
    ht->items[index] = item;
    ht->count++;
}

char* ht_search(ht_hash_table* ht, const char* key){
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
        if(item != &HT_DELETED_ITEM){
            if (strcmp(item->key, key) == 0) {
                return item->value;
            }
        }
        
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    } 
    return NULL;
}

void ht_delete(ht_hash_table *ht, const char* key){

    const int load = ht->count * 100 / ht->size;
    if(load < 10){
        ht_resize(ht, -1);
    }


    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    while (item != NULL && item != &HT_DELETED_ITEM) {
        if (strcmp(item->key, key) == 0) {
            ht_del_item(item);
            ht->items[index] = &HT_DELETED_ITEM;
        }
        index = ht_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    } 
    ht->count--;
}






