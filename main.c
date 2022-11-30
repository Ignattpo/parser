#include <stdio.h>

#include "lexer.h"
#include "parser.h"

int main(int argc, char** argv) {
  char c[255];
  printf("Command line or File? (Enter C or F): ");
  scanf("%s", c);
  if (strcmp(c, "f") == 0 || strcmp(c, "F") == 0) {
    // File input
    printf("Ok, please tell me the name of the file: ");
    scanf("%s", c);

    yyin = fopen(c, "r");
    if (!yyin) {
      printf("ERROR: Couldn't open file %s\n", c);
      exit(-1);
    }
    yyparse();

    printf("All done with %s\n", c);
  } else {
    // Command line
    printf("Ok, command line it is!\n");

    yyin = stdin;
    yyparse();
  }
}
