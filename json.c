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
#include <stdbool.h>
#include <stdio.h>

#include "json.h"
#include "json_object.h"
#include "json_tokenizer.h"
#include "json_debug.h"
#include "json_error.h"


/* Parses a JSON string. */
json_object * json_parse_string(const char * string, json_error * error) {
    return json_parse(json_reader_string(string), error);
}

/* Parses a JSON file. */
json_object * json_parse_file_buf(const char * filename, bool buffered, int bufferSize, json_error * error) {
    FILE * file = fopen(filename, "r");
    if (buffered == true) {
        setvbuf(file, NULL, _IOFBF, bufferSize);
    }
    else {
        setvbuf(file, NULL, _IONBF, 0);
    }
    json_object * obj = json_parse_stream(file, error);
    fclose(file);
    return obj;
}
    
/* Parses JSON incoming from a steam. */
json_object * json_parse_stream(FILE * stream, json_error * error) {
    return json_parse(json_reader_stream(stream), error);
}

static json_object * json_parse_recursive(json_tokenizer * tokenizer, json_error * error);
static inline json_object * json_parse_recursive_map(json_tokenizer * tokenizer, json_error * error);
static inline json_object * json_parse_recursive_array(json_tokenizer * tokenizer, json_error * error);

#define THROW_ERROR(errcode) { if (error) { error->code = errcode; error->line = tokenizer->line; error->pos = tokenizer->pos; } return NULL; }
#define THROW_ERROR2(errcode) { if (error) { error->code = errcode; error->line = tokenizer.line; error->pos = tokenizer.pos; } return NULL; }

/* Parses JSON. */
json_object * json_parse(json_reader reader, json_error * error) {
    json_tokenizer tokenizer;
    json_tokenizer_init(&tokenizer, reader);
    json_object * object = json_parse_recursive(&tokenizer, error);
    if (object == NULL) return NULL;
    
    // EOF wanted
    bool eofOk = json_tokenizer_next(&tokenizer);
    if (!eofOk || tokenizer.token.type != JSON_TOKEN_EOF) {
        json_token_free(&tokenizer.token);
        json_object_free(object);
        THROW_ERROR2(JSON_ERROR_GARBAGE); // unexpected garbage...
    }
    
    return object;
}

static json_object * json_parse_recursive(json_tokenizer * tokenizer, json_error * error) {
    bool tokenOk;
    
    tokenOk = json_tokenizer_next(tokenizer);
    if (!tokenOk) THROW_ERROR(tokenizer->error);
    
    switch (tokenizer->token.type) {
    case JSON_TOKEN_NULL:
        return json_null();
    case JSON_TOKEN_INTEGER:
        return json_int(tokenizer->token.data.intValue);
    case JSON_TOKEN_BOOL:
        return json_bool(tokenizer->token.data.boolValue);
    case JSON_TOKEN_FLOAT:
        return json_float(tokenizer->token.data.floatValue);
    case JSON_TOKEN_STRING:
        return json_string_ref(json_token_hijack(&tokenizer->token));
    case JSON_TOKEN_BRACE_OPENING:
        return json_parse_recursive_map(tokenizer, error);
    case JSON_TOKEN_BRACKET_OPENING:
        return json_parse_recursive_array(tokenizer, error);
    case JSON_TOKEN_EOF:
        THROW_ERROR(JSON_ERROR_UNEXPECTED_EOF);
    default:
        json_token_free(&tokenizer->token);
        THROW_ERROR(JSON_ERROR_UNRESOLVED_TOKEN); // unknown token
    }
}

static inline json_object * json_parse_recursive_map(json_tokenizer * tokenizer, json_error * error) {
    bool tokenOk;
    bool notFinished = true;
    json_object * map = json_map();
    char * key = NULL;
    
    #define THROW_MAP_ERROR(e) { json_object_free(map); if (key) { free(key); JSON_DEBUG_FREE; } THROW_ERROR(e); }

    do {
        // read the string key
        tokenOk = json_tokenizer_next(tokenizer);
        if (!tokenOk) THROW_MAP_ERROR(tokenizer->error);
        if (tokenizer->token.type == JSON_TOKEN_EOF) THROW_MAP_ERROR(JSON_ERROR_UNEXPECTED_EOF);
        if (tokenizer->token.type != JSON_TOKEN_STRING) { 
            json_token_free(&tokenizer->token);
            THROW_MAP_ERROR(JSON_ERROR_EXPECTED_STRING);
        }
        
        key = json_token_hijack(&tokenizer->token);
        
        // read ":"
        tokenOk = json_tokenizer_next(tokenizer);
        if (!tokenOk) THROW_MAP_ERROR(tokenizer->error);
        if (tokenizer->token.type == JSON_TOKEN_EOF) THROW_MAP_ERROR(JSON_ERROR_UNEXPECTED_EOF);
        if (tokenizer->token.type != JSON_TOKEN_COLON) {
            json_token_free(&tokenizer->token);
            THROW_MAP_ERROR(JSON_ERROR_EXPECTED_COLON);
        }
        
        // read the value
        json_object * value = json_parse_recursive(tokenizer, error);
        if (value == NULL) THROW_MAP_ERROR(error->code);
        
        // store it and free od value (in the case of a collision)
        json_object * oldValue = json_map_put_ext(map, key, value, false); // store to the map, don't copy the key
        if (oldValue != NULL) json_object_free(oldValue);
        key = NULL;
        
        // read "," or "}"
        tokenOk = json_tokenizer_next(tokenizer);
        if (!tokenOk) THROW_MAP_ERROR(tokenizer->error);
        if (tokenizer->token.type == JSON_TOKEN_EOF) THROW_MAP_ERROR(JSON_ERROR_UNEXPECTED_EOF);
        if (tokenizer->token.type == JSON_TOKEN_BRACE_CLOSING) {
            notFinished = false;
        }
        else if (tokenizer->token.type != JSON_TOKEN_COMMA) {
            json_token_free(&tokenizer->token);
            THROW_MAP_ERROR(JSON_ERROR_EXPECTED_COMMA_OR_CLOSING_BRACE);
        }
    } while (notFinished);
    
    return map;
}

static inline json_object * json_parse_recursive_array(json_tokenizer * tokenizer, json_error * error) {
    bool tokenOk;
    bool notFinished = true;
    json_object * array = json_array();
    
    #define THROW_ARRAY_ERROR(e) { json_object_free(array); THROW_ERROR(e); }
    
    do {
        // read the item
        json_object * item = json_parse_recursive(tokenizer, error);
        if (item == NULL) THROW_ARRAY_ERROR(error->code);
        
        // add the item to the array
        json_array_add(array, item);
        
        // read "," or "]"
        tokenOk = json_tokenizer_next(tokenizer);
        if (!tokenOk) THROW_ARRAY_ERROR(tokenizer->error);
        if (tokenizer->token.type == JSON_TOKEN_EOF) THROW_ARRAY_ERROR(JSON_ERROR_UNEXPECTED_EOF);
        if (tokenizer->token.type == JSON_TOKEN_BRACKET_CLOSING) {
            notFinished = false;
        }
        else if (tokenizer->token.type != JSON_TOKEN_COMMA) {
            json_token_free(&tokenizer->token);
            THROW_ARRAY_ERROR(JSON_ERROR_EXPECTED_COMMA_OR_CLOSING_BRACKET);
        }
    } while (notFinished);
    
    return array;
}


