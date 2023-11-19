/**
 * @file AndFilterObject.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */

#include "inc/AndFilterObject.h"

filterTypes AndFilter::getFilterType() { return AND; };
AndFilter::~AndFilter() {
    for (std::vector<FilterObject *>::iterator it = filters.begin();
         it != filters.end(); ++it) {
        delete *it;
    }
}
