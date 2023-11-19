/**
 * @file DatabaseController.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
#include "inc/DatabaseController.h"

std::vector<DatabaseObject> DatabaseController::loadAllRows() {
  std::vector<DatabaseObject> result;
  while (!file.eof()) {
    int err;
    result.push_back(loadNextRow(&err));
  }
  return result;
}

std::vector<unsigned char>
DatabaseController::sanitaze(std::vector<unsigned char> input) {
  std::vector<unsigned char> result;
  // allow only printable ascii charactes and numbers
  for (unsigned long int i = 0; i < input.size(); i++) {
    if (input[i] >= 32 && input[i] <= 126) {
      result.push_back(input[i]);
    }
  }
  return result;
}

DatabaseObject DatabaseController::loadNextRow(int *err) {

  if (file.eof()) {
    *err = 1;
    return DatabaseObject(std::vector<unsigned char>(),
                          std::vector<unsigned char>(),
                          std::vector<unsigned char>());
  }

  std::vector<unsigned char> name;
  std::vector<unsigned char> uid;
  std::vector<unsigned char> email;

  // read csv line
  std::string line;
  std::getline(file, line);
  if (line == "") {
    *err = 1;
    return DatabaseObject(std::vector<unsigned char>(),
                          std::vector<unsigned char>(),
                          std::vector<unsigned char>());
  }
  std::stringstream ss(line);
  std::string token;
  std::getline(ss, token, ';');

  name = DatabaseController::sanitaze(
      std::vector<unsigned char>(token.begin(), token.end()));
  std::getline(ss, token, ';');
  uid = DatabaseController::sanitaze(
      std::vector<unsigned char>(token.begin(), token.end()));
  std::getline(ss, token, ';');
  email = DatabaseController::sanitaze(
      std::vector<unsigned char>(token.begin(), token.end()));
  *err = 0;
  return DatabaseObject(name, uid, email);
}

DatabaseController::DatabaseController(std::string fileName) {
  file.open(fileName);
}

DatabaseController::~DatabaseController() { file.close(); }