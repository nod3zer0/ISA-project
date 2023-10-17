#ifndef EQUALITYMATCHFILTEROBJECT_H
#define EQUALITYMATCHFILTEROBJECT_H
#include "inc/FilterObject.h"
#include "inc/ber_helper_functions.h"

#include <vector>

class equalityMatchFilter : public filter {
private:
  std::vector<unsigned char> attributeDescription;
  std::vector<unsigned char> assertionValue;

public:
  equalityMatchFilter(std::vector<unsigned char> attributeDescription,
                      std::vector<unsigned char> assertionValue);
  std::vector<unsigned char> getAttributeDescription();
  std::vector<unsigned char> getAssertionValue();
  filterTypes getFilterType();
};
#endif