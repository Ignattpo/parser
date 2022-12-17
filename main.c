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
    //    log_open();
    // Command line
    printf("Ok, command line it is!\n");

    char command[1024];
    while (1) {
      scanf("%s", command);
      size_t command_size = strlen(command);
      command[command_size] = '\n';
      command[command_size + 1] = '\0';
      YY_BUFFER_STATE buffer = yy_scan_string(command);
      yyparse();
      yy_delete_buffer(buffer);
    }
    //    log_close();
  }
}
