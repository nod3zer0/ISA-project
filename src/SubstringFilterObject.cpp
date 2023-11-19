/**
 * @file SubstringFilterObject.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
#include "inc/SubstringFilterObject.h"

SubstringFilter::SubstringFilter(
    std::vector<unsigned char> attributeDescription,
    std::vector<unsigned char> subInitial,
    std::vector<std::vector<unsigned char>> subAny,
    std::vector<unsigned char> subFinal) {
  this->attributeDescription = attributeDescription;
  this->subInitial = subInitial;
  this->subAny = subAny;
  this->subFinal = subFinal;
};

std::vector<unsigned char> SubstringFilter::getAttributeDescription() {
  return attributeDescription;
};
std::vector<unsigned char> SubstringFilter::getSubInitial() {
  return subInitial;
};
std::vector<std::vector<unsigned char>> SubstringFilter::getSubAny() {
  return subAny;
};
std::vector<unsigned char> SubstringFilter::getSubFinal() { return subFinal; };
filterTypes SubstringFilter::getFilterType() { return substrings; };
