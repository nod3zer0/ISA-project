#ifndef BERINTOBJECT_H
#define BERINTOBJECT_H
#include "inc/BerObject.h"
#include "inc/ber_constants.h"
#include "inc/ber_helper_functions.h"
#include <vector>

class BerIntObject : public BerObject {
private:
  int value;

public:
  berObjectTypes getBerObjectType();
  int getValue();
  void setValue(int value);
  long long int getLenght();
  std::vector<unsigned char> getBerRepresentation();
  BerIntObject();
  BerIntObject(int value);
};

#endif