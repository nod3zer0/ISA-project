
#include "inc/ber_constants.h"
#include "inc/ber_helper_functions.h"
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

typedef struct searchedAttributes {
  bool cn;
  bool email;
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
  std::vector<unsigned char> *messageID;
  int messageIDLength;
  int sizeLimit;
  searchedAttributesType attributes;
} searchRequestType;

/// @brief Inicializes and alocates partial empty attribute list
/// @param partialAttributeList pointer to empty attribute list
/// @return 0 if success, -1 if error

int InitSearchResultEntry(std::vector<unsigned char> &partialAttributeList,
                          std::vector<unsigned char> messageID,
                          std::vector<unsigned char> LDAPDN, int LDAPDNLength);

int AddToSearchResultEntry(std::vector<unsigned char> &partialAttributeList,
                           std::vector<unsigned char> &attributeDescription,
                           int attributeDescriptionLength,
                           std::vector<unsigned char> &attributeValue,
                           int attributeValueLength);
int CreateBindResponse(std::vector<unsigned char> &bindRequest,
                       std::vector<unsigned char> &bindResponse);

int sendSearchResultEntry(unsigned char *searchRequest, int comm_socket);

int searchRequestHandler(std::vector<unsigned char> &searchRequest,
                         int comm_socket);

int loadEnvelope(std::vector<unsigned char> &bindRequest, int comm_socket);