#ifndef ANDFILTEROBJECT_H
#define ANDFILTEROBJECT_H
#include "inc/ber_helper_functions.h"
#include "inc/FilterObject.h"

#include <vector>


class andFilter : public filter {
public:
  std::vector<filter *> filters;
  filterTypes getFilterType();
};

#endif