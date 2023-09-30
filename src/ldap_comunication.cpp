#include "inc/ldap_comunication.h"
/// @brief Inicializes and alocates partial empty attribute list
/// @param partialAttributeList pointer to empty attribute list
/// @return 0 if success, -1 if error
int InitSearchResultEntry(unsigned char **partialAttributeList, char *messageID,
                          unsigned char *LDAPDN, int LDAPDNLength) {
  // 64 04 04 00 30 00

  (*partialAttributeList) = (unsigned char *)malloc(11 + messageID[1]);

  (*partialAttributeList)[0] = 0x30;
  (*partialAttributeList)[1] = 0x09 + LDAPDNLength; // length of sequence
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

  (*partialAttributeList)[5 + x] =
      BER_SEARCH_RESULT_ENTRY_C; // Aplication 4 tag
  (*partialAttributeList)[6 + x] =
      0x04 + LDAPDNLength; // length of PartialAttributeList
  // LDAPN
  (*partialAttributeList)[7 + x] = 0x04;         // octet string
  (*partialAttributeList)[8 + x] = LDAPDNLength; // length of string

  int i = 0;
  for (; i < LDAPDNLength; i++) {
    (*partialAttributeList)[9 + x + i] = LDAPDN[i];
  }

  // sequence
  (*partialAttributeList)[9 + x + i] = 0x30;
  (*partialAttributeList)[10 + x + i] = 0x00; // length of sequence
  // TODO
  return 0;
}

int AddToSearchResultEntry(unsigned char **partialAttributeList,
                           unsigned char *attributeDescription,
                           int attributeDescriptionLength,
                           unsigned char *attributeValue,
                           int attributeValueLength) {

  int increaseBy = 8 + attributeValueLength + attributeDescriptionLength;
  unsigned char *newPartialAttributeList = (unsigned char *)realloc(
      (*partialAttributeList),
      increaseBy + (*partialAttributeList)[1] + 2); // todo: support longform
  // error checking
  if (newPartialAttributeList == NULL) {
    return -1;
  }
  (*partialAttributeList) = newPartialAttributeList;

  int positionOfSequence = 2 + (*partialAttributeList)[3] + 2;
  positionOfSequence +=
      (*partialAttributeList)[positionOfSequence + 3] + 2 + 2 + 1;

  int lenghtOforiginalList =
      (*partialAttributeList)[1] + 1; // TODO: support ints
  (*partialAttributeList)[1] += increaseBy;
  (*partialAttributeList)[6] += increaseBy; // TODO support longform

  // increase length of sequence at the end of partialAttributeList
  (*partialAttributeList)[positionOfSequence] += increaseBy;
  // create sequence at the end
  (*partialAttributeList)[lenghtOforiginalList + 1] = BER_SEQUENCE_C;
  (*partialAttributeList)[lenghtOforiginalList + 2] = increaseBy - 2;

  // add attribute description
  (*partialAttributeList)[lenghtOforiginalList + 3] =
      BER_OCTET_STRING_C; // octet string
  (*partialAttributeList)[lenghtOforiginalList + 4] =
      attributeDescriptionLength; // length of string
  for (int i = 0; i < attributeDescriptionLength; i++) {
    (*partialAttributeList)[lenghtOforiginalList + 5 + i] =
        attributeDescription[i];
  }

  (*partialAttributeList)[lenghtOforiginalList + 5 +
                          attributeDescriptionLength] = BER_SET_C; // set A0
  (*partialAttributeList)[lenghtOforiginalList + 6 +
                          attributeDescriptionLength] =
      attributeValueLength + 2; // length of sequence
  // add attribute value
  (*partialAttributeList)[lenghtOforiginalList + 7 +
                          attributeDescriptionLength] =
      BER_OCTET_STRING_C; // octet string
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
  bindResponse[0] = BER_SEQUENCE_C;
  bindResponse[1] = 0x0c; // length of sequence
  // message ID
  bindResponse[2] = BER_INT_C;
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
  bindResponse[5 + x] = BER_BIND_REQUEST_C; // BindResponse tag
  bindResponse[6 + x] = 0x07;               // length of bind response
  // LDAP result
  bindResponse[7 + x] = BER_ENUM_C;       // enum
  bindResponse[8 + x] = 0x01;             // length of enum
  bindResponse[9 + x] = BER_LDAP_SUCCESS; // LDAP result code - success (0)
  // matched DN
  bindResponse[10 + x] = BER_OCTET_STRING_C; // octet string
  bindResponse[11 + x] = 0x00;               // length of matched DN
  // diagnostic message
  bindResponse[12 + x] = BER_OCTET_STRING_C; // octet string
  bindResponse[13 + x] = 0x00;               // length of diagnostic message
  return 14 + x;
}

int sendSearchResultEntry(unsigned char *searchRequest, int comm_socket) {

  // 30 07 02 01 01 64 02 04 00
  char envelope[30];
  // sequence
  envelope[0] = BER_SEQUENCE_C;
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
  envelope[0] = BER_SEQUENCE_C;
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

  envelope[5 + x] = BER_SEARCH_RESULT_DONE_C;
  envelope[6 + x] = 0x07;

  envelope[7 + x] = BER_ENUM_C;
  envelope[8 + x] = 0x01;
  envelope[9 + x] = BER_LDAP_SUCCESS;
  envelope[10 + x] = BER_OCTET_STRING_C;
  envelope[11 + x] = 0x00;
  envelope[12 + x] = BER_OCTET_STRING_C;
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

int addLDAPDN(unsigned char **searchRequest, unsigned char *LDAPDN,
              int LDAPDNLength) {

  int increaseTo = LDAPDNLength + (*searchRequest)[1] + 2;
  unsigned char *newsearchRequest =
      (unsigned char *)realloc((*searchRequest), increaseTo);
  // error checking
  if (newsearchRequest == NULL) {
    return -1;
  }
  (*searchRequest) = newsearchRequest;

  (*searchRequest)[1] += LDAPDNLength;
  int messageIDlenght = (*searchRequest)[3];

  (*searchRequest)[6 + messageIDlenght] += LDAPDNLength;
  (*searchRequest)[8 + messageIDlenght] = LDAPDNLength;

  // copy ldapdn to search request
  for (int i = 0; i < LDAPDNLength; i++) {
    (*searchRequest)[9 + messageIDlenght + i] = LDAPDN[i];
  }
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

  unsigned char testVal[] = "cn";
  unsigned char testVal2[] = "test";
  unsigned char testVal3[] = "test3";
  unsigned char testVal4[] = "cgn";

  unsigned char *searchResultEntry;
  InitSearchResultEntry(&searchResultEntry, sr.messageID, testVal3, 5);

  // print searchResultEntry hex values
  printf("searchResultEntry: ");
  for (int i = 0; i < searchResultEntry[1] + 2; i++) {
    printf("%02x ", searchResultEntry[i]);
  }
  printf("\n");

  AddToSearchResultEntry(&searchResultEntry, testVal, 2, testVal2, 4);
  AddToSearchResultEntry(&searchResultEntry, testVal4, 3, testVal3, 5);

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
