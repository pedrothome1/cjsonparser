#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "json.h"
#include "chstack.h"

#define ERROR_CODE  -1
#define SUCCESS_CODE 1

#define BUF_SIZE 512
#define BUF_LAST BUF_SIZE-1

#define B_INC(b) (*(b) >= BUF_LAST) ? BUF_LAST : (*(b))++
#define B_GET(b) (*(b) >= BUF_LAST) ? BUF_LAST : *(b)

#define IN_OBJ 1
#define IN_ARR 2

typedef struct current_pos {
  int c;
  int prev_c;
  int in;
  int prev_in;
} current_pos;

int is_struct_token(int c) {
  switch (c) {
  case '{': case '}':
  case '[': case ']':
  case ',': case ':':
  case '"':
    return 1;
  default:
    return 0;
  }
}

int next(current_pos *cp, FILE *fp) {
  if (is_struct_token(cp->c))
    cp->prev_c = cp->c;
  
  cp->c = fgetc(fp);
  cp->prev_in = cp->in;

  if (cp->c == '{')
    cp->in = IN_OBJ;
  else if (cp->c == '[')
    cp->in = IN_ARR;

  return cp->c;
}

int isvalid_escape(int c) {
  switch (c) {
  case '\\':
  case 'b': case 'f': case 'n':
  case 'r': case 't': case '"':
    return 1;
  default:
    return 0;
  }
}

int escape(int c) {
  switch (c) {
  case '\\': return '\\';
  case 'b':  return '\b';
  case 'f':  return '\f';
  case 'n':  return '\n';
  case 'r':  return '\r';
  case 't':  return '\t';
  case '"':  return '\"';
  default:   return c;
  }
}

int escape_pos(current_pos *cp) {
  int isvalid = isvalid_escape(cp->c);

  if (isvalid)
    cp->c = escape(cp->c);

  return isvalid;
}

int parse(const char *filename) {
  FILE *fp = fopen(filename, "r");
  int retcode = ERROR_CODE;
  int advance = 1;  /* should read next character from the stream? */
  int b = 0;        /* MAX: BUF_LAST */
  char buf[BUF_SIZE];
  current_pos pos = { 0, 0, 0, 0 };
  ch_stack *brackets = cs_init(128);

  next(&pos, fp);

  while (pos.c != EOF) {
    b = 0; /* flush buffer */

    if (pos.c == '{' || pos.c == '[') {
      cs_push(brackets, pos.c);
    }
    else if (pos.c == '}' || pos.c == ']') {
      if ((cs_isempty(brackets)) ||
          (cs_peek(brackets) != '{' && pos.c == '}') ||
          (cs_peek(brackets) != '[' && pos.c == ']') ||
          (pos.prev_c == ','))
        goto END;
      cs_pop(brackets);
    }
    else if (pos.c == '"') {
      while ((next(&pos, fp)) != EOF) {
        if (pos.c == '\\') {
          next(&pos, fp);
          if (!escape_pos(&pos)) {
            goto END;
          }
        }
        else if (pos.c == '"') {
          buf[B_GET(&b)] = '\0';
          break;
        }
        buf[B_INC(&b)] = pos.c;
      }
      printf("String: %s\n", buf);
    }
    else if (isdigit(pos.c)) {
      advance = 0;  /* because the last character read here is not part of the value */
      buf[B_INC(&b)] = pos.c;
      
      /* We are not really validating the number */
      /* If it's not valid will be 0.0 because of atof */
      while ((next(&pos, fp)) != EOF) {
        if (isdigit(pos.c) || pos.c == '.') {
          buf[B_INC(&b)] = pos.c;
        }
        else {
          buf[B_GET(&b)] = '\0';
          break;
        }
      }

      double num = atof(buf);
      printf("Number: %f\n", num);
    }
    else if (pos.c == 'n') {  /* null */
      char tmp[] = { pos.c, fgetc(fp), fgetc(fp), fgetc(fp), '\0' };
      if (strcmp(tmp, "null") != 0)
        goto END;
      printf("null\n");
    }
    else if (pos.c == 't') {  /* true */
      char tmp[] = { pos.c, fgetc(fp), fgetc(fp), fgetc(fp), '\0' };
      if (strcmp(tmp, "true") != 0)
        goto END;
      printf("true\n");
    }
    else if (pos.c == 'f') {  /* false */
      char tmp[] = { pos.c, fgetc(fp), fgetc(fp), fgetc(fp), fgetc(fp), '\0' };
      if (strcmp(tmp, "false") != 0)
        goto END;
      printf("false\n");
    }
    else if (pos.c != ',' && pos.c != ':' && !isspace(pos.c) ||
             pos.c == ':' && pos.prev_c != '"' ||
             pos.c == ',' && pos.prev_c == ',') {
      printf("Invalid token `%d`\n", pos.c);
      goto END;
    }

    if (advance == 0)
      advance = 1;
    else
      next(&pos, fp);
  }

  retcode = SUCCESS_CODE;

END:
  if (retcode == ERROR_CODE) {
    fprintf(stderr, "Error: invalid JSON.\n");
  }

  fclose(fp);
  cs_destroy(brackets);

  return retcode;
}
