#ifndef FUNC_H
#define FUNC_H

// Calculate factorial
double factorial(double n) {
  double x;
  double f = 1;

  for (x = 1; x <= n; x++) {
    f *= x;
  }

  return f;
}

// Calculate modulus
int modulo(double x, double y) {
  return (int)x % (int)y;
}

#endif
