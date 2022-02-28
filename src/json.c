#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "json.h"
#include "chstack.h"

#define BUF_SIZE 512
#define BUF_LAST BUF_SIZE-1

#define B_INC(b) (*(b) >= BUF_LAST) ? BUF_LAST : (*(b))++
#define B_GET(b) (*(b) >= BUF_LAST) ? BUF_LAST : *(b)

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
  default:   return 0;
  }
}

/* TODO: Validate bracket balancing */
int parse(const char *filename) {
  FILE *fp = fopen(filename, "r");
  int c, retcode = -1; /* -1 is error */
  
  int b = 0;  /* MAX: BUF_LAST */
  char buf[BUF_SIZE];

  ch_stack *brackets = cs_init(128);

  c = fgetc(fp);

  while (c != EOF) {
    b = 0; /* flush buffer */

    if (isspace(c)) {
      c = fgetc(fp);
      continue;
    }

    if (c == '{' || c == '[') {
      cs_push(brackets, c);
      c = fgetc(fp);
    }
    else if (c == '}' || c == ']') {
      if (cs_isempty(brackets) || (cs_peek(brackets) != '{' && c == '}') || (cs_peek(brackets) != '[' && c == ']'))
        goto END;
      cs_pop(brackets);
      c = fgetc(fp);
    }
    else if (c == ',' || c == ':') {
      c = fgetc(fp);
    }
    else if (c == '"') {
      while ((c = fgetc(fp)) != EOF) {
        if (c == '\\') {
          c = fgetc(fp);
          if (!isvalid_escape(c)) {
            goto END;
          }
          c = escape(c);
        }
        else if (c == '"') {
          buf[B_GET(&b)] = '\0';
          break;
        }
        buf[B_INC(&b)] = c;
      }
      c = fgetc(fp);
      printf("String: %s\n", buf);
    }
    else if (isdigit(c)) {
      buf[B_INC(&b)] = c;
      
      /* We are not really validating the number */
      /* If it's not valid will be 0.0 because of atof */
      while ((c = fgetc(fp)) != EOF) {
        if (isdigit(c) || c == '.') {
          buf[B_INC(&b)] = c;
        }
        else {
          buf[B_GET(&b)] = '\0';
          break;
        }
      }
      double num = atof(buf);
      printf("Number: %f\n", num);
    }
    else if (c == 'n') {  /* null */
      char tmp[] = { c, fgetc(fp), fgetc(fp), fgetc(fp), '\0' };
      if (strcmp(tmp, "null") == 0)
        c = fgetc(fp);
      else
        goto END;
      printf("null\n");
    }
    else if (c == 't') {  /* true */
      char tmp[] = { c, fgetc(fp), fgetc(fp), fgetc(fp), '\0' };
      if (strcmp(tmp, "true") == 0)
        c = fgetc(fp);
      else
        goto END;
      printf("true\n");
    }
    else if (c == 'f') {  /* false */
      char tmp[] = { c, fgetc(fp), fgetc(fp), fgetc(fp), fgetc(fp), '\0' };
      if (strcmp(tmp, "false") == 0)
        c = fgetc(fp);
      else
        goto END;
      printf("false\n");
    }
    else {
      printf("Invalid token `%c`\n", c);
      goto END;
    }
  }

  retcode = 1; /* Success */

END:
  fclose(fp);
  cs_destroy(brackets);

  return retcode;
}
