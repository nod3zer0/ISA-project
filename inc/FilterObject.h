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