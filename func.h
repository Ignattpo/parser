#ifndef FUNC_H
#define FUNC_H

void resolve_variable(int socket, char* variable);
void resolve_address(int socket, long addr);

void read_variable(int socket, char* type, char* variable);
void read_address(int socket, char* type, long addr);

#endif // FUNC_H
