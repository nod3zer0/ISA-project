/**
 * @file FilterObject.h
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @brief Base class for all filter objects
 * @date 2023-11-19
 */
#ifndef FILTER_OBJECT_H
#define FILTER_OBJECT_H
#include "inc/ber_helper_functions.h"

#include <vector>

/**
 * @brief base class for all filter objects
 *
 */
class FilterObject {
public:
  virtual filterTypes getFilterType();
  virtual ~FilterObject();
};


#endif