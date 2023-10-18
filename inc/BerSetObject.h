#ifndef BERSETOBJECT_H
#define BERSETOBJECT_H
#include "inc/BerObject.h"
#include "inc/ber_constants.h"
#include "inc/ber_helper_functions.h"

#include <vector>

class BerSetObject : public BerObject {
public:
  std::vector<BerObject *> objects;
  berObjectTypes getBerObjectType();
  long long int getLenght();
  std::vector<unsigned char> getBerRepresentation();
  BerSetObject();
};

#endif