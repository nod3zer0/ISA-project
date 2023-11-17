#ifndef ANDFILTEROBJECT_H
#define ANDFILTEROBJECT_H
#include "inc/FilterObject.h"
#include "inc/ber_helper_functions.h"

#include <vector>

class AndFilter : public FilterObject {
public:
  std::vector<FilterObject *> filters;
  filterTypes getFilterType();
  ~AndFilter();
};

#endif