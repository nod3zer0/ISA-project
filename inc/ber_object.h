#ifndef BER_OBJECT_H
#define BER_OBJECT_H
#include "inc/ber_helper_functions.h"


#include <vector>

class filter {
public:
  virtual filterTypes  getFilterType();
};

class andFilter : public filter {
public:
  std::vector<filter*> filters;
  filterTypes getFilterType();
};

class orFilter : public filter {
public:
  std::vector<filter> filters;
  filterTypes getFilterType();
};

class notFilter : public filter {
public:
  filter filter;
  filterTypes getFilterType();
};

class SubstringFilter : public filter {

private:
  std::vector<unsigned char> initial;
  std::vector<unsigned char> any;
  std::vector<unsigned char> final;

public:
  SubstringFilter(std::vector<unsigned char> initial,
                  std::vector<unsigned char> any,
                  std::vector<unsigned char> final);
  std::vector<unsigned char> getInitial();
  std::vector<unsigned char> getAny();
  std::vector<unsigned char> getFinal();
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