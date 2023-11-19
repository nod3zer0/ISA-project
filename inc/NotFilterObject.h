/**
 * @file NotFilterObject.h
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @brief Helper functions for parsing arguments
 * @date 2023-11-19
 */
#ifndef NOTFILTEROBJECT_H
#define NOTFILTEROBJECT_H
#include "inc/FilterObject.h"
#include "inc/ber_helper_functions.h"

#include <vector>

class NotFilter : public FilterObject {
public:
  FilterObject *filter;
  filterTypes getFilterType();
  ~NotFilter();
};

#endif