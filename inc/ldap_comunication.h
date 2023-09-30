
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
#include "inc/ber_constants.h"
#include "inc/ber_helper_functions.h"


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
  char *messageID;
  int messageIDLength;
  int sizeLimit;
  searchedAttributesType attributes;
} searchRequestType;

/// @brief Inicializes and alocates partial empty attribute list
/// @param partialAttributeList pointer to empty attribute list
/// @return 0 if success, -1 if error
int InitSearchResultEntry(unsigned char **partialAttributeList, char *messageID,
                          unsigned char *LDAPDN, int LDAPDNLength);

int AddToSearchResultEntry(unsigned char **partialAttributeList,
                           unsigned char *attributeDescription,
                           int attributeDescriptionLength,
                           unsigned char *attributeValue,
                           int attributeValueLength);
int CreateBindResponse(unsigned char *bindRequest, unsigned char *bindResponse);

int sendSearchResultEntry(unsigned char *searchRequest, int comm_socket);

int searchRequestHandler(unsigned char *searchRequest, int comm_socket);

int loadEnvelope(unsigned char *bindRequest, int comm_socket);