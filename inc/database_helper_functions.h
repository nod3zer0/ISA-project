#ifndef DATABASE_HELPER_FUNCTIONS_H
#define DATABASE_HELPER_FUNCTIONS_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "inc/DatabaseObject.h"

/**
 * @brief Removes dupplicates from vector of DatabaseObjects
 *
 * @param input
 * @return std::vector<DatabaseObject>
 */
std::vector<DatabaseObject> removeDuplicates(std::vector<DatabaseObject> input);

#endif