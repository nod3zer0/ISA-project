// #include "inc/ber_helper_functions.h"
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
#define DEBUG

// Macro for printing err message and closing socket when err != 0
#define CHECK_ERR(err, msg)                                                    \
  if (err != 0) {                                                              \
    printf("%s\n", msg);                                                       \
    close(childSocket);                                                        \
    exit(0);                                                                   \
  }

void SigCatcher(int n) {
  int pid = wait3(NULL, WNOHANG, NULL);
  printf("Child %d died.\n", pid);
}
int childSocket = 0;
int communicationSocket;
std::vector<int> children = {};
void SigIntCatcher(int n) {
  printf("Killing children.\n");
  for (int i = 0; i < children.size(); i++) {
    kill(children[i], SIGQUIT);
  }
  close(communicationSocket);
  printf("Closing.\n");
  exit(0);
}

void SigQuitCatcher(int n) {
  printf("Dying. %d\n", childSocket);
  close(childSocket);
  exit(0);
}

int ldapServer(int port, char *dbPath) {
  int returnCode;

  struct sockaddr_in6 sa;
  struct sockaddr_in6 clientSA;
  char str[INET6_ADDRSTRLEN];
  socklen_t ClientSALen = sizeof(clientSA);
  if ((communicationSocket = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
    perror("socket() failed");
    exit(EXIT_FAILURE);
  }

    returnCode = setsockopt(communicationSocket, SOL_SOCKET, SO_REUSEADDR,
                            (char *)&sa, sizeof(sa));
    if (returnCode < 0) {
      perror("setsockopt() failed");
      close(communicationSocket);
      exit(-1);
    }

  memset(&sa, 0, sizeof(sa));
  sa.sin6_family = AF_INET6;
  sa.sin6_addr = in6addr_any;
  sa.sin6_port = htons(port);
  if ((returnCode =
           bind(communicationSocket, (struct sockaddr *)&sa, sizeof(sa))) < 0) {
    perror("bind() failed");
    exit(EXIT_FAILURE);
  }
  if ((returnCode = listen(communicationSocket, 1)) < 0) {
    perror("listen() failed");
    exit(EXIT_FAILURE);
  }

  signal(SIGINT, SigIntCatcher);
  signal(SIGCHLD, SigCatcher);
  signal(SIGQUIT, SigQuitCatcher);

  while (1) {
    childSocket =
        accept(communicationSocket, (struct sockaddr *)&clientSA, &ClientSALen);
    int flags = fcntl(communicationSocket, F_GETFL, 0);
    returnCode = fcntl(communicationSocket, F_SETFL, flags | O_NONBLOCK);
    if (returnCode < 0) {
      perror("ERROR: fcntl");
      exit(EXIT_FAILURE);
    }
    returnCode = fcntl(communicationSocket, F_SETFD, FD_CLOEXEC);
    if (returnCode < 0) {
      perror("ERROR: fcntl");
      exit(EXIT_FAILURE);
    }
    if (childSocket <= 0)
      continue;

    int pid = fork();
    if (pid < 0) {
      perror("fork() failed");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) // new process to maintain client's requests:
    {
      int childPid = getpid();
      close(communicationSocket); // not necessary in child process
      printf("New connection (maintained by %d) (socket %d):\n", childPid,
             childSocket);
      if (inet_ntop(AF_INET6, &clientSA.sin6_addr, str, sizeof(str))) {
        printf("%d:Client address is %s\n", childPid, str);
        printf("%d:Client port is %d\n", childPid, ntohs(clientSA.sin6_port));
      }

      // load envelope
      int err = 0;
      std::vector<unsigned char> envelope;

      while (1) {
        envelope.clear();
        int lenghtOfEnvelope = loadEnvelope(envelope, childSocket);
        if (lenghtOfEnvelope == -1) {
          printf("connection closed\n");
          close(childSocket);
          exit(0);
        }
        printf("received message\n");

        std::vector<unsigned char>::iterator envelopeTagPointer =
            envelope.begin();

        GoIntoTag(envelopeTagPointer, &err, envelope.end());
        CHECK_ERR(err, "error going into tag");
        SkipTags(envelopeTagPointer, 1, &err,
                 envelope.end()); // todo handle error
        CHECK_ERR(err, "error skipping tags");
        BerObject *EnvelopeObject =
            ParseBerObject(envelope.begin(), &err, envelope.end());
        switch (envelopeTagPointer[0]) {
        case 0x63:
          printf("search request\n");
          searchRequestHandler(EnvelopeObject, childSocket, dbPath);
          break;
        case 0x62: // unbind request TODO: handle unbind
          printf("unbind request\n");
          printf("Connection to %s closed\n", str);
          close(childSocket);
          exit(0);
          break;
        case 0x60: { // bind request
          // send bind response
          BerObject *berBindResponse =
              CreateBindResponse(EnvelopeObject, BER_LDAP_SUCCESS);
          std::vector<unsigned char> bindResponse =
              berBindResponse->getBerRepresentation();
          // check for correct auth method
          BerUndefinedObject *authMethod =
              (BerUndefinedObject *)((BerSequenceObject *)((BerSequenceObject *)
                                                               EnvelopeObject)
                                         ->objects[1])
                  ->objects[2];
          std::vector<unsigned char> authMethodValue =
              authMethod->getBerRepresentation();

          if (authMethodValue[0] != 0x80 && authMethodValue[1] != 0x00) {
            printf("invalid auth method\n");
            BerObject *berBindResponse = CreateBindResponse(
                EnvelopeObject, BER_LDAP_AUTH_METHOD_NOT_SUPPORTED);
            std::vector<unsigned char> bindResponse =
                berBindResponse->getBerRepresentation();
            send(childSocket, &bindResponse[0], bindResponse.size(), 0);
            close(childSocket);
            exit(0);
          }

          send(childSocket, &bindResponse[0], bindResponse.size(), 0);
          break;
        }
        default:
          printf("unknown request\n");
          close(childSocket);
          exit(0);
          break;
        }
      }

      printf("Connection to %s closed\n", str);
      close(childSocket);
      exit(0);
    } else {
      children.push_back(pid);
      close(childSocket);
    }
  }
}

int main(int argc, const char *argv[]) {

  args_t args = parseArguments(argc, argv);

  if (args.err) {
    fprintf(stderr, "Error parsing arguments\n");
    return 1;
  }
  ldapServer(args.port, args.dbPath);
}
