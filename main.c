#include <stdbool.h>
#include <stdio.h>

#include "lexer.h"
#include "log.h"
#include "parser.h"

enum command_type_t {
  COMMAND_CALC = 0,
  COMMAND_EXIT,
  COMMAND_LOG_SHOW,
  COMMAND_LOG_PREV,
  COMMAND_LOG_NEXT
};

enum command_type_t get_command_type(char* command) {
  if (!strcmp(command, "log")) {
    return COMMAND_LOG_SHOW;
  }
  if (!strcmp(command, "!!")) {
    return COMMAND_LOG_PREV;
  }
  if (!strcmp(command, "!#")) {
    return COMMAND_LOG_NEXT;
  }
  if (!strcmp(command, "exit")) {
    return COMMAND_EXIT;
  }

  return COMMAND_CALC;
}

int main(int argc, char** argv) {
  char c[255];
  bool is_terminated = false;
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
    struct command_log_t log;
    command_log_init(&log);
    char command[SIZE_COMMMAND];
    while (!is_terminated) {
      scanf("%s", command);
      enum command_type_t command_type = get_command_type(command);
      switch (command_type) {
        case COMMAND_LOG_NEXT:
          printf("%s\n", command_log_get_next(&log));
          break;

        case COMMAND_LOG_PREV:
          printf("%s\n", command_log_get_prev(&log));
          break;

        case COMMAND_LOG_SHOW:
          command_log_show(&log);
          break;

        case COMMAND_EXIT:
          is_terminated = true;
          break;

        case COMMAND_CALC: {
          command_log_add(&log, command);
          size_t command_size = strlen(command);
          command[command_size] = '\n';
          command[command_size + 1] = '\0';
          YY_BUFFER_STATE buffer = yy_scan_string(command);
          yyparse();
          yy_delete_buffer(buffer);

          break;
        }
      }
    }
    command_log_free(&log);
  }
}
