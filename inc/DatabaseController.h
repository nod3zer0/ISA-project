#ifndef DATABASECONTROLLER_H
#define DATABASECONTROLLER_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "inc/DatabaseObject.h"

class DatabaseController {
private:
  std::ifstream file;
  std::vector<unsigned char> sanitaze(std::vector<unsigned char> input);

public:
  DatabaseObject loadNextRow(int* err);
  std::vector<DatabaseObject> loadAllRows();

  DatabaseController(std::string fileName);
  ~DatabaseController();
};

#endif