#include "inc/DatabaseObject.h"

std::vector<unsigned char> DatabaseObject::get_name() { return this->name; }
std::vector<unsigned char> DatabaseObject::get_uid() { return this->uid; }
std::vector<unsigned char> DatabaseObject::get_email() { return this->email; }
DatabaseObject::DatabaseObject(std::vector<unsigned char> name,
                                 std::vector<unsigned char> uid,
                                 std::vector<unsigned char> email) {
  this->name = name;
  this->uid = uid;
  this->email = email;
}






