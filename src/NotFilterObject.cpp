/**
 * @file NotFilterObject.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
#include "inc/NotFilterObject.h"

filterTypes NotFilter::getFilterType() { return NOT; };

NotFilter::~NotFilter() { delete filter; }