#ifndef BER_HELPER_FUNCTIONS_H
#define BER_HELPER_FUNCTIONS_H
#include <vector>
#include "inc/ber_constants.h"
/**
 * @brief parses 1 integer from ldap coded message
 *
 * @param s start of the integer in char array
 * @return int - parsed integer
 */
int ParseINT(unsigned char *s, int *err);

/**
 * @brief writes int in BER LDAP format to char array
 *
 * @param s start of the string in char array
 * @param value int to be written
 * @return -1 if error, 0 if success
 */

typedef enum filterTypes {
  AND,
  OR,
  NOT,
  equalityMatch,
  substrings,
  undefined,
} filterTypes;

typedef enum berObjectTypes {
  berSequenceObject,
  berIntObject,
  berStringObject,
  berSetObject,
  berEnumObject,
  berUndefined,
} berObjectTypes;

int HowManyBytesWillIntUse(int value);

int WriteIntAppend(std::vector<unsigned char> &s, int value);

void WriteLenght4Bytes(std::vector<unsigned char>::iterator &start, int value, int *err) ;


void AppendLenght4Bytes(std::vector<unsigned char> &start, int value, int *err) ;

int ParseLength(std::vector<unsigned char>::iterator start, int *err);

unsigned int ParseINT(std::vector<unsigned char>::iterator s, int *err);

int getLengthLength(std::vector<unsigned char>::iterator start, int *err);

void skipTags(std::vector<unsigned char>::iterator &start, int n, int *err);

void goIntoTag(std::vector<unsigned char>::iterator &start, int *err);

void IncreaseLength4Bytes(std::vector<unsigned char>::iterator &start, int n,
                          int *err);

filterTypes getFilterType(std::vector<unsigned char>::iterator start);

#endif