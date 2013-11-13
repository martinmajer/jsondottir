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

