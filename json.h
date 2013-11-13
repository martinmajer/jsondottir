/* 
 * File:   json.h
 * Author: Martin
 *
 * Created on 10. listopad 2013, 18:53
 */

#ifndef JSON_H
#define	JSON_H

#include <stdio.h>

#include "json_reader.h"
#include "json_object.h"
#include "json_error.h"


#ifdef	__cplusplus
extern "C" {
#endif

#define JSON_BUFFER_DEFAULT_SIZE 1024
    
    
/* Parses JSON. */
extern json_object * json_parse(json_reader reader, json_error * error);

/* Parses a JSON string. */
extern json_object * json_parse_string(const char * string, json_error * error);

/* Parses a JSON file. */
extern json_object * json_parse_file_buf(const char * filename, bool buffered, int bufferSize, json_error * error);

/* Parses a JSON file. */
inline static json_object * json_parse_file(const char * filename, json_error * error) {
    return json_parse_file_buf(filename, true, JSON_BUFFER_DEFAULT_SIZE, error);
}
    
/* Parses JSON incoming from a steam. */
extern json_object * json_parse_stream(FILE * stream, json_error * error);



#ifdef	__cplusplus
}
#endif

#endif	/* JSON_H */

