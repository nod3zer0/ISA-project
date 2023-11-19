/**
 * @file BerSetObject.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
#include "inc/BerSetObject.h"

berObjectTypes BerSetObject::getBerObjectType() { return berSequenceObject; }

long long int BerSetObject::getLenght() {

  long long int dataLenght = 0;

  for (long unsigned int i = 0; i < objects.size(); i++) {
    dataLenght += objects[i]->getLenght();
  }

  return BER_TAG_LENGTH + BER_LENGTH_OF_LENGTH_TAG + BER_4BYTE_LENGTH_LENGTH +
         dataLenght;
}

std::vector<unsigned char> BerSetObject::getBerRepresentation() {
  std::vector<unsigned char> berRepresentation;
  long long int dataLenght = 0;
  for (long unsigned int i = 0; i < objects.size(); i++) {
    dataLenght += objects[i]->getLenght();
  }

  berRepresentation.push_back(BER_SET_C);
  AppendLenght4Bytes(berRepresentation, dataLenght);
  for (long unsigned int i = 0; i < objects.size(); i++) {
    std::vector<unsigned char> objectRepresentation =
        objects[i]->getBerRepresentation();
    berRepresentation.insert(berRepresentation.end(),
                             objectRepresentation.begin(),
                             objectRepresentation.end());
  }

  return berRepresentation;
}

BerSetObject::BerSetObject() {}

BerSetObject::~BerSetObject() {
  for (long unsigned int i = 0; i < objects.size(); i++) {
    delete objects[i];
  }
}
