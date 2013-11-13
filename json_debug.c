/* 
 * File:   json_debug.c
 * Author: Martin
 *
 * Created on 8. listopad 2013, 14:42
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "json_debug.h"
#include "json_tokenizer.h"


#ifdef JSON_DEBUG
int json_debug_memblocks = 0;
int json_debug_objects = 0;
#endif


void json_tokenizer_dumpTokens(json_tokenizer * tokenizer, FILE * stream) {
    bool eof = false;
    while (!eof) {
        bool ok = json_tokenizer_next(tokenizer);
        if (!ok) {
            fprintf(stream, "ERROR (char '%c' pos %d:%d)\n", tokenizer->ch, tokenizer->line, tokenizer->pos);
            break;
        }
        switch (tokenizer->token.type) {
            case JSON_TOKEN_EOF:
                eof = true;
                fprintf(stream, "EOF\n");
                break;
            case JSON_TOKEN_BRACE_OPENING:
                fprintf(stream, "{ ");
                break;
            case JSON_TOKEN_BRACE_CLOSING:
                fprintf(stream, "} "); 
                break;
            case JSON_TOKEN_BRACKET_OPENING:
                fprintf(stream, "[ "); 
                break;
            case JSON_TOKEN_BRACKET_CLOSING:
                fprintf(stream, "] ");
                break;
            case JSON_TOKEN_COLON:
                fprintf(stream, ": ");
                break;
            case JSON_TOKEN_COMMA:
                fprintf(stream, ", "); 
                break;
            case JSON_TOKEN_STRING:
                fprintf(stream, "string(%s) ", tokenizer->token.data.string.data);
                break;
            case JSON_TOKEN_INTEGER:
                fprintf(stream, "int(%i) ", tokenizer->token.data.intValue);
                break;
            case JSON_TOKEN_FLOAT:
                fprintf(stream, "float(%f) ", tokenizer->token.data.floatValue);
                break;
            case JSON_TOKEN_SYMBOL:
                fprintf(stream, "sym(%s) ", tokenizer->token.data.string.data);
                break;
            case JSON_TOKEN_BOOL:
                fprintf(stream, "bool(%i) ", tokenizer->token.data.boolValue);
                break;
            case JSON_TOKEN_NULL:
                fprintf(stream, "null ");
                break;
            default:
                break;
        }
        json_token_free(&tokenizer->token);
    }
}

