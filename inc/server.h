#ifndef SERVER_H
#define SERVER_H
#include "inc/BerEnumObject.h"
#include "inc/BerIntObject.h"
#include "inc/BerObject.h"
#include "inc/BerParser.h"
#include "inc/BerSequenceObject.h"
#include "inc/BerSetObject.h"
#include "inc/BerStringObject.h"
#include "inc/DatabaseObject.h"
#include "inc/FilterObject.h"
#include "inc/argument_helper_functions.h"
#include "inc/ldap_comunication.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Macro for printing err message and closing socket when err != 0
#define CHECK_ERR(err, msg)                                                    \
  if (err != 0) {                                                              \
    printf("%s\n", msg);                                                       \
    close(childSocket);                                                        \
    exit(0);                                                                   \
  }

int ldapServer(int port, char *dbPath);

#endif