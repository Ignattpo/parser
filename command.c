#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "command.h"
#include "log.h"

#include "parser.h"

#include "lexer.h"

static enum command_type_t get_command_type(char* command) {
  if (!strcmp(command, "log")) {
    return COMMAND_LOG_SHOW;
  }
  if (!strcmp(command, "!!")) {
    return COMMAND_LOG_PREV;
  }
  if (!strcmp(command, "!#")) {
    return COMMAND_LOG_NEXT;
  }
  if (!strcmp(command, "clear")) {
    return COMMAND_LOG_CLEAR;
  }

  return COMMAND_CALC;
}

void command_parser_init(struct parser_command_t* parser, int socket) {
  parser->state = STATE_UNKNOWN;
  parser->socket = socket;
  command_log_init(&parser->log);
  variables_init(&parser->variables);
}

void command_parser_free(struct parser_command_t* parser) {
  command_log_free(&parser->log);
  variables_free(&parser->variables);
}

void set_parser_state(struct parser_command_t* parser,
                      char* buff,
                      size_t buff_size) {
  char c[255];
  memcpy(c, buff, buff_size);
  c[buff_size - 2] = '\0';

  if (strcmp(c, "f") == 0 || strcmp(c, "F") == 0) {
    parser->state = STATE_FILE;
    char* string_out = "Ok, please tell me the name of the file: \n";
    send(parser->socket, string_out, strlen(string_out), 0);
  } else if (strcmp(c, "c") == 0 || strcmp(c, "C") == 0) {
    parser->state = STATE_COMMAND;
    char* string_out = "Ok, command line it is!\n";
    send(parser->socket, string_out, strlen(string_out), 0);
  } else {
    parser->state = STATE_UNKNOWN;
    char* string_out = "Command line or File? (Enter C or F): \n";
    send(parser->socket, string_out, strlen(string_out), 0);
  }
}

void command_parser(struct parser_command_t* parser,
                    char* buff,
                    size_t buff_size) {
  if (parser->state == STATE_UNKNOWN) {
    set_parser_state(parser, buff, buff_size);
    return;
  }
  char string_out[1024];
  char command[SIZE_COMMMAND];
  memcpy(command, buff, buff_size);
  command[buff_size - 2] = '\0';

  if (parser->state == STATE_FILE) {
    yyin = fopen(command, "r");
    if (!yyin) {
      sprintf(string_out, "ERROR: Couldn't open file %s\n", command);
      send(parser->socket, string_out, strlen(string_out), 0);
      return;
    }

    yyparse(parser);
    yylex_destroy();
    parser->state = STATE_UNKNOWN;

    sprintf(string_out, "All done with %s\n", command);
    send(parser->socket, string_out, strlen(string_out), 0);
    return;
  }
  if (parser->state == STATE_COMMAND) {
    enum command_type_t command_type = get_command_type(command);
    switch (command_type) {
      case COMMAND_LOG_NEXT:
        sprintf(string_out, "%s\n", command_log_get_next(&parser->log));
        send(parser->socket, string_out, strlen(string_out), 0);
        break;

      case COMMAND_LOG_PREV:
        sprintf(string_out, "%s\n", command_log_get_prev(&parser->log));
        send(parser->socket, string_out, strlen(string_out), 0);
        break;

      case COMMAND_LOG_SHOW:
        for (int i = 0; i < parser->log.count; i++) {
          sprintf(string_out, "%s\n", &parser->log.log[i * SIZE_COMMMAND]);
          send(parser->socket, string_out, strlen(string_out), 0);
        }
        break;

      case COMMAND_LOG_CLEAR:
        command_log_free(&parser->log);
        command_log_init(&parser->log);
        break;

      case COMMAND_CALC: {
        command_log_add(&parser->log, command);
        size_t command_size = strlen(command);
        command[command_size] = '\n';
        command[command_size + 1] = '\0';
        command[command_size + 2] = '\0';
        YY_BUFFER_STATE buffer = yy_scan_string(command);
        yyparse(parser);
        yy_delete_buffer(buffer);
        yylex_destroy();

        break;
      }
    }
  }
}
