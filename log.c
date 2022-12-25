#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#define LOG_LENGHT 50

void command_log_init(struct command_log_t* log) {
  log->lenght = LOG_LENGHT;
  log->log = malloc(log->lenght * SIZE_COMMMAND);
  log->index = 0;
  log->count = 0;
}

void command_log_free(struct command_log_t* log) {
  free(log->log);
  log->index = 0;
  log->count = 0;
  log->lenght = 0;
}

void command_log_add(struct command_log_t* log, char* command) {
  if (log->count + 1 >= log->lenght) {
    log->lenght += LOG_LENGHT;
    log->log = realloc(log->log, log->lenght * SIZE_COMMMAND);
  }
  log->count++;
  log->index = log->count - 1;
  memcpy(&log->log[log->index * SIZE_COMMMAND], command, strlen(command));
}

char* command_log_get_prev(struct command_log_t* log) {
  int index = log->index;
  log->index--;
  if (log->index < 0) {
    log->index = 0;
  }
  return &log->log[index * 1024];
}

char* command_log_get_next(struct command_log_t* log) {
  log->index++;
  if (log->index >= log->count) {
    log->index = log->count - 1;
  }
  return &log->log[log->index * SIZE_COMMMAND];
}
