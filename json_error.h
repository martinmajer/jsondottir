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

#ifndef JSON_ERROR_H
#define	JSON_ERROR_H

#include <stdlib.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct JSON_ERROR {
    int code;
    int line;
    int pos;
} json_error;

static const json_error JSON_ERROR_EMPTY = { 0, 0, 0 };


enum {
    JSON_ERROR_UNEXPECTED_TOKEN_END,
    JSON_ERROR_UNEXPECTED_CHARACTER,
    JSON_ERROR_STR_UNEXPECTED_EOF,
    JSON_ERROR_STR_UNEXPECTED_CTRL,
    JSON_ERROR_STR_INVALID_ESCAPE,
    JSON_ERROR_STR_INVALID_UNICODE,
    JSON_ERROR_GARBAGE,
    JSON_ERROR_UNEXPECTED_EOF,
    JSON_ERROR_UNRESOLVED_TOKEN,
    JSON_ERROR_EXPECTED_STRING,
    JSON_ERROR_EXPECTED_COLON,
    JSON_ERROR_EXPECTED_COMMA_OR_CLOSING_BRACE,
    JSON_ERROR_EXPECTED_COMMA_OR_CLOSING_BRACKET
};


extern const char * json_error_message[];


#ifdef	__cplusplus
}
#endif

#endif	/* JSON_ERROR_H */

