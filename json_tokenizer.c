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
#include "json_debug.h"
#include "json_error.h"

#define JSON_STRING_CAPACITY 8


// token states
enum {
    JSON_STRING_OPEN = 1, JSON_STRING_CLOSED = 2, JSON_STRING_BACKSLASH = 3,
    JSON_STRING_UNI_0 = 4, JSON_STRING_UNI_1 = 5, JSON_STRING_UNI_2 = 6, JSON_STRING_UNI_3 = 7
};

enum { 
    JSON_NUMERIC_SIGN = 1, JSON_NUMERIC_ZERO = 2, JSON_NUMERIC_INTEGER = 3, 
    JSON_NUMERIC_POINT = 4, JSON_NUMERIC_FLOAT = 5, 
    JSON_NUMERIC_EXP = 6, JSON_NUMERIC_EXP_SIGN = 7, JSON_NUMERIC_EXP_VALUE = 8
};

enum {
    JSON_SYMBOL_FIRST_CHAR = 1, JSON_SYMBOL_NEXT_CHAR = 2
};

/* Initializes memory for a token containing character string */
static void json_token_string_init(json_token * token) {
    JSON_DEBUG_MALLOC;
    token->data.string.data = malloc(sizeof(char) * JSON_STRING_CAPACITY);
    token->data.string.length = 0;
    token->data.string.data[0] = '\0';
    token->data.string.capacity = JSON_STRING_CAPACITY;
}

/* Appends a character to the token's string. */
static void json_token_string_append(json_token * token, char c) {
    if (token->data.string.length + 1 == token->data.string.capacity) {
        token->data.string.capacity *= 2;
        token->data.string.data = realloc(token->data.string.data, sizeof(char) * token->data.string.capacity);
    }
    token->data.string.data[token->data.string.length++] = c;
    token->data.string.data[token->data.string.length] = '\0';
}

/* Frees the token's string. */
static inline void json_token_string_free(json_token * token) {
    free(token->data.string.data);
    JSON_DEBUG_FREE;
}

/* Returns the pointer to the token's string. The memory won't be freed when json_token_free() is called. */
char * json_token_hijack(json_token * token) {
    char * ptr = token->data.string.data;
    token->data.string.data = NULL;
    return ptr;
}

/* Frees the token internal memory (string). */
void json_token_free(json_token * token) {
    if (token->type == JSON_TOKEN_STRING || token->type == JSON_TOKEN_SYMBOL) {
        if (token->data.string.data != NULL) json_token_string_free(token);
    }
}

static bool json_tokenizer_finishNumeric(json_tokenizer * tokenizer);
static bool json_tokenizer_finishSymbol(json_tokenizer * tokenizer);

/* Finishes a token. */
static bool json_tokenizer_finishToken(json_tokenizer * tokenizer) {
    if (tokenizer->_currentToken.type == JSON_TOKEN_NUMERIC) {
        return json_tokenizer_finishNumeric(tokenizer);
    }
    else if (tokenizer->_currentToken.type == JSON_TOKEN_SYMBOL) {
        return json_tokenizer_finishSymbol(tokenizer);
    }
    else return true;
}

/* Finishes a numeric token. */
static bool json_tokenizer_finishNumeric(json_tokenizer * tokenizer) {
    bool validNumber = true;
    if (tokenizer->_currentTokenStatus == JSON_NUMERIC_SIGN || tokenizer->_currentTokenStatus == JSON_NUMERIC_POINT || tokenizer->_currentTokenStatus == JSON_NUMERIC_EXP || tokenizer->_currentTokenStatus == JSON_NUMERIC_EXP_SIGN) validNumber = false;
    if (validNumber) {
        if (tokenizer->_currentTokenStatus < JSON_NUMERIC_POINT) {
            tokenizer->_currentToken.type = JSON_TOKEN_INTEGER;
            int value = atoi(tokenizer->_currentToken.data.string.data);
            json_token_string_free(&tokenizer->_currentToken);
            tokenizer->_currentToken.data.intValue = value;
        }
        else {
            tokenizer->_currentToken.type = JSON_TOKEN_FLOAT;
            float value = (float)atof(tokenizer->_currentToken.data.string.data);
            json_token_string_free(&tokenizer->_currentToken);
            tokenizer->_currentToken.data.floatValue = value;
        }
    }
    else {
        json_token_string_free(&tokenizer->_currentToken);
    }
    return validNumber;
}

/* Finishes a symbol token. */
static bool json_tokenizer_finishSymbol(json_tokenizer * tokenizer) {
    if (strcmp(tokenizer->_currentToken.data.string.data, "true") == 0) {
        tokenizer->_currentToken.type = JSON_TOKEN_BOOL;
        json_token_string_free(&tokenizer->_currentToken);
        tokenizer->_currentToken.data.boolValue = true;
    }
    else if (strcmp(tokenizer->_currentToken.data.string.data, "false") == 0) {
        tokenizer->_currentToken.type = JSON_TOKEN_BOOL;
        json_token_string_free(&tokenizer->_currentToken);
        tokenizer->_currentToken.data.boolValue = false;
    }
    else if (strcmp(tokenizer->_currentToken.data.string.data, "null") == 0) {
        tokenizer->_currentToken.type = JSON_TOKEN_NULL;
        json_token_string_free(&tokenizer->_currentToken);
    }
    // @todo nepovolit ostatní symboly
    return true;
}

/* Resets the tokenizer to the default state. */
static inline void json_resetTokenizerStatus(json_tokenizer * tokenizer) {
    tokenizer->_currentToken.type = JSON_TOKEN_UNKNOWN;
    tokenizer->_currentTokenStatus = 0;
}

/* Initializes the tokenizer. */
void json_tokenizer_init(json_tokenizer * tokenizer, json_reader reader) {
    tokenizer->line = 1;
    tokenizer->pos = 0;
    
    tokenizer->reader = reader;
    
    json_resetTokenizerStatus(tokenizer);
}


// some helper functions

static inline bool is_alpha_or_underscore(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static inline bool is_numeric(int c) {
    return (c >= '0' && c <= '9');
}

static int hex_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static inline int json_tokenizer_processNumeric(json_tokenizer * tokenizer, int c);
static inline int json_tokenizer_processString(json_tokenizer * tokenizer, int c);

#define THROW_ERROR(code) { tokenizer->error = code; return false; }

/* Emit the previous token. */
#define EMIT_PREVIOUS_TOKEN \
if (tokenizer->_currentToken.type != JSON_TOKEN_UNKNOWN) { \
    if (!json_tokenizer_finishToken(tokenizer)) THROW_ERROR(JSON_ERROR_UNEXPECTED_TOKEN_END); \
    tokenizer->token = tokenizer->_currentToken; \
    json_resetTokenizerStatus(tokenizer); \
    tokenizer->_notEmitted = false; \
}

/* Finds the next token. */
bool json_tokenizer_next(json_tokenizer * tokenizer) {    
    int c; // current character
    tokenizer->_notEmitted = true;
    
    // if EOF was found in previous call, return it immediately
    if (tokenizer->_currentToken.type == JSON_TOKEN_EOF) {
        EMIT_PREVIOUS_TOKEN;
    }
    
    // process characters until we find a token to return
    while(tokenizer->_notEmitted) {
        c = tokenizer->reader.nextChar(&tokenizer->reader.data);
        tokenizer->ch = c;
        tokenizer->pos++;
        
        if (tokenizer->_currentToken.type != JSON_TOKEN_STRING) {
            switch (c) {
            // EOF
            case EOF: 
            case '\0':
                // end-of-file
                EMIT_PREVIOUS_TOKEN else { // emit EOF immediately!
                    tokenizer->token.type = JSON_TOKEN_EOF;
                    tokenizer->_notEmitted = false;
                }
                tokenizer->_currentToken.type = JSON_TOKEN_EOF;
                break;
            // whitespace
            case ' ':
            case '\n':
            case '\r':
            case '\t':
            case '\f':
                EMIT_PREVIOUS_TOKEN;
                tokenizer->_currentToken.type = JSON_TOKEN_UNKNOWN;
                if (c == '\n') {
                    tokenizer->pos = 0;
                    tokenizer->line++;
                }
                break;
            // braces and brackets
            case '{':
                EMIT_PREVIOUS_TOKEN;
                tokenizer->_currentToken.type = JSON_TOKEN_BRACE_OPENING;
                break;
            case '}':
                EMIT_PREVIOUS_TOKEN;
                tokenizer->_currentToken.type = JSON_TOKEN_BRACE_CLOSING;
                break;
            case '[':
                EMIT_PREVIOUS_TOKEN;
                tokenizer->_currentToken.type = JSON_TOKEN_BRACKET_OPENING;
                break;
            case ']':
                EMIT_PREVIOUS_TOKEN;
                tokenizer->_currentToken.type = JSON_TOKEN_BRACKET_CLOSING;
                break;

            // separators
            case ':':
                EMIT_PREVIOUS_TOKEN;
                tokenizer->_currentToken.type = JSON_TOKEN_COLON;
                break;
            case ',':
                EMIT_PREVIOUS_TOKEN;
                tokenizer->_currentToken.type = JSON_TOKEN_COMMA;
                break;

            // string beginning
            case '"':
                EMIT_PREVIOUS_TOKEN;
                tokenizer->_currentToken.type = JSON_TOKEN_STRING;
                json_token_string_init(&tokenizer->_currentToken);
                tokenizer->_currentTokenStatus = JSON_STRING_OPEN;
                break;

            // something else with lower priority...
            default:
                // parse the rest of the number
                if (tokenizer->_currentToken.type == JSON_TOKEN_NUMERIC) {
                    if (!json_tokenizer_processNumeric(tokenizer, c)) return false;
                }

                // parse the rest of the symbol
                else if (tokenizer->_currentToken.type == JSON_TOKEN_SYMBOL) {
                    if (is_alpha_or_underscore(c) || is_numeric(c)) {
                        json_token_string_append(&tokenizer->_currentToken, c);
                        tokenizer->_currentTokenStatus = JSON_SYMBOL_NEXT_CHAR;
                    }
                    else {
                        json_token_string_free(&tokenizer->_currentToken);
                        THROW_ERROR(JSON_ERROR_UNEXPECTED_CHARACTER);
                    }
                }

                else {
                    if (c == '-') { // start a number
                        EMIT_PREVIOUS_TOKEN;
                        tokenizer->_currentToken.type = JSON_TOKEN_NUMERIC;
                        json_token_string_init(&tokenizer->_currentToken);
                        json_token_string_append(&tokenizer->_currentToken, c);
                        tokenizer->_currentTokenStatus = JSON_NUMERIC_SIGN;
                    }
                    else if (is_numeric(c)) { // start a number
                        EMIT_PREVIOUS_TOKEN;
                        tokenizer->_currentToken.type = JSON_TOKEN_NUMERIC;
                        json_token_string_init(&tokenizer->_currentToken);
                        json_token_string_append(&tokenizer->_currentToken, c);
                        if (c != '0') tokenizer->_currentTokenStatus = JSON_NUMERIC_INTEGER;
                        else tokenizer->_currentTokenStatus = JSON_NUMERIC_ZERO;
                    }
                    else if (is_alpha_or_underscore(c)) { // start a symbol (identifier)
                        EMIT_PREVIOUS_TOKEN;
                        tokenizer->_currentToken.type = JSON_TOKEN_SYMBOL;
                        json_token_string_init(&tokenizer->_currentToken);
                        json_token_string_append(&tokenizer->_currentToken, c);
                        tokenizer->_currentTokenStatus = JSON_SYMBOL_FIRST_CHAR;
                    }
                    // unknown...
                    else {       
                        EMIT_PREVIOUS_TOKEN;
                        THROW_ERROR(JSON_ERROR_UNEXPECTED_CHARACTER);
                    }
                }
            }
        }
        
        else { // tokenizer->_status.stringOpened == true
            if (!json_tokenizer_processString(tokenizer, c)) return false;
        }
    }   
    
    return true;
}

/* Processing a number. */
static inline int json_tokenizer_processNumeric(json_tokenizer * tokenizer, int c) {
    switch (tokenizer->_currentTokenStatus) {
    case JSON_NUMERIC_SIGN:
        if (c == '0') {
            json_token_string_append(&tokenizer->_currentToken, c);
            tokenizer->_currentTokenStatus = JSON_NUMERIC_ZERO;
        }
        else if (c >= '1' && c <= '9') {
            json_token_string_append(&tokenizer->_currentToken, c);
            tokenizer->_currentTokenStatus = JSON_NUMERIC_INTEGER;
        }
        else {
            json_token_string_free(&tokenizer->_currentToken);
            THROW_ERROR(JSON_ERROR_UNEXPECTED_CHARACTER);
        }
        break;
    case JSON_NUMERIC_ZERO:
        if (c == '.') {
            json_token_string_append(&tokenizer->_currentToken, c);
            tokenizer->_currentTokenStatus = JSON_NUMERIC_POINT;
        }
        else if (c == 'e' || c == 'E') {
            json_token_string_append(&tokenizer->_currentToken, c);
            tokenizer->_currentTokenStatus = JSON_NUMERIC_EXP;
        }
        else {
            json_token_string_free(&tokenizer->_currentToken);
            THROW_ERROR(JSON_ERROR_UNEXPECTED_CHARACTER);
        }
        break;
    case JSON_NUMERIC_INTEGER:
        if (c == '.') {
            json_token_string_append(&tokenizer->_currentToken, c);
            tokenizer->_currentTokenStatus = JSON_NUMERIC_POINT;
        }
        else if (c == 'e' || c == 'E') {
            json_token_string_append(&tokenizer->_currentToken, c);
            tokenizer->_currentTokenStatus = JSON_NUMERIC_EXP;
        }
        else if (is_numeric(c)) {
            json_token_string_append(&tokenizer->_currentToken, c);
        }
        else {
            json_token_string_free(&tokenizer->_currentToken);
            THROW_ERROR(JSON_ERROR_UNEXPECTED_CHARACTER);
        }
        break;
    case JSON_NUMERIC_POINT:
        if (is_numeric(c)) {
            json_token_string_append(&tokenizer->_currentToken, c);
            tokenizer->_currentTokenStatus = JSON_NUMERIC_FLOAT;
        }
        else {
            json_token_string_free(&tokenizer->_currentToken);
            THROW_ERROR(JSON_ERROR_UNEXPECTED_CHARACTER);
        }
        break;
    case JSON_NUMERIC_FLOAT:
        if (is_numeric(c)) {
            json_token_string_append(&tokenizer->_currentToken, c);
        }
        else if (c == 'e' || c == 'E') {
            json_token_string_append(&tokenizer->_currentToken, c);
            tokenizer->_currentTokenStatus = JSON_NUMERIC_EXP;
        }
        else {
            json_token_string_free(&tokenizer->_currentToken);
            THROW_ERROR(JSON_ERROR_UNEXPECTED_CHARACTER);
        }
        break;
    case JSON_NUMERIC_EXP:
        if (c == '+' || c == '-') {
            json_token_string_append(&tokenizer->_currentToken, c);
            tokenizer->_currentTokenStatus = JSON_NUMERIC_EXP_SIGN;
        }
        else if (is_numeric(c)) {
            json_token_string_append(&tokenizer->_currentToken, c);
            tokenizer->_currentTokenStatus = JSON_NUMERIC_EXP_VALUE;
        }
        else {
            json_token_string_free(&tokenizer->_currentToken);
            THROW_ERROR(JSON_ERROR_UNEXPECTED_CHARACTER);
        }
        break;
    case JSON_NUMERIC_EXP_SIGN:
        if (is_numeric(c)) {
            json_token_string_append(&tokenizer->_currentToken, c);
            tokenizer->_currentTokenStatus = JSON_NUMERIC_EXP_VALUE;
        }
        else {
            json_token_string_free(&tokenizer->_currentToken);
            THROW_ERROR(JSON_ERROR_UNEXPECTED_CHARACTER);
        }
        break;
    case JSON_NUMERIC_EXP_VALUE:
        if (is_numeric(c)) {
            json_token_string_append(&tokenizer->_currentToken, c);
        }
        else {
            json_token_string_free(&tokenizer->_currentToken);
            THROW_ERROR(JSON_ERROR_UNEXPECTED_CHARACTER);
        }
        break;
    }
    return true;
}

/* Processing a string. */
static inline int json_tokenizer_processString(json_tokenizer * tokenizer, int c) {
    if (c == EOF || c == '\0') {
        json_token_string_free(&tokenizer->_currentToken);
        THROW_ERROR(JSON_ERROR_STR_UNEXPECTED_EOF);
    }
    else if (c == '\n' || c == '\r' || c == '\t' || c == '\f' || c == '\b') {
        json_token_string_free(&tokenizer->_currentToken);
        THROW_ERROR(JSON_ERROR_STR_UNEXPECTED_CTRL);
    }
    else {
        if (tokenizer->_currentTokenStatus == JSON_STRING_OPEN) {
            if (c == '\\') {
                tokenizer->_currentTokenStatus = JSON_STRING_BACKSLASH;
            }
            else if (c == '"') { // close the string
                // tokenizer->_currentTokenStatus = JSON_STRING_NONE;
                EMIT_PREVIOUS_TOKEN;
            }
            else {
                // add the character to the buffer
                json_token_string_append(&tokenizer->_currentToken, c);
            }
        }
        else if (tokenizer->_currentTokenStatus == JSON_STRING_BACKSLASH) {
            #define APPEND_CHAR(C) \
                    json_token_string_append(&tokenizer->_currentToken, C); \
                    tokenizer->_currentTokenStatus = JSON_STRING_OPEN; \
                    break;

            switch (c) {
            case '"':   APPEND_CHAR('"');
            case '\\':  APPEND_CHAR('\\');
            case '/':   APPEND_CHAR('/');
            case 'b':   APPEND_CHAR('\b');
            case 'f':   APPEND_CHAR('\f');
            case 'n':   APPEND_CHAR('\n');
            case 'r':   APPEND_CHAR('\r');
            case 't':   APPEND_CHAR('\t');
            case 'u':
                tokenizer->_currentTokenStatus = JSON_STRING_UNI_0;
                tokenizer->_unicodeChar = 0;
                break;
            default:
                json_token_string_free(&tokenizer->_currentToken);
                    THROW_ERROR(JSON_ERROR_STR_INVALID_ESCAPE);
            }
        }
        else if (tokenizer->_currentTokenStatus == JSON_STRING_UNI_0) {
            int hex = hex_to_int(c);
            if (hex == -1) THROW_ERROR(JSON_ERROR_STR_INVALID_UNICODE);
            tokenizer->_unicodeChar |= hex << 12;
            tokenizer->_currentTokenStatus = JSON_STRING_UNI_1;
        }
        else if (tokenizer->_currentTokenStatus == JSON_STRING_UNI_1) {
            int hex = hex_to_int(c);
            if (hex == -1) THROW_ERROR(JSON_ERROR_STR_INVALID_UNICODE);
            tokenizer->_unicodeChar |= hex << 8;
            tokenizer->_currentTokenStatus = JSON_STRING_UNI_2;
        }
        else if (tokenizer->_currentTokenStatus == JSON_STRING_UNI_2) {
            int hex = hex_to_int(c);
            if (hex == -1) THROW_ERROR(JSON_ERROR_STR_INVALID_UNICODE);
            tokenizer->_unicodeChar |= hex << 4;
            tokenizer->_currentTokenStatus = JSON_STRING_UNI_3;
        }
        else if (tokenizer->_currentTokenStatus == JSON_STRING_UNI_3) {
            int hex = hex_to_int(c);
            if (hex == -1) THROW_ERROR(JSON_ERROR_STR_INVALID_UNICODE);
            tokenizer->_unicodeChar |= hex;
            
            int u = tokenizer->_unicodeChar;
            if (u < 0x80) {
                json_token_string_append(&tokenizer->_currentToken, u & 0x7F); // 0xxxxxxx
            }
            else if (u < 0x800) {
                json_token_string_append(&tokenizer->_currentToken, ((u >> 6)  & 0x1F) | 0xC0);    // 110xxxxx 
                json_token_string_append(&tokenizer->_currentToken, ( u        & 0x3F) | 0x80);    // 10xxxxxx
            }
            else {
                json_token_string_append(&tokenizer->_currentToken, ((u >> 12) & 0x0F) | 0xE0);    // 1110xxxx 
                json_token_string_append(&tokenizer->_currentToken, ((u >> 6)  & 0x3F) | 0x80);    // 10xxxxxx 
                json_token_string_append(&tokenizer->_currentToken, ( u        & 0x3F) | 0x80);    // 10xxxxxx
            }
            
            tokenizer->_currentTokenStatus = JSON_STRING_OPEN;
        }
    }
    return true;
}
