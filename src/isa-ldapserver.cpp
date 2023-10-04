// #include "inc/ber_helper_functions.h"
#include "inc/database_object.h"
#include "inc/ldap_comunication.h"
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

void SigCatcher(int n) {
  int pid = wait3(NULL, WNOHANG, NULL);
  printf("Child %d spawned.\n", pid);
}

// // test main, TODO: remove
// int main(int argc, const char *argv[]) {
//   //_________________TEST ParseLength_______________________
//     // test parsing long form and short form from ber
//     // unsigned char bytes[4]{0x30, 0x82, 0x00, 0x05};
//     // int err = 0;
//     // int value = ParseLength((bytes + 1), &err);
//     // printf("value: %d\n", value);

//     // unsigned char bytes2[4] = {0x30, 0x82, 0x03, 0xFC};
//     // err = 0;
//     // value = ParseLength((bytes2 + 1), &err);
//     // printf("value: %d\n", value);

// //   // __________________TEST skipTags________________________
// //   unsigned char bytes[17]{0x30, 0x0e, 0x02, 0x82, 0x00, 0x01, 0x01, 0x61,
// 0x07,
// //                           0x0a, 0x01, 0x00, 0x04, 0x00, 0x04, 0x00};
// //   int err;
// //   unsigned char *ptr = goIntoTag(bytes, &err);
// //   printf("tag: %x\n", *((unsigned char *)ptr));
// //   ptr = skipTags((unsigned char *)ptr, 1, &err);
// //   printf("tag: %x\n", *((unsigned char *)ptr));
// //   ptr = goIntoTag((unsigned char *)ptr, &err);
// //   printf("tag: %x\n", *((unsigned char *)ptr));
// //   ptr = skipTags((unsigned char *)ptr, 2, &err);
// //   printf("tag: %x\n", *((unsigned char *)ptr));

//   // 0x30 0x0c 0x02 0x01 0x01 0x61 0x07 0x0a 0x01 0x00 0x04 0x00 0x04 0x00

//   unsigned char bytes[17]{0x84, 0x12, 0x34, 0x56, 0x78, 0x07, 0x0a,
//                           0x01, 0x00, 0x04, 0x00, 0x04, 0x00};
//   int err;
//   int lenght = ParseLength(bytes, &err);
//   printf("lenght: %d\n", lenght);
//   IncreaseLength4Bytes(bytes, 4, &err);
//   lenght = ParseLength(bytes, &err);
//   printf("lenght: %d\n", lenght);
// }
// test databaseObject
// int main(int argc, const char *argv[]) {

//   int x = 0;
//   databaseController db("ldap-lidi-ascii.csv");
//   database_object obj = db.loadNextRow();
//   std::vector<unsigned char> name = obj.get_name();
//   std::vector<unsigned char> uid = obj.get_uid();
//   std::vector<unsigned char> email = obj.get_email();
//   name.push_back(0);
//   uid.push_back(0);
//   email.push_back(0);
//   printf("name: %s\n", &name[0]);
//   printf("uid: %s\n", &uid[0]);
//   printf("email: %s\n", &email[0]);
//   obj = db.loadNextRow();
//   name = obj.get_name();
//   uid = obj.get_uid();
//   email = obj.get_email();
//   name.push_back(0);
//   uid.push_back(0);
//   email.push_back(0);
//   printf("name: %s\n", &name[0]);
//   printf("uid: %s\n", &uid[0]);
//   printf("email: %s\n", &email[0]);
//   databaseController db2("ldap-lidi-ascii.csv");
//   obj = db2.loadNextRow();
//   name = obj.get_name();
//   uid = obj.get_uid();
//   email = obj.get_email();
//   name.push_back(0);
//   uid.push_back(0);
//   email.push_back(0);
//   printf("name: %s\n", &name[0]);
//   printf("uid: %s\n", &uid[0]);
//   printf("email: %s\n", &email[0]);
// }

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
  sa.sin6_port = htons(10011);
  if ((rc = bind(welcome_socket, (struct sockaddr *)&sa, sizeof(sa))) < 0) {
    perror("bind() failed");
    exit(EXIT_FAILURE);
  }
  if ((rc = listen(welcome_socket, 1)) < 0) {
    perror("listen() failed");
    exit(EXIT_FAILURE);
  }

  signal(SIGCHLD, SigCatcher);

  // while (1)  TODO: reactivate forking
  {
    int comm_socket =
        accept(welcome_socket, (struct sockaddr *)&sa_client,
        &sa_client_len);
    // if (comm_socket <= 0)  TODO: reactivate forking
    //  continue;  TODO: reactivate forking

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
        printf("%d:Client port is %d\n", child_pid,
        ntohs(sa_client.sin6_port));
      }
      // 30 11 02 01 01 61 0C 02 01 01 0A 01 00 04 02 A2 4F 04 00
      //  unsigned char bytes[19]{0x30, 0x11, 0x02, 0x01, 0x01, 0x61, 0x0C,
      //  0x02, 0x01, 0x01, 0x0A, 0x01, 0x00, 0x04, 0x02, 0xA2, 0x4F, 0x04,
      //  0x00};
      // 30 0B 02 01 01 61 06 02 01 01 0a 01 00
      //  unsigned char bytes[13]{0x30, 0x0B, 0x02, 0x01, 0x01, 0x61, 0x06,
      //  0x02, 0x01, 0x01, 0x0A, 0x01, 0x00};

      // 30 13 02 01 01 61 0E 02 01 01 0A 01 00 04 02 A2 4F 04 02  A2 4F
      // unsigned char bytes[21]{0x30, 0x13, 0x02, 0x01, 0x01, 0x61, 0x0E,
      0x02,
      // 0x01, 0x01, 0x0A, 0x01, 0x00, 0x04, 0x02, 0xA2, 0x4F, 0x04, 0x02,
      0xA2,
      // 0x4F};

      // 30 0F 02 01 01 61 0A 02 01 01 0A 01 00 04 00 04 00
      //  unsigned char bytes[17]{0x30, 0x0F, 0x02, 0x01, 0x01, 0x61, 0x0A,
      //  0x02, 0x01, 0x01, 0x0A, 0x01, 0x00, 0x04, 0x00, 0x04, 0x00};
      //________works
      // 30 0c 02 01 01 61 07 0a 01 00 04 00 04 00
      // unsigned char bytes[15]{0x30, 0x0c, 0x02, 0x01, 0x01, 0x61, 0x07,
      0x0a,
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
      //         int responseLenght = CreateBindResponse(buff, bindResponse);
      //
      //         TODO : check err send(comm_socket, bindResponse,
      //         responseLenght, 0); length = 0;
      //       }

      //       fflush(stdout);
      //     } else // error or end of connection
      //       break;
      //   }

      // request -> response
      std::vector<unsigned char> bindRequest;
      int lenght = loadEnvelope(bindRequest, comm_socket);
      if (lenght < 0) {
        printf("error receiving message\n"); // TODO send err
        close(comm_socket);
        exit(0);
      }
      printf("received message\n");
      std::vector<unsigned char> bindResponse;
      int responseLenght =
          CreateBindResponse(bindRequest, bindResponse); // TODO : check err
      send(comm_socket, &bindResponse[0], bindResponse.size(), 0);

      // search request -> search response

      std::vector<unsigned char> envelope;
      while (1) {
        envelope.clear();
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
