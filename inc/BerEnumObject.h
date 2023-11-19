/**
 * @file BerEnumObject.h
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @brief Enum object for BER LDAP
 * @date 2023-11-19
 */

#ifndef BERENUMOBJECT_H
#define BERENUMOBJECT_H
#include "inc/BerObject.h"
#include "inc/ber_constants.h"
#include "inc/ber_helper_functions.h"
#include <vector>

class BerEnumObject : public BerObject {
private:
  int value;

public:
  berObjectTypes getBerObjectType();
  long long int getLenght();
  std::vector<unsigned char> getBerRepresentation();
  BerEnumObject(char value);
  ~BerEnumObject();
};

#endif