#ifndef DATABASE_OBJECT_H
#define DATABASE_OBJECT_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class database_object {
private:
  std::vector<unsigned char> name;
  std::vector<unsigned char> uid;
  std::vector<unsigned char> email;

public:
  std::vector<unsigned char> get_name();
  std::vector<unsigned char> get_uid();
  std::vector<unsigned char> get_email();
  database_object(std::vector<unsigned char> name,
                  std::vector<unsigned char> uid,
                  std::vector<unsigned char> email);
};

class databaseController {
private:
  std::ifstream file;
  std::vector<unsigned char> sanitaze(std::vector<unsigned char> input);

public:
  database_object loadNextRow(int* err);
  std::vector<database_object> loadAllRows();

  databaseController(std::string fileName);
  ~databaseController();
};


std::vector<database_object> removeDuplicates(std::vector<database_object> input);

#endif