#include "inc/BerEnumObject.h"

#include "inc/BerIntObject.h"

berObjectTypes BerEnumObject::getBerObjectType() { return berEnumObject; }

long long int BerEnumObject::getLenght() {
  const int ENUM_DATA_LENGTH = 1;
  return BER_TAG_LENGTH + BER_LENGTH_OF_LENGTH_TAG + BER_4BYTE_LENGTH_LENGTH +
         ENUM_DATA_LENGTH;
}

std::vector<unsigned char> BerEnumObject::getBerRepresentation() {
  const int ENUM_DATA_LENGTH = 1;
  std::vector<unsigned char> berRepresentation;
  berRepresentation.push_back(BER_ENUM_C);
  AppendLenght4Bytes(berRepresentation, ENUM_DATA_LENGTH);
  berRepresentation.push_back(value);
  return berRepresentation;
}

BerEnumObject::BerEnumObject(char value) { this->value = value; }

BerEnumObject::~BerEnumObject() {}