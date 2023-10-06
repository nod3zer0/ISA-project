#include "inc/ldap_comunication.h"

/// @brief Inicializes and alocates partial empty attribute list
/// @param partialAttributeList pointer to empty attribute list
/// @return 0 if success, -1 if error
int InitSearchResultEntry(std::vector<unsigned char> &partialAttributeList,
                          std::vector<unsigned char> messageID,
                          std::vector<unsigned char> LDAPDN, int LDAPDNLength) {
  // 64 04 04 00 30 00
  // all tags + all lengths = 11

  partialAttributeList.push_back(BER_SEQUENCE_C);
  partialAttributeList.push_back(0x09 + LDAPDNLength); // length of sequence

  // message ID
  partialAttributeList.push_back(BER_INT_C);
  partialAttributeList.push_back(messageID[1]); // length of message ID
  partialAttributeList.push_back(messageID[0]); // message ID
  // copies messageID to bind response
  // _____________________________________________________________
  //   int x = 0; //TODO: support longform
  //   if (1 < (*partialAttributeList)[3]) {
  //     x = 1;
  //     for (; x < messageID[1]; x++) {
  //       (*partialAttributeList)[4 + x] = messageID[x];
  //     }
  //   } // extends length of sequence by length of message ID

  // (*partialAttributeList)[1] += x;
  //_____________________________________________________________________________________________
  // error checking
  //   if ((*partialAttributeList) == NULL) {
  //     return -1;
  //   }

  partialAttributeList.push_back(BER_SEARCH_RESULT_ENTRY_C); // Aplication 4 tag
  partialAttributeList.push_back(0x04 +
                                 LDAPDNLength); // length of
                                                // PartialAttributeList LDAPN
  partialAttributeList.push_back(BER_OCTET_STRING_C); // octet string
  partialAttributeList.push_back(LDAPDNLength);       // length of string

  int i = 0;
  for (; i < LDAPDNLength; i++) {
    partialAttributeList.push_back(LDAPDN[i]);
  }

  // sequence
  partialAttributeList.push_back(BER_SEQUENCE_C);
  partialAttributeList.push_back(0x00); // length of sequence
  // TODO
  return 0;
}

std::vector<database_object>
filterHandler(filter *f, int *err, std::vector<database_object> &database) {

  // cn
  std::vector<unsigned char> cn = {'c', 'n'};
  // email
  std::vector<unsigned char> email = {'e', 'm', 'a', 'i', 'l'};
  // uid
  std::vector<unsigned char> uid = {'u', 'i', 'd'};

  std::vector<unsigned char> attributeDescription;
  std::vector<unsigned char> assertionValue;

  equalityMatchFilter *emf;
  andFilter *af;
  orFilter *of;

  std::vector<database_object> localDB;
  switch (f->getFilterType()) {
  case equalityMatch:
    emf = (equalityMatchFilter *)f;
    attributeDescription = emf->getAttributeDescription();
    assertionValue = emf->getAssertionValue();

    if (attributeDescription == cn) {
      for (int i = 0; i < database.size(); i++) {
        if (database[i].get_name() == assertionValue) {
          localDB.push_back(database[i]);
        }
      }
    } else if (attributeDescription == email) {
      for (int i = 0; i < database.size(); i++) {
        if (database[i].get_email() == assertionValue) {
          localDB.push_back(database[i]);
        }
      }
    } else if (attributeDescription == uid) {
      for (int i = 0; i < database.size(); i++) {
        if (database[i].get_uid() == assertionValue) {
          localDB.push_back(database[i]);
        }
      }
    }
    break;
  case substrings:
    // TODO
    break;
  case AND:
    af = (andFilter *)f;
    localDB = filterHandler(af->filters[0], err, database);
    for (int i = 1; i < af->filters.size(); i++) {
      localDB = filterHandler(af->filters[i], err, localDB);
    }

    break;
  case OR:
    of = (orFilter *)f;
    for (int i = 0; i < of->filters.size(); i++) {
      std::vector<database_object> tmpDB =
          filterHandler(of->filters[i], err, database);
      localDB.insert(localDB.end(), tmpDB.begin(), tmpDB.end());
    }

    break;
  case NOT:
    // TODO
    break;
  default:
    break;
  }

  return localDB;
}

int AddToSearchResultEntry(std::vector<unsigned char> &partialAttributeList,
                           std::vector<unsigned char> &attributeDescription,
                           int attributeDescriptionLength,
                           std::vector<unsigned char> &attributeValue,
                           int attributeValueLength) {
  int err = 0;
  int numberOfNewTags = 4;
  int numberOfNewLengths = 4;
  int increaseBy = numberOfNewTags + numberOfNewLengths + attributeValueLength +
                   attributeDescriptionLength;
  //   unsigned char *newPartialAttributeList = (unsigned char *)realloc(
  //       (*partialAttributeList),
  //       increaseBy + ParseLength((*partialAttributeList) + 1, &err) +
  //           2); // todo: support longform
  //   // error checking
  //   if (newPartialAttributeList == NULL) {
  //     return -1;
  //   }
  //   (*partialAttributeList) = newPartialAttributeList;

  // Sequence
  //      string messageID
  //      application 4 (searchResultEntry)
  //          string LDAPDN
  //          sequence partialAttributeList
  //              sequence
  //                  string attributeDescription
  //                  set
  //                      string attributeValue

  if (partialAttributeList[1] + increaseBy >
      0x7F) { // TODO: implement longform increase
    printf("increaseBy too big, not implemented!\n");
  }
  partialAttributeList[1] += increaseBy;

  std::vector<unsigned char>::iterator locationOfSequence =
      partialAttributeList.begin();
  goIntoTag(locationOfSequence, &err); // go into sequence
  if (err != 0)
    return -1;
  skipTags(locationOfSequence, 1, &err); // skip string messageID
  if (err != 0)
    return -1;
  *(locationOfSequence + 1) += increaseBy; // TODO: longform
  goIntoTag(locationOfSequence, &err);     // go into application 4
  if (err != 0)
    return -1;
  skipTags(locationOfSequence, 1, &err); // skip string LDAPDN
  std::vector<unsigned char>::iterator locationOfPartialAttributeList =
      locationOfSequence;
  if (err != 0)
    return -1;
  skipTags(locationOfSequence, 1,
           &err); // skip to the end of partialAttributeList
  if (err != 0)
    return -1;

  *(locationOfPartialAttributeList + 1) += increaseBy; // TODO: longform

  partialAttributeList.push_back(BER_SEQUENCE_C);
  partialAttributeList.push_back(increaseBy - 2);

  // add attribute description
  partialAttributeList.push_back(BER_OCTET_STRING_C); // octet string
  partialAttributeList.push_back(
      attributeDescriptionLength); // length of string
  for (int i = 0; i < attributeDescriptionLength; i++) {
    partialAttributeList.push_back(attributeDescription[i]);
  }

  partialAttributeList.push_back(BER_SET_C); // set A0
  partialAttributeList.push_back(attributeValueLength +
                                 2); // length of sequence
  // add attribute value
  partialAttributeList.push_back(BER_OCTET_STRING_C);   // octet string
  partialAttributeList.push_back(attributeValueLength); // length of string
  for (int i = 0; i < attributeValueLength; i++) {
    partialAttributeList.push_back(attributeValue[i]);
  }

  // create sequence at the end

  //   (locationOfSequence)[0] = BER_SEQUENCE_C;
  //   (locationOfSequence)[1] = increaseBy - 2;

  //   // add attribute description
  //   (locationOfSequence)[2] = BER_OCTET_STRING_C;         // octet string
  //   (locationOfSequence)[3] = attributeDescriptionLength; // length of
  //   string for (int i = 0; i < attributeDescriptionLength; i++) {
  //     (locationOfSequence)[4 + i] = attributeDescription[i];
  //   }

  //   (locationOfSequence)[4 + attributeDescriptionLength] = BER_SET_C; //
  //   set A0 (locationOfSequence)[5 + attributeDescriptionLength] =
  //       attributeValueLength + 2; // length of sequence
  //   // add attribute value
  //   (locationOfSequence)[6 + attributeDescriptionLength] =
  //       BER_OCTET_STRING_C; // octet string
  //   (locationOfSequence)[7 + attributeDescriptionLength] =
  //       attributeValueLength; // length of string
  //   for (int i = 0; i < attributeValueLength; i++) {
  //     (locationOfSequence)[8 + attributeDescriptionLength + i] =
  //         attributeValue[i];
  //   }

  return 0;
}

/// @brief Creates bind response from bind request
/// @param bindRequest
/// @param bindResponse
/// @return length of bind response, -1 if error
int CreateBindResponse(std::vector<unsigned char> &bindRequest,
                       std::vector<unsigned char> &bindResponse) {
  // 30 0c 02 01 01 61 07 0a 01 00 04 00 04 00
  // sequence
  bindResponse.push_back(BER_SEQUENCE_C);
  bindResponse.push_back(0x0c); // length of sequence
  // message ID
  bindResponse.push_back(BER_INT_C);
  bindResponse.push_back(bindRequest[3]); // length of message ID
  // bindResponse.push_back(bindRequest[4]);// message ID

  // copies messageID to bind response
  // _____________________________________________________________

  bindResponse.push_back(bindRequest[4]); // TODO: support longform

  // extends length of sequence by length of message ID
  //_____________________________________________________________________________________________

  // bind response
  bindResponse.push_back(BER_BIND_RESPONSE_C); // BindResponse tag
  bindResponse.push_back(0x07);                // length of bind response
  // LDAP result
  bindResponse.push_back(BER_ENUM_C);       // enum
  bindResponse.push_back(0x01);             // length of enum
  bindResponse.push_back(BER_LDAP_SUCCESS); // LDAP result code - success (0)
  // matched DN
  bindResponse.push_back(BER_OCTET_STRING_C); // octet string
  bindResponse.push_back(0x00);               // length of matched DN
  // diagnostic message
  bindResponse.push_back(BER_OCTET_STRING_C); // octet string
  bindResponse.push_back(0x00);               // length of diagnostic message
  return bindResponse.size();
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

int sendSearchResultDone(std::vector<unsigned char> &searchRequest,
                         int comm_socket) {

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

filter *convertToFilterObject(std::vector<unsigned char>::iterator BERfilter) {

  filter *f;
  int err;
  int lenght = 0;
  int ll = 0;
  int attributeDescriptionLenght;
  int assertionValueLenght;
  std::vector<unsigned char> attributeDescription;
  std::vector<unsigned char> assertionValue;

  switch (getFilterType(BERfilter)) {
  case equalityMatch:

    goIntoTag(BERfilter, &err);
    if (err != 0)
      return new filter();
    lenght = ParseLength(BERfilter + 1, &err);

    ll = getLengthLength(BERfilter + 1, &err);

    for (int i = 0; i < lenght; i++) {
      attributeDescription.push_back(BERfilter[1 + ll + i]);
    }

    skipTags(BERfilter, 1, &err);
    if (err != 0)
      return new filter();

    lenght = ParseLength(BERfilter + 1, &err);
    ll = getLengthLength(BERfilter + 1, &err);

    for (int i = 0; i < lenght; i++) {
      assertionValue.push_back(BERfilter[1 + ll + i]);
    }

    f = new equalityMatchFilter(attributeDescription, assertionValue);

    break;
  case substrings:
    // TODO
    break;
  case AND:
    f = new andFilter();

    lenght = ParseLength(BERfilter + 1, &err);
    goIntoTag(BERfilter, &err);
    if (err != 0)
      return new filter();
    for (int i = 0; i < lenght;) {

      if (err != 0)
        return new filter();
      filter *tmpF = convertToFilterObject(BERfilter);
      printf("filter type: %d\n", tmpF->getFilterType());
      fflush(stdout);
      ((andFilter *)f)->filters.push_back(tmpF);
      i += 1 + getLengthLength(BERfilter + 1, &err) +
           ParseLength(BERfilter + 1, &err);
      skipTags(BERfilter, 1, &err);
    }

    // TODO
    break;
  case OR:
    f = new orFilter();
    lenght = ParseLength(BERfilter + 1, &err);
    goIntoTag(BERfilter, &err);
    if (err != 0)
      return new filter();

    for (int i = 0; i < lenght;) {
      if (err != 0)
        return new filter();
      filter *tmpF = convertToFilterObject(BERfilter);
      ((orFilter *)f)->filters.push_back(tmpF);
      i += 1 + getLengthLength(BERfilter + 1, &err) +
           ParseLength(BERfilter + 1, &err);
      skipTags(BERfilter, 1, &err);
    }

    break;
  case NOT:
    // TODO
    break;
  default:
    break;
  }
  // print values
  for (int i = 0; i < attributeDescription.size(); i++) {
    printf("%c", attributeDescription[i]);
  }
  printf("\n");
  for (int i = 0; i < assertionValue.size(); i++) {
    printf("%c", assertionValue[i]);
  }

  return f;
}

bool equalityMatchHSolver(unsigned char *filter) {}

bool andSolver(unsigned char *filter) {}

int searchRequestHandler(std::vector<unsigned char> &searchRequest,
                         int comm_socket) {
  // 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17
  // 30 42 02 01 02 63 3d 04 00 0a 01 02 0a 01 00 02 01 00 02 01 00 01 01 00
  // a0 20  a3 0d 04 03 75 69 64 04 06 72 69 65 6d 61 6e a3 0f 04 03 75 69
  // 64 04 08 65 69 6e 73 74 61 69 6e  30 08 04 02 63 6e 04 02 64 6e
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
  sr.messageID = new std::vector<unsigned char>();
  sr.sizeLimit = 0;
  sr.attributes.cn = false;
  sr.attributes.email = false;

  int envelopeLength = searchRequest[1] + 2; // lenght of envelope

  // 3: sequenc, searchRequest[4] lenght of string, 1: string tag
  int skipLength = 2; // skip envelope tag and lenght
  // set message ID
  sr.messageIDLength = searchRequest[skipLength + 1];
  // sr.messageID = (char *)malloc(sr.messageIDLength);
  // memcpy(sr.messageID, &searchRequest[0] + skipLength, sr.messageIDLength
  // + 2);
  for (int i = 0; i < sr.messageIDLength + 2; i++) {
    sr.messageID->push_back(searchRequest[2 + i]);
  }

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
  // std::vector<unsigned char> filter;
  //  std::copy(searchRequest.begin() + skipLength, searchRequest.end(),
  //          filter.begin());
  filter *f = convertToFilterObject(searchRequest.begin() + skipLength);
  skipLength += searchRequest[skipLength + 1] + 2; // skip sequence filter

  // print types of filters and their values
  printf("filter type: %d\n", f->getFilterType());

  andFilter *af = (andFilter *)f;

  std::vector<database_object> database;
  databaseController db("ldap-lidi-ascii.csv");

  database = db.loadAllRows();

  int errr = 0;
  std::vector<database_object> result;
  result = filterHandler(f, &errr, database);

  result = removeDuplicates(result);

  // print database
  //   for (int i = 0; i < result.size(); i++) {
  //     printf("name: %s\n", result[i].get_name().data());
  //     printf("email: %s\n", result[i].get_email().data());
  //     printf("uid: %s\n", result[i].get_uid().data());
  //   }
  // print all filters
  //   int iii = af->filters.size();
  //   for (int i = 0; i < af->filters.size(); i++) {
  //     printf("filter type: %d\n", af->filters[i]->getFilterType());
  //     // print values of inside filters
  //     if (af->filters[i]->getFilterType() == equalityMatch) {
  //       equalityMatchFilter *emf = (equalityMatchFilter *)af->filters[i];
  //       printf("attributeDescription: ");
  //       std::vector<unsigned char> attributeDescription =
  //           emf->getAttributeDescription();
  //       int size = attributeDescription.size();
  //       for (int j = 0; j < attributeDescription.size(); j++) {
  //         printf("%c", attributeDescription[j]);
  //       }
  //       printf("\n");
  //       printf("assertionValue: ");
  //       std::vector<unsigned char> assertionValue = emf->getAssertionValue();
  //       for (int j = 0; j < assertionValue.size(); j++) {
  //         printf("%c", assertionValue[j]);
  //       }
  //       printf("\n");
  //     }
  //   }

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

  // cn
  std::vector<unsigned char> cn = {'c', 'n'};
  // email
  std::vector<unsigned char> email = {'e', 'm', 'a', 'i', 'l'};
  // uid
  std::vector<unsigned char> uid = {'u', 'i', 'd'};

  for (int i = 0; i < result.size(); i++) {
    std::vector<unsigned char> searchResultEntry;
    InitSearchResultEntry(searchResultEntry, *(sr.messageID),
                          result[i].get_uid(),  result[i].get_uid().size());
    std::vector<unsigned char> resCN = result[i].get_name();
    std::vector<unsigned char> resEmail = result[i].get_email();
    std::vector<unsigned char> resUID = result[i].get_uid();
    AddToSearchResultEntry(searchResultEntry, cn, 2, resCN,
                           result[i].get_name().size());
    AddToSearchResultEntry(searchResultEntry, email, 5, resEmail,
                            result[i].get_email().size());
    AddToSearchResultEntry(searchResultEntry, uid, 3, resUID,
                            result[i].get_uid().size());
    send(comm_socket, &searchResultEntry[0], searchResultEntry.size(), 0);
  }

  sendSearchResultDone(searchRequest, comm_socket);

//   std::vector<unsigned char> testVal;
//   testVal.insert(testVal.end(), {
//                                     'c',
//                                     'n',
//                                 });
//   std::vector<unsigned char> testVal2;
//   testVal2.insert(testVal2.end(), {'t', 'e', 's', 't'});
//   std::vector<unsigned char> testVal3;
//   testVal3.insert(testVal3.end(), {
//                                       't',
//                                       'e',
//                                       's',
//                                       't',
//                                       '3',
//                                   });
//   std::vector<unsigned char> testVal4;
//   testVal4.insert(testVal4.end(), {'c', 'g', 'n'});

//   std::vector<unsigned char> searchResultEntry;
//   InitSearchResultEntry(searchResultEntry, *(sr.messageID), testVal3, 5);

//   // print searchResultEntry hex values
//   printf("searchResultEntry: ");
//   for (int i = 0; i < searchResultEntry[1] + 2; i++) {
//     printf("%02x ", searchResultEntry[i]);
//   }
//   printf("\n");

//   AddToSearchResultEntry(searchResultEntry, testVal, 2, testVal2, 4);
//   AddToSearchResultEntry(searchResultEntry, testVal4, 3, testVal3, 5);

//   send(comm_socket, &searchResultEntry[0], searchResultEntry[1] + 2, 0);
//   send(comm_socket, &searchResultEntry[0], searchResultEntry[1] + 2, 0);

//   sendSearchResultDone(searchRequest, comm_socket);

//   // print searchResultEntry hex values
//   printf("searchResultEntry: ");
//   for (int i = 0; i < searchResultEntry[1] + 2; i++) {
//     printf("%02x ", searchResultEntry[i]);
//   }
//   printf("\n");

  return 0;
}

int loadEnvelope(std::vector<unsigned char> &bindRequest, int comm_socket) {
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
        for (int i = 0; i < length + 2; i++) { // returns bind request
          bindRequest.push_back(buff[i]);
        }
        return length + 2;
      }
    } else // error or end of connection
      break;
  }
  return -1;
}
