#include "inc/EqualityMatchFilterObject.h"

equalityMatchFilter::equalityMatchFilter(
    std::vector<unsigned char> attributeDescription,
    std::vector<unsigned char> assertionValue) {
  this->attributeDescription = attributeDescription;
  this->assertionValue = assertionValue;
};

std::vector<unsigned char> equalityMatchFilter::getAttributeDescription() {
  return attributeDescription;
};

std::vector<unsigned char> equalityMatchFilter::getAssertionValue() {
  return assertionValue;
};

filterTypes equalityMatchFilter::getFilterType() { return equalityMatch; };