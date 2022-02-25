#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "json.h"

#define BUF_SZ 100

#define IN_WSP 0
#define IN_OBJ 1
#define IN_ARR 2
#define IN_STR 3
#define IN_KEY 4
#define IN_VAL 5
#define IN_NUM 6
#define IN_BOL 7
#define IN_NUL 8

int where(int c) {
  if (isspace(c))
    return IN_WSP;
  if (c == '{')
    return IN_OBJ;
  if (c == '[')
    return IN_ARR;
  if (c == '"')
    return IN_STR;
  return -1;
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

/* I'm not checking for the bounds of the buffers */
int parse(const char *filename) {
  FILE *fp = fopen(filename, "r");
  int c, in, retcode = -1; /* -1 is error */
  char buf[BUF_SZ];
  int b = 0;

  /* Here we begin to read the chars of the file */
  /* If we are in white spaces, skip them */
  while ((c = fgetc(fp)) != EOF) {
    in = where(c);

    /* We got to a {, so is probably an object */
    if (in == IN_OBJ) {
      /* Read what's inside the "object" */
      while ((c = fgetc(fp)) != EOF) {
        in = where(c);
        
        /* Skip whitespaces */
        if (in == IN_WSP)
          continue;

        /* If we got to something that is not a double-quote, return with error */
        /* Because the first thing an object has inside is a quoted key */
        if (in != IN_STR) {
          goto END;
        }

        /* ======= Read a key ======= */

        /* Key is a string and can have anything. We should check for escapes */
        while ((c = fgetc(fp)) != EOF) {
          if (c == '\\') {
            c = fgetc(fp);
            if (!isvalid_escape(c)) {
              goto END;
            }
          }
          else if (c == '"') {
            buf[b] = '\0';
            break;
          }
          buf[b++] = c;
        }

        /* An empty key is invalid */
        if (b == 0) {
          goto END;
        }

        printf("Key: %s\n", buf);

        b = 0;  /* flush buffer */

        /* ======= Read a value ======= */

        while ((c = fgetc(fp)) != EOF) {
          if (c == ':')
            break;
          if (isspace(c))
            continue;
          goto END;
        }

        /* Skip whitespaces */
        while ((c = fgetc(fp)) != EOF && isspace(c))
          ;

        if (c == '"') {
          while ((c = fgetc(fp)) != EOF) {
            if (c == '\\') {
              c = fgetc(fp);
              if (!isvalid_escape(c)) {
                goto END;
              }
            }
            else if (c == '"') {
              buf[b] = '\0';
              break;
            }
            buf[b++] = c;
          }
        }
        else if (isdigit(c)) {
          while ((c = fgetc(fp)) != EOF) {
            
          }
        }
        else if (c == 'n') {  /* null */

        }
        else if (c == 't') {  /* true */

        }
        else if (c == 'f') {  /* false */

        }

      }
    }
  }

  retcode = 1; /* Success */

END:
  fclose(fp);

  return retcode;
}
