/**
 * @file ber_helper_functions.h
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @brief Helper functions for parsing BER LDAP
 * @date 2023-11-19
 */
#ifndef BER_HELPER_FUNCTIONS_H
#define BER_HELPER_FUNCTIONS_H
#include <vector>
#include "inc/ber_constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

/**
 * @brief converts std::vector<unsigned char> to lowercase
 * @param input std::vector<unsigned char> to be converted
 * @return converted std::vector<unsigned char>
 */
std::vector<unsigned char> ToLowerCase(std::vector<unsigned char> input);

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
  berBoolObject,
  berUndefined,
  berErr,
} berObjectTypes;

/**
 * @brief returns the number of bytes that will be used to encode the int
 *
 * @param value
 * @return int
 */
int HowManyBytesWillIntUse(int value);

/**
 * @brief writes int in BER LDAP format to char array
 *
 * @param s start of the string in char array
 * @param value int to be written
 * @return -1 if error, 0 if success
 */
int WriteIntAppend(std::vector<unsigned char> &s, int value);

/**
 * @brief appends length in BER LDAP format to char array (4 bytes)
 *
 * @param start vector to append to
 * @param value
 * @param err
 */
void AppendLenght4Bytes(std::vector<unsigned char> &start, int value);

/**
 * @brief Get the length of the data of BER attribute
 *
 * @param start start of the length
 * @param err 1 if error, 0 if success
 * @param end end of the array
 * @return
 */
int GetLength(std::vector<unsigned char>::iterator start, int *err,std::vector<unsigned char>::iterator end);

/**
 * @brief Parses value of BERInteger from char array
 *
 * @param s start of the integer in char array
 * @param err 1 if error, 0 if success
 * @param end end of the array
 * @return unsigned int
 */
unsigned int ParseINT(std::vector<unsigned char>::iterator s, int *err,std::vector<unsigned char>::iterator end);

/**
 * @brief Get the Length Of Length of BER attribute
 *
 * @param start  start of the length
 * @param err  1 if error, 0 if success
 * @param end  end of the array
 * @return int
 */
int GetLengthOfLength(std::vector<unsigned char>::iterator start, int *err,std::vector<unsigned char>::iterator end);

/**
 * @brief skips n BER attributes from char array, returns incremented iterator
 *
 * @param start  start of the tag
 * @param n  number of tags to skip
 * @param err  1 if error, 0 if success
 * @param end  end of the array
 */
void SkipTags(std::vector<unsigned char>::iterator &start, int n, int *err, std::vector<unsigned char>::iterator end);

/**
 * @brief goes into the sequence/set tag and returns incremented iterator which points to the first attribute in the sequence/set
 *
 * @param start start of the tag
 * @param err 1 if error, 0 if success
 * @param end end of the array
 */
void GoIntoTag(std::vector<unsigned char>::iterator &start, int *err, std::vector<unsigned char>::iterator end);

/**
 * @brief Increases 4Bytes longform length of the attribute by n
 *
 * @param start  start of the length
 * @param n number by which the length will be increased
 * @param err 1 if error, 0 if success
 * @param end end of the array
 */
void IncreaseLength4Bytes(std::vector<unsigned char>::iterator &start, int n,
                          int *err,std::vector<unsigned char>::iterator end);

/**
 * @brief Parses type of filter from char array and returns its enum
 *
 * @param start start of the filter
 * @return filterTypes
 */
filterTypes getFilterType(std::vector<unsigned char>::iterator start);

#endif