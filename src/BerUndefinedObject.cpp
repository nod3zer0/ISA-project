#include "inc/BerUndefinedObject.h"


berObjectTypes BerUndefinedObject::getBerObjectType(){
    return berUndefined;
}
long long int  BerUndefinedObject::getLenght(){
    int err = 0;
    return BER_TAG_LENGTH + getLengthLength(value.begin() +1, &err)  + ParseLength(value.begin() +1, &err);
}
std::vector<unsigned char> BerUndefinedObject::getBerRepresentation(){
    return value;
}

BerUndefinedObject::BerUndefinedObject(std::vector<unsigned char> value){
    this->value = value;
}