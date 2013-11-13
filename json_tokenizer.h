/* 
 * File:   json_tokenizer.h
 * Author: Martin
 *
 * Created on 6. listopad 2013, 22:40
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

