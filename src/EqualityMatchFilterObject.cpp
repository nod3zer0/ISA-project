#include "inc/EqualityMatchFilterObject.h"

EqualityMatchFilter::EqualityMatchFilter(
    std::vector<unsigned char> attributeDescription,
    std::vector<unsigned char> assertionValue) {
  this->attributeDescription = attributeDescription;
  this->assertionValue = assertionValue;
};

std::vector<unsigned char> EqualityMatchFilter::getAttributeDescription() {
  return attributeDescription;
};

std::vector<unsigned char> EqualityMatchFilter::getAssertionValue() {
  return assertionValue;
};

filterTypes EqualityMatchFilter::getFilterType() { return equalityMatch; };