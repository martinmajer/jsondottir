/* 
 * File:   json_cpp.hpp
 * Author: Martin
 *
 * Created on 11. listopad 2013, 0:09
 */

#ifndef JSON_CPP_HPP
#define	JSON_CPP_HPP

#include <string>

#include "json_object.h"

/* C++ interface for the json_object type */

/*static inline json_object * operator[](const json_object * obj, const char * key) {
    return json_map_get(obj, key);
}*/

typedef const char * _json_const_char_ptr;

class json_undefined {
    
};

class json_type_error {
    
};

class json_cpp {
private:
    const json_object * obj;
public:
    json_cpp(const json_object * obj): obj(obj) {
        if (obj == NULL) throw json_undefined();
    };
    
    json_cpp operator =(const json_object * obj) { return json_cpp(obj); };
    
    json_cpp operator[](const char * key) const { 
        if (obj->type != JSON_OBJECT_MAP) throw json_type_error();
        return json_cpp(json_map_get(obj, key));
    };
    
    json_cpp operator[](int index) const { 
        if (obj->type != JSON_OBJECT_ARRAY) throw json_type_error();
        return json_cpp(json_array_get(obj, index));
    };
    
    operator int() const { 
        if (obj->type != JSON_OBJECT_INT) throw json_type_error();
        return json_int_value(obj);
    };
    
    operator float() const { 
        if (obj->type != JSON_OBJECT_FLOAT) throw json_type_error();
        return json_float_value(obj);
    };
    
    operator bool() const {
        if (obj->type != JSON_OBJECT_BOOL) throw json_type_error();
        return json_bool_value(obj);
    };
    
    operator _json_const_char_ptr() const {
        if (obj->type != JSON_OBJECT_STRING) throw json_type_error();
        return json_string_value(obj);
    };
    
    bool isNull() const { 
        return obj->type == JSON_OBJECT_NULL; 
    };
    
    json_object_type type() const {
        return obj->type;
    };
    
    int size() const { 
        if (obj->type == JSON_OBJECT_ARRAY) return json_array_size(obj);
        else if (obj->type == JSON_OBJECT_MAP) return json_map_size(obj);
        else throw json_type_error();
    };
    
};


#endif	/* JSON_CPP_HPP */

