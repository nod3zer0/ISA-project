#ifndef ANDFILTEROBJECT_H
#define ANDFILTEROBJECT_H
#include "inc/ber_helper_functions.h"
#include "inc/FilterObject.h"

#include <vector>


class andFilter : public FilterObject {
public:
  std::vector<FilterObject *> filters;
  filterTypes getFilterType();
};

#endif