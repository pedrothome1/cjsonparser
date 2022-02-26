#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "json.h"

#define BUFSZ 100

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

/* I'm not checking for the bounds of the buffers */
int parse(const char *filename) {
  FILE *fp = fopen(filename, "r");
  int c, retcode = -1; /* -1 is error */
  int b = 0;
  char buf[BUFSZ];

  /* Here we begin to read the chars of the file */
  /* Skip whitespaces */
  while ((c = fgetc(fp)) != EOF && isspace(c))
    ;

  while (c != EOF) {
    /* We got to a {, so is probably an object */
    if (c == '{') {
      /* Read what's inside the "object" */

      do {
        /* Skip whitespaces and { */
        while ((c = fgetc(fp)) != EOF && (isspace(c) || c == '{'))
          ;

        /* If we got to something that is not a double-quote, return with error */
        /* Because the first thing an object has inside is a quoted key */
        if (c != '"') {
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
            c = escape(c);
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

        /* ======= Parsing the value ======= */
        /*
          Be careful so that `c` is consistent after parsing
          any of the types. In this case, it will be the
          character next to the last character of the value
          being parsed.
        */
        if (c == '"') {
          while ((c = fgetc(fp)) != EOF) {
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
          c = fgetc(fp);
          printf("Value: %s\n\n", buf);
        }
        /* Warning: Potential bug */
        else if (isdigit(c)) {
          buf[b++] = c;
          /* We are not really validating the number */
          /* If it's not valid will be 0.0 because of atof */
          while ((c = fgetc(fp)) != EOF) {
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

        b = 0;  /* flush buffer */

        /* Skip whitespaces and closing curly braces/square brackets */
        while (isspace(c) || c == '}' || c == ']')
          c = fgetc(fp);

      } while (c == ',');
    }
    else if (c == '[') {
      do {
        /* Skip whitespaces */
        while ((c = fgetc(fp)) != EOF && isspace(c))
          ;
        
        /* ======= Parsing the value ======= */
        /*
          Be careful so that `c` is consistent after parsing
          any of the types. In this case, it will be the
          character next to the last character of the value
          being parsed.
        */
        if (c == '"') {
          while ((c = fgetc(fp)) != EOF) {
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
          c = fgetc(fp);
          printf("Value: %s\n\n", buf);
        }
        /* Warning: Potential bug */
        else if (isdigit(c)) {
          buf[b++] = c;
          /* We are not really validating the number */
          /* If it's not valid will be 0.0 because of atof */
          while ((c = fgetc(fp)) != EOF) {
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

        b = 0;  /* flush buffer */

        while (isspace(c))
          c = fgetc(fp);
        
      } while (c == ',');
    }
  }

  retcode = 1; /* Success */

END:
  fclose(fp);

  return retcode;
}
