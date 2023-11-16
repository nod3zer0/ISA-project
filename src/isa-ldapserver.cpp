#include "inc/server.h"
#include "inc/argument_helper_functions.h"

int main(int argc, const char *argv[]) {

  args_t args = parseArguments(argc, argv);

  if (args.err) {
    fprintf(stderr, "Error parsing arguments\n");
    return 1;
  }
  ldapServer(args.port, args.dbPath);
}

