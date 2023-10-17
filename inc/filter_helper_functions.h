
#ifndef INC_FILTER_HELPER_FUNCTIONS_H
#define INC_FILTER_HELPER_FUNCTIONS_H


#include "inc/FilterObject.h"
#include "inc/NotFilterObject.h"
#include "inc/AndFilterObject.h"
#include "inc/OrFilterObject.h"
#include "inc/EqualityMatchFilterObject.h"
#include "inc/SubstringFilterObject.h"
#include "inc/DatabaseObject.h"
#include "inc/DatabaseController.h"
#include "vector"

bool substrFilterHandler(SubstringFilter *sf, int *err,
                         std::vector<unsigned char> attribute);

bool equalityMatchHandler(equalityMatchFilter *emf, int *err,
                          std::vector<unsigned char> attribute) ;

bool filterLine(filter *f, int *err, DatabaseObject &databaseEntry) ;

std::vector<DatabaseObject>
filterHandler(filter *f, int *err, const char *dbLocation, int sizeLimit);

filter *convertToFilterObject(std::vector<unsigned char>::iterator BERfilter);

#endif