#ifndef EQUALITYMATCHFILTEROBJECT_H
#define EQUALITYMATCHFILTEROBJECT_H
#include "inc/FilterObject.h"
#include "inc/ber_helper_functions.h"

#include <vector>

class EqualityMatchFilter : public FilterObject {
private:
  std::vector<unsigned char> attributeDescription;
  std::vector<unsigned char> assertionValue;

public:
  EqualityMatchFilter(std::vector<unsigned char> attributeDescription,
                      std::vector<unsigned char> assertionValue);
  std::vector<unsigned char> getAttributeDescription();
  std::vector<unsigned char> getAssertionValue();
  filterTypes getFilterType();
};
#endif