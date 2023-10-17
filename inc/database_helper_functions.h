#ifndef DATABASE_HELPER_FUNCTIONS_H
#define DATABASE_HELPER_FUNCTIONS_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "inc/DatabaseObject.h"

std::vector<DatabaseObject> removeDuplicates(std::vector<DatabaseObject> input);

#endif