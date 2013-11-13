/* 
Copyright (c) 2013, Martin Majer
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <string.h>

#include "json_object.h"
#include "json_debug.h"

/* Creates a new JSON object with specified type. */
json_object * json_object_new(json_object_type type) {
    JSON_DEBUG_OBJECT_NEW;
    JSON_DEBUG_MALLOC;
    json_object * obj = malloc(sizeof(json_object));
    obj->type = type;
    obj->_private.refs = 1;
    return obj;
}

/* Deletes the JSON object and it's contents recursively. */
void json_object_free(json_object * obj) {
    if (obj->_private.refs > 1) {
        obj->_private.refs--;
    }
    else {
        if (obj->type == JSON_OBJECT_STRING) {
            json_string_free(obj);
        }
        else if (obj->type == JSON_OBJECT_ARRAY) {
            json_array_free_contents(obj);
            json_array_free(obj);
        }
        else if (obj->type == JSON_OBJECT_MAP) {
            json_map_free_contents(obj);
            json_map_free(obj);
        }
        free(obj);
        JSON_DEBUG_FREE;
        JSON_DEBUG_OBJECT_FREE;
    }
}

/* References the object. */
extern json_object * json_object_reference(json_object * obj) {
    obj->_private.refs++;
    return obj;
}


/* Initializes a string. */
void json_string_init_ext(json_object * string, char * str, bool copy) {
    if (copy) {
        JSON_DEBUG_MALLOC;
        char * newMemory = malloc(sizeof(char)*(strlen(str)+1));
        str = strcpy(newMemory, str);
    }
    string->json_string.string = str;
}

/* Frees a string. */
void json_string_free(json_object * string) {
    free(string->json_string.string);
    JSON_DEBUG_FREE;
}



#define JSON_ARRAY_CAPACITY 8


/* Initializes an empty array object. */
void json_array_init(json_object * array) {
    JSON_DEBUG_MALLOC;
    array->json_array.items = malloc(sizeof(json_object*)*JSON_ARRAY_CAPACITY);
    array->json_array.size = 0;
    array->json_array.capacity = JSON_ARRAY_CAPACITY;
}

/* Adds a new item to the array. */
void json_array_add(json_object * array, json_object * newItem) {
    if (array->json_array.size == array->json_array.capacity) {
        array->json_array.capacity *= 2; // double the capacity
        array->json_array.items = realloc(array->json_array.items, sizeof(json_object*)*array->json_array.capacity);
    }
    array->json_array.items[array->json_array.size++] = newItem;
}

/* Returns the size of the array. */
int json_array_size(const json_object * array) {
    return array->json_array.size;
}

/* Returns an object from the array. */
json_object * json_array_get(const json_object * array, int index) {
    if (index < 0 || index >= array->json_array.size) return NULL;
    else return array->json_array.items[index];
}

/* Deletes the array contents. */
void json_array_free_contents(json_object * array) {
    for (int i = 0; i < array->json_array.size; i++) {
        json_object_free(array->json_array.items[i]);
    }
}

/* Deletes the array (without deleting it's contents). */
void json_array_free(json_object * array) {
    free(array->json_array.items);
    JSON_DEBUG_FREE;
}



#define JSON_HASHTABLE_SIZE 7


static unsigned json_hashString(const char * string) {
    unsigned hash = 5381;
    int c;
    while ((c = *string++)) hash = ((hash << 5) + hash) + c;
    return hash;
}


/* Initializes an empty map. */
void json_map_init(json_object * map) {
    map->json_map.size = 0;
    map->json_map.hashtableSize = JSON_HASHTABLE_SIZE;
    JSON_DEBUG_MALLOC;
    map->json_map.hashtable = malloc(sizeof(struct json_map_hashtable_item*)*JSON_HASHTABLE_SIZE);
    for (int i = 0; i < JSON_HASHTABLE_SIZE; i++) {
        map->json_map.hashtable[i] = NULL;
    }
}

/* Doubles the size of the hashtable. */
static void json_map_expandHashtable(json_object * map) {
    int oldSize = map->json_map.hashtableSize;
    int newSize = 2*oldSize + 1;
    
    map->json_map.hashtable = realloc(map->json_map.hashtable, sizeof(struct json_map_hashtable_item*)*newSize);
    map->json_map.hashtableSize = newSize;

    // fill the new portion of the array with zeros
    for (int i = oldSize; i < newSize; i++) {
        map->json_map.hashtable[i] = NULL;
    }

    // check the old portion of the array
    for (int i = 0; i < oldSize; i++) {
        struct json_map_hashtable_item * item = map->json_map.hashtable[i];
        struct json_map_hashtable_item * prevItem = NULL;
        
        while (item != NULL) {
            struct json_map_hashtable_item * nextItem = item->next;
            
            // should be the item at a different position?
            int newIndex = json_hashString(item->key) % newSize;
            if (newIndex != i) {
                // remove from the linked list
                if (prevItem == NULL) map->json_map.hashtable[i] = nextItem;
                else prevItem->next = nextItem;
                
                // insert to the other linked list
                item->next = map->json_map.hashtable[newIndex];
                map->json_map.hashtable[newIndex] = item;
            }
            else prevItem = item;
            
            item = nextItem;
        }
    }
}

/* Adds a value to the map. */
json_object * json_map_put_ext(json_object * map, char * key, json_object * value, bool copyKey) {
    if (copyKey) {
        JSON_DEBUG_MALLOC;
        char * newMemory = malloc(sizeof(char)*(strlen(key)+1));
        key = strcpy(newMemory, key);
    }
    
    if (map->json_map.size >= map->json_map.hashtableSize / 2) {
        json_map_expandHashtable(map);
    }
    
    int index = json_hashString(key) % map->json_map.hashtableSize;
    struct json_map_hashtable_item * existingItem = map->json_map.hashtable[index];
    struct json_map_hashtable_item * currentItem = existingItem;
    
    struct json_map_hashtable_item * collision = NULL;
    
    while (currentItem != NULL) {
        if (strcmp(key, currentItem->key) == 0) { // duplicate key
            collision = currentItem;
            break;
        }
        currentItem = currentItem->next;
    }
    
    if (collision == NULL) {
        JSON_DEBUG_MALLOC;
        struct json_map_hashtable_item * newItem = malloc(sizeof(struct json_map_hashtable_item));
        newItem->key = key;
        newItem->value = value;
        newItem->next = NULL;
        
        newItem->next = existingItem;
        map->json_map.hashtable[index] = newItem;
        map->json_map.size++;
        return NULL;
    }
    else {
        free(collision->key);
        JSON_DEBUG_FREE;
        collision->key = key;
        json_object * obj = collision->value;
        collision->value = value;
        return obj;
    }
}

/* Returns number of items in the map. */
extern int json_map_size(const json_object * map) {
    return map->json_map.size;
}

/* Finds a value in the map. */
json_object * json_map_get(const json_object * map, const char * key) {
    int index = json_hashString(key) % map->json_map.hashtableSize;
    struct json_map_hashtable_item * item = map->json_map.hashtable[index];
    while (item != NULL) {
        if (strcmp(key, item->key) == 0) return item->value;
        item = item->next;
    }
    return NULL;
}

/* Deletes the map contents. */
void json_map_free_contents(json_object * map) {
    for (int i = 0; i < map->json_map.hashtableSize; i++) {
        struct json_map_hashtable_item * item = map->json_map.hashtable[i];
        while (item != NULL) {
            json_object_free(item->value);
            item = item->next;
        }
    }
}

/* Deletes the map (without deleting it's contents). */
void json_map_free(json_object * map) {
    for (int i = 0; i < map->json_map.hashtableSize; i++) {
        struct json_map_hashtable_item * item = map->json_map.hashtable[i];
        while (item != NULL) {
            free(item->key);
            JSON_DEBUG_FREE;
            struct json_map_hashtable_item * nextItem = item->next;
            free(item);
            JSON_DEBUG_FREE;
            item = nextItem;
        }
    }
    free(map->json_map.hashtable);
    JSON_DEBUG_FREE;
}


/* Counts the collisions in a hashmap. */
int json_map_hashtable_collisions(const json_object * map) {
    int collisions = 0;
    for (int i = 0; i < map->json_map.hashtableSize; i++) {
        struct json_map_hashtable_item * item = map->json_map.hashtable[i];
        int itemsInList = 0;
        while (item != NULL) {
            itemsInList++;
            item = item->next;
        }
        if (itemsInList > 1) collisions += itemsInList - 1;
    }
    return collisions;
}

/* Returns the capacity of the hashtable. */
int json_map_hashtable_size(const json_object * map) {
    return map->json_map.hashtableSize;
}


/* Initializes a new map iterator. */
void json_map_iterator_init(json_map_iterator * iterator, const json_object * map) {
    iterator->map = map;
    iterator->key = NULL;
    iterator->value = NULL;
    iterator->hashtablePosition = 0;
    iterator->hashtableItem = NULL;
}

/* Returns the next (key, value) pair. */
bool json_map_iterator_next(json_map_iterator * iterator) {
    while (iterator->hashtableItem == NULL) {
        if (iterator->hashtablePosition == iterator->map->json_map.hashtableSize) return false;
        iterator->hashtableItem = iterator->map->json_map.hashtable[iterator->hashtablePosition++];
    }
    iterator->key = iterator->hashtableItem->key;
    iterator->value = iterator->hashtableItem->value;
    iterator->hashtableItem = iterator->hashtableItem->next;
    return true;
}


/* Initializes a new array iterator. */
void json_array_iterator_init(json_array_iterator * iterator, const json_object * array) {
    iterator->array = array;
    iterator->index = 0;
    iterator->item = NULL;
}

/* Returns the next array item. */
bool json_array_iterator_next(json_array_iterator * iterator) {
    if (iterator->index < json_array_size(iterator->array)) {
        iterator->item = json_array_get(iterator->array, iterator->index++);
        return true;
    }
    else return false;
}

