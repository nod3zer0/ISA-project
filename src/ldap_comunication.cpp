#include "inc/ldap_comunication.h"
#define DEBUG

/// @brief Inicializes and alocates partial empty attribute list
/// @param partialAttributeList pointer to empty attribute list
/// @return 0 if success, -1 if error
BerObject *InitSearchResultEntry(BerObject *searchRequest,
                                 std::vector<unsigned char> LDAPDN) {
  BerSequenceObject *envelope = new BerSequenceObject();
  envelope->objects.push_back(
      ((BerSequenceObject *)(searchRequest))->objects[0]); // copy message ID
  BerSequenceObject *PartialAttributeList =
      new BerSequenceObject(BER_SEARCH_RESULT_ENTRY_C);
  envelope->objects.push_back(PartialAttributeList);
  PartialAttributeList->objects.push_back(new BerStringObject(LDAPDN));
  PartialAttributeList->objects.push_back(new BerSequenceObject());
  return envelope;
}

int AddToSearchResultEntry(BerObject *envelope,
                           std::vector<unsigned char> &attributeDescription,
                           std::vector<unsigned char> &attributeValue) {
  int err = 0;
  BerSequenceObject *SearchResultEntry =
      (BerSequenceObject *)(((BerSequenceObject *)envelope)->objects[1]);
  BerSequenceObject *PartialAttributeList =
      (BerSequenceObject *)(((BerSequenceObject *)SearchResultEntry)
                                ->objects[1]);
  BerSequenceObject *attributeValueSequence = new BerSequenceObject();
  attributeValueSequence->objects.push_back(
      new BerStringObject(attributeDescription));
  BerSetObject *attributeValueSet = new BerSetObject();
  attributeValueSet->objects.push_back(new BerStringObject(attributeValue));
  attributeValueSequence->objects.push_back(attributeValueSet);

  PartialAttributeList->objects.push_back(attributeValueSequence);

  // debug print SearchResultEntry
#ifdef DEBUG
  std::vector<unsigned char> SearchResultEntryBer =
      SearchResultEntry->getBerRepresentation();
  printf("SearchResultEntryBer: ");
  for (unsigned long int i = 0; i < SearchResultEntryBer.size(); i++) {
    printf("%02x ", SearchResultEntryBer[i]);
    fflush(stdout);
  }
#endif
  return 0;
}

BerObject *CreateBindResponse(BerObject *bindRequest, int resultCode) {
  BerSequenceObject *envelope = new BerSequenceObject();
  envelope->objects.push_back(((BerSequenceObject *)(bindRequest))->objects[0]);
  BerSequenceObject *bindResponseSequence =
      new BerSequenceObject(BER_BIND_RESPONSE_C);
  envelope->objects.push_back(bindResponseSequence);
  bindResponseSequence->objects.push_back(new BerEnumObject(resultCode));
  bindResponseSequence->objects.push_back(new BerStringObject());
  bindResponseSequence->objects.push_back(new BerStringObject());
  return envelope;
}


int sendSearchResultDone(BerSequenceObject *searchRequest, int comm_socket,
                         unsigned int result_code) {
  int err = 0;
  BerSequenceObject *envelope = new BerSequenceObject();
  envelope->objects.push_back(searchRequest->objects[0]); // copy message ID
  BerSequenceObject *searchResultDoneSequence =
      new BerSequenceObject(BER_SEARCH_RESULT_DONE_C);
  envelope->objects.push_back(searchResultDoneSequence);
  searchResultDoneSequence->objects.push_back(new BerEnumObject(result_code));
  searchResultDoneSequence->objects.push_back(new BerStringObject(""));
  searchResultDoneSequence->objects.push_back(new BerStringObject(""));

  std::vector<unsigned char> envelopeBer = envelope->getBerRepresentation();
  send(comm_socket, &envelopeBer[0], envelopeBer.size(), 0);
  // print searchResultDoneSequenceBer hex

#ifdef DEBUG
  printf("searchResultDoneSequenceBer: ");
  for (unsigned long int i = 0; i < envelopeBer.size(); i++) {
    printf("%02x ", envelopeBer[i]);
    fflush(stdout);
  }
#endif
  return 0;
}

int searchRequestHandler(BerObject *searchRequest, int comm_socket,
                         const char *dbPath) {
  int err = 0;

  BerSequenceObject *envelope = (BerSequenceObject *)searchRequest;
  BerIntObject *messageID = (BerIntObject *)envelope->objects[0];
  BerSequenceObject *searchRequestSequence =
      (BerSequenceObject *)envelope->objects[1];

  searchRequestType sr;
  // inicialization TODO make constructor
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

  sr.sizeLimit =
      ((BerIntObject *)searchRequestSequence->objects[3])->getValue();
    std::vector<unsigned char> filtersBer = (searchRequestSequence->objects[6])->getBerRepresentation();
  filter *f = convertToFilterObject(
      filtersBer.begin(),filtersBer.end());
  printf("filter type: %d\n", f->getFilterType());

  std::vector<DatabaseObject> result;
  int errr = 0;
  result = filterHandler(f, &errr, dbPath, sr.sizeLimit);

  result = removeDuplicates(result);
  bool sizeLimitExceeded = false;
  if (errr == 1) {
    sizeLimitExceeded = true;
  }

  BerSequenceObject *attributesSequence =
      (BerSequenceObject *)searchRequestSequence->objects[7];

  for (int i = 0; i < attributesSequence->objects.size(); i++) {

    if (((BerStringObject *)attributesSequence->objects[i])->value == cn) {
      sr.attributes.cn = true;
    }
    if (((BerStringObject *)attributesSequence->objects[i])->value == email) {
      sr.attributes.email = true;
    }
    if (((BerStringObject *)attributesSequence->objects[i])->value == uid) {
      sr.attributes.uid = true;
    }
  }
  for (unsigned long int i = 0; i < result.size(); i++) {
    BerObject *searchResultEntry =
        InitSearchResultEntry(envelope, result[i].get_uid());

    std::vector<unsigned char> resCN = result[i].get_name();
    std::vector<unsigned char> resEmail = result[i].get_email();
    std::vector<unsigned char> resUID = result[i].get_uid();

    if (sr.attributes.cn) {
      AddToSearchResultEntry(searchResultEntry, cn, resCN);
    }
    if (sr.attributes.email) {
      AddToSearchResultEntry(searchResultEntry, email, resEmail);
    }
    if (sr.attributes.uid) {
      AddToSearchResultEntry(searchResultEntry, uid, resUID);
    }

    std::vector<unsigned char> searchResultEntryBer =
        searchResultEntry->getBerRepresentation();
    send(comm_socket, &searchResultEntryBer[0], searchResultEntryBer.size(), 0);
    // debub print searchResultEntryBer hex
#ifdef DEBUG
    printf("searchResultEntryBer: ");
    for (unsigned long int i = 0; i < searchResultEntryBer.size(); i++) {
      printf("%02x ", searchResultEntryBer[i]);
      fflush(stdout);
    }
#endif
  }

  if (sizeLimitExceeded) {

    sendSearchResultDone((BerSequenceObject *)envelope, comm_socket,
                         BER_LDAP_SIZE_LIMIT_EXCEEDED);
  } else {
    sendSearchResultDone((BerSequenceObject *)envelope, comm_socket,
                         BER_LDAP_SUCCESS);
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
    int returnCode = recv(comm_socket, buff + resNow, 1024, 0);
    if (returnCode == 0)
      return -1;
    resNow += returnCode;

    if (resNow >= 2) {
      if ((buff[1] < 0x80) ||
          (buff[1] & 0x7F) <= resNow) { // checks if bytes containing lenght of
        // message are complete
        bindRequest.insert(bindRequest.end(), buff, buff + resNow);
        lenghtOfMessage = GetLength(bindRequest.begin() + 1, &err, bindRequest.end()) +
                          GetLengthOfLength(bindRequest.begin() + 1, &err, bindRequest.end()) + 1;
        break;
      }
    }
  }
  resAll = resNow;
  for (;;) {
    resNow = 0;
    if (resAll < lenghtOfMessage) {
      int returnCode = recv(comm_socket, buff, 1024, 0);
      if (returnCode == 0)
        return -1;
      resNow += returnCode;
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
