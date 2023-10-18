#include "inc/BerSetObject.h"

berObjectTypes BerSetObject::getBerObjectType() {
  return berSequenceObject;
}

long long int BerSetObject::getLenght() {

  long long int dataLenght = 0;

  for (int i = 0; i < objects.size(); i++) {
    dataLenght += objects[i]->getLenght();
  }

  return BER_TAG_LENGTH + BER_LENGTH_OF_LENGTH_TAG + BER_4BYTE_LENGTH_LENGTH +
         dataLenght;
}

std::vector<unsigned char> BerSetObject::getBerRepresentation() {
  int err = 0;
  std::vector<unsigned char> berRepresentation;
  long long int dataLenght = 0;
  for (int i = 0; i < objects.size(); i++) {
    dataLenght += objects[i]->getLenght();
  }

  berRepresentation.push_back(BER_SET_C);
  AppendLenght4Bytes(berRepresentation, dataLenght, &err);
  for (int i = 0; i < objects.size(); i++) {
    std::vector<unsigned char> objectRepresentation =
        objects[i]->getBerRepresentation();
    berRepresentation.insert(berRepresentation.end(),
                             objectRepresentation.begin(),
                             objectRepresentation.end());
  }

  return berRepresentation;
}

BerSetObject::BerSetObject() { }
