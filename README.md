# Jsondottir

Jsondottir is a lightweight and fast JSON parser for C (C99), with a nice C++ interface.
It doesn't depend on any other libraries. It can be compiled as a shared library,
or included directly to your project as source code.

It is published under the BSD (new) license.

## Basic usage

Include the required header file:

    #include "json.h"
    
Parse a JSON string:
    
    json_object * obj = json_parse_string("{\"hello\": \"world\"}", NULL);
    
Or parse a JSON file:

    json_object * obj = json_parse_file("file.json", NULL);
    
Handle errors:

    json_error error;
    json_object * obj = json_parse_file("file.json", &error);
    
    if (obj == NULL) { // parse error
        printf("JSON error: %s\n", json_error_message[error.code]);
        printf("at %i:%i\n", error.line, error.pos);
    }
    
Retrieve the data:

    json_object * datasheet = json_parse_string("{\"persons\":[{\"name\": \"Martin\", \"age\": 23, \"married\": false}, {\"name:\": \"John\", \"age\": 32, \"married\": true}]}", NULL);
    
    if (!datasheet || datasheet->type != JSON_OBJECT_MAP) return 0;
    json_object * persons = json_map_get(datasheet, "persons");
    
    if (!persons || persons->type != JSON_OBJECT_ARRAY) return 0;
    json_object * firstGuy = json_array_get(persons, 0);
    
    if (!firstGuy || firstGuy->type != JSON_OBJECT_MAP) return 0;
    
    json_object * name = json_map_get(firstGuy, "name");
    json_object * age = json_map_get(firstGuy, "age");
    json_object * married = json_map_get(firstGuy, "married");
    
    if (name && name->type == JSON_OBJECT_STRING) {
        printf("Name: %s\n", json_string_value(name));
    }
    if (age && age->type == JSON_OBJECT_INT) {
        printf("Age: %s\n", json_int_value(age));
    }
    if (married && married->type == JSON_OBJECT_BOOL) {
        printf("Married: %s\n", json_bool_value(married) ? "yes" : "no");
    }
    
    json_object_free(datasheet);
    
Don't forget to free the memory:

    json_object * obj = json_parse_file("file.json", NULL);
    // ...
    
    json_object_free(obj); 
    
    
## Fancy using C++?

    #include "json_cpp.hpp"

    json_object * obj = json_parse_file("datasheet.json", NULL);
    if (obj == NULL) return 0;
    
    try {
        json_cpp datasheet = obj;
        json_cpp firstGuy = datasheet["persons"][0];
        std::cout << "Name: " << (char*)firstGuy["name"] << std::endl;
        std::cout << "Age: " << (int)firstGuy["age"] << std::endl;
        std::cout << "Married: " << ((bool)firstGuy["married"] ? "yes" : "no") << std::endl;
    }
    catch (json_undefined e) {
        // array or map value not found
    }
    catch (json_type_error e) {
        // invalid type cast
    }
    
    json_object_free(obj);


## Encoding

Only UTF-8 is supported and all string data are stored in regular `char[]` arrays. Unicode sequences (`\uxxxx`)
are converted to UTF-8, however, surrogate pairs are not supported (yet).

## The name

The name is a joke based on Icelandic names, meaning something like "the dauther of the son of J" :)
However, for simplicity, all functions and data structures are prefixed only with `json_`.

## Need more information?    
    
For more information, please visit the repository wiki (under construction, unfortunately - see the header and source files).