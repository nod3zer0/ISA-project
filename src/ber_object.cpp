/**
 * @file ber_object.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-02
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "inc/ber_object.h"

// class BerObject {
// public:
//   BerObject() {}
//   virtual ~BerObject() {}
//   char *lenghtPTR() { return position + 1; };
//   char *start;
//   char *position;
//   char *dataPTR() { // TODO
//     return nullptr;
//   };
//   int getDataLenght() { // TODO
//     return 0;
//   };
//   int getWholeLenght() { // TODO
//     return 0;
//   };
//   char *getLenhtLenght() { // TODO
//     return nullptr;
//   };
//   void gotToNextTag(){};
//   void goIntoTag(){};
//   void goToStart(){};
//   void appendCurrentTag(){};

//   char *tagPTR;
// };

filterTypes andFilter::getFilterType() { return AND; };

filterTypes orFilter::getFilterType() { return OR; };

filterTypes notFilter::getFilterType() { return NOT; };

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

filterTypes filter::getFilterType() { return undefined; };

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