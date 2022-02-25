#ifndef JSON_H
#define JSON_H

enum jvalue_type {
  JV_OBJECT, JV_ARRAY, JV_NUMBER,
  JV_BOOL, JV_STRING, JV_NULL
};

struct jvalue {
  enum jvalue_type jv_type;

  union {
    double jv_number;
    char jv_bool;
    char *jv_string;
    struct jvalue *jv_next;
  };
};

int parse(const char *filename);

#endif
