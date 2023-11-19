/**
 * @file BerObject.h
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @brief Base class for all BER objects
 * @date 2023-11-19
 */
#ifndef BER_OBJECT_H
#define BER_OBJECT_H
#include "inc/ber_helper_functions.h"

/**
 * @brief Base class for all BER objects
 *
 */
class BerObject {
public:
  /**
   * @brief Get type of BerObject
   *
   * @return berObjectTypes
   */
  virtual berObjectTypes getBerObjectType();
  /**
   * @brief Get the Lenght of BerObject representation in BER (including tag and
   * lenght)
   *
   * @return long long int
   */
  virtual long long int getLenght();
  /**
   * @brief Returns the BER representation of BerObject
   *
   * @return std::vector<unsigned char>
   */
  virtual std::vector<unsigned char> getBerRepresentation();

  virtual ~BerObject();
};

#endif