#include "inc/argument_helper_functions.h"

argsT parseArguments(int argc, const char **argv) {

  // initialize args
  argsT args;
  args.err = false;
  args.dbPath = (char *)malloc(sizeof(char) * 1000);
  args.port = 389;

  // main loop for parsing arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-f") == 0) {
      // check if there is value for host and if it is not too long
      if (i + 1 >= argc) {
        strcpy(args.dbPath, "");
        return args;
      } else {
        if (strlen(argv[i + 1]) > 1000) {
          args.err = true;
          return args;
        }
        strcpy(args.dbPath, argv[i + 1]);
      }
      i++; // skip next argument
      continue;
    }
    if (strcmp(argv[i], "-p") == 0) {
      // check if there is value for port and if it is in range
      if (i + 1 >= argc || atoi(argv[i + 1]) <= 0 ||
          atoi(argv[i + 1]) > 65535) {
        args.err = true;
        return args;
      }
      args.port = atoi(argv[i + 1]);
      i++; // skip next argument
      continue;
    }
    // check if there is unknown argument
    fprintf(stderr, "Unknown argument: %s\n", argv[i]);
    args.err = true;
    return args;
  }
  return args;
}