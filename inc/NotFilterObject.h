#ifndef NOTFILTEROBJECT_H
#define NOTFILTEROBJECT_H
#include "inc/ber_helper_functions.h"
#include "inc/FilterObject.h"

#include <vector>

class notFilter : public FilterObject {
public:
  FilterObject *filter;
  filterTypes getFilterType();
};

#endif