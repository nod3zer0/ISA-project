#ifndef DATABASECONTROLLER_H
#define DATABASECONTROLLER_H
#include "inc/DatabaseObject.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

/**
 * @brief class for loading and parsing database file
 *
 */
class DatabaseController {
private:
  std::ifstream file;
  std::vector<unsigned char> sanitaze(std::vector<unsigned char> input);

public:
  /**
   * @brief loads next row from database file
   *
   * @param err  1 if EOF, 0 if success
   * @return DatabaseObject
   */
  DatabaseObject loadNextRow(int *err);

  /**
   * @brief loads all rows from database file
   *
   * @return std::vector<DatabaseObject>
   */
  std::vector<DatabaseObject> loadAllRows();

  /**
   * @brief Construct a new Database Controller object
   *
   * @param fileName path to database csv file
   */
  DatabaseController(std::string fileName);
  ~DatabaseController();
};

#endif