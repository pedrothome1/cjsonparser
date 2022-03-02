#include <stdio.h>
#include "parser.h"

int main() {
  FILE *f = fopen("example.json", "r");
  
  parse("example.json");

  fclose(f);
}
