#include "inc/BerIntObject.h"

berObjectTypes BerIntObject::getBerObjectType() { return berIntObject; }

long long int  BerIntObject::getLenght() {

  return BER_TAG_LENGTH + BER_LENGTH_OF_LENGTH_TAG + BER_4BYTE_LENGTH_LENGTH +
         HowManyBytesWillIntUse(value);
}

std::vector<unsigned char> BerIntObject::getBerRepresentation() {
  int err = 0;
  std::vector<unsigned char> berRepresentation;
  std::vector<unsigned char> intData;
  int dataLenght = WriteIntAppend(intData, value);
  berRepresentation.push_back(BER_INT_C);
  AppendLenght4Bytes(berRepresentation, dataLenght, &err);
  berRepresentation.insert(berRepresentation.end(), intData.begin(),
                           intData.end());
  return berRepresentation;
}

BerIntObject::BerIntObject() { value = 0;}

int BerIntObject::getValue() { return value; }

void BerIntObject::setValue(int value) { this->value = value; }