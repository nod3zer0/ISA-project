/**
 * @file isa-ldapserver.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
#include "inc/argument_helper_functions.h"
#include "inc/server.h"
#include <iostream>
#include <locale>

bool file_exists(char *name) {
  if (FILE *file = fopen(name, "r")) {
    fclose(file);
    return true;
  } else {
    return false;
  }
}

int main(int argc, const char *argv[]) {

  args_t args = parseArguments(argc, argv);
  if (args.err) {
    fprintf(stderr, "Error parsing arguments\n");
    return 1;
  }
  if (!file_exists(args.dbPath)) {
    std::cout << "File doesn't exist!" << std::endl;
    return 1;
  }
  ldapServer(args.port, args.dbPath);
}
