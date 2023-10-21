#ifndef LDAP_COMUNICATION_H
#define LDAP_COMUNICATION_H
#include "inc/AndFilterObject.h"
#include "inc/BerBoolObject.h"
#include "inc/BerEnumObject.h"
#include "inc/BerIntObject.h"
#include "inc/BerObject.h"
#include "inc/BerParser.h"
#include "inc/BerSequenceObject.h"
#include "inc/BerSetObject.h"
#include "inc/BerStringObject.h"
#include "inc/BerUndefinedObject.h"
#include "inc/DatabaseController.h"
#include "inc/DatabaseObject.h"
#include "inc/EqualityMatchFilterObject.h"
#include "inc/FilterObject.h"
#include "inc/NotFilterObject.h"
#include "inc/OrFilterObject.h"
#include "inc/SubstringFilterObject.h"
#include "inc/ber_constants.h"
#include "inc/ber_helper_functions.h"
#include "inc/database_helper_functions.h"
#include "inc/filter_helper_functions.h"
#include <algorithm>
#include <arpa/inet.h>
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
#include <vector>
#include <string>

typedef struct searchedAttributes {
  bool cn;
  bool email;
  bool uid;
} searchedAttributesType;

// enum for attributes (cn, email, uid)
typedef enum { cn, email, uid } atributeDescriptions;

// sequence - envelope
//     int   - message ID
//     application 3 - search request
//     octed string - base object
//     enum - scope
//     enum - derefAliases
//     int - sizeLimit
//     int - timeLimit
//     bool - typesOnly
//     sequence - filter
//     sequence - attributes

typedef struct searchRequest {
  int messageIDLength;
  unsigned int sizeLimit;
  searchedAttributesType attributes;
} searchRequestType;

/// @brief Inicializes and alocates partial empty attribute list
/// @param partialAttributeList pointer to empty attribute list
/// @return 0 if success, -1 if error

BerObject *CreateBindResponse(BerObject *bindRequest, int resultCode);

int sendSearchResultDone(BerSequenceObject *searchRequest, int comm_socket,
                         unsigned int result_code);
int searchRequestHandler(BerObject *searchRequest, int comm_socket,
                         const char *dbPath);
int loadEnvelope(std::vector<unsigned char> &bindRequest, int comm_socket);

int copyMessageIDappend(std::vector<unsigned char>::iterator messageID,
                        std::vector<unsigned char> &target);
#endif