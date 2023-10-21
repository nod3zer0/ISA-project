#include "inc/BerIntObject.h"

berObjectTypes BerIntObject::getBerObjectType() { return berIntObject; }

long long int BerIntObject::getLenght() {

  return HowManyBytesWillIntUse(value);
}

std::vector<unsigned char> BerIntObject::getBerRepresentation() {
  std::vector<unsigned char> berRepresentation;
  WriteIntAppend(berRepresentation, value);
  return berRepresentation;
}

BerIntObject::BerIntObject() { value = 0; }

int BerIntObject::getValue() { return value; }

void BerIntObject::setValue(int value) { this->value = value; }