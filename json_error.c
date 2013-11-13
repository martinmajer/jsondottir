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

#include "json_error.h"

const char * json_error_message[] = {
    [JSON_ERROR_UNEXPECTED_TOKEN_END] = "Unexpected token end",
    [JSON_ERROR_UNEXPECTED_CHARACTER] = "Unexpected character",
    [JSON_ERROR_STR_UNEXPECTED_EOF] = "Unexpected EOF while parsing string",
    [JSON_ERROR_STR_UNEXPECTED_CTRL] = "Unexpected control character in string",
    [JSON_ERROR_STR_INVALID_ESCAPE] = "Invalid escape sequence",
    [JSON_ERROR_STR_INVALID_UNICODE] = "Invalid unicode sequence",
    [JSON_ERROR_GARBAGE] = "Trailing garbage at the end of file",
    [JSON_ERROR_UNEXPECTED_EOF] = "Unexpected EOF",
    [JSON_ERROR_UNRESOLVED_TOKEN] = "Unresolved token",
    [JSON_ERROR_EXPECTED_STRING] = "String expected",
    [JSON_ERROR_EXPECTED_COLON] = "Colon ':' expected",
    [JSON_ERROR_EXPECTED_COMMA_OR_CLOSING_BRACE] = "Comma ',' or closing brace '}' expected",
    [JSON_ERROR_EXPECTED_COMMA_OR_CLOSING_BRACKET] = "Comma ',' or closing bracket ']' expected"
};
