#ifndef LOG_H
#define LOG_H

#define SIZE_COMMMAND 1024

struct command_log_t {
  char* log;
  int index;
  int count;
  int lenght;
};

void command_log_init(struct command_log_t* log);
void command_log_free(struct command_log_t* log);
void command_log_add(struct command_log_t* log, char* command);
char* command_log_get_prev(struct command_log_t* log);
char* command_log_get_next(struct command_log_t* log);

#endif  // LOG_H
