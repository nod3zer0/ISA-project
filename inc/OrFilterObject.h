/**
 * @file OrFilterObject.h
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @brief Object for OR filter
 * @date 2023-11-19
 */
#ifndef ORFILTEROBJECT_H
#define ORFILTEROBJECT_H
#include "inc/FilterObject.h"
#include "inc/ber_helper_functions.h"

#include <vector>

class OrFilter : public FilterObject {
public:
  std::vector<FilterObject *> filters;
  filterTypes getFilterType();
  ~OrFilter();
};

#endif