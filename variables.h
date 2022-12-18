#ifndef MEM_H
#define MEM_H

struct variables_t {
  char* names[100];
  int set[100];
  int counter;
  long values[100];
};

void variables_init(struct variables_t* variables);
void variables_free(struct variables_t* variables);
char* get_string(const char* var);
int get_variable(struct variables_t* variables, char* var_name);
int add_variable(struct variables_t* variables, char* var_name);
int set_variable(struct variables_t* variables, int index, long val);

#endif
