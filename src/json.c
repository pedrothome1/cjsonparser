#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "json.h"

#define BUF_SIZE  100

#define BUF_LAST  BUF_SIZE-1
#define NULL_LEN  4
#define TRUE_LEN  4
#define FALSE_LEN 5

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

  c = fgetc(fp);

  while (c != EOF) {
    b = 0; /* flush buffer */

    if (c == '{' || c == '}' || c == '[' || c == ']' || c == ',' || isspace(c)) {
      while ((c = fgetc(fp)) != EOF && isspace(c))
        ;
    }
    else if (c == '"') {
      while ((c = fgetc(fp)) != EOF) {
        if (b > BUF_LAST)
          goto END;

        if (c == '\\') {
          c = fgetc(fp);
          if (!isvalid_escape(c)) {
            goto END;
          }
          c = escape(c);
        }
        else if (c == '"') {
          buf[b] = '\0';
          break;
        }
        buf[b++] = c;
      }
      
      while ((c = fgetc(fp)) != EOF && isspace(c))
        ;

      if (c == ':') {
        printf("Key: %s\n", buf);
        while ((c = fgetc(fp)) != EOF && isspace(c))
          ;
      }
      else if (c == ',' || c == '}' || c == ']') {
        printf("Value: %s\n\n", buf);
      } else {
        goto END;
      }
    }
    else if (isdigit(c)) {
      if (b <= BUF_LAST)
        buf[b++] = c;
      /* We are not really validating the number */
      /* If it's not valid will be 0.0 because of atof */
      while ((c = fgetc(fp)) != EOF) {
        if (b > BUF_LAST)
          goto END;

        if (isdigit(c) || c == '.') {
          buf[b++] = c;
        }
        else {  /* May be a comma or a whitespace */
          buf[b] = '\0';
          break;
        }
      }
      double num = atof(buf);
      printf("Value: %f\n\n", num);
    }
    else if (c == 'n') {  /* null */
      if (b > BUF_LAST - NULL_LEN)
        goto END;
      buf[b++] = c;
      buf[b++] = fgetc(fp);
      buf[b++] = fgetc(fp);
      buf[b++] = fgetc(fp);
      buf[b] = '\0';
      if (strcmp(buf, "null") != 0)
        goto END;
      c = fgetc(fp);
      printf("Value: %s\n\n", buf);
    }
    else if (c == 't') {  /* true */
      if (b > BUF_LAST - TRUE_LEN)
        goto END;
      buf[b++] = c;
      buf[b++] = fgetc(fp);
      buf[b++] = fgetc(fp);
      buf[b++] = fgetc(fp);
      buf[b] = '\0';
      if (strcmp(buf, "true") != 0)
        goto END;
      c = fgetc(fp);
      printf("Value: %s\n\n", buf);
    }
    else if (c == 'f') {  /* false */
      if (b > BUF_LAST - FALSE_LEN)
        goto END;
      buf[b++] = c;
      buf[b++] = fgetc(fp);
      buf[b++] = fgetc(fp);
      buf[b++] = fgetc(fp);
      buf[b++] = fgetc(fp);
      buf[b] = '\0';
      if (strcmp(buf, "false") != 0)
        goto END;
      c = fgetc(fp);
      printf("Value: %s\n\n", buf);
    }
    else {
      printf("Invalid token `%c`\n", c);
      goto END;
    }
  }

  retcode = 1; /* Success */

END:
  fclose(fp);

  return retcode;
}