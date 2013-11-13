/* 
 * File:   json_object.h
 * Author: Martin
 *
 * Created on 8. listopad 2013, 15:35
 */

#ifndef JSON_OBJECT_H
#define	JSON_OBJECT_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* JSON object type. */
typedef enum JSON_OBJECT_TYPE {
    JSON_OBJECT_NULL = 0,
    JSON_OBJECT_INT,
    JSON_OBJECT_BOOL,
    JSON_OBJECT_FLOAT,
    JSON_OBJECT_STRING,
    JSON_OBJECT_ARRAY,
    JSON_OBJECT_MAP
} json_object_type;

#define JSON_TYPE json_object_type type

struct json_object_private { JSON_TYPE; int refs; };

struct json_int {
    struct json_object_private _p;
    int value;
};

struct json_bool {
    struct json_object_private _p;
    bool value;
};

struct json_float {
    struct json_object_private _p;
    float value;
};

struct json_string {
    struct json_object_private _p;
    char * string;
};

struct json_array {
    struct json_object_private _p;
    union JSON_OBJECT ** items;
    int size;
    int capacity;
};

struct json_map_hashtable_item;

struct json_map {
    struct json_object_private _p;
    int size;
    struct json_map_hashtable_item ** hashtable;
    int hashtableSize;
};

struct json_map_hashtable_item {
    char * key;
    union JSON_OBJECT * value;
    struct json_map_hashtable_item * next;
};

/* JSON object union. */
typedef union JSON_OBJECT {
    JSON_TYPE;
    struct json_int json_int;
    struct json_bool json_bool;
    struct json_float json_float;
    struct json_string json_string;
    struct json_array json_array;
    struct json_map json_map;
    struct json_object_private _private;
} json_object;



typedef struct JSON_MAP_ITERATOR {
    const json_object * map;
    char * key;
    union JSON_OBJECT * value;
    int hashtablePosition;
    struct json_map_hashtable_item * hashtableItem;
} json_map_iterator;


typedef struct JSON_ARRAY_ITERATOR {
    const json_object * array;
    union JSON_OBJECT * item;
    int index;
} json_array_iterator;


/* Creates a new JSON object with specified type. */
extern json_object * json_object_new(json_object_type type);


/* Deletes the JSON object and it's contents recursively. */
extern void json_object_free(json_object * obj);


/* Makes a reference to the object. */
extern json_object * json_object_reference(json_object * obj);



/* Returns the integer value. */
static inline int json_int_value(const json_object * obj) { return obj->json_int.value; }

/* Returns the boolean value. */
static inline bool json_bool_value(const json_object * obj) { return obj->json_bool.value; }

/* Returns the float value. */
static inline float json_float_value(const json_object * obj) { return obj->json_float.value; }


/* Initializes a string. */
extern void json_string_init_ext(json_object * string, char * str, bool copy);

/* Initializes a string. */
static inline void json_string_init(json_object * string, char * str) {
    json_string_init_ext(string, str, true);
}

/* Returns the string value. */
static inline char * json_string_value(const json_object * string) { return string->json_string.string; }

/* Frees the string. */
extern void json_string_free(json_object * string);


/* Initializes an empty array object. */
extern void json_array_init(json_object * array);

/* Adds a new item to the array. */
extern void json_array_add(json_object * array, json_object * newItem);

/* Returns the size of the array. */
extern int json_array_size(const json_object * array);

/* Returns an object from the array. */
extern json_object * json_array_get(const json_object * array, int index);

/* Deletes the array contents. */
extern void json_array_free_contents(json_object * array);

/* Deletes the array (without deleting it's contents). */
extern void json_array_free(json_object * array);


/* Initializes an empty map. */
extern void json_map_init(json_object * map);

/* Adds a value to the map. */
extern json_object * json_map_put_ext(json_object * map, char * key, json_object * value, bool copyKey);

/* Adds a value to the map. */
static inline json_object * json_map_put(json_object * map, const char * key, json_object * value) {
    return json_map_put_ext(map, (char*)key, value, true);
}

/* Returns number of items in the map. */
extern int json_map_size(const json_object * map);

/* Finds a value in the map. */
extern json_object * json_map_get(const json_object * map, const char * key);

/* Deletes the map contents. */
extern void json_map_free_contents(json_object * map);

/* Deletes the map (without deleting it's contents). */
extern void json_map_free(json_object * map);


/* Counts the collisions in a hashmap. */
extern int json_map_hashtable_collisions(const json_object * map);

/* Returns the capacity of the hashtable. */
extern int json_map_hashtable_size(const json_object * map);


/* Initializes a new map iterator. */
extern void json_map_iterator_init(json_map_iterator * iterator, const json_object * map);

/* Returns the next (key, value) pair. */
extern bool json_map_iterator_next(json_map_iterator * iterator);


/* Initializes a new array iterator. */
extern void json_array_iterator_init(json_array_iterator * iterator, const json_object * array);

/* Returns the next array item. */
extern bool json_array_iterator_next(json_array_iterator * iterator);



static inline json_object * json_null() {
    return json_object_new(JSON_OBJECT_NULL);
}

static inline json_object * json_int(int value) {
    json_object * obj = json_object_new(JSON_OBJECT_INT);
    obj->json_int.value = value;
    return obj;
}

static inline json_object * json_bool(bool value) {
    json_object * obj = json_object_new(JSON_OBJECT_BOOL);
    obj->json_bool.value = value;
    return obj;
}

static inline json_object * json_float(float value) {
    json_object * obj = json_object_new(JSON_OBJECT_FLOAT);
    obj->json_float.value = value;
    return obj;
}

static inline json_object * json_string_ref(char * string) {
    json_object * obj = json_object_new(JSON_OBJECT_STRING);
    json_string_init_ext(obj, string, false);
    return obj;
}

static inline json_object * json_string(const char * string) {
    json_object * obj = json_object_new(JSON_OBJECT_STRING);
    json_string_init_ext(obj, (char*)string, true);
    return obj;
}

static inline json_object * json_array() {
    json_object * obj = json_object_new(JSON_OBJECT_ARRAY);
    json_array_init(obj);
    return obj;
}

static inline json_object * json_map() {
    json_object * obj = json_object_new(JSON_OBJECT_MAP);
    json_map_init(obj);
    return obj;
}

#ifdef	__cplusplus
}
#endif

#endif	/* JSON_OBJECT_H */

