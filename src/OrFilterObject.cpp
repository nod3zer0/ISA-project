#include "inc/OrFilterObject.h"

filterTypes OrFilter::getFilterType() { return OR; };

OrFilter::~OrFilter() {
    for (std::vector<FilterObject *>::iterator it = filters.begin();
         it != filters.end(); ++it) {
        delete *it;
    }
}
