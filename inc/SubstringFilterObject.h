/**
 * @file SubstringFilterObject.h
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @brief Object for substring filter
 * @date 2023-11-19
 */
#ifndef SUBSTRINGFILTER_H
#define SUBSTRINGFILTER_H
#include "inc/ber_helper_functions.h"
#include "inc/FilterObject.h"
#include <vector>

class SubstringFilter : public FilterObject {

private:
  std::vector<unsigned char> attributeDescription;
  std::vector<unsigned char> subInitial;
  std::vector<std::vector<unsigned char>> subAny;
  std::vector<unsigned char> subFinal;

public:
  SubstringFilter(std::vector<unsigned char> attributeDescription,
                  std::vector<unsigned char> subInitial,
                  std::vector<std::vector<unsigned char>> subAny,
                  std::vector<unsigned char> subFinal);
  std::vector<unsigned char> getAttributeDescription();
  std::vector<unsigned char> getSubInitial();
  std::vector<std::vector<unsigned char>> getSubAny();
  std::vector<unsigned char> getSubFinal();
  filterTypes getFilterType();
};

#endif