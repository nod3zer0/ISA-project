/**
 * @file BerSetObject.h
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @brief Set object for BER LDAP
 * @date 2023-11-19
 */

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
  ~BerSetObject();
};

#endif