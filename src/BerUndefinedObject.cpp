/**
 * @file BerUndefinedObject.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
#include "inc/BerUndefinedObject.h"


berObjectTypes BerUndefinedObject::getBerObjectType(){
    return berUndefined;
}
long long int  BerUndefinedObject::getLenght(){
    int err = 0;
    return BER_TAG_LENGTH + GetLengthOfLength(value.begin() +1, &err, value.end())  + GetLength(value.begin() +1, &err, value.end());
}
std::vector<unsigned char> BerUndefinedObject::getBerRepresentation(){
    return value;
}

BerUndefinedObject::BerUndefinedObject(std::vector<unsigned char> value){
    this->value = value;
}