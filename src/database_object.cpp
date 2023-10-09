#include "inc/database_object.h"

std::vector<unsigned char> database_object::get_name() { return this->name; }
std::vector<unsigned char> database_object::get_uid() { return this->uid; }
std::vector<unsigned char> database_object::get_email() { return this->email; }
database_object::database_object(std::vector<unsigned char> name,
                                 std::vector<unsigned char> uid,
                                 std::vector<unsigned char> email) {
  this->name = name;
  this->uid = uid;
  this->email = email;
}

std::vector<database_object> databaseController::loadAllRows() {
  std::vector<database_object> result;
  while (!file.eof()) {
    result.push_back(loadNextRow());
  }
  return result;
}


std::vector<unsigned char> databaseController::sanitaze(std::vector<unsigned char> input) {
  std::vector<unsigned char> result;
  //allow only printable ascii charactes and numbers
    for (unsigned long int i = 0; i < input.size(); i++) {
        if (input[i] >= 32 && input[i] <= 126) {
        result.push_back(input[i]);
        }
    }
  return result;
}

database_object databaseController::loadNextRow() {
  std::vector<unsigned char> name;
  std::vector<unsigned char> uid;
  std::vector<unsigned char> email;

  // read csv line
  std::string line;
  std::getline(file, line);
  std::stringstream ss(line);
  std::string token;
  std::getline(ss, token, ';');

  name = databaseController::sanitaze(std::vector<unsigned char>(token.begin(), token.end()));
  std::getline(ss, token, ';');
  uid = databaseController::sanitaze(std::vector<unsigned char>(token.begin(), token.end()));
  std::getline(ss, token, ';');
  email = databaseController::sanitaze(std::vector<unsigned char>(token.begin(), token.end()));

  return database_object(name, uid, email);
}

std::vector<database_object> removeDuplicates(std::vector<database_object> input) {
  std::vector<database_object> result;
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

databaseController::databaseController(std::string fileName) {
  file.open(fileName);
}

databaseController::~databaseController() { file.close(); }
