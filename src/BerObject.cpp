#include "inc/BerObject.h"


berObjectTypes BerObject::getBerObjectType(){
    return berErr;
}
long long int  BerObject::getLenght(){
    return -1;
}
std::vector<unsigned char> BerObject::getBerRepresentation(){
    std::vector<unsigned char> result;
    return result;
}
