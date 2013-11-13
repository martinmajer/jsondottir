/* 
 * File:   json_reader.c
 * Author: Martin
 *
 * Created on 8. listopad 2013, 11:22
 */

#include <stdlib.h>
#include <stdio.h>

#include "json_reader.h"

static int json_reader_string_nextChar(void ** data) {
    return *(*(char**)data)++;
}

/* Creates a new string reader. */
json_reader json_reader_string(const char * string) {
    json_reader reader;
    reader.data = (void*)string;
    reader.nextChar = json_reader_string_nextChar;
    return reader;
}

static int json_reader_stream_nextChar(void ** data) {
    return fgetc((FILE*)*data);
}

/* Creates a new unbuffered stream reader. */
json_reader json_reader_stream(FILE * stream) {
    json_reader reader;
    reader.data = stream;
    reader.nextChar = json_reader_stream_nextChar;
    return reader;
}
