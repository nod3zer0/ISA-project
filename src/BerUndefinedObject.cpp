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