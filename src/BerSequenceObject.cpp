/**
 * @file BerSequenceObject.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
#include "inc/BerSequenceObject.h"

berObjectTypes BerSequenceObject::getBerObjectType() {
  return berSequenceObject;
}

long long int BerSequenceObject::getLenght() {

  long long int dataLenght = 0;

  for (long unsigned int i = 0; i < objects.size(); i++) {
    dataLenght += objects[i]->getLenght();
  }

  return BER_TAG_LENGTH + BER_LENGTH_OF_LENGTH_TAG + BER_4BYTE_LENGTH_LENGTH +
         dataLenght;
}

std::vector<unsigned char> BerSequenceObject::getBerRepresentation() {
  std::vector<unsigned char> berRepresentation;
  long long int dataLenght = 0;
  for (long unsigned int i = 0; i < objects.size(); i++) {
    dataLenght += objects[i]->getLenght();
  }

  berRepresentation.push_back(tag);
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

BerSequenceObject::BerSequenceObject() { tag = BER_SEQUENCE_C; }

int BerSequenceObject::GetTag() { return tag; }

BerSequenceObject::BerSequenceObject(int tag) { this->tag = tag; }

BerSequenceObject::~BerSequenceObject() {
  for (long unsigned int i = 0; i < objects.size(); i++) {
    delete objects[i];
  }
}