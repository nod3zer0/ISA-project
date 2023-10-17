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
  // longform 4 bytes
  partialAttributeList.push_back(0x84); // length of sequence
  std::vector<unsigned char>::iterator lengthLocation =
      partialAttributeList.begin() + 1;
  partialAttributeList.push_back(0x00);
  partialAttributeList.push_back(0x00);
  partialAttributeList.push_back(0x00);
  partialAttributeList.push_back(0x00);

  int err = 0;
  IncreaseLength4Bytes(lengthLocation, LDAPDNLength + 0x09 + 0x04 + 0x04, &err);

  int MsgIdLenght =
      copyMessageIDappend(messageID.begin(), partialAttributeList);

  IncreaseLength4Bytes(lengthLocation, MsgIdLenght,
                       &err); // length of sequence

  partialAttributeList.push_back(BER_SEARCH_RESULT_ENTRY_C); // Aplication 4 tag
  partialAttributeList.push_back(0x84);
  lengthLocation =
      partialAttributeList.begin() + partialAttributeList.size() - 1;

  partialAttributeList.push_back(0x00);
  partialAttributeList.push_back(0x00);
  partialAttributeList.push_back(0x00);
  partialAttributeList.push_back(0x00);
  IncreaseLength4Bytes(lengthLocation, 0x04 + 0x04 + LDAPDNLength, &err);
  // length of
  // PartialAttributeList LDAPN
  partialAttributeList.push_back(BER_OCTET_STRING_C); // octet string
  partialAttributeList.push_back(LDAPDNLength);       // length of string

  int i = 0;
  for (; i < LDAPDNLength; i++) {
    partialAttributeList.push_back(LDAPDN[i]);
  }

  // sequence
  partialAttributeList.push_back(BER_SEQUENCE_C);
  partialAttributeList.push_back(0x84);
  partialAttributeList.push_back(0x00);
  partialAttributeList.push_back(0x00);
  partialAttributeList.push_back(0x00);
  partialAttributeList.push_back(0x00); // length of sequence

  return 0;
}

bool substrFilterHandler(SubstringFilter *sf, int *err,
                         std::vector<unsigned char> attribute) {
  std::vector<unsigned char> attributeInital;
  std::vector<unsigned char> attributeMiddle;
  std::vector<unsigned char> attributeFinal;

  // extract initial
  if (!sf->getSubInitial().empty()) {
    if (attribute.size() >= sf->getSubInitial().size()) {
      attributeInital = std::vector<unsigned char>(
          attribute.begin(), attribute.begin() + sf->getSubInitial().size());
      attributeMiddle = std::vector<unsigned char>(
          attribute.begin() + sf->getSubInitial().size(), attribute.end());
    } else {
      return false;
    }
  }
  // extract final
  if (!sf->getSubFinal().empty()) {
    if (attribute.size() >= sf->getSubFinal().size()) {
      attributeFinal = std::vector<unsigned char>(
          attribute.end() - sf->getSubFinal().size(), attribute.end());
      if (attributeMiddle.size() > sf->getSubFinal().size()) {
        attributeMiddle = std::vector<unsigned char>(
            attributeMiddle.begin(),
            attributeMiddle.end() - sf->getSubFinal().size());
      }
    } else {
      return false;
    }
  }

  if (!sf->getSubInitial().empty() && attributeInital != sf->getSubInitial()) {
    return false;
  }
  if (!sf->getSubFinal().empty() && attributeFinal != sf->getSubFinal()) {
    return false;
  }
  unsigned long int x = 0;

  for (unsigned long int y = 0; y < sf->getSubAny().size(); y++) {
    bool match = false;
    for (; x < attributeMiddle.size(); x++) {

      if (attributeMiddle[x] == sf->getSubAny()[y][0]) {
        match = true;
        for (unsigned long int z = 0; z < sf->getSubAny()[y].size(); z++) {
          if (attributeMiddle[x + z] != sf->getSubAny()[y][z]) {
            match = false;
            break;
          }
        }
        if (match) {
          break;
        }
      }
    }
    if (!match) {
      return false;
    }
  }
  return true;
}

bool equalityMatchHandler(equalityMatchFilter *emf, int *err,
                          std::vector<unsigned char> attribute) {
  if (emf->getAssertionValue() == attribute) {
    return true;
  }
  return false;
}

bool filterLine(filter *f, int *err, DatabaseObject &databaseEntry) {

  // cn
  std::vector<unsigned char> cn = {'c', 'n'};
  // email
  std::vector<unsigned char> email = {'e', 'm', 'a', 'i', 'l'};
  // uid
  std::vector<unsigned char> uid = {'u', 'i', 'd'};

  switch (f->getFilterType()) {
  case equalityMatch: {
    std::vector<unsigned char> attributeDescription =
        ((equalityMatchFilter *)f)->getAttributeDescription();
    if (attributeDescription == cn) {
      return equalityMatchHandler((equalityMatchFilter *)f, err,
                                  databaseEntry.get_name());
    } else if (attributeDescription == email) {
      return equalityMatchHandler((equalityMatchFilter *)f, err,
                                  databaseEntry.get_email());
    } else if (attributeDescription == uid) {
      return equalityMatchHandler((equalityMatchFilter *)f, err,
                                  databaseEntry.get_uid());
    }

  } break;
  case substrings: {
    std::vector<unsigned char> attributeDescription =
        ((SubstringFilter *)f)->getAttributeDescription();
    if (attributeDescription == cn) {
      return substrFilterHandler((SubstringFilter *)f, err,
                                 databaseEntry.get_name());
    } else if (attributeDescription == email) {
      return substrFilterHandler((SubstringFilter *)f, err,
                                 databaseEntry.get_email());
    } else if (attributeDescription == uid) {
      return substrFilterHandler((SubstringFilter *)f, err,
                                 databaseEntry.get_uid());
    }
  } break;
  case AND: {
    andFilter *af = (andFilter *)f;
    for (unsigned long int i = 0; i < af->filters.size(); i++) {
      if (!filterLine(af->filters[i], err, databaseEntry)) {
        return false;
      }
    }
    return true;

  } break;
  case OR: {
    orFilter *of = (orFilter *)f;
    for (unsigned long int i = 0; i < of->filters.size(); i++) {
      if (filterLine(of->filters[i], err, databaseEntry)) {
        return true;
      }
    }
    return false;
  } break;
  case NOT: {
    notFilter *nf = (notFilter *)f;
    return !filterLine((nf->filter), err, databaseEntry);
  } break;
  default:
    return false;
  }
  return false;
}

std::vector<DatabaseObject>
filterHandler(filter *f, int *err, const char *dbLocation, int sizeLimit) {

  std::vector<DatabaseObject> resultDB;
  int dbErr = 0;
  DatabaseController db(dbLocation);
  int lineCounter = 0;
  if (f->getFilterType() == undefined) {
    while (true) {

      DatabaseObject obj = db.loadNextRow(&dbErr);

      if (dbErr != 0)
        break;
      resultDB.push_back(obj);
      lineCounter++;
      if (lineCounter >= sizeLimit && sizeLimit != 0) {
        *err = 1;
        break;
      }
    }
  } else {

    while (true) {

      DatabaseObject obj = db.loadNextRow(&dbErr);

      if (dbErr != 0)
        break;

      if (filterLine(f, err, obj)) {
        resultDB.push_back(obj);
        lineCounter++;
      }

      if (lineCounter >= sizeLimit && sizeLimit != 0) {
        *err = 1;
        break;
      }
    }
  }

  return resultDB;
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

  // increase length of sequence

  std::vector<unsigned char>::iterator locationOfSequence =
      partialAttributeList.begin();

  // partialAttributeList[5] += increaseBy;
  std::vector<unsigned char>::iterator locationOfLength =
      locationOfSequence + 1;
  IncreaseLength4Bytes(locationOfLength, increaseBy, &err);

  goIntoTag(locationOfSequence, &err);
  // go into sequence
  if (err != 0)
    return -1;
  skipTags(locationOfSequence, 1, &err); // skip string messageID
  if (err != 0)
    return -1;
  std::vector<unsigned char>::iterator locationOfPartialAttributeListLenght =
      locationOfSequence + 1;
  IncreaseLength4Bytes(locationOfPartialAttributeListLenght, increaseBy, &err);
  goIntoTag(locationOfSequence, &err); // go into application 4
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

  locationOfPartialAttributeListLenght = locationOfPartialAttributeList + 1;

  IncreaseLength4Bytes(locationOfPartialAttributeListLenght, increaseBy, &err);

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

  // print partialAttributeList hex values
  printf("partialAttributeList: ");
  for (unsigned long int i = 0; i < partialAttributeList.size(); i++) {
    printf("%02x ", partialAttributeList[i]);
    fflush(stdout);
  }

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
  bindResponse.push_back(0x84); // length of sequence
  bindResponse.push_back(0x00);
  bindResponse.push_back(0x00);
  bindResponse.push_back(0x00);
  bindResponse.push_back(0x00);
  int err = 0;
  std::vector<unsigned char>::iterator messageIDLocation =
      bindRequest.begin() + 2;
  int messageIDLenght = copyMessageIDappend(messageIDLocation, bindResponse);
  std::vector<unsigned char>::iterator lengthLocation =
      bindResponse.begin() + 1;
  IncreaseLength4Bytes(lengthLocation, 0x0c + messageIDLenght, &err);

  // message ID

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

int copyMessageIDappend(std::vector<unsigned char>::iterator messageID,
                        std::vector<unsigned char> &target) {
  target.push_back(messageID[0]); // copy tag TODO longform
  target.push_back(messageID[1]); // copy length
  int x = 0;
  if (1 < messageID[1]) {
    x = 1;
    for (; x < messageID[1]; x++) {
      target.push_back(messageID[2 + x]);
    }
  } else {
    target.push_back(messageID[2]);
  }
  return x;
}

int sendSearchResultDone(std::vector<unsigned char> &searchRequest,
                         int comm_socket, unsigned int result_code) {
  int err = 0;
  std::vector<unsigned char>::iterator messageIDlocation =
      searchRequest.begin();
  goIntoTag(messageIDlocation, &err);
  // 30 0c 02 01 02 65 07 0a 01 00 04 00 04 00
  std::vector<unsigned char> envelope;
  // char envelope[30];
  //  sequence
  envelope.push_back(BER_SEQUENCE_C);
  envelope.push_back(0x0c); // length of sequence
                            // message ID

  int x = copyMessageIDappend(messageIDlocation, envelope);

  envelope.push_back(BER_SEARCH_RESULT_DONE_C);
  envelope.push_back(0x07); // length

  envelope.push_back(BER_ENUM_C);
  envelope.push_back(0x01); // length
  envelope.push_back(result_code);
  envelope.push_back(BER_OCTET_STRING_C);
  envelope.push_back(0x00); // length
  envelope.push_back(BER_OCTET_STRING_C);
  envelope.push_back(0x00); // length


  send(comm_socket, &envelope[0], 14 + x, 0);

  return 1;
}

filter *convertToFilterObject(std::vector<unsigned char>::iterator BERfilter) {

  filter *f;
  int err;
  int lenght = 0;
  int ll = 0;
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
  case substrings: {
    std::vector<unsigned char> attributeDescription;
    std::vector<unsigned char> initial;
    std::vector<std::vector<unsigned char>> any;
    std::vector<unsigned char> final;
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
    int lenghtOfSequence = ParseLength(BERfilter + 1, &err);
    int lenghtOflenOfSequence = getLengthLength(BERfilter + 1, &err);
    goIntoTag(BERfilter, &err);
    if (err != 0)
      return new filter();

    int currentBitPointer = 0;
    while (currentBitPointer < lenghtOfSequence + lenghtOflenOfSequence) {
      switch (BERfilter[0]) {
      case 0x80: {
        int dataLenght = ParseLength(BERfilter + 1, &err);
        int LenghtLenght = getLengthLength(BERfilter + 1, &err);
        initial = std::vector<unsigned char>(BERfilter + 1 + LenghtLenght,
                                             BERfilter + 1 + LenghtLenght +
                                                 dataLenght);
      } break;
      case 0x81: {
        int dataLenght = ParseLength(BERfilter + 1, &err);
        int LenghtLenght = getLengthLength(BERfilter + 1, &err);
        std::vector<unsigned char> tmp = std::vector<unsigned char>(
            BERfilter + 1 + LenghtLenght,
            BERfilter + 1 + LenghtLenght + dataLenght);
        any.push_back(tmp);
      } break;
      case 0x82: {
        int dataLenght = ParseLength(BERfilter + 1, &err);
        int LenghtLenght = getLengthLength(BERfilter + 1, &err);
        final = std::vector<unsigned char>(BERfilter + 1 + LenghtLenght,
                                           BERfilter + 1 + LenghtLenght +
                                               dataLenght);
      } break;
      }
      currentBitPointer += 1 + getLengthLength(BERfilter + 1, &err) +
                           ParseLength(BERfilter + 1, &err);
      skipTags(BERfilter, 1, &err);
    }
    f = new SubstringFilter(attributeDescription, initial, any, final);

  } break;
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
  case NOT: {
    f = new notFilter();
    lenght = ParseLength(BERfilter + 1, &err);
    goIntoTag(BERfilter, &err);
    if (err != 0)
      return new filter();
    filter *tmpF = convertToFilterObject(BERfilter);
    ((notFilter *)f)->filter = tmpF;
    break;
  }
  default:
    f = new filter();
    break;
  }
  // print values
  for (unsigned long int i = 0; i < attributeDescription.size(); i++) {
    printf("%c", attributeDescription[i]);
  }
  printf("\n");
  for (unsigned long int i = 0; i < assertionValue.size(); i++) {
    printf("%c", assertionValue[i]);
  }

  return f;
}

int searchRequestHandler(std::vector<unsigned char> &searchRequest,
                         int comm_socket) {
  int err = 0;
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
  sr.attributes.uid = false;

  // cn
  std::vector<unsigned char> cn = {'c', 'n'};
  // email
  std::vector<unsigned char> email = {'e', 'm', 'a', 'i', 'l'};
  // uid
  std::vector<unsigned char> uid = {'u', 'i', 'd'};

  // longform
  std::vector<unsigned char>::iterator envelopePointer = searchRequest.begin();

  goIntoTag(envelopePointer, &err);
  // 3: sequenc, searchRequest[4] lenght of string, 1: string tag
  int skipLength = 2; // skip envelope tag and lenght
  // set message ID
  sr.messageIDLength = ParseLength(searchRequest.begin() + 1, &err);

  for (int i = 0; i < sr.messageIDLength + 2; i++) {
    sr.messageID->push_back(envelopePointer[i]);
  }

  skipTags(envelopePointer, 1, &err); // skip message ID
  goIntoTag(envelopePointer, &err);   // go into application 3
  skipTags(envelopePointer, 1, &err); // skip base object
  skipTags(envelopePointer, 1, &err); // skip enum scope
  skipTags(envelopePointer, 1, &err); // skip enum derefAliases
  sr.sizeLimit = ParseINT(envelopePointer, &err);
  skipTags(envelopePointer, 1, &err); // skip int sizeLimit
  skipTags(envelopePointer, 1, &err); // skip int timeLimit
  skipTags(envelopePointer, 1, &err); // skip bool typesOnly

  filter *f = convertToFilterObject(envelopePointer);
  skipTags(envelopePointer, 1, &err); // skip filter

  printf("filter type: %d\n", f->getFilterType());

  int errr = 0;
  std::vector<DatabaseObject> result;
  result = filterHandler(f, &errr, "ldap-lidi-ascii.csv", sr.sizeLimit);

  result = removeDuplicates(result);
  bool sizeLimitExceeded = false;
  if (errr == 1) {
    sizeLimitExceeded = true;
  }

  std::vector<unsigned char>::iterator attributesPointer = envelopePointer;

  int attributesLength = ParseLength(attributesPointer + 1, &err);

  goIntoTag(attributesPointer, &err); // go into attributes sequence

  skipLength = 0;

  for (; skipLength < attributesLength;) {

    int atrributeLengthLenght = getLengthLength(attributesPointer + 1, &err);
    int atributeDataLength = ParseLength(attributesPointer + 1, &err);

    std::vector<unsigned char> attributeDescription =
        std::vector<unsigned char>(
            &attributesPointer[1 + atrributeLengthLenght],
            &attributesPointer[1 + atributeDataLength + atrributeLengthLenght]);

    if (attributeDescription == cn) {
      sr.attributes.cn = true;
    }
    if (attributeDescription == email) {
      sr.attributes.email = true;
    }
    if (attributeDescription == uid) {
      sr.attributes.uid = true;
    }
    skipLength += 1 + atributeDataLength + atrributeLengthLenght;
    skipTags(attributesPointer, 1, &err);
  }

  for (unsigned long int i = 0; i < result.size(); i++) {
    std::vector<unsigned char> searchResultEntry;
    InitSearchResultEntry(searchResultEntry, *(sr.messageID),
                          result[i].get_uid(), result[i].get_uid().size());
    std::vector<unsigned char> resCN = result[i].get_name();
    std::vector<unsigned char> resEmail = result[i].get_email();
    std::vector<unsigned char> resUID = result[i].get_uid();

    if (sr.attributes.cn) {
      AddToSearchResultEntry(searchResultEntry, cn, 2, resCN,
                             result[i].get_name().size());
    }
    if (sr.attributes.email) {
      AddToSearchResultEntry(searchResultEntry, email, 5, resEmail,
                             result[i].get_email().size());
    }
    if (sr.attributes.uid) {
      AddToSearchResultEntry(searchResultEntry, uid, 3, resUID,
                             result[i].get_uid().size());
    }
    send(comm_socket, &searchResultEntry[0], searchResultEntry.size(), 0);
  }
  if (sizeLimitExceeded) {
    sendSearchResultDone(searchRequest, comm_socket,
                         BER_LDAP_SIZE_LIMIT_EXCEEDED);
  } else {
    sendSearchResultDone(searchRequest, comm_socket, BER_LDAP_SUCCESS);
  }
  return 0;
}

int loadEnvelope(std::vector<unsigned char> &bindRequest, int comm_socket) {
  unsigned char buff[1024];
  int lenghtOfMessage = 0;
  int err;
  int resNow = 0;
  int resAll = 0;
  for (;;) { // loads lenght of message

    resNow += recv(comm_socket, buff + resNow, 1024, 0);
    if (resNow >= 2) {
      if ((buff[1] < 0x80) ||
          (buff[1] & 0x7F) <= resNow) { // checks if bytes containing lenght of
        // message are complete
        bindRequest.insert(bindRequest.end(), buff, buff + resNow);
        lenghtOfMessage = ParseLength(bindRequest.begin() + 1, &err) +
                          getLengthLength(bindRequest.begin() + 1, &err) + 1;
        break;
      }
    }
  }
  resAll = resNow;
  for (;;) {
    resNow = 0;
    if (resAll < lenghtOfMessage) {
      resNow += recv(comm_socket, buff, 1024, 0);
      resAll += resNow;
    }

    if (resAll > 0) {
      // check if message is envelope
      if (buff[0] != 0x30) {
        printf("invalid message\n");
        // TODO respond with error message
        break;
      }
      int length = 0;
      length = buff[1]; // TODO: longform

      bindRequest.insert(bindRequest.end(), buff, buff + resNow);
      // if whole message received, return response
      if (resAll >= lenghtOfMessage) {
        return length + 2;
      }
    } else // error or end of connection
      break;
  }
  return -1;
}
