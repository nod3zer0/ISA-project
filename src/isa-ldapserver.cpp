// #include "inc/ber_helper_functions.h"
#include "inc/DatabaseObject.h"
#include "inc/ldap_comunication.h"
#include "inc/FilterObject.h"
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

// int main(int argc, const char *argv[]) {
//     //test filter objects
//     filter *f1 = new andFilter();

//     printf("filter type: %d\n", f1->getFilterType());

//     // cast to andFilter
//     andFilter *f2 = (andFilter *)f1;

//     printf("filter type: %d\n", f2->getFilterType());

//     f2->filters.push_back(new andFilter());
//     f2->filters.push_back(new orFilter());
//     f2->filters.push_back(new notFilter());
//     f2->filters.push_back(new SubstringFilter(std::vector<unsigned char>(),
//     std::vector<unsigned char>(), std::vector<unsigned char>()));
//     f2->filters.push_back(new equalityMatchFilter(std::vector<unsigned
//     char>(), std::vector<unsigned char>()));

//     for (int i = 0; i < f2->filters.size(); i++) {
//         //printf("filter type: %d\n", f2->filters[i]->getFilterType());
//         switch (f2->filters[i]->getFilterType())
//         {
//         case AND:
//             printf("AND\n");
//             break;
//         case OR:
//             printf("OR\n");
//             break;
//         case NOT:
//             printf("NOT\n");
//             break;
//         case equalityMatch:
//             printf("equalityMatch\n");
//             break;
//         case substrings:
//             printf("substrings\n");
//             break;
//         default:
//             printf("undefined\n");
//             break;
//         }
//     }

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
  sa.sin6_port = htons(10018);
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
        accept(welcome_socket, (struct sockaddr *)&sa_client, &sa_client_len);
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
        printf("%d:Client port is %d\n", child_pid, ntohs(sa_client.sin6_port));
      }

      std::vector<unsigned char> bindRequest;
      int lenght = loadEnvelope(bindRequest, comm_socket);
      if (lenght < 0) {
        printf("error receiving message\n"); // TODO send err
        close(comm_socket);
        exit(0);
      }
      printf("received message\n");
      std::vector<unsigned char> bindResponse;

      CreateBindResponse(bindRequest, bindResponse); // TODO : check err
      send(comm_socket, &bindResponse[0], bindResponse.size(), 0);

      // search request -> search response
      int err = 0;
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

        std::vector<unsigned char>::iterator envelopeTagPointer =
            envelope.begin();

        goIntoTag(envelopeTagPointer, &err);
        skipTags(envelopeTagPointer, 1, &err); // todo handle error
        switch (envelopeTagPointer[0]) {
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

      printf("Connection to %s closed\n", str);
      close(comm_socket);
      exit(0);
    } else // welcome process
    {
      close(comm_socket);
    }
  }
}
