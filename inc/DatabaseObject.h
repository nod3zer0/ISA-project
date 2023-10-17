#ifndef DATABASE_OBJECT_H
#define DATABASE_OBJECT_H
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class DatabaseObject {
private:
  std::vector<unsigned char> name;
  std::vector<unsigned char> uid;
  std::vector<unsigned char> email;

public:
  std::vector<unsigned char> get_name();
  std::vector<unsigned char> get_uid();
  std::vector<unsigned char> get_email();
  DatabaseObject(std::vector<unsigned char> name,
                  std::vector<unsigned char> uid,
                  std::vector<unsigned char> email);
};


#endif