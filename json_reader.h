/* 
 * File:   json_reader.h
 * Author: Martin
 *
 * Created on 6. listopad 2013, 23:44
 */

#ifndef JSON_READER_H
#define	JSON_READER_H

#include <stdio.h>

#ifdef	__cplusplus
extern "C" {
#endif

/* Structure for a generic character reader. */
typedef struct JSON_READER {
    int (* nextChar)(void ** data); // reads the next character
    void * data; // private data
} json_reader;

/* Creates a new string reader. */
extern json_reader json_reader_string(const char * string);

/* Creates a new stream reader. */
extern json_reader json_reader_stream(FILE * stream);


#ifdef	__cplusplus
}
#endif

#endif	/* JSON_READER_H */

