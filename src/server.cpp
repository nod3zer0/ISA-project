#include "inc/server.h"

// Writes which child died
void SigCatcher(int n) {
  int pid = wait3(NULL, WNOHANG, NULL);
  printf("Child %d died.\n", pid);
}
// global variable for signal handlers
int childSocket = 0;
int communicationSocket;
std::vector<int> children = {};

/**
 * @brief signal handler for main process, when SIGINT is received, it kills all
 * children and closes the socket
 *
 * @param n
 */
void SigIntCatcher(int n) {
  printf("Killing children.\n");
  for (long unsigned int i = 0; i < children.size(); i++) {
    kill(children[i], SIGQUIT);
  }
  close(communicationSocket);
  printf("Closing.\n");
  exit(0);
}

/**
 * @brief signal handler for child process, when SIGQUIT is received, it closes
 * the socket and exits
 *
 * @param n
 */
void SigQuitCatcher(int n) {
  sendNoticeOfDisconnection(childSocket,BER_LDAP_UNAVAILABLE);
  printf("Dying. %d\n", childSocket);
  close(childSocket);
  exit(0);
}

// makro for destroying berBindResponse object
#define CLEANUP_SERVER delete ((BerSequenceObject *)berBindResponse);

/**
 * @brief Ldap server, This part was inspired by the example from stubs demo tcp
 * server
 * https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs/cpp/DemoTcp
 * by Vladimir Vesely Ph.D.
 *
 * @param port port to listen on
 * @param dbPath path to database file
 * @return int
 */
int ldapServer(int port, char *dbPath) {
  int returnCode;
  // starting main socket
  if ((communicationSocket = socket(PF_INET6, SOCK_STREAM, 0)) < 0) {
    perror("socket() failed");
    exit(EXIT_FAILURE);
  }
  // setting options for main socket
  const int enable = 1;
  const int disable = 0;
  returnCode = setsockopt(communicationSocket, IPPROTO_IPV6, IPV6_V6ONLY,
                          &disable, sizeof(int));

  if (returnCode < 0) {
    perror("setsockopt() failed");
    close(communicationSocket);
    exit(1);
  }
  returnCode = setsockopt(communicationSocket, SOL_SOCKET, SO_REUSEADDR,
                          &enable, sizeof(int));
  if (returnCode < 0) {
    perror("setsockopt() failed");
    close(communicationSocket);
    exit(1);
  }
  returnCode = setsockopt(communicationSocket, SOL_SOCKET, SO_REUSEPORT,
                          &enable, sizeof(int));
  if (returnCode < 0) {
    perror("setsockopt() failed");
    close(communicationSocket);
    exit(1);
  }

  // setting up main socket
  struct sockaddr_in6 sa = {0};
  struct sockaddr_in6 clientSA = {0};
  char str[INET6_ADDRSTRLEN];
  socklen_t ClientSALen = sizeof(clientSA);
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
  // setup signal handlers
  signal(SIGINT, SigIntCatcher);
  signal(SIGCHLD, SigCatcher);
  signal(SIGQUIT, SigQuitCatcher);

  // main loop
  while (1) {
    //seting up child socket
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

    //spliting into child process and parent process
    int pid = fork();
    if (pid < 0) {
      perror("fork() failed");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) // new process to maintain client's requests:
    {
      int childPid = getpid();
      close(communicationSocket); // closing for child process
      printf("New ldap connection (maintained by %d):\n", childPid);
      if (inet_ntop(AF_INET6, &clientSA.sin6_addr, str, sizeof(str))) {
        printf("%d:Client address:%s\n", childPid, str);
        printf("%d:Client port: %d\n", childPid, ntohs(clientSA.sin6_port));
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
        printf("%d: received message\n", childPid);

        std::vector<unsigned char>::iterator envelopeTagPointer =
            envelope.begin();

        GoIntoTag(envelopeTagPointer, &err, envelope.end());
        CHECK_ERR(err, "error going into tag");
        SkipTags(envelopeTagPointer, 1, &err, envelope.end());
        CHECK_ERR(err, "error skipping tags");
        BerObject *EnvelopeObject =
            ParseBerObject(envelope.begin(), &err, envelope.end());
        switch (envelopeTagPointer[0]) {
        case 0x63:
          printf("search request\n");
          searchRequestHandler(EnvelopeObject, childSocket, dbPath);
          delete EnvelopeObject;
          break;
        case 0x62: // unbind request
          printf("%d: Connection closed\n", childPid);
          close(childSocket);
          delete EnvelopeObject;
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
            send(childSocket, &bindResponse[0], bindResponse.size(), MSG_NOSIGNAL);
            delete EnvelopeObject;
            close(childSocket);
            exit(0);
          }

          send(childSocket, &bindResponse[0], bindResponse.size(), MSG_NOSIGNAL);
          delete EnvelopeObject;
          break;
        }
        default:
          printf("unknown request\n");
          delete EnvelopeObject;
          close(childSocket);
          exit(0);
          break;
        }
      }

      printf("%d: Connection closed\n", childPid);

      close(childSocket);
      exit(0);
    } else {
      children.push_back(pid);
      close(childSocket);
    }
  }
}
