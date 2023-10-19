#ifndef BERUNDEFINEDOBJECT_H
#define BERUNDEFINEDOBJECT_H
#include "inc/BerObject.h"
#include "inc/ber_helper_functions.h"

class BerUndefinedObject : public BerObject {
private:
  std::vector<unsigned char> value;

public:
  berObjectTypes getBerObjectType();
  long long int getLenght();
  std::vector<unsigned char> getBerRepresentation();
  BerUndefinedObject(std::vector<unsigned char> value);
};

#endif