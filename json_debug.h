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

#ifndef JSON_DEBUG_H
#define	JSON_DEBUG_H

#include <stdio.h>
#include "json_tokenizer.h"

#ifdef	__cplusplus
extern "C" {
#endif

// #define JSON_MALLOC_LOG
    
#ifdef JSON_MALLOC_LOG
#define JSON_MALLOC_DUMP printf("malloc %s:%i\n", __FILE__, __LINE__)
#define JSON_FREE_DUMP printf("free %s:%i\n", __FILE__, __LINE__)
#else
#define JSON_MALLOC_DUMP
#define JSON_FREE_DUMP
#endif

#ifdef JSON_DEBUG
extern int json_debug_memblocks;
extern int json_debug_objects;
#define JSON_DEBUG_MALLOC json_debug_memblocks++; JSON_MALLOC_DUMP;
#define JSON_DEBUG_FREE json_debug_memblocks--; JSON_FREE_DUMP;
#define JSON_DEBUG_OBJECT_NEW json_debug_objects++
#define JSON_DEBUG_OBJECT_FREE json_debug_objects--
#else
#define JSON_DEBUG_MALLOC
#define JSON_DEBUG_FREE
#define JSON_DEBUG_OBJECT_NEW
#define JSON_DEBUG_OBJECT_FREE
#endif


extern void json_tokenizer_dumpTokens(json_tokenizer * tokenizer, FILE * stream);


#ifdef	__cplusplus
}
#endif

#endif	/* JSON_DEBUG_H */

