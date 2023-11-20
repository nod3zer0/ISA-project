/**
 * @file argument_helper_functions.h
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @brief Helper functions for parsing arguments
 * @date 2023-11-19
 */
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

/**
 * @brief Parses the arguments from the command line for ldapserver
 *
 * @param argc count of arguments
 * @param argv values of arguments
 * @return argsT
 */
argsT parseArguments(int argc,const char **argv);

#endif