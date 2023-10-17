#ifndef FILTER_OBJECT_H
#define FILTER_OBJECT_H
#include "inc/ber_helper_functions.h"

#include <vector>

class filter {
public:
  virtual filterTypes getFilterType();
};


#endif