#ifndef JSON_H
#define JSON_H

typedef enum json_boolnull json_boolnull;
typedef enum json_type json_type;
typedef union json_key json_key;
typedef struct json_entry json_entry;
typedef struct json_value json_value;
typedef struct json_root json_root;

enum json_boolnull {
  TRUE = 1, FALSE, NULL
};

enum json_type {
  NUMBER, STRING, OBJECT, ARRAY, BOOLEAN, NULL
};

union json_key {
  int index;  /* for arrays */
  char *string;
};

struct json_entry {
  json_key key;
  json_value value;
};

struct json_value {
  json_type type;

  union {
    double number;
    char *string;
    json_entry *object;
    json_entry *array;
    json_boolnull bn;
  };
};

struct json_root {
  json_entry *elem;
};

#endif
