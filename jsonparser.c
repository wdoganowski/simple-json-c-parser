/*
 * MIT License http://dogan.mit-license.org/
 */

#include "jsonparser.h"

#define DEBUG_OFF
#include "debug.h"

 typedef enum JSP_ErrorType {
  JSP_OK                      = 0x0,    // OK
  JSP_EMPTY                   = 0x1,    // Empty value 
  JSP_ERROR_NULL_INPUT        = 0x101,  // The NULL pointer passed to function   
  JSP_ERROR_NOT_JSON          = 0x202,  // The JSON needs to start with { or [
  JSP_ERROR_NOT_STRING        = 0x203,  // The string needs to start with "
  JSP_ERROR_NOT_OBJECT        = 0x204,  // The object needs to start with {
  JSP_ERROR_NOT_ARRAY         = 0x205,  // The array needs to start with [
  JSP_ERROR_NOT_FOUND         = 0x206,  // The colon, semicolon or beggining of object or array not found
  JSP_ERROR_UNEXPECTED_QUOTE  = 0x301,  // The primitive should not include "
  JSP_ERROR_UNEXPECTED_CHAR_IN_PRIMITIVE = 0x302,  // The primitive needs to be a number 
  JSP_ERROR_UNEXPECTED_CHAR_IN_OBJECT    = 0x303,  // The object needs to be in format {label:value,label:value} 
  JSP_ERROR_UNEXPECTED_CHAR_IN_ARRAY     = 0x304,  // The array needs to be in format [value,value,value]
  JSP_ERROR_UNEXPECTED_END    = 0x401,  // The json finished too early
  JSP_FINISHED                = 0xFFF,
} JSP_ErrorType;

DEBUG_CODE(void print_value(char* value, uint16_t value_length) { \
    char* v = calloc(value_length + 1, sizeof(char)); \
    snprintf(v, value_length + 1, "%s", value); \
    APP_LOG(APP_LOG_LEVEL_INFO, "      Found value [%d] %s ", value_length, v); \
    free(v); \
});

DEBUG_CODE(void print_pair(char* label, uint16_t label_length, char* value, uint16_t value_length) { \
  char* l = calloc(label_length + 1, sizeof(char)); \
  char* v = calloc(value_length + 1, sizeof(char)); \
  snprintf(l, label_length + 1, "%s", label); \
  snprintf(v, value_length + 1, "%s", value); \
  APP_LOG(APP_LOG_LEVEL_INFO, "Found label [%d] %s : [%d] %s", label_length, l, value_length, v); \
  free(v); free(l); \
});

static JSP_ErrorType json_object_parser(char** json);
static JSP_ErrorType json_array_parser(char** json);

inline JSP_ValueType json_value_type(char* value) {
  switch (value[0]) {
    case '"': return JSP_VALUE_STRING;
    case '{': return JSP_VALUE_OBJECT;
    case '[': return JSP_VALUE_ARRAY;
    default : return JSP_VALUE_PRIMITIVE;
  }
} 

static JSP_ErrorType skip_spaces(char** string) {
  if (*string == NULL) return JSP_ERROR_NULL_INPUT;
  switch (*string[0]) {
    case ' ' : {*string += 1; return skip_spaces(string);};
    default  : return JSP_OK;
  }
}

static JSP_ErrorType skip_character(char to_skip, char** string) {
  JSP_ErrorType error;
  if (*string == NULL) return JSP_ERROR_NULL_INPUT;

  error = skip_spaces(string);
  if (error != JSP_OK) return error;

  if (*string[0] != to_skip) return JSP_ERROR_NOT_FOUND;

  *string += 1;

  error = skip_spaces(string);
  if (error != JSP_OK) return error;
  return JSP_OK;
}

static JSP_ErrorType remaining_string_length(char* string, uint16_t *length) {
  DEEP_DEBUG_CODE(LOG_DEBUG("string length %d %x %c --%s--", *length, string[0], string[0], string));
  switch (string[0]) {
    case '\0': 
      return JSP_ERROR_UNEXPECTED_END;
    case '"' : 
      *length += 1;
      return JSP_OK; // end of string
    default  :
      *length += 1;
      return remaining_string_length(string + 1, length);
  }
}

static JSP_ErrorType string_length(char* string, uint16_t *length) {
  if (!string) return JSP_ERROR_NULL_INPUT;
  switch (string[0]) {
    case '\0' : 
      return JSP_EMPTY; // end of string
    case '"': 
      *length += 1;
      return remaining_string_length(string + 1, length);
    default  : 
      return JSP_ERROR_NOT_STRING;
  }
}

static JSP_ErrorType primitive_length(char* string, uint16_t *length) {
  DEEP_DEBUG_CODE(LOG_DEBUG("primitive length %d %x %c --%s--", *length, string[0], string[0], string));
  if (!string) return JSP_ERROR_NULL_INPUT;
  switch (string[0]) {
    case '\0': 
      return (*length == 0)? JSP_EMPTY : JSP_ERROR_UNEXPECTED_END; // empty
    case ',' : 
    case '}' :
    case ']' :
    case ' ' :
    case '\n':
    case '\r': 
      return (*length)? JSP_OK : JSP_ERROR_UNEXPECTED_END; // end of primitive
   case '"':
      return JSP_ERROR_UNEXPECTED_QUOTE;
    case '.' :
    case '0' :
    case '1' :
    case '2' :
    case '3' :
    case '4' :
    case '5' : 
    case '6' : 
    case '7' : 
    case '8' :
    case '9' :
      *length += 1;
      return primitive_length(string + 1, length);
    default:
      return JSP_ERROR_UNEXPECTED_CHAR_IN_PRIMITIVE;
  }
}

static JSP_ErrorType remaining_object_length(char* string, uint16_t *length, uint8_t *depth) {
  DEEP_DEBUG_CODE(LOG_DEBUG("object length %d depth %d %x %c --%s--", *length, *depth, string[0], string[0], string));
  switch (string[0]) {
    case '\0': 
      return JSP_ERROR_UNEXPECTED_END;
    case '{': // new object
      *length += 1;
      *depth += 1;
      return remaining_object_length(string + 1, length, depth);
    case '}' :
      *length += 1;
      if (*depth) {
        *depth -= 1;
        return remaining_object_length(string + 1, length, depth);
      }
      return JSP_OK; // end of object
    default  :
      *length += 1;
      return remaining_object_length(string + 1, length, depth);
  }
}

static JSP_ErrorType object_length(char* string, uint16_t *length) {
  if (!string) return JSP_ERROR_NULL_INPUT;
  switch (string[0]) {
    case '\0' : 
      return JSP_EMPTY; // end of string
    case '{': {
      uint8_t depth = 0;
      *length += 1;
      return remaining_object_length(string + 1, length, &depth);
    }
    default  : 
      return JSP_ERROR_NOT_OBJECT;
  }
}

static JSP_ErrorType remaining_array_length(char* string, uint16_t *length, uint8_t *depth) {
  DEEP_DEBUG_CODE(LOG_DEBUG("array length %d depth %d %x %c --%s--", *length, *depth, string[0], string[0], string));
  switch (string[0]) {
    case '\0': 
      return JSP_ERROR_UNEXPECTED_END;
    case '[': // new array
      *length += 1;
      *depth += 1;
      return remaining_array_length(string + 1, length, depth);
    case ']' :
      *length += 1;
      if (*depth) {
        *depth -= 1;
        return remaining_array_length(string + 1, length, depth);
      }
      return JSP_OK; // end of object
    default  :
      *length += 1;
      return remaining_array_length(string + 1, length, depth);
  }
}

static JSP_ErrorType array_length(char* string, uint16_t *length) {
  if (!string) return JSP_ERROR_NULL_INPUT;
  switch (string[0]) {
    case '\0' : 
      return JSP_EMPTY; // end of string
    case '[': {
      uint8_t depth = 0;
      *length += 1;
      return remaining_array_length(string + 1, length, &depth);
    }
    default  : 
      return JSP_ERROR_NOT_ARRAY;
  }
}

static JSP_ErrorType value_length(char* string, uint16_t *length) {
  // Requires a pointer to first character of value, without spaces
  if (!string) return JSP_ERROR_NULL_INPUT;
  switch (json_value_type(string)) {
    case JSP_VALUE_OBJECT: return object_length(string, length);
    case JSP_VALUE_ARRAY : return array_length(string, length);
    case JSP_VALUE_STRING: return string_length(string, length);
    default              : return primitive_length(string, length);
  }
}

static JSP_ErrorType json_value_parser(char** json, JSP_ArrayCallback callback) {
  // we can have a primitive, string, object or array value
  // If this is an object, process it, 
  // If this is an array, process the values to see if there are objects in it
  // otherwise skip over it

  skip_spaces(json);

  switch (json_value_type(*json)) {
    case JSP_VALUE_OBJECT: { // if object, process it
      JSP_ErrorType result = json_object_parser(json);
      if (result != JSP_OK) return result;
      break;
    }
    case JSP_VALUE_ARRAY: { // if array, process it
      JSP_ErrorType result = json_array_parser(json);
      if (result != JSP_OK) return result;
      break;
    }
    default: { // otherwise, callback & skip over it
      uint16_t length = 0;
      JSP_ErrorType result = value_length(*json, &length);
      if (result != JSP_OK) return result;

      DEBUG_CODE(print_value(*json, length));

      // Callback
      LOG_DEBUG("CALLBACK value");
      if (callback) callback(json_value_type(*json), *json, length);
      
      *json += length;
    }
  }

  skip_spaces(json);

  return JSP_OK;
}

char* json_pair_parser(char** json, char* label) {
  // we shopuld have pairs label:value, the label needs to be a string, value - string, primitive, object or array
  LOG_DEBUG("json_pair_parser %s", *json);

  uint16_t length = 0;
  JSP_ErrorType result;
  char* label_ptr;
  uint16_t label_len;

  skip_spaces(json);

  result = string_length(*json, &length);
  if (result != JSP_OK) return NULL;

  label_ptr = *json + 1;
  label_len = length - 2;
 
  // process value, and issue call back

  // skip label
  *json += length; 
  // skip semicolon
  result = skip_character(':', json);
  if (result != JSP_OK) return result;
    
  // count the value length 
  length = 0; 
  result = value_length(*json, &length);
  if (result != JSP_OK) return result;

  DEBUG_CODE(print_pair(label_ptr, label_len, *json, length));

  if (strncmp(label, label_ptr, label_len) == 0)
  // Callback
  LOG_DEBUG("CALLBACK pair");
  if (object_callback_ptr) object_callback_ptr(json_value_type(*json), label_ptr, label_len, *json, length);

  // skip the value if it us a primitive or string or go deeper otherwise
  result = json_value_parser(json, NULL);
  if (result != JSP_OK) return result;
  
  skip_spaces(json);

  return JSP_OK;
}

static char* json_object_parser(char** json, char* label) {
  // we should have json object {pair, pair, pair}
  LOG_DEBUG("json_object_parser %s", *json);

  JSP_ErrorType result;

  // skip {
  result = skip_character('{', json);
  if (result != JSP_OK) return NULL;

  do {
    // parse the pair
    char* ptr = json_pair_parser(json, label);
    if (ptr) return ptr;
    
    // now, we may have an end of object or next pair
    DEEP_DEBUG_CODE(LOG_DEBUG("json_object_parser checking character %s ", *json));
    if (*json[0] == ',') {
      *json += 1; 
      continue;
    } else if (*json[0] == '}') {
      *json += 1;
      break; 
    } else return JSP_ERROR_UNEXPECTED_CHAR_IN_OBJECT;
    
  } while (1);


  return JSP_OK;
}

static JSP_ErrorType json_array_parser(char** json) {
  // we should have json array [value, value, value]
  LOG_DEBUG("json_array_parser %s", *json);

  JSP_ErrorType result;

  // skip {
  result = skip_character('[', json);
  if (result != JSP_OK) return result;

  do {
    // parse the value
    result = json_value_parser(json, array_callback_ptr);
    if (result != JSP_OK) return result;

    // now, we may have an end of array or next pair
    DEEP_DEBUG_CODE(LOG_DEBUG("json_array_parser checking character %s ", *json));
    if (*json[0] == ',') {
      *json += 1; 
      continue;
    } else if (*json[0] == ']') {
      *json += 1;
      break; 
    } else return JSP_ERROR_UNEXPECTED_CHAR_IN_ARRAY;
    
  } while (1);

  return JSP_OK;
}

char* json_parser(const char* json, char* label) {
  char* json_ptr = (char*) json;
  if (!json_ptr) return JSP_ERROR_NULL_INPUT;
  if (json_value_type(json_ptr)) return json_object_parser(&json_ptr, label);
  else return NULL;
  }
}