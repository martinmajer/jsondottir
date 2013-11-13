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


#ifndef JSON_TOKENIZER_H
#define	JSON_TOKENIZER_H

#include "json_reader.h"

#ifdef	__cplusplus
extern "C" {
#endif

/* JSON token type */
typedef enum JSON_TOKEN_TYPE {
    JSON_TOKEN_UNKNOWN, // internal use only (whitespaces)
    JSON_TOKEN_EOF,     // end-of-file or \0
    
    JSON_TOKEN_BRACE_OPENING,   // {    
    JSON_TOKEN_BRACE_CLOSING,   // }
    JSON_TOKEN_BRACKET_OPENING, // [
    JSON_TOKEN_BRACKET_CLOSING, // ]
    JSON_TOKEN_COLON,           // :
    JSON_TOKEN_COMMA,           // ,
    
    JSON_TOKEN_STRING,  // string
    
    JSON_TOKEN_NUMERIC, // unresolved numeric, internal use only
    JSON_TOKEN_INTEGER, // integer
    JSON_TOKEN_FLOAT,   // float
    
    JSON_TOKEN_SYMBOL,  // unresolved symbol
    JSON_TOKEN_NULL,    // null value
    JSON_TOKEN_BOOL     // boolean
} json_tokenType;

/* JSON token */
typedef struct JSON_TOKEN {
    /* Token type. */
    enum JSON_TOKEN_TYPE type;
    
    /* Stored data. */
    union {
        struct {
            char * data;
            int length;
            int capacity;
        } string;
        float floatValue;
        int intValue;
        bool boolValue;
    } data;
} json_token;

/* JSON tokenizer */
typedef struct JSON_TOKENIZER {
    json_token token; // emitted token
    int error; // error code

    int ch; // current character
    int line, pos; // line and position
    
    // function which returns next character from the input (or buffer)
    json_reader reader;
    
    // private fields
    bool _notEmitted;
    json_token _currentToken;
    int _currentTokenStatus;
    int _unicodeChar;
} json_tokenizer;

/* 
 * Initializes a new tokenizer.
 */
extern void json_tokenizer_init(json_tokenizer * tokenizer, json_reader reader);

/* 
 * Finds the next token, which will be stored in tokenizer.token field.
 * 
 * Returns false, if an error occurs.
 */
extern bool json_tokenizer_next(json_tokenizer * tokenizer);

/* 
 * Returns the pointer for the char* data of the token (types string or symbol). 
 * The caller must free the memory block manually after this function has been called. 
 */
extern char * json_token_hijack(json_token * token);

/* 
 * Frees the char* data of the token.
 */
extern void json_token_free(json_token * token);


#ifdef	__cplusplus
}
#endif

#endif	/* JSON_TOKENIZER_H */

