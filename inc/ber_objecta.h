#ifndef BER_OBJECT_H
#define BER_OBJECT_H
#include "inc/ber_helper_functions.h"

#include <vector>

class filter {
public:
  virtual filterTypes getFilterType();
};

class andFilter : public filter {
public:
  std::vector<filter *> filters;
  filterTypes getFilterType();
};

class orFilter : public filter {
public:
  std::vector<filter *> filters;
  filterTypes getFilterType();
};

class notFilter : public filter {
public:
  filter *filter;
  filterTypes getFilterType();
};

class SubstringFilter : public filter {

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