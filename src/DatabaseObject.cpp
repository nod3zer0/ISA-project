/**
 * @file DatabaseObject.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
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






