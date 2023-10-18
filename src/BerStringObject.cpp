#include "inc/BerStringObject.h"

berObjectTypes BerStringObject::getBerObjectType() { return berStringObject; }
long long int  BerStringObject::getLenght() {

  return BER_TAG_LENGTH + BER_LENGTH_OF_LENGTH_TAG + BER_4BYTE_LENGTH_LENGTH +
         value.size();
}

std::vector<unsigned char> BerStringObject::getBerRepresentation() {
  int err = 0;
  std::vector<unsigned char> berRepresentation;
  berRepresentation.push_back(BER_OCTET_STRING_C);
  AppendLenght4Bytes(berRepresentation, value.size(), &err);
  berRepresentation.insert(berRepresentation.end(), value.begin(), value.end());
  return berRepresentation;
}

BerStringObject::BerStringObject() {}
BerStringObject::BerStringObject(char * value) {
    for (int i = 0; i < strlen(value); i++) {
        this->value.push_back(value[i]);
    }
}