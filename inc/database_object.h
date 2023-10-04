#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class database_object
{
  private:
    std::vector<unsigned char> name;
    std::vector<unsigned char> uid;
    std::vector<unsigned char> email;
  public:
    std::vector<unsigned char> get_name();
    std::vector<unsigned char> get_uid();
    std::vector<unsigned char> get_email();
    database_object(std::vector<unsigned char> name, std::vector<unsigned char> uid, std::vector<unsigned char> email);
};

class databaseController{
    private:
     std::ifstream  file;
    public:
    database_object loadNextRow();
    databaseController(std::string fileName);
    ~databaseController();
};