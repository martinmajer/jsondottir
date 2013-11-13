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

