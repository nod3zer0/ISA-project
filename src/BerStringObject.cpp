/**
 * @file BerStringObject.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
#include "inc/BerStringObject.h"

berObjectTypes BerStringObject::getBerObjectType() { return berStringObject; }
long long int BerStringObject::getLenght() {

  return BER_TAG_LENGTH + BER_LENGTH_OF_LENGTH_TAG + BER_4BYTE_LENGTH_LENGTH +
         value.size();
}

std::vector<unsigned char> BerStringObject::getBerRepresentation() {
  std::vector<unsigned char> berRepresentation;
  berRepresentation.push_back(BER_OCTET_STRING_C);
  AppendLenght4Bytes(berRepresentation, value.size());
  berRepresentation.insert(berRepresentation.end(), value.begin(), value.end());
  return berRepresentation;
}

BerStringObject::BerStringObject() {}

BerStringObject::BerStringObject(std::vector<unsigned char> value) {
  this->value = value;
}
BerStringObject::BerStringObject(std::string value) {

  this->value = std::vector<unsigned char>(value.begin(), value.end());
}
