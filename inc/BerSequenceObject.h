#ifndef BERSEQUENCEOBJECT_H
#define BERSEQUENCEOBJECT_H
#include "inc/BerObject.h"
#include "inc/ber_constants.h"
#include "inc/ber_helper_functions.h"

#include <vector>

class BerSequenceObject : public BerObject {
private:
  int tag;

public:
  std::vector<BerObject *> objects;
  berObjectTypes getBerObjectType();
  long long int getLenght();
  std::vector<unsigned char> getBerRepresentation();
  BerSequenceObject(int tag);
  BerSequenceObject();
  int GetTag();
};

#endif