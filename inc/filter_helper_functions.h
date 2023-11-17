
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

/**
 * @brief evaluates if filter is true for given database entry
 *
 * @param sf substring filter object
 * @param err 1 if error, 0 if success
 * @param attribute attribute to be filtered
 * @return true
 * @return false
 */
bool substrFilterHandler(SubstringFilter *sf, int *err,
                         std::vector<unsigned char> attribute);

/**
 * @brief evalues if filter is true for given database entry
 *
 * @param emf  equality match filter object
 * @param err  1 if error, 0 if success
 * @param attribute  attribute to be filtered
 * @return true
 * @return false
 */
bool equalityMatchHandler(EqualityMatchFilter *emf, int *err,
                          std::vector<unsigned char> attribute) ;

/**
 * @brief evaluates if filter is true for given database entry
 *
 * @param f  filter object
 * @param err  1 if error, 0 if success
 * @param databaseEntry  database entry to be filtered
 * @return true
 * @return false
 */
bool filterLine(FilterObject *f, int *err, DatabaseObject &databaseEntry) ;

/**
 * @brief evaluates if filter is true for given database entries
 *
 * @param f filter object
 * @param err 1 if error, 0 if success
 * @param dbLocation path to database file
 * @param sizeLimit maximum number of entries to be returned
 * @return std::vector<DatabaseObject>
 */
std::vector<DatabaseObject>
filterHandler(FilterObject *f, int *err, const char *dbLocation, int sizeLimit);

/**
 * @brief converts BER representation of filters to filter object
 *
 * @param BERfilter start of the BER filter
 * @param end end of the BER filter
 * @return FilterObject*
 */
FilterObject *convertToFilterObject(std::vector<unsigned char>::iterator BERfilter, std::vector<unsigned char>::iterator end);

#endif