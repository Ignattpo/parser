#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "variables.h"

void variables_init(struct variables_t* variables) {
  variables->counter = 0;
  memset(variables->set, 0, sizeof(variables->set));
  memset(variables->values, 0, sizeof(variables->values));
}

void variables_free(struct variables_t* variables) {
  for (int i = 0; i < variables->counter; i++) {
    free(variables->names[i]);
  }
  variables->counter = 0;
  memset(variables->set, 0, sizeof(variables->set));
  memset(variables->values, 0, sizeof(variables->values));
}

char* get_string(const char* var) {
  int len = strlen(var);
  char* res = malloc(sizeof(char) * (len - 2));
  int new_len = len;
  int j = 0;
  int i = 1;
  for (j = 0, i = 1; i < len - 1; i++, j++) {
    if ((var[i] == '\\') && (var[i + 1] == 'n')) {
      res[j] = '\n';
      i++;
      new_len--;
      continue;
    }
    res[j] = var[i];
  }

  res[new_len - 1] = '\0';
  return res;
}

/* Add a variable name to the memory store */
int get_variable(struct variables_t* variables, char* var_name) {
  int i;  // Index var

  /* Search for the variable and return its index if found */
  for (i = 0; i < variables->counter; i++) {
    if (strcmp(var_name, variables->names[i]) == 0) {
      return i;
    }
  }
  return -1;
}

/* Add a variable name to the memory store */
int add_variable(struct variables_t* variables, char* var_name) {
  int i;  // Index var
  /* Search for the variable and return its index if found */
  for (i = 0; i < variables->counter; i++) {
    if (strcmp(var_name, variables->names[i]) == 0) {
      return i;
    }
  }

  /* Variable not found yet. */
  /* Define it and add it to the end of the array. */
  variables->counter++;
  variables->names[i] = strdup(var_name);
  return i;
}

/* Set a variables value in the memory store */
int set_variable(struct variables_t* variables, int index, long val) {
  variables->values[index] = val;
  variables->set[index] = 1;

  return val;
}
