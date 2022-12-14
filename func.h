#ifndef FUNC_H
#define FUNC_H

void resolve_variable(int socket, char* variable, long* res);
void resolve_address(int socket, long addr);

void read_variable(int socket, char* type, char* variable, long* res);
void read_address(int socket, char* type, long addr, long* res);

void write_variable(int socket, char* type, char* variable, long data);
void write_address(int socket, char* type, long addr, long data);

void memory_dump_address(int socket, long addr, long byte_count);
void memory_dump_variable(int socket, char* variable, long byte_count);

void memory_write_address(int socket, long addr, char* dump);
void memory_write_variable(int socket, char* variable, char* dump);

void memory_dump_to_file_address(int socket,
                                 char* file_name,
                                 long addr,
                                 long byte_count);
void memory_dump_to_file_variable(int socket,
                                  char* file_name,
                                  char* variable,
                                  long byte_count);

#endif  // FUNC_H
