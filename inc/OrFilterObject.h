#ifndef ORFILTEROBJECT_H
#define ORFILTEROBJECT_H
#include "inc/ber_helper_functions.h"
#include "inc/FilterObject.h"

#include <vector>

class orFilter : public FilterObject {
public:
  std::vector<FilterObject *> filters;
  filterTypes getFilterType();
};


#endif