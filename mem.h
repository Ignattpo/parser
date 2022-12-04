#ifndef MEM_H
#define MEM_H

/* Series of functions used to add memory store functionality to the calculator
 */

// Store variable names
char* variable_names[100];
// Flags for if the variables have been set
int variable_set[100];
// Number of variables defined
int variable_counter = 0;
// Store values of the variables
long variable_values[100];

char* get_string(const char* var) {
  int len = strlen(var);
  char* res = malloc(sizeof(char) * (len - 2));
  memcpy(res, &var[1], len - 2);
  res[len - 2] = '\0';
  return res;
}

/* Add a variable name to the memory store */
int get_variable(char* var_name) {
  int i;  // Index var

  /* Search for the variable and return its index if found */
  for (i = 0; i < variable_counter; i++) {
    if (strcmp(var_name, variable_names[i]) == 0) {
      return i;
    }
  }
  return -1;
}

/* Add a variable name to the memory store */
int add_variable(char* var_name) {
  int i;  // Index var
  /* Search for the variable and return its index if found */
  for (i = 0; i < variable_counter; i++) {
    if (strcmp(var_name, variable_names[i]) == 0) {
      return i;
    }
  }

  /* Variable not found yet. */
  /* Define it and add it to the end of the array. */
  variable_counter++;
  variable_names[i] = strdup(var_name);
  return i;
}

/* Set a variables value in the memory store */
int set_variable(int index, long val) {
  variable_values[index] = val;
  variable_set[index] = 1;

  return val;
}

#endif
