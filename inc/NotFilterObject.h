#ifndef NOTFILTEROBJECT_H
#define NOTFILTEROBJECT_H
#include "inc/ber_helper_functions.h"
#include "inc/FilterObject.h"

#include <vector>

class notFilter : public filter {
public:
  filter *filter;
  filterTypes getFilterType();
};

#endif