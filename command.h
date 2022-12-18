#ifndef COMMAND_H
#define COMMAND_H
#include <stdlib.h>

#include "log.h"
#include "variables.h"

enum command_type_t {
  COMMAND_CALC = 0,
  COMMAND_LOG_CLEAR,
  COMMAND_LOG_SHOW,
  COMMAND_LOG_PREV,
  COMMAND_LOG_NEXT
};

enum parser_state_t { STATE_UNKNOWN = 0, STATE_COMMAND, STATE_FILE };

struct parser_command_t {
  enum parser_state_t state;
  int socket;
  struct variables_t variables;
  struct command_log_t log;
};

void command_parser_init(struct parser_command_t* parser, int socket);
void command_parser_free(struct parser_command_t* parser);

void command_parser(struct parser_command_t* parser,
                    char* buff,
                    size_t buff_size);

#endif  // COMMAND_H
