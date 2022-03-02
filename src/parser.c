#include <stdio.h>
#include "parser.h"

int escape(int c) {
  switch (c) {
  case '\\': return '\\';
  case '/':  return '/';
  case 'b':  return '\b';
  case 'f':  return '\f';
  case 'n':  return '\n';
  case 'r':  return '\r';
  case 't':  return '\t';
  case '"':  return '"';
  default:   return -1;
  }
}

int parse(FILE *fp) {
  
}
