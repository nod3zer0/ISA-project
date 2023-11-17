#include "inc/AndFilterObject.h"

filterTypes AndFilter::getFilterType() { return AND; };
AndFilter::~AndFilter() {
    for (std::vector<FilterObject *>::iterator it = filters.begin();
         it != filters.end(); ++it) {
        delete *it;
    }
}
