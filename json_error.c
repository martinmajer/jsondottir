/* 
 * File:   json_error.c
 * Author: Martin
 *
 * Created on 11. listopad 2013, 23:11
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
