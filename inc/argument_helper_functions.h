#ifndef ARGUMENT_HELPER_FUNCTIONS_H
#define ARGUMENT_HELPER_FUNCTIONS_H
#include "string.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct args_t {
  char *dbPath;
  int port;
  bool err;
} argsT;

argsT parseArguments(int argc, char **argv);

#endif