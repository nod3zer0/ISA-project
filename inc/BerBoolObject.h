#ifndef BERBOOLOBJECT_H
#define BERBOOLOBJECT_H
#include "inc/BerObject.h"
#include "inc/ber_constants.h"
#include "inc/ber_helper_functions.h"
#include <vector>

class BerBoolObject : public BerObject {
private:
  bool value;

public:
  berObjectTypes getBerObjectType();
  long long int getLenght();
  std::vector<unsigned char> getBerRepresentation();
  BerBoolObject(char value);
  ~BerBoolObject();
};

#endif