#include "inc/BerBoolObject.h"

berObjectTypes BerBoolObject::getBerObjectType() { return berBoolObject; }

long long int BerBoolObject::getLenght() {
  const int BOOL_DATA_LENGTH = 1;
  return BER_TAG_LENGTH + BER_LENGTH_OF_LENGTH_TAG + BOOL_DATA_LENGTH;
}

std::vector<unsigned char> BerBoolObject::getBerRepresentation() {
  const int BOOL_DATA_LENGTH = 1;
  std::vector<unsigned char> berRepresentation;
  berRepresentation.push_back(BER_BOOL_C);
  berRepresentation.push_back(BOOL_DATA_LENGTH);
  if (value > 0) {
    berRepresentation.push_back(0xFF);
  } else {
    berRepresentation.push_back(0x00);
  }
  return berRepresentation;
}

BerBoolObject::BerBoolObject(char value) { this->value = value; }
