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
#define WELCOME_MSG                                                            \
  "Hi, type anything. To end type 'bye.' at a separate line.\n"

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

void SigCatcher(int n) {
  int pid = wait3(NULL, WNOHANG, NULL);
  printf("Child %d spawned.\n", pid);
}

/// @brief Inicializes and alocates partial empty attribute list
/// @param partialAttributeList pointer to empty attribute list
/// @return 0 if success, -1 if error
int InitSearchResultEntry(unsigned char **partialAttributeList,
                          char *messageID) {
  // 64 04 04 00 30 00

  (*partialAttributeList) = (unsigned char *)malloc(11 + messageID[1]);

  (*partialAttributeList)[0] = 0x30;
  (*partialAttributeList)[1] = 0x09; // length of sequence
  // message ID
  (*partialAttributeList)[2] = 0x02;
  (*partialAttributeList)[3] = messageID[1]; // length of message ID
  (*partialAttributeList)[4] = messageID[0]; // message ID
  // copies messageID to bind response
  // _____________________________________________________________
  int x = 0;
  if (1 < (*partialAttributeList)[3]) {
    x = 1;
    for (; x < messageID[1]; x++) {
      (*partialAttributeList)[4 + x] = messageID[x];
    }
  } // extends length of sequence by length of message ID

  (*partialAttributeList)[1] += x;
  //_____________________________________________________________________________________________
  // error checking
  if ((*partialAttributeList) == NULL) {
    return -1;
  }

  (*partialAttributeList)[5 + x] = 0x64; // Aplication 4 tag
  (*partialAttributeList)[6 + x] = 0x04; // length of PartialAttributeList
  // LDAPN
  (*partialAttributeList)[7 + x] = 0x04; // octet string
  (*partialAttributeList)[8 + x] = 0x00; // length of string
  // sequence
  (*partialAttributeList)[9 + x] = 0x30;
  (*partialAttributeList)[10 + x] = 0x00; // length of sequence
  // TODO
  return 0;
}

int AddToSearchResultEntry(unsigned char **partialAttributeList,
                           unsigned char *attributeDescription,
                           int attributeDescriptionLength,
                           unsigned char *attributeValue,
                           int attributeValueLength) {

  int increaseBy = 8 + attributeValueLength + attributeDescriptionLength;
  unsigned char *newPartialAttributeList =
      (unsigned char *)realloc((*partialAttributeList), increaseBy);
  // error checking
  if (newPartialAttributeList == NULL) {
    return -1;
  }
  (*partialAttributeList) = newPartialAttributeList;

  int lenghtOforiginalList =
      (*partialAttributeList)[1] + 1; // TODO: support ints
  (*partialAttributeList)[1] += increaseBy;
  (*partialAttributeList)[6] += increaseBy;

  // increase length of sequence at the end of partialAttributeList
  (*partialAttributeList)[lenghtOforiginalList] += increaseBy;
  // create sequence at the end
  (*partialAttributeList)[lenghtOforiginalList + 1] = 0x30;
  (*partialAttributeList)[lenghtOforiginalList + 2] = increaseBy - 2;

  // add attribute description
  (*partialAttributeList)[lenghtOforiginalList + 3] = 0x04; // octet string
  (*partialAttributeList)[lenghtOforiginalList + 4] =
      attributeDescriptionLength; // length of string
  for (int i = 0; i < attributeDescriptionLength; i++) {
    (*partialAttributeList)[lenghtOforiginalList + 5 + i] =
        attributeDescription[i];
  }

  (*partialAttributeList)[lenghtOforiginalList + 5 +
                          attributeDescriptionLength] = 0xA0; // set A0
  (*partialAttributeList)[lenghtOforiginalList + 6 +
                          attributeDescriptionLength] =
      attributeValueLength + 2; // length of sequence
  // add attribute value
  (*partialAttributeList)[lenghtOforiginalList + 7 +
                          attributeDescriptionLength] = 0x04; // octet string
  (*partialAttributeList)[lenghtOforiginalList + 8 +
                          attributeDescriptionLength] =
      attributeValueLength; // length of string
  for (int i = 0; i < attributeValueLength; i++) {
    (*partialAttributeList)[lenghtOforiginalList + 9 +
                            attributeDescriptionLength + i] = attributeValue[i];
  }

  return 0;
}

/// @brief Creates bind response from bind request
/// @param bindRequest
/// @param bindResponse
/// @return length of bind response, -1 if error
int CreateBindResponse(unsigned char *bindRequest,
                       unsigned char *bindResponse) {
  // 30 0c 02 01 01 61 07 0a 01 00 04 00 04 00
  // sequence
  bindResponse[0] = 0x30;
  bindResponse[1] = 0x0c; // length of sequence
  // message ID
  bindResponse[2] = 0x02;
  bindResponse[3] = bindRequest[3]; // length of message ID
  bindResponse[4] = bindRequest[4]; // message ID

  // copies messageID to bind response
  // _____________________________________________________________
  int x = 0;
  if (1 < bindRequest[3]) {
    x = 1;
    for (; x < bindRequest[3]; x++) {
      bindResponse[4 + x] = bindRequest[4 + x];
    }
  } // extends length of sequence by length of message ID
  //_____________________________________________________________________________________________

  // bind response
  bindResponse[5 + x] = 0x61; // BindResponse tag
  bindResponse[6 + x] = 0x07; // length of bind response
  // LDAP result
  bindResponse[7 + x] = 0x0A; // enum
  bindResponse[8 + x] = 0x01; // length of enum
  bindResponse[9 + x] = 0x00; // LDAP result code - success (0)
  // matched DN
  bindResponse[10 + x] = 0x04; // octet string
  bindResponse[11 + x] = 0x00; // length of matched DN
  // diagnostic message
  bindResponse[12 + x] = 0x04; // octet string
  bindResponse[13 + x] = 0x00; // length of diagnostic message
  return 14 + x;
}

int sendSearchResultEntry(unsigned char *searchRequest, int comm_socket) {

  // 30 07 02 01 01 64 02 04 00
  char envelope[30];
  // sequence
  envelope[0] = 0x30;
  envelope[1] = 0x07; // length of sequence
  // message ID
  envelope[2] = 0x02;             // todo support bigger message ID
  envelope[3] = searchRequest[3]; // length of message ID
  envelope[4] = searchRequest[4]; // message ID

  // copies messageID to bind response
  // _____________________________________________________________
  int x = 0;
  if (1 < searchRequest[3]) {
    x = 1;
    for (; x < searchRequest[3]; x++) {
      envelope[4 + x] = searchRequest[4 + x];
    }
  } // extends length of sequence by length of message ID
  //_____________________________________________________________________________________________
  return 0;
}

int sendSearchResultDone(unsigned char *searchRequest, int comm_socket) {


  // 30 0c 02 01 02 65 07 0a 01 00 04 00 04 00
  char envelope[30];
  // sequence
  envelope[0] = 0x30;
  envelope[1] = 0x0c; // length of sequence
  // message ID
  envelope[2] = 0x02;             // todo support bigger message ID
  envelope[3] = searchRequest[3]; // length of message ID
  envelope[4] = searchRequest[4]; // message ID

  // copies messageID to bind response
  // _____________________________________________________________
  int x = 0;
  if (1 < searchRequest[3]) {
    x = 1;
    for (; x < searchRequest[3]; x++) {
      envelope[4 + x] = searchRequest[4 + x];
    }
  } // extends length of sequence by length of message ID
  //_____________________________________________________________________________________________

  // 30 0e 02 01 02 65 09 30 07 0a 01 00 04 00 04 00
  // 30 0c 02 01 02 65 07 0a 01 00 04 00 04 00
  //  bind response
  envelope[5 + x] = 0x65;
  envelope[6 + x] = 0x07;

  envelope[7 + x] = 0x0a;
  envelope[8 + x] = 0x01;
  envelope[9 + x] = 0x00;
  envelope[10 + x] = 0x04;
  envelope[11 + x] = 0x00;
  envelope[12 + x] = 0x04;
  envelope[13 + x] = 0x00;

  // print envelope hex values
  printf("envelope: ");
  for (int i = 0; i < 14 + x; i++) {
    printf("%02x ", envelope[i]);
  }

  send(comm_socket, envelope, 14 + x, 0);

  return 1;
}

int sendSearchResultEntry() {
  // TODO:
  return 0;
}

int searchRequestHandler(unsigned char *searchRequest, int comm_socket) {
  // 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17
  // 30 42 02 01 02 63 3d 04 00 0a 01 02 0a 01 00 02 01 00 02 01 00 01 01 00 a0
  // 20  a3 0d 04 03 75 69 64 04 06 72 69 65 6d 61 6e a3 0f 04 03 75 69 64 04 08
  // 65 69 6e 73 74 61 69 6e  30 08 04 02 63 6e 04 02 64 6e
  // sequence - envelope
  //    int   - message ID
  //    application 3 - search request
  //        octed string - base object
  //        enum - scope
  //        enum - derefAliases
  //        int - sizeLimit
  //        int - timeLimit
  //        bool - typesOnly
  //        sequence - filter
  //        sequence - attributes
  searchRequestType sr;
  // inicialization TODO make constructor
  sr.messageIDLength = 0;
  sr.sizeLimit = 0;
  sr.attributes.cn = false;
  sr.attributes.email = false;

  int envelopeLength = searchRequest[1] + 2; // lenght of envelope

  // 3: sequenc, searchRequest[4] lenght of string, 1: string tag
  int skipLength = 2; // skip envelope tag and lenght
  // set message ID
  sr.messageIDLength = searchRequest[skipLength + 1];
  sr.messageID = (char *)malloc(sr.messageIDLength);
  memcpy(sr.messageID, searchRequest + skipLength, sr.messageIDLength + 2);
  // ---------------------------------------------
  skipLength += searchRequest[skipLength + 1] + 2; // skip message ID
  skipLength += 2;                                 // go behind application 3
  skipLength += searchRequest[skipLength + 1] + 2; // skip base object
  skipLength += searchRequest[skipLength + 1] + 2; // skip enum scope
  skipLength += searchRequest[skipLength + 1] + 2; // skip enum derefAliases
  sr.sizeLimit =
      searchRequest[skipLength +
                    2]; // set size limit TODO: support int size limit
  skipLength += searchRequest[skipLength + 1] + 2; // skip int sizeLimit
  skipLength += searchRequest[skipLength + 1] + 2; // skip int timeLimit
  skipLength += searchRequest[skipLength + 1] + 2; // skip bool typesOnly
  skipLength += searchRequest[skipLength + 1] + 2; // skip sequence filter
  // skipLength += searchRequest[skipLength + 1] + 2; // skip sequence
  // attributes

  // gets attribute descriptions, TODO: rework
  skipLength += 2; // go into attributes sequence

  char attributeDescription[200];

  for (; skipLength < envelopeLength;) {

    // get attribute description
    int atrributeLength = 0;
    for (; atrributeLength < searchRequest[skipLength + 1]; atrributeLength++) {
      attributeDescription[atrributeLength] =
          searchRequest[skipLength + 2 + atrributeLength];
    }
    attributeDescription[atrributeLength + 1] = 0;

    if (strcmp(attributeDescription, "cn") == 0) {
      sr.attributes.cn = true;
    }
    if (strcmp(attributeDescription, "email") == 0) {
      sr.attributes.email = true;
    }
    skipLength += 2 + atrributeLength;
  }

  unsigned char *searchResultEntry;
  InitSearchResultEntry(&searchResultEntry, sr.messageID);

  // print searchResultEntry hex values
  printf("searchResultEntry: ");
  for (int i = 0; i < searchResultEntry[1] + 2; i++) {
    printf("%02x ", searchResultEntry[i]);
  }
  printf("\n");

  unsigned char testVal[] = "cn";
  unsigned char testVal2[] = "test";

  AddToSearchResultEntry(&searchResultEntry, testVal, 2, testVal2, 4);

  send(comm_socket, searchResultEntry, searchResultEntry[1] + 2, 0);
  send(comm_socket, searchResultEntry, searchResultEntry[1] + 2, 0);
  sendSearchResultDone(searchRequest, comm_socket);

  // print searchResultEntry hex values
  printf("searchResultEntry: ");
  for (int i = 0; i < searchResultEntry[1] + 2; i++) {
    printf("%02x ", searchResultEntry[i]);
  }
  printf("\n");

  return 0;
}

int loadEnvelope(unsigned char *bindRequest, int comm_socket) {
  unsigned char buff[2048];
  int res = 0;
  for (;;) {
    if (res >= 1024) { // TODO: eroro
      printf("message too long\n");
      break;
    }
    res += recv(comm_socket, buff + res, 1024, 0);
    if (res > 0) {
      // check if message is envelope
      if (buff[0] != 0x30) { // not a sequence //TODO check for bind request
        printf("invalid message\n");
        // TODO respond with error message
        break;
      }
      int length = 0;
      length = buff[1]; // TODO: check if works

      // if whole message received, return response
      if (res >= length + 2) {
        memcpy(bindRequest, buff, length + 2); // returns bind request
        return length + 2;
      }
    } else // error or end of connection
      break;
  }
  return -1;
}

int main(int argc, const char *argv[]) {
  int rc;
  int welcome_socket;
  struct sockaddr_in6 sa;
  struct sockaddr_in6 sa_client;
  char str[INET6_ADDRSTRLEN];
  socklen_t sa_client_len = sizeof(sa_client);
  if ((welcome_socket = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
    perror("socket() failed");
    exit(EXIT_FAILURE);
  }

  memset(&sa, 0, sizeof(sa));
  sa.sin6_family = AF_INET6;
  sa.sin6_addr = in6addr_any;
  sa.sin6_port = htons(11052);
  if ((rc = bind(welcome_socket, (struct sockaddr *)&sa, sizeof(sa))) < 0) {
    perror("bind() failed");
    exit(EXIT_FAILURE);
  }
  if ((rc = listen(welcome_socket, 1)) < 0) {
    perror("listen() failed");
    exit(EXIT_FAILURE);
  }

  signal(SIGCHLD, SigCatcher);

  while (1) {
    int comm_socket =
        accept(welcome_socket, (struct sockaddr *)&sa_client, &sa_client_len);
    if (comm_socket <= 0)
      continue;

    int pid = fork();
    if (pid < 0) {
      perror("fork() failed");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) // new process to maintain client's requests:
    {
      int child_pid = getpid();
      close(welcome_socket); // not necessary in child process
      printf("New connection (maintained by %d):\n", child_pid);
      if (inet_ntop(AF_INET6, &sa_client.sin6_addr, str, sizeof(str))) {
        printf("%d:Client address is %s\n", child_pid, str);
        printf("%d:Client port is %d\n", child_pid, ntohs(sa_client.sin6_port));
      }
      // 30 11 02 01 01 61 0C 02 01 01 0A 01 00 04 02 A2 4F 04 00
      //  unsigned char bytes[19]{0x30, 0x11, 0x02, 0x01, 0x01, 0x61, 0x0C,
      //  0x02, 0x01, 0x01, 0x0A, 0x01, 0x00, 0x04, 0x02, 0xA2, 0x4F, 0x04,
      //  0x00};
      // 30 0B 02 01 01 61 06 02 01 01 0a 01 00
      //  unsigned char bytes[13]{0x30, 0x0B, 0x02, 0x01, 0x01, 0x61, 0x06,
      //  0x02, 0x01, 0x01, 0x0A, 0x01, 0x00};

      // 30 13 02 01 01 61 0E 02 01 01 0A 01 00 04 02 A2 4F 04 02  A2 4F
      // unsigned char bytes[21]{0x30, 0x13, 0x02, 0x01, 0x01, 0x61, 0x0E, 0x02,
      // 0x01, 0x01, 0x0A, 0x01, 0x00, 0x04, 0x02, 0xA2, 0x4F, 0x04, 0x02, 0xA2,
      // 0x4F};

      // 30 0F 02 01 01 61 0A 02 01 01 0A 01 00 04 00 04 00
      //  unsigned char bytes[17]{0x30, 0x0F, 0x02, 0x01, 0x01, 0x61, 0x0A,
      //  0x02, 0x01, 0x01, 0x0A, 0x01, 0x00, 0x04, 0x00, 0x04, 0x00};
      //________works
      // 30 0c 02 01 01 61 07 0a 01 00 04 00 04 00
      // unsigned char bytes[15]{0x30, 0x0c, 0x02, 0x01, 0x01, 0x61, 0x07, 0x0a,
      // 0x01, 0x00, 0x04, 0x00, 0x04, 0x00};
      //________

      //     send(comm_socket, bytes, 15, 0);

      //	send(comm_socket, WELCOME_MSG, strlen(WELCOME_MSG), 0);

      //   unsigned char buff[2048];
      //   int res = 0;
      //   for (;;) {

      //     if (res >= 1024) { // TODO: eroro
      //       printf("message too long\n");
      //       break;
      //     }

      //     res += recv(comm_socket, buff + res, 1024, 0);

      //     if (res > 0) {
      //       // check if message is envelope
      //       if (buff[0] != 0x30) { // not a sequence //TODO check for bind
      //       request
      //         printf("invalid message\n");
      //         // TODO respond with error message
      //         break;
      //       }
      //       int length = 0;
      //       length = buff[1]; // TODO: check if works

      //       // if whole message received, send response
      //       if (res >= length + 2) {
      //         printf("received message\n");

      //         unsigned char bindResponse[2048];
      //         int responseLenght = CreateBindResponse(buff, bindResponse); //
      //         TODO : check err send(comm_socket, bindResponse,
      //         responseLenght, 0); length = 0;
      //       }

      //       fflush(stdout);
      //     } else // error or end of connection
      //       break;
      //   }

      // request -> response
      unsigned char bindRequest[2048];
      int lenght = loadEnvelope(bindRequest, comm_socket);
      if (lenght < 0) {
        printf("error receiving message\n"); // TODO send err
        close(comm_socket);
        exit(0);
      }
      printf("received message\n");
      unsigned char bindResponse[2048];
      int responseLenght =
          CreateBindResponse(bindRequest, bindResponse); // TODO : check err
      send(comm_socket, bindResponse, responseLenght, 0);

      // search request -> search response

      unsigned char envelope[2048];
      while (1) {
        loadEnvelope(envelope, comm_socket);
        if (lenght < 0) {
          printf("error receiving message\n"); // TODO send err
          close(comm_socket);
          exit(0);
        }
        printf("received message\n");

        switch (envelope[5]) {
        case 0x63:
          printf("search request\n");
          searchRequestHandler(envelope, comm_socket);
          break;
        case 0x62: // unbind request
          printf("unbind request\n");
          printf("Connection to %s closed\n", str);
          close(comm_socket);
          exit(0);

        default:
          printf("unknown request\n");
          close(comm_socket);
          exit(0);
          break;
        }
      }

      // The Search Request is defined as follows:

      //         SearchRequest ::= [APPLICATION 3] SEQUENCE {
      //                 baseObject      LDAPDN,
      //                 scope           ENUMERATED {
      //                         baseObject              (0),
      //                         singleLevel             (1),
      //                         wholeSubtree            (2) },
      //                 derefAliases    ENUMERATED {
      //                         neverDerefAliases       (0),
      //                         derefInSearching        (1),
      //                         derefFindingBaseObj     (2),

      //                         derefAlways             (3) },
      //                 sizeLimit       INTEGER (0 .. maxInt),
      //                 timeLimit       INTEGER (0 .. maxInt),
      //                 typesOnly       BOOLEAN,
      //                 filter          Filter,
      //                 attributes      AttributeDescriptionList }

      //         Filter ::= CHOICE {
      //                 and             [0] SET OF Filter,
      //                 or              [1] SET OF Filter,
      //                 not             [2] Filter,
      //                 equalityMatch   [3] AttributeValueAssertion,
      //                 substrings      [4] SubstringFilter,
      //                 greaterOrEqual  [5] AttributeValueAssertion,
      //                 lessOrEqual     [6] AttributeValueAssertion,
      //                 present         [7] AttributeDescription,
      //                 approxMatch     [8] AttributeValueAssertion,
      //                 extensibleMatch [9] MatchingRuleAssertion }

      //         SubstringFilter ::= SEQUENCE {
      //                 type            AttributeDescription,
      //                 -- at least one must be present
      //                 substrings      SEQUENCE OF CHOICE {
      //                         initial [0] LDAPString,
      //                         any     [1] LDAPString,
      //                         final   [2] LDAPString } }

      //         MatchingRuleAssertion ::= SEQUENCE {
      //                 matchingRule    [1] MatchingRuleId OPTIONAL,
      //                 type            [2] AttributeDescription OPTIONAL,
      //                 matchValue      [3] AssertionValue,
      //                 dnAttributes    [4] BOOLEAN DEFAULT FALSE }

      printf("Connection to %s closed\n", str);
      close(comm_socket);
      exit(0);
    } else // welcome process
    {
      close(comm_socket);
    }
  }
}
