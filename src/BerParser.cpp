#include "inc/BerParser.h"

BerObject *ParseBerObject(std::vector<unsigned char>::iterator start,
                          int *err) {
  BerObject *berObject;

  unsigned char tag = start[0];

  switch (tag) {
    {
    case BER_SEQUENCE_C:
      BerSequenceObject *berSequenceObject = new BerSequenceObject();
      int length = ParseLength(start + 1, err);
      goIntoTag(start, err);
      getLengthLength(start +1, err);
      int i = 0;
      while (i < length) {
        BerObject *parsedBerObject = ParseBerObject(start, err);
        berSequenceObject->objects.push_back(parsedBerObject);
          i += BER_TAG_LENGTH + getLengthLength(start +1, err) +
             ParseLength(start + 1, err);
        skipTags(start, 1, err);
      }
      berObject = berSequenceObject;
      break;
    }
    {
    case BER_BIND_REQUEST_C:
      BerSequenceObject *berSequenceObject =
          new BerSequenceObject(BER_BIND_REQUEST_C);
      int length = ParseLength(start + 1, err);
      goIntoTag(start, err);

      getLengthLength(start +1, err);
      int i = 0;
      while (i < length) {
        BerObject *parsedBerObject = ParseBerObject(start, err);
        berSequenceObject->objects.push_back(parsedBerObject);
          i += BER_TAG_LENGTH + getLengthLength(start +1, err) +
             ParseLength(start + 1, err);
        skipTags(start, 1, err);
      }
      berObject = berSequenceObject;
      break;
    }
    {
    case BER_SEARCH_REQUEST_C:
      BerSequenceObject *berSequenceObject =
          new BerSequenceObject(BER_SEARCH_REQUEST_C);
      int length = ParseLength(start + 1, err);
      goIntoTag(start, err);

      getLengthLength(start +1, err);
      int i = 0;
      while (i < length) {
        BerObject *parsedBerObject = ParseBerObject(start, err);
        berSequenceObject->objects.push_back(parsedBerObject);
        i += BER_TAG_LENGTH + getLengthLength(start +1, err) +
             ParseLength(start + 1, err);
        skipTags(start, 1, err);
      }
      berObject = berSequenceObject;
      break;
    }
  case BER_INT_C: {
    BerIntObject *berIntObject = new BerIntObject();
    int value = ParseINT(start, err);
    berIntObject->setValue(value);
    berObject = berIntObject;
    break;
  }

  case BER_ENUM_C: {
    int lenghtOfLenght = getLengthLength(start +1 , err);

    char value = start[lenghtOfLenght + BER_TAG_LENGTH];

    berObject = new BerEnumObject(value);
    break;
  }
  case BER_SET_C: {
    BerSetObject *berSetObject = new BerSetObject();
    int length = ParseLength(start + 1, err);
    goIntoTag(start, err);

    int i = 0;
    while (i < length) {

      BerObject *parsedBerObject = ParseBerObject(start, err);
      berSetObject->objects.push_back(parsedBerObject);
      i += BER_TAG_LENGTH + getLengthLength(start +1, err) +
             ParseLength(start + 1, err);
      skipTags(start, 1, err);
    }
    berObject = berSetObject;
    break;
  }
  case BER_OCTET_STRING_C: {

    int lenghtOfLenght = getLengthLength(start +1, err);
    int length = ParseLength(start + 1, err);
    BerStringObject *berStringObject = new BerStringObject();
    berStringObject->value = std::vector<unsigned char>(
        start + lenghtOfLenght + BER_TAG_LENGTH,
        start + lenghtOfLenght + BER_TAG_LENGTH + length);
    berObject = berStringObject;
    break;
  }
  case BER_BOOL_C: {
    int lenghtOfLenght = getLengthLength(start +1, err);
    ParseLength(start + 1, err);
    berObject = new BerBoolObject(start[lenghtOfLenght + BER_TAG_LENGTH]);
    break;
  }
  default:
    std::vector<unsigned char> berVector = std::vector<unsigned char>(
        start, start + getLengthLength(start+1, err) + BER_TAG_LENGTH +
                   ParseLength(start + 1, err));
    berObject = new BerUndefinedObject(berVector);

    printf("unknown tag: %x\n", tag);
    break;
  }
  return berObject;
}