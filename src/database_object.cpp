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

  name = std::vector<unsigned char>(token.begin(), token.end());
  std::getline(ss, token, ';');
  uid = std::vector<unsigned char>(token.begin(), token.end());
  std::getline(ss, token, ';');
  email = std::vector<unsigned char>(token.begin(), token.end());



  return database_object(name, uid, email);
}
databaseController::databaseController(std::string fileName) {
  file.open(fileName);
}

databaseController::~databaseController() { file.close(); }
