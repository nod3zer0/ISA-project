#include "inc/ber_helper_functions.h"
#include <stdio.h>

/**
 * @brief parses 1 integer from ldap coded message
 *
 * @param s start of the integer in char array
 * @return int - parsed integer
 */
int ParseINT(unsigned char *s, int *err) {
  int value = 0;
  if (s[1] > 4) {
    return 0;
    *err = 2;
  }

  switch (s[1]) {
  case 0x00:
    value = 0;
    *err = 1;
    break;
  case 0x01:
    value = s[2];
    err = 0;
    break;
  case 0x02:
    value = s[2] << 8 | s[3];
    err = 0;
    break;
  case 0x03:
    value = s[2] << 16 | s[3] << 8 | s[4];
    err = 0;
    break;
  case 0x04:
    value = s[2] << 24 | s[3] << 16 | s[4] << 8 | s[5];
    err = 0;
    break;

  default:
    break;
  }
  return value;
}

/**
 * @brief writes int in BER LDAP format to char array
 *
 * @param s start of the string in char array
 * @param value int to be written
 * @return -1 if error, 0 if success
 */
int writeInt(unsigned char *s, int value) {
  if (value < 0) {
    return -1;
  }
  if (value < 0x100) {
    s[0] = 0x02;
    s[1] = 0x01;
    s[2] = value;
  } else if (value < 0x10000) {
    s[0] = 0x02;
    s[1] = 0x02;
    s[2] = value >> 8;
    s[3] = value;
  } else if (value < 0x1000000) {
    s[0] = 0x02;
    s[1] = 0x03;
    s[2] = value >> 16;
    s[3] = value >> 8;
    s[4] = value;
  } else if (value < 0x100000000) {
    s[0] = 0x02;
    s[1] = 0x04;
    s[2] = value >> 24;
    s[3] = value >> 16;
    s[4] = value >> 8;
    s[5] = value;
  } else {
    return -1;
  }
  return 0;
}

/**
 * @brief gets number of bytes that takes to necode length of data in BER
 * @param start start of the length in char array
 * @param err error code
 * @return number of bytes that takes to necode length of data in BER
 */
int getLengthLength(unsigned char *start, int *err) {

  int length = 0;
  if ((start[0] >> 7) != 1) { // if first bit is 0 -> shortform
    length = 1;
    *err = 0;
  } else {
    length = start[0] & 0x7F;
    length += 1;
  }
  return length;
}

/**
 * @brief parses length of BER encoded data (supports longform and shortform),
 * supports only 4 bytes
 * @param start start of the length in char array
 * @param err error code, err 1 if length is longer than 4 bytes
 * @return length of the data (without tag and lenght bytes)
 */
int ParseLength(unsigned char *start, int *err) { // TODO doesnt work
  int length = 0;
  if ((start[0] >> 7) != 1) { // if first bit is 0 -> shortform
    length = start[0];
    *err = 0;
  } else {
    int lengthLength = start[0] & 0x7F; // remove bit indicating longform
    if (lengthLength > 4 &&
        start[lengthLength - 4] > 0x00) { // only support up to 4 bytes, more is
                                          // not necessary for this project
      *err = 1;
      return 0;
    }
    int startOfLength = lengthLength - 3;
    if (startOfLength < 0) {
      startOfLength = 1;
    }
    for (int i = startOfLength; i <= lengthLength; i++) {
      length = length << 8 | start[i];
    }
    *err = 0;
  }
  return length;
}

/**
 * @brief skips n number of tags to next tag
 * @param start  char array starting with tag from which to skip tags
 * @param n number of tags to skip
 * @param err error code
 * @return pointer to next tag
 */
unsigned char *skipTags(unsigned char *start, int n, int *err) { // TODO: test

  int i = 0;
  int jumpLength = 1;
  while (i < n) {
    int l = ParseLength(start + jumpLength, err);
    int ll = getLengthLength(start + jumpLength, err);
    int length = ParseLength(start + jumpLength, err) +
                 getLengthLength(start + jumpLength, err);
    if (*err != 0) {
      *err = 1;
      return start;
    }
    jumpLength += length + 1; // +1 for tag
    i++;
  }
  return start + jumpLength - 1; // -1 to get index of tag instead of length
}

/**
 * @brief go into tag
 * @param start  char array starting with tag from which to skip tags
 * @param err error code
 * @return pointer to next tag
 */
unsigned char *goIntoTag(unsigned char *start, int *err) { // TODO: test
  int length = getLengthLength(start + 1, err) + 1;        // +1 for tag
  if (*err != 0) {
    *err = 1;
    return start;
  }
  return start + length; // return pointer to next tag
}