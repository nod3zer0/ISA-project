#include "inc/NotFilterObject.h"

filterTypes NotFilter::getFilterType() { return NOT; };

NotFilter::~NotFilter() { delete filter; }