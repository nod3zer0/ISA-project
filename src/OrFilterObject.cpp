/**
 * @file OrFilterObject.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
#include "inc/OrFilterObject.h"

filterTypes OrFilter::getFilterType() { return OR; };

OrFilter::~OrFilter() {
    for (std::vector<FilterObject *>::iterator it = filters.begin();
         it != filters.end(); ++it) {
        delete *it;
    }
}
