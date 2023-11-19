/**
 * @file AndFilterObject.h
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @brief And filter object for BER LDAP
 * @date 2023-11-19
 */
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