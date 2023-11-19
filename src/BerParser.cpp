/**
 * @file BerParser.cpp
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @date 2023-11-19
 */
#include "inc/BerParser.h"

/**
 * @brief Parses BER encoded message and converts it to BerObject, it works recursively
 *
 * @param start  start of the BER
 * @param err  1 if error, 0 if success
 * @param end  end of the array
 * @return BerObject*
 */
BerObject *ParseBerObject(std::vector<unsigned char>::iterator start,
                          int *err, std::vector<unsigned char>::iterator end) {
  BerObject *berObject;

  unsigned char tag = start[0];
    // main switch for all types
  switch (tag) {
    {
    case BER_SEQUENCE_C:
      BerSequenceObject *berSequenceObject = new BerSequenceObject();
      int length = GetLength(start + 1, err, end);
      GoIntoTag(start, err, end);
      GetLengthOfLength(start +1, err, end);
      int i = 0;
      while (i < length) {
        BerObject *parsedBerObject = ParseBerObject(start, err, end);
        berSequenceObject->objects.push_back(parsedBerObject);
          i += BER_TAG_LENGTH + GetLengthOfLength(start +1, err, end) +
             GetLength(start + 1, err, end);
        SkipTags(start, 1, err, end);
      }
      berObject = berSequenceObject;
      break;
    }
    {
    case BER_BIND_REQUEST_C:
      BerSequenceObject *berSequenceObject =
          new BerSequenceObject(BER_BIND_REQUEST_C);
      int length = GetLength(start + 1, err, end);
      GoIntoTag(start, err, end);

      GetLengthOfLength(start +1, err, end);
      int i = 0;
      while (i < length) {
        BerObject *parsedBerObject = ParseBerObject(start, err, end);
        berSequenceObject->objects.push_back(parsedBerObject);
          i += BER_TAG_LENGTH + GetLengthOfLength(start +1, err, end) +
             GetLength(start + 1, err, end);
        SkipTags(start, 1, err, end);
      }
      berObject = berSequenceObject;
      break;
    }
    {
    case BER_SEARCH_REQUEST_C:
      BerSequenceObject *berSequenceObject =
          new BerSequenceObject(BER_SEARCH_REQUEST_C);
      int length = GetLength(start + 1, err, end);
      GoIntoTag(start, err, end);

      GetLengthOfLength(start +1, err, end);
      int i = 0;
      while (i < length) {
        BerObject *parsedBerObject = ParseBerObject(start, err, end);
        berSequenceObject->objects.push_back(parsedBerObject);
        i += BER_TAG_LENGTH + GetLengthOfLength(start +1, err, end) +
             GetLength(start + 1, err, end);
        SkipTags(start, 1, err, end);
      }
      berObject = berSequenceObject;
      break;
    }
  case BER_INT_C: {
    BerIntObject *berIntObject = new BerIntObject();
    int value = ParseINT(start, err, end);
    berIntObject->setValue(value);
    berObject = berIntObject;
    break;
  }

  case BER_ENUM_C: {
    int lenghtOfLenght = GetLengthOfLength(start +1 , err, end);

    char value = start[lenghtOfLenght + BER_TAG_LENGTH];

    berObject = new BerEnumObject(value);
    break;
  }
  case BER_SET_C: {
    BerSetObject *berSetObject = new BerSetObject();
    int length = GetLength(start + 1, err, end);
    GoIntoTag(start, err,end);

    int i = 0;
    while (i < length) {

      BerObject *parsedBerObject = ParseBerObject(start, err, end);
      berSetObject->objects.push_back(parsedBerObject);
      i += BER_TAG_LENGTH + GetLengthOfLength(start +1, err, end) +
             GetLength(start + 1, err, end);
      SkipTags(start, 1, err, end);
    }
    berObject = berSetObject;
    break;
  }
  case BER_OCTET_STRING_C: {

    int lenghtOfLenght = GetLengthOfLength(start +1, err, end);
    int length = GetLength(start + 1, err, end);
    BerStringObject *berStringObject = new BerStringObject();
    if (length == 0) {
      berStringObject->value = std::vector<unsigned char>();
      berObject = berStringObject;
      break;
    }
    berStringObject->value = std::vector<unsigned char>(
        start + lenghtOfLenght + BER_TAG_LENGTH,
        start + lenghtOfLenght + BER_TAG_LENGTH + length);
    berObject = berStringObject;
    break;
  }
  case BER_BOOL_C: {
    int lenghtOfLenght = GetLengthOfLength(start +1, err, end);
    GetLength(start + 1, err, end);
    berObject = new BerBoolObject(start[lenghtOfLenght + BER_TAG_LENGTH]);
    break;
  }
  default: // filters and other types
    std::vector<unsigned char> berVector = std::vector<unsigned char>(
        start, start + GetLengthOfLength(start+1, err, end) + BER_TAG_LENGTH +
                   GetLength(start + 1, err, end));
    berObject = new BerUndefinedObject(berVector);

    break;
  }
  return berObject;
}