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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "json_tokenizer.h"
#include "json_reader.h"
#include "json_debug.h"
#include "json_object.h"
#include "json.h"

typedef bool (* json_unit_test)(void);

#define JSON_TEST_START_INNER printf("%s... ", __FUNCTION__)
#ifdef JSON_DEBUG
#define JSON_TEST_START { JSON_TEST_START_INNER; json_debug_memblocks = 0; json_debug_objects = 0; }
#else
#define JSON_TEST_START { JSON_TEST_START_INNER; }
#endif
#define JSON_TEST_DONE { printf("OK\n"); return true; }
#define JSON_TEST_FAILED { printf("Failed (%s:%i)\n", __FILE__, __LINE__); return false; }

#define JSON_TEST_ASSERT(condition) if (!(condition)) JSON_TEST_FAILED

#ifdef JSON_DEBUG
#define JSON_MEMBLOCKS_CHECK JSON_TEST_ASSERT(json_debug_memblocks == 0);
#define JSON_OBJECTS_CHECK JSON_TEST_ASSERT(json_debug_objects == 0);
#else
#define JSON_MEMBLOCKS_CHECK
#define JSON_OBJECTS_CHECK
#endif

static int json_unit_tests_passed = 0;
static int json_unit_tests_failed = 0;

/* String reader test. */
static bool test_reader_1(void) {
    JSON_TEST_START;
    char * str = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    json_reader reader = json_reader_string(str);
    int pos = 0;
    int readChar;
    while ((readChar = reader.nextChar(&reader.data))) {
        JSON_TEST_ASSERT(str[pos++] == readChar);
    }
    JSON_TEST_DONE;
}

/* File reader test. */
static bool test_reader_2(void) {
    JSON_TEST_START;
    char * fileContents = "Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua.";
    FILE * stream = fopen("test_files/test_reader_2.txt", "r");
    json_reader reader = json_reader_stream(stream);
    int pos = 0;
    int readChar;
    while ((readChar = reader.nextChar(&reader.data)) != EOF) {
        JSON_TEST_ASSERT(fileContents[pos++] == readChar);
    }
    JSON_TEST_ASSERT(strlen(fileContents) == pos);
    fclose(stream);
    JSON_TEST_DONE;
}

/* Tokenizer - testing empty file (string). */
static bool test_tokenizer_1(void) {
    JSON_TEST_START;
    char * emptyString = "";
    json_tokenizer t;
    json_tokenizer_init(&t, json_reader_string(emptyString));
    bool ok = json_tokenizer_next(&t);
    JSON_TEST_ASSERT(ok);
    JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_EOF);
    JSON_TEST_DONE;
}

/* Tokenizer - testing whitespace characters. */
static bool test_tokenizer_2(void) {
    JSON_TEST_START;
    char * emptyString = " \r\n\t";
    json_tokenizer t;
    json_tokenizer_init(&t, json_reader_string(emptyString));
    bool ok = json_tokenizer_next(&t);
    JSON_TEST_ASSERT(ok);
    JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_EOF);
    JSON_TEST_DONE;
}

/* Tokenizer - testing basic tokens. */
static bool test_tokenizer_3(void) {
    JSON_TEST_START;
    char * json = "{[:,]} { [ : , ] } ";
    json_tokenizer t;
    json_tokenizer_init(&t, json_reader_string(json));
    
    for (int i = 0; i < 12; i++) {
        bool ok = json_tokenizer_next(&t);
        JSON_TEST_ASSERT(ok);
        JSON_TEST_ASSERT(t.token.type != JSON_TOKEN_EOF);
        switch (i) {
        case 0: case 6:
            JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_BRACE_OPENING);
            break;
        case 1: case 7:
            JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_BRACKET_OPENING);
            break;
        case 2: case 8:
            JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_COLON);
            break;
        case 3: case 9:
            JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_COMMA);
            break;
        case 4: case 10:
            JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_BRACKET_CLOSING);
            break;
        case 5: case 11:
            JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_BRACE_CLOSING);
            break;
        }
    }
    
    JSON_TEST_DONE;
    
}

/* Tokenizer - testing symbols, null and boolean values. */
static bool test_tokenizer_4(void) {
    JSON_TEST_START;
    char * json = "true false null _true False NULL";
    json_tokenizer t;
    json_tokenizer_init(&t, json_reader_string(json));
    
    for (int i = 0; i < 6; i++) {
        bool ok = json_tokenizer_next(&t);
        JSON_TEST_ASSERT(ok);
        JSON_TEST_ASSERT(t.token.type != JSON_TOKEN_EOF);
        switch (i) {
        case 0:
            JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_BOOL);
            JSON_TEST_ASSERT(t.token.data.boolValue == true);
            break;
        case 1:
            JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_BOOL);
            JSON_TEST_ASSERT(t.token.data.boolValue == false);
            break;
        case 2:
            JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_NULL);
            break;
        case 3:
            JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_SYMBOL);
            JSON_TEST_ASSERT(strcmp(t.token.data.string.data, "_true") == 0);
            json_token_free(&t.token);
            break;
        case 4:
            JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_SYMBOL);
            JSON_TEST_ASSERT(strcmp(t.token.data.string.data, "False") == 0);
            json_token_free(&t.token);
            break;
        case 5:
            JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_SYMBOL);
            JSON_TEST_ASSERT(strcmp(t.token.data.string.data, "NULL") == 0);
            json_token_free(&t.token);
            break;
        }
    }
    
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Tokenizer - testing data hijacking and unexpected character. */
static bool test_tokenizer_5(void) {
    JSON_TEST_START;
    char * json = "hijack me!";
    json_tokenizer t;
    json_tokenizer_init(&t, json_reader_string(json));
    bool ok;
    
    ok = json_tokenizer_next(&t);
    JSON_TEST_ASSERT(ok);
    JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_SYMBOL);
    JSON_TEST_ASSERT(strcmp(t.token.data.string.data, "hijack") == 0);
    char * string = json_token_hijack(&t.token);
    JSON_TEST_ASSERT(strcmp(string, "hijack") == 0);
    json_token_free(&t.token);
    free(string); JSON_DEBUG_FREE;
    
    ok = json_tokenizer_next(&t); // should fail on !
    JSON_TEST_ASSERT(ok == false);
    
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Tokenizer - integers test. */
static bool test_tokenizer_6(void) {
    JSON_TEST_START;
    char * json = "0 -1 128 -256";
    json_tokenizer t;
    json_tokenizer_init(&t, json_reader_string(json));
    
    for (int i = 0; i < 4; i++) {
        bool ok = json_tokenizer_next(&t);
        JSON_TEST_ASSERT(ok);
        JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_INTEGER);
        switch (i) {
        case 0:
            JSON_TEST_ASSERT(t.token.data.intValue == 0);
            break;
        case 1:
            JSON_TEST_ASSERT(t.token.data.intValue == -1);
            break;
        case 2:
            JSON_TEST_ASSERT(t.token.data.intValue == 128);
            break;
        case 3:
            JSON_TEST_ASSERT(t.token.data.intValue == -256);
            break;
        }
    }
    
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Tokenizer - floats test. */
static bool test_tokenizer_7(void) {
    JSON_TEST_START;
    char * json = "0.0 -1.0 -0.1 2e4 -4e8 8e+2 8e-2 1.234e-5 -9.876e+5";
    json_tokenizer t;
    json_tokenizer_init(&t, json_reader_string(json));
    
    for (int i = 0; i < 9; i++) {
        bool ok = json_tokenizer_next(&t);
        JSON_TEST_ASSERT(ok);
        JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_FLOAT);
        switch (i) {
        case 0:
            JSON_TEST_ASSERT(t.token.data.floatValue == (float)atof("0.0"));
            break;
        case 1:
            JSON_TEST_ASSERT(t.token.data.floatValue == (float)atof("-1.0"));
            break;
        case 2:
            JSON_TEST_ASSERT(t.token.data.floatValue == (float)atof("-0.1"));
            break;
        case 3:
            JSON_TEST_ASSERT(t.token.data.floatValue == (float)atof("2e4"));
            break;
        case 4:
            JSON_TEST_ASSERT(t.token.data.floatValue == (float)atof("-4e8"));
            break;
        case 5:
            JSON_TEST_ASSERT(t.token.data.floatValue == (float)atof("8e+2"));
            break;
        case 6:
            JSON_TEST_ASSERT(t.token.data.floatValue == (float)atof("8e-2"));
            break;
        case 7:
            JSON_TEST_ASSERT(t.token.data.floatValue == (float)atof("1.234e-5"));
            break;
        case 8:
            JSON_TEST_ASSERT(t.token.data.floatValue == (float)atof("-9.876e+5"));
            break;
        }
    }
    
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Tokenizer - invalid numbers test. */
static bool test_tokenizer_8(void) {
    JSON_TEST_START;
    char * json[] = {
        "08", "0xABCDEF", "1+1", "150000000000L", "0.123456789f", "-number", "4HoY"
    };
    json_tokenizer t;
    
    for (int i = 0; i < 7; i++) {
        json_tokenizer_init(&t, json_reader_string(json[i]));
        bool ok = json_tokenizer_next(&t);
        JSON_TEST_ASSERT(ok == false);
    }
    
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Tokenizer - string tests. */
static bool test_tokenizer_9(void) {
    JSON_TEST_START;
    char * json = "\"Hello world\" \"\" \"\\r\\n\\t\\\\\\\"\" \"epic fail string";
    json_tokenizer t;
    json_tokenizer_init(&t, json_reader_string(json));
    
    for (int i = 0; i < 4; i++) {
        bool ok = json_tokenizer_next(&t);
        if (i < 3) {
            JSON_TEST_ASSERT(ok);
        }
        else {
            JSON_TEST_ASSERT(ok == false); 
            break;
        }
        
        JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_STRING);
        switch (i) {
        case 0:
            JSON_TEST_ASSERT(strcmp(t.token.data.string.data, "Hello world") == 0);
            break;
        case 1:
            JSON_TEST_ASSERT(strcmp(t.token.data.string.data, "") == 0);
            break;
        case 2:
            JSON_TEST_ASSERT(strcmp(t.token.data.string.data, "\r\n\t\\\"") == 0);
            break;
        }
        
        json_token_free(&t.token);
    }
    
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Tokenizer - unicode sequences test. */
static bool test_tokenizer_10(void) {
    JSON_TEST_START;
    
    char * json = "\"\\u0041\\u0048\\u004f\\u004A p\\u0159\\u00EDli\\u0161 \\u017elu\\u0165ou\\u010dk\\u00FD k\\u016F\\u0148 \\u0429\\u042e\\u05D0\\u05E9\\u2EE3\\u30B7\"";
    char * str = "AHOJ příliš žluťoučký kůň ЩЮאש⻣シ"; // utf-8 string
    
    json_tokenizer t;
    json_tokenizer_init(&t, json_reader_string(json));
    
    bool ok = json_tokenizer_next(&t);
    JSON_TEST_ASSERT(ok);
    JSON_TEST_ASSERT(t.token.type == JSON_TOKEN_STRING);
    JSON_TEST_ASSERT(strcmp(t.token.data.string.data, str) == 0);
    
    json_token_free(&t.token);
    
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Basic JSON type test. */
static bool test_object_1(void) {
    JSON_TEST_START;
    
    json_object * obj;
    
    obj = json_null();
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_NULL);
    json_object_free(obj);
    
    obj = json_int(128);
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_INT);
    JSON_TEST_ASSERT(json_int_value(obj) == 128);
    json_object_free(obj);
    
    obj = json_bool(true);
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_BOOL);
    JSON_TEST_ASSERT(json_bool_value(obj) == true);
    json_object_free(obj);
    
    obj = json_float(128.0);
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_FLOAT);
    JSON_TEST_ASSERT(json_float_value(obj) == 128.0);
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Strings test. */
static bool test_object_2(void) {
    JSON_TEST_START;
    
    json_object * obj;
    
    obj = json_string("Hello world");
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_STRING);
    JSON_TEST_ASSERT(strcmp(json_string_value(obj), "Hello world") == 0);
    json_object_free(obj);
    
    char * str = malloc(sizeof(char)*16); JSON_DEBUG_MALLOC;
    strcpy(str, "Steal me.");
    obj = json_string_ref(str);
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_STRING);
    JSON_TEST_ASSERT(strcmp(json_string_value(obj), "Steal me.") == 0);
    JSON_TEST_ASSERT(obj->json_string.string == str);
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Array test. */
static bool test_object_3(void) {
    JSON_TEST_START;
    
    json_object * array = json_array();
    json_array_add(array, json_int(10));
    json_array_add(array, json_bool(false));
    json_array_add(array, json_null());
    json_array_add(array, json_int(20));
    
    JSON_TEST_ASSERT(json_array_size(array) == 4);
    
    json_object * item;
    
    item = json_array_get(array, 0);
    JSON_TEST_ASSERT(item->type == JSON_OBJECT_INT);
    JSON_TEST_ASSERT(json_int_value(item) == 10);
    
    item = json_array_get(array, 1);
    JSON_TEST_ASSERT(item->type == JSON_OBJECT_BOOL);
    JSON_TEST_ASSERT(json_bool_value(item) == false);
    
    item = json_array_get(array, 2);
    JSON_TEST_ASSERT(item->type == JSON_OBJECT_NULL);
    
    item = json_array_get(array, 3);
    JSON_TEST_ASSERT(item->type == JSON_OBJECT_INT);
    JSON_TEST_ASSERT(json_int_value(item) == 20);
    
    item = json_array_get(array, -1);
    JSON_TEST_ASSERT(item == NULL);
    
    item = json_array_get(array, 4);
    JSON_TEST_ASSERT(item == NULL);
    
    json_object_free(array);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Array - dynamic resizing test. */
static bool test_object_4(void) {
    JSON_TEST_START;
    
    for (int size = 1; size <= 1024; size++) {
        json_object * array = json_array();
        for (int i = 0; i < size; i++) {
            json_array_add(array, json_int(i));
        }
        JSON_TEST_ASSERT(json_array_size(array) == size);
        for (int i = 0; i < size; i++) {
            JSON_TEST_ASSERT(json_int_value(json_array_get(array, i)) == i);
        }
        json_object_free(array);
    }
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Array - memory management test. */
static bool test_object_5(void) {
    JSON_TEST_START;
    
    json_object * array = json_array();
    for (int i = 0; i < 100; i++) {
        char * str = malloc(sizeof(str) * 8); JSON_DEBUG_MALLOC;
        sprintf(str, "%i", i*i);
        json_array_add(array, json_string_ref(str));
    }
    JSON_TEST_ASSERT(json_array_size(array) == 100);
    json_object_free(array);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Hashmap - basic test. */
static bool test_object_6(void) {
    JSON_TEST_START;
    
    json_object * map = json_map();
    char * words[] = {
        "one", "two", "three", "four", "five", "lorem", "ipsum", "dolor", "sit", "amet", "Astar", "Seran", "příliš", "žluťoučký", "kůň", "úpěl", "ďábelské", "ódy", NULL
    };
    
    char ** currentWordPtr; int i;
    
    currentWordPtr = words; i = 0;
    while (*currentWordPtr != NULL) {
        json_map_put(map, *currentWordPtr++, json_int(i++));
    }
    
    JSON_TEST_ASSERT(json_map_size(map) == i);
    
    currentWordPtr = words; i = 0;
    while (*currentWordPtr != NULL) {
        JSON_TEST_ASSERT(json_map_get(map, *currentWordPtr) != NULL);
        JSON_TEST_ASSERT(json_int_value(json_map_get(map, *currentWordPtr)) == i++);
        currentWordPtr++;
    }
    
    JSON_TEST_ASSERT(json_map_get(map, "onetwo") == NULL);
    JSON_TEST_ASSERT(json_map_get(map, "One") == NULL);
    JSON_TEST_ASSERT(json_map_get(map, "") == NULL);
    
    json_object_free(map);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Hashmap - testing duplicates. */
static bool test_object_7(void) {
    JSON_TEST_START;
    
    json_object * map = json_map();
    char * words[] = {
        "one", "two", "one", "three", "one", "three", "four", "five", "five", "six", NULL
    };
    
    char ** currentWordPtr; int i;
    
    currentWordPtr = words; i = 0;
    int duplicates = 0;
    while (*currentWordPtr != NULL) {
        json_object * oldValue = json_map_put(map, *currentWordPtr++, json_int(i++));
        if (oldValue != NULL) {
            duplicates++;
            json_object_free(oldValue);
        }
    }
    JSON_TEST_ASSERT(json_map_size(map) == 6);
    JSON_TEST_ASSERT(duplicates == 4);
    
    JSON_TEST_ASSERT(json_int_value(json_map_get(map, "one")) == 4);
    JSON_TEST_ASSERT(json_int_value(json_map_get(map, "two")) == 1);
    JSON_TEST_ASSERT(json_int_value(json_map_get(map, "three")) == 5);
    JSON_TEST_ASSERT(json_int_value(json_map_get(map, "four")) == 6);
    JSON_TEST_ASSERT(json_int_value(json_map_get(map, "five")) == 8);
    JSON_TEST_ASSERT(json_int_value(json_map_get(map, "six")) == 9);
    
    json_object_free(map);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Hashmap - testing large data quantities and iterator. */
static bool test_object_8(void) {
    JSON_TEST_START;
    
    json_object * map = json_map();
    
    char * words1[] = { "lorem", "ipsum", "dolor", "sit", "amet", "consectetur", "adipisici", "elit", "sed", "eiusmod" };
    char * words2[] = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten" };
    char * words3[] = { "asdjflskafjdlaskfjalf", "hustodemonsky krutoprisny", "Astar Seran", "test123456789", " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", "Michal Kolesa R.I.P.", "123456789", "sdfjasjfjaksld asdjfasjfoi sjdal sfjasfj", "something", "shorter" };
    
    bool iterated[10][10][10];
    
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 10; k++) {
                char * key = malloc(sizeof(char)*256); JSON_DEBUG_MALLOC;
                sprintf(key, "%s %s %s", words1[i], words2[j], words3[k]);
                json_object * inner = json_array();
                json_array_add(inner, json_int(i));
                json_array_add(inner, json_int(j));
                json_array_add(inner, json_int(k));
                json_map_put_ext(map, key, inner, false);
                
                iterated[i][j][k] = false;
            }
        }
    }
    
    JSON_TEST_ASSERT(json_map_size(map) == 1000);
    
    json_map_iterator iterator;
    json_map_iterator_init(&iterator, map);
    
    char str[256];
    
    while (json_map_iterator_next(&iterator)) {
        char * key = iterator.key;
        int i = json_int_value(json_array_get(iterator.value, 0));
        int j = json_int_value(json_array_get(iterator.value, 1));
        int k = json_int_value(json_array_get(iterator.value, 2));
        sprintf(str, "%s %s %s", words1[i], words2[j], words3[k]);
        JSON_TEST_ASSERT(strcmp(key, str) == 0);
        
        iterated[i][j][k] = true;
    }
    
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 10; k++) {
                JSON_TEST_ASSERT(iterated[i][j][k] == true);
            }
        }
    }
    
    json_object_free(map);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Testing references. */
static bool test_object_9(void) {
    JSON_TEST_START;
    
    json_object * array = json_array();
    json_object * item = json_string("Hello world!");
    
    for (int i = 0; i < 16; i++) {
        json_array_add(array, json_object_reference(item));
    }
    
    json_object * map = json_map();
    json_map_put(map, "key1", array);
    json_map_put(map, "key2", json_object_reference(array));
    json_map_put(map, "key3", json_object_reference(item));
    
    json_object_free(item);
    json_object_free(json_object_reference(map));
    json_object_free(map);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Array iterator test. */
static bool test_object_10(void) {
    JSON_TEST_START;
    
    int values[] = {1, 2, 4, 8, 16, 32, 64, 128};
    int totalValue = 0;
    int count = 0;
    
    json_object * array = json_array();
    
    for (int i = 0; i < 8; i++) {
        int value = values[i];
        json_array_add(array, json_int(value));
        totalValue |= value;
        count++;
    }
    
    JSON_TEST_ASSERT(json_array_size(array) == 8);
    
    json_array_iterator it;
    json_array_iterator_init(&it, array);
    
    while (json_array_iterator_next(&it)) {
        int value = json_int_value(it.item);
        count--;
        totalValue &= ~value;
    }
    
    JSON_TEST_ASSERT(count == 0);
    JSON_TEST_ASSERT(totalValue == 0);
    
    json_object_free(array);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Empty JSON test. */
static bool test_parser_1(void) {
    JSON_TEST_START;
    
    json_object * obj = json_parse_string("", NULL);
    JSON_TEST_ASSERT(obj == NULL);
    
    JSON_OBJECTS_CHECK;
    // printf("%d\n", json_debug_memblocks);
    
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Scalar JSON test #1. */
static bool test_parser_2(void) {
    JSON_TEST_START;
    
    json_object * obj = json_parse_string("true", NULL);
    JSON_TEST_ASSERT(obj != NULL);
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_BOOL);
    JSON_TEST_ASSERT(json_bool_value(obj) == true);
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Scalar JSON test #2. */
static bool test_parser_3(void) {
    JSON_TEST_START;
    
    json_object * obj = json_parse_string("\"Hello world\"", NULL);
    JSON_TEST_ASSERT(obj != NULL);
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_STRING);
    JSON_TEST_ASSERT(strcmp(json_string_value(obj), "Hello world") == 0);
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Array test. */
static bool test_parser_4(void) {
    JSON_TEST_START;
    
    json_object * obj = json_parse_string("[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]", NULL);
    JSON_TEST_ASSERT(obj != NULL);
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_ARRAY);
    JSON_TEST_ASSERT(json_array_size(obj) == 10);
    
    for (int i = 0; i < 10; i++) {
        JSON_TEST_ASSERT(json_array_get(obj, i) != NULL);
        JSON_TEST_ASSERT(json_array_get(obj, i)->type == JSON_OBJECT_INT);
        JSON_TEST_ASSERT(json_int_value(json_array_get(obj, i)) == i);
    }
    
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
};

/* Map test. */
static bool test_parser_5(void) {
    JSON_TEST_START;
    
    char * words[] = { "lorem", "ipsum", "dolor", "sit", "amet", NULL };
    
    json_object * obj = json_parse_string("{\"lorem\": 1, \"ipsum\": 2, \"dolor\": 3, \"sit\": 4, \"amet\": 5}", NULL);
    JSON_TEST_ASSERT(obj != NULL);
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_MAP);
    JSON_TEST_ASSERT(json_map_size(obj) == 5);
    
    int i = 1;
    char ** currentWordPtr = words;
    while (*currentWordPtr) {
        JSON_TEST_ASSERT(json_map_get(obj, *currentWordPtr) != NULL);
        JSON_TEST_ASSERT(json_map_get(obj, *currentWordPtr)->type == JSON_OBJECT_INT);
        JSON_TEST_ASSERT(json_int_value(json_map_get(obj, *currentWordPtr++)) == i++);
    }
    
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Map with nested arrays test. */
static bool test_parser_6(void) {
    JSON_TEST_START;
    
    json_object * obj = json_parse_string("{\"0\": [0], \"1\": [0,[1]], \"2\": [0,[1,[2]]], \"3\": [0,[1,[2,[3]]]], \"4\": [0,[1,[2,[3,[4]]]]]}", NULL);
    JSON_TEST_ASSERT(obj != NULL);
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_MAP);
    JSON_TEST_ASSERT(json_map_size(obj) == 5);
    
    for (int i = 0; i < 5; i++) {
        char key[2];
        key[0] = '0' + i; key[1] = '\0';
        
        json_object * nested = json_map_get(obj, key);
        JSON_TEST_ASSERT(nested != NULL);
        JSON_TEST_ASSERT(nested->type == JSON_OBJECT_ARRAY);
        
        int elems = 0;
        
        do {
            JSON_TEST_ASSERT(nested->type == JSON_OBJECT_ARRAY);
            json_object * first = json_array_get(nested, 0);
            JSON_TEST_ASSERT(first != NULL);
            JSON_TEST_ASSERT(first->type == JSON_OBJECT_INT);
            JSON_TEST_ASSERT(json_int_value(first) == elems++);
            nested = json_array_get(nested, 1);
        } while (nested != NULL);
        
        JSON_TEST_ASSERT(elems == i + 1);
    }
    
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

/* Duplicate keys in a map. */
/* Map with nested arrays test. */
static bool test_parser_7(void) {
    JSON_TEST_START;
    
    json_object * obj = json_parse_string("{\"hello\": [128,256,512,1024], \"world\": null, \"hello\": \"world\"}", NULL);
    JSON_TEST_ASSERT(obj != NULL);
    
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_parser_error_1(void) {
    JSON_TEST_START;
    
    json_error err = JSON_ERROR_EMPTY;
    json_object * obj = json_parse_string("[1, 2, 3] some garbage!", &err);
    JSON_TEST_ASSERT(obj == NULL);
    JSON_TEST_ASSERT(err.code == JSON_ERROR_GARBAGE);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_parser_error_2(void) {
    JSON_TEST_START;
    
    json_error err = JSON_ERROR_EMPTY;
    json_object * obj = json_parse_string("{ invalid_key: true }", &err);
    JSON_TEST_ASSERT(obj == NULL);
    JSON_TEST_ASSERT(err.code == JSON_ERROR_EXPECTED_STRING);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_parser_error_3(void) {
    JSON_TEST_START;
    
    json_error err = JSON_ERROR_EMPTY;
    json_object * obj = json_parse_string("{ \"wrong map separator\", 42 }", &err);
    JSON_TEST_ASSERT(obj == NULL);
    JSON_TEST_ASSERT(err.code == JSON_ERROR_EXPECTED_COLON);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_parser_error_4(void) {
    JSON_TEST_START;
    
    json_error err = JSON_ERROR_EMPTY;
    json_object * obj = json_parse_string("{ \"key\": \"value\"; \"unexpected\": \"semicolon\" }", &err);
    JSON_TEST_ASSERT(obj == NULL);
    JSON_TEST_ASSERT(err.code == JSON_ERROR_UNEXPECTED_CHARACTER);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_parser_error_5(void) {
    JSON_TEST_START;
    
    json_error err = JSON_ERROR_EMPTY;
    json_object * obj = json_parse_string("{ \"key\": \"value\", \"unfinished\": \"map\", ", &err);
    JSON_TEST_ASSERT(obj == NULL);
    JSON_TEST_ASSERT(err.code == JSON_ERROR_UNEXPECTED_EOF);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_parser_error_6(void) {
    JSON_TEST_START;
    
    json_error err = JSON_ERROR_EMPTY;
    json_object * obj = json_parse_string("{ \"unfinished key", &err);
    JSON_TEST_ASSERT(obj == NULL);
    JSON_TEST_ASSERT(err.code == JSON_ERROR_STR_UNEXPECTED_EOF);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_parser_error_7(void) {
    JSON_TEST_START;
    
    json_error err = JSON_ERROR_EMPTY;
    json_object * obj = json_parse_string("[invalid array]", &err);
    JSON_TEST_ASSERT(obj == NULL);
    JSON_TEST_ASSERT(err.code == JSON_ERROR_UNRESOLVED_TOKEN);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_parser_error_8(void) {
    JSON_TEST_START;
    
    json_error err = JSON_ERROR_EMPTY;
    json_object * obj = json_parse_string("[1, 2, 3, 4; 5]", &err);
    JSON_TEST_ASSERT(obj == NULL);
    JSON_TEST_ASSERT(err.code == JSON_ERROR_UNEXPECTED_CHARACTER);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_parser_error_9(void) {
    JSON_TEST_START;
    
    json_error err = JSON_ERROR_EMPTY;
    json_object * obj = json_parse_string("{1, 2, 3, 4, 5}", &err);
    JSON_TEST_ASSERT(obj == NULL);
    JSON_TEST_ASSERT(err.code == JSON_ERROR_EXPECTED_STRING);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}


static bool test_parser_error_10(void) {
    JSON_TEST_START;
    
    json_error err = JSON_ERROR_EMPTY;
    json_object * obj = json_parse_string("[{\"hello\": [1, 2, 3, 4, 5, {\"lorem\": 1, \"ipsum\":[123,456,789]}], \"world\": [6, 7, 8, 9, true, false, null]}, bang!]", &err);
    JSON_TEST_ASSERT(obj == NULL);
    JSON_TEST_ASSERT(err.code == JSON_ERROR_UNEXPECTED_CHARACTER);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_parser_error_11(void) {
    JSON_TEST_START;
    
    json_error err = JSON_ERROR_EMPTY;
    json_object * obj = json_parse_string("something unexpected", &err);
    JSON_TEST_ASSERT(obj == NULL);
    JSON_TEST_ASSERT(err.code == JSON_ERROR_UNRESOLVED_TOKEN);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_file_1(void) {
    JSON_TEST_START;
    
    json_object * obj = json_parse_file("test_files/test_ok_1.json", NULL);
    JSON_TEST_ASSERT(obj != NULL);
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_MAP);
    
    json_object * obj2 = json_map_get(obj, "glossary");
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_MAP);
    
    obj2 = json_map_get(obj2, "GlossDiv");
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_MAP);
    
    obj2 = json_map_get(obj2, "GlossList");
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_MAP);
    
    obj2 = json_map_get(obj2, "GlossEntry");
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_MAP);
    
    obj2 = json_map_get(obj2, "GlossDef");
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_MAP);
    
    obj2 = json_map_get(obj2, "GlossSeeAlso");
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_ARRAY);
    
    obj2 = json_array_get(obj2, 0);
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_STRING);
    JSON_TEST_ASSERT(strcmp(json_string_value(obj2), "GML") == 0);
    
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_file_2(void) {
    JSON_TEST_START;
    
    json_object * obj = json_parse_file("test_files/test_ok_2.json", NULL);
    JSON_TEST_ASSERT(obj != NULL);
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_MAP);
    
    json_object * obj2 = json_map_get(obj, "menu");
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_MAP);
    
    obj2 = json_map_get(obj2, "popup");
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_MAP);
    
    obj2 = json_map_get(obj2, "menuitem");
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_ARRAY);
    
    obj2 = json_array_get(obj2, 2);
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_MAP);
    
    obj2 = json_map_get(obj2, "onclick");
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_STRING);
    JSON_TEST_ASSERT(strcmp(json_string_value(obj2), "CloseDoc()") == 0);
    
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_file_3(void) {
    JSON_TEST_START;
    
    json_object * obj = json_parse_file("test_files/test_ok_3.json", NULL);
    JSON_TEST_ASSERT(obj != NULL);
    JSON_TEST_ASSERT(obj->type == JSON_OBJECT_MAP);
    
    json_object * obj2 = json_map_get(obj, "widget");
    JSON_TEST_ASSERT(obj2 != NULL);
    JSON_TEST_ASSERT(obj2->type == JSON_OBJECT_MAP);
    JSON_TEST_ASSERT(json_map_size(obj2) == 4);
    
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_file_4(void) {
    JSON_TEST_START;
    
    json_object * obj = json_parse_file("test_files/test_ok_4.json", NULL);
    JSON_TEST_ASSERT(obj != NULL);
    
    // just check parsing and memory leaks...
    
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static bool test_file_5(void) {
    JSON_TEST_START;
    
    json_object * obj = json_parse_file("test_files/test_ok_5.json", NULL);
    JSON_TEST_ASSERT(obj != NULL);
    
    // just check parsing and memory leaks...
    
    json_object_free(obj);
    
    JSON_OBJECTS_CHECK;
    JSON_MEMBLOCKS_CHECK;
    JSON_TEST_DONE;
}

static json_unit_test tests[] = {
    test_reader_1, test_reader_2, // reader tests
    test_tokenizer_1, test_tokenizer_2, test_tokenizer_3, // basic tokens
    test_tokenizer_4, test_tokenizer_5, // symbol tokens
    test_tokenizer_6, test_tokenizer_7, test_tokenizer_8, // number tokens
    test_tokenizer_9, 
    test_tokenizer_10, // string tokens
    test_object_1, test_object_2, // basic datatypes
    test_object_3, test_object_4, test_object_5, // arrays
    test_object_6, test_object_7, test_object_8, // hashmaps
    test_object_9, // references
    test_object_10,
    test_parser_1, test_parser_2, test_parser_3, // simple parser tests
    test_parser_4, test_parser_5, test_parser_6, test_parser_7, // complex types
    test_parser_error_1,
    test_parser_error_2, test_parser_error_3, test_parser_error_4, test_parser_error_5, test_parser_error_6,
    test_parser_error_7, test_parser_error_8, test_parser_error_9, test_parser_error_10,
    test_parser_error_11,
    test_file_1, test_file_2, test_file_3, test_file_4, test_file_5,
    NULL
};

// static json_unit_test tests2[] = { test_parser_2, NULL };

/*
 * Main function, for testing purposes.
 */
int main(int argc, char** argv) {
    json_unit_test * currentTest = tests;
    while (*currentTest != NULL) {
        bool result = (*currentTest)();
        if (result) json_unit_tests_passed++;
        else json_unit_tests_failed++;
        currentTest++;
    }
    printf("%i tests finished, %i failed.\n", json_unit_tests_passed + json_unit_tests_failed, json_unit_tests_failed);
    return (EXIT_SUCCESS);
}

