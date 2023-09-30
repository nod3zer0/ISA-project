
/*
 * IPK
 *
 * Demonstration of a basic TCP server with fork.
 *
 * Ondrej Rysavy (rysavy@fit.vutbr.cz)
 *
 */

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define WELCOME_MSG                                                            \
  "Hi, type anything. To end type 'bye.' at a separate line.\n"

using std::string;

string GetID(string &s) {
  string id;

  return s;
}

string GetINT(string &s) {
  string id;

  return s;
}

long long int ConvertINT(unsigned char *s) {

  // TODO: 2 and more octeds doesnt work, they are reversed
  long long int value = 0;
  std::memcpy(&value, s + 2, s[1]);
  return value;
}

int respondToMessage(unsigned char *s) {}

int createSequence(unsigned char *s) {}

/// @brief Creates bind response from bind request
/// @param bindRequest
/// @param bindResponse
void CreateBindResponse(unsigned char *bindRequest,
                        unsigned char *bindResponse) {

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
  }

  bindResponse[1] =
      bindResponse[1] + x; // extends length of sequence by length of message ID
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
}

int GetLength(unsigned char *s) {

  int value = 0;
  std::memcpy(&value, s, sizeof(char));
  return value;

  // TODO: check for other forms
  //  if(s[0]==0){ // short form

  // }else if(s[0]==1){ //TODO: long form
  //     return -1;
  // }else {// TODO: indefinite form
  //     return -1;
  // }

  return -1;
}

/**
 * @brief parses 1 integer from ldap coded message
 *
 * @param s start of the integer in char array
 * @return int - parsed integer
 */
int ParseINT(unsigned char *s, int *err) {
  int value = 0;
  if (s[1] > 4) {
    return 0;
    *err = 2;
  }

  switch (s[1]) {
  case 0x00:
    value = 0;
    *err = 1;
    break;
  case 0x01:
    value = s[2];
    err = 0;
    break;
  case 0x02:
    value = s[2] << 8 | s[3];
    err = 0;
    break;
  case 0x03:
    value = s[2] << 16 | s[3] << 8 | s[4];
    err = 0;
    break;
  case 0x04:
    value = s[2] << 24 | s[3] << 16 | s[4] << 8 | s[5];
    err = 0;
    break;

  default:
    break;
  }
  return value;
}

/**
 * @brief writes int in BER LDAP format to char array
 *
 * @param s start of the string in char array
 * @param value int to be written
 * @return -1 if error, 0 if success
 */
int writeInt(unsigned char *s, int value) {
    if (value < 0) {
        return -1;
    }
    if (value < 0x100) {
        s[0] = 0x02;
        s[1] = 0x01;
        s[2] = value;
    } else if (value < 0x10000) {
        s[0] = 0x02;
        s[1] = 0x02;
        s[2] = value >> 8;
        s[3] = value;
    } else if (value < 0x1000000) {
        s[0] = 0x02;
        s[1] = 0x03;
        s[2] = value >> 16;
        s[3] = value >> 8;
        s[4] = value;
    } else if (value < 0x100000000) {
        s[0] = 0x02;
        s[1] = 0x04;
        s[2] = value >> 24;
        s[3] = value >> 16;
        s[4] = value >> 8;
        s[5] = value;
    } else {
        return -1;
    }
    return 0;
}

int main(int argc, const char *argv[]) {

  int err = 0;

  unsigned char bytes[6]{0x02, 0x12, 0x12, 0x34, 0x56, 0x78};
  printf("%d\n", ParseINT(bytes, &err));
  bytes[1] = 0x04;
  printf("controll: %d\n", 0x12345678);
  printf("%d\n", ParseINT(bytes, &err));
  bytes[1] = 0x03;
  printf("controll: %d\n", 0x123456);
  printf("%d\n", ParseINT(bytes, &err));
  bytes[1] = 0x02;
  printf("controll: %d\n", 0x1234);
  printf("%d\n", ParseINT(bytes, &err));
  bytes[1] = 0x01;
  printf("controll: %d\n", 0x12);
  printf("%d\n", ParseINT(bytes, &err));
  bytes[1] = 0x00;
  printf("%d\n", ParseINT(bytes, &err));

  // test writeInt

  unsigned char bytes2[6];
  writeInt(bytes2, 0x12);
  printf("%x\n", ParseINT(bytes2, &err));
  writeInt(bytes2, 0x1234);
  printf("%x\n", ParseINT(bytes2, &err));
  writeInt(bytes2, 0x123456);
  printf("%x\n", ParseINT(bytes2, &err));
  writeInt(bytes2, 0x12345678);
  printf("%x\n", ParseINT(bytes2, &err));

  for(int i = 0; i< __INT32_MAX__; i++){
      writeInt(bytes2, i);
     // printf("%x\n", ParseINT(bytes2, &err));
     if(i != ParseINT(bytes2, &err)){
            printf("error: %d\n", i);
     }
  }

  // unsigned char bytes[4]{0x02, 0x02, 0x12, 0x34};
  printf("%lld", ConvertINT(bytes));

  // unsigned char bytes[4]{ 0xdd, 0xcc, 0xbb, 0xaa };
  // int value;
  // std::memcpy(&value, bytes, sizeof(int));

  // std::cout << std::hex << value << '\n';
}
