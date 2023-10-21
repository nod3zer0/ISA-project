#ifndef BERSTRINGOBJECT_H
#define BERSTRINGOBJECT_H
#include "inc/BerObject.h"
#include "inc/ber_constants.h"
#include "inc/ber_helper_functions.h"
#include <string>
#include <vector>

class BerStringObject : public BerObject {
public:
  berObjectTypes getBerObjectType();
  std::vector<unsigned char> value;
  long long int getLenght();
  std::vector<unsigned char> getBerRepresentation();
  BerStringObject();
  BerStringObject(std::vector<unsigned char> value);
BerStringObject(std::string value);
};

#endif