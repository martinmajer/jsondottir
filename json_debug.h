/* 
 * File:   json_debug.h
 * Author: Martin
 *
 * Created on 8. listopad 2013, 14:42
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

