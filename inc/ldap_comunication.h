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
#include <string>
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
//     sequence - FilterObject
//     sequence - attributes

typedef struct searchRequest {
  int messageIDLength;
  unsigned int sizeLimit;
  searchedAttributesType attributes;
} searchRequestType;

/**
 * @brief Initialize the search result entry envelope
 *
 * @param searchRequest search request envelope for which the search result entry
 * @param LDAPDN LDAPDN of the entry
 * @return BerObject*
 */
BerObject *InitSearchResultEntry(BerObject *searchRequest,
                                 std::vector<unsigned char> LDAPDN);

/**
 * @brief Adds an attribute to the search result entry envelope
 *
 * @param envelope search result entry envelope
 * @param attributeDescription
 * @param attributeValue
 * @return int
 */
int AddToSearchResultEntry(BerObject *envelope,
                           std::vector<unsigned char> &attributeDescription,
                           std::vector<unsigned char> &attributeValue);
/**
 * @brief checks if the search request is valid
 *
 * @param searchRequest
 * @return int 0 if valid, -1 if inavalid application sequence, -2 if invalid message id or whole envelope
 */
int checkSearchRequest(BerObject *searchRequest);

/**
 * @brief sends notice of disconnection to the client
 *
 * @param comSocket socket to send the notice to
 * @param errCode error code
 * @return int
 */
int sendNoticeOfDisconnection(int comSocket, char errCode);

/**
 * @brief sends search result entry to the client
 *
 * @param envelope search request envelope
 * @param comSocket socket to send the envelope to
 * @return int
 */
int searchRequestHandler(BerObject *searchRequest, int comm_socket,
                         const char *dbPath);

/**
 * @brief Create a Bind Response object
 *
 * @param bindRequest
 * @param resultCode
 * @return BerObject*
 */
BerObject *CreateBindResponse(BerObject *bindRequest, int resultCode);

/**
 * @brief loads the envelope from the client, waits until all the date are received
 *
 * @param bindRequest returns the envelope as a vector of unsigned chars
 * @param comm_socket socket to receive the envelope from
 * @return int 0 if success, -1 if error ocured
 */
int loadEnvelope(std::vector<unsigned char> &bindRequest, int comm_socket) ;

/**
 * @brief sends the search result done envelope to the client
 *
 * @param searchRequest  search request envelope for which the search result done is
 * @param comm_socket  socket to send the envelope to
 * @param result_code
 * @return int
 */
int sendSearchResultDone(BerSequenceObject *searchRequest, int comm_socket,
                         unsigned int result_code);
#endif