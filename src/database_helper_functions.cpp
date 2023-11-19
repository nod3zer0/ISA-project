/**
 * @file database_helper_functions.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
#include "inc/database_helper_functions.h"

std::vector<DatabaseObject>
removeDuplicates(std::vector<DatabaseObject> input) {
  std::vector<DatabaseObject> result;
  for (unsigned long int i = 0; i < input.size(); i++) {
    bool found = false;
    for (unsigned long int j = 0; j < result.size(); j++) {
      if (input[i].get_uid() == result[j].get_uid()) {
        found = true;
        break;
      }
    }
    if (!found) {
      result.push_back(input[i]);
    }
  }
  return result;
}