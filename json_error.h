/* 
 * File:   json_error.h
 * Author: Martin
 *
 * Created on 11. listopad 2013, 22:39
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

