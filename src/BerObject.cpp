/**
 * @file BerObject.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
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

BerObject::~BerObject(){

}