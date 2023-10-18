#ifndef BER_OBJECT_H
#define BER_OBJECT_H
#include "inc/ber_helper_functions.h"

class BerObject {
public:
  virtual berObjectTypes getBerObjectType();
  virtual long long int  getLenght();
  virtual std::vector<unsigned char> getBerRepresentation();
};

#endif