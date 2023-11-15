#include "inc/ber_helper_functions.h"

filterTypes getFilterType(std::vector<unsigned char>::iterator start) {
  if (start[0] == 0xA0) {
    return AND;
  } else if (start[0] == 0xA1) {
    return OR;
  } else if (start[0] == 0xA2) {
    return NOT;
  } else if (start[0] == 0xA3) {
    return equalityMatch;
  } else if (start[0] == 0xA4) {
    return substrings;
  }
  return undefined;
}

// int GetNumberOfBytesNeddedForINT(int value) {
//   if (value < 0) {
//     return -1;
//   }
//   if (value < 0x100) {
//     return 1 + 2;
//   } else if (value < 0x10000) {
//     return 2 + 2;
//   } else if (value < 0x1000000) {
//     return 3 + 2;
//   } else if (value < 0x100000000) {
//     return 4 + 2;
//   }
//   return -1;
// }

// /**
//  * @brief parses 1 integer from ldap coded message
//  *
//  * @param s start of the integer in char array
//  * @return int - parsed integer
//  */
unsigned int ParseINT(std::vector<unsigned char>::iterator s, int *err, std::vector<unsigned char>::iterator end) {
  unsigned int value = 0;
  int length = GetLength(s + 1, err, end);
  int lengthLength = GetLengthOfLength(s + 1, err, end);
  if (length > 4) {
    return 0;
    *err = 2;
  }

  switch (length) {
  case 0:
    value = 0;
    *err = 1;
    break;
  case 1:
    value = s[1 + lengthLength];
    err = 0;
    break;
  case 2:
    value = s[1 + lengthLength] << 8 | s[2 + lengthLength];
    err = 0;
    break;
  case 3:
    value = s[1 + lengthLength] << 16 | s[2 + lengthLength] << 8 |
            s[3 + lengthLength];
    err = 0;
    break;
  case 4:
    value = s[1 + lengthLength] << 24 | s[2 + lengthLength] << 16 |
            s[3 + lengthLength] << 8 | s[4 + lengthLength];
    err = 0;
    break;

  default:
    break;
  }
  return value;
}

// /**
//  * @brief writes int in BER LDAP format to char array
//  *
//  * @param s start of the string in char array
//  * @param value int to be written
//  * @param length number of bytes written (including tag and length bytes)
//  * @return -1 if error, 0 if success
//  */
// int writeInt(unsigned char *s, int value, int *length) {
//   if (value < 0) {
//     return -1;
//   }
//   if (value < 0x100) {
//     s[0] = 0x02;
//     s[1] = 0x01;
//     s[2] = value;
//     (*length) = 1 + 2;
//   } else if (value < 0x10000) {
//     s[0] = 0x02;
//     s[1] = 0x02;
//     s[2] = value >> 8;
//     s[3] = value;
//     (*length) = 2 + 2;
//   } else if (value < 0x1000000) {
//     s[0] = 0x02;
//     s[1] = 0x03;
//     s[2] = value >> 16;
//     s[3] = value >> 8;
//     s[4] = value;
//     (*length) = 3 + 2;
//   } else if (value < 0x100000000) {
//     s[0] = 0x02;
//     s[1] = 0x04;
//     s[2] = value >> 24;
//     s[3] = value >> 16;
//     s[4] = value >> 8;
//     s[5] = value;
//     (*length) = 4 + 2;
//   } else {
//     return -1;
//   }
//   return 0;
// }

/**
 * @brief gets number of bytes that takes to necode length of data in BER
 * @param start start of the length in char array
 * @param err error code
 * @return number of bytes that takes to necode length of data in BER
 */
int GetLengthOfLength(std::vector<unsigned char>::iterator start, int *err, std::vector<unsigned char>::iterator end) {
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
int GetLength(std::vector<unsigned char>::iterator start,
                int *err, std::vector<unsigned char>::iterator end) { // TODO doesnt work
  int length = 0;
  if ((start[0] >> 7) != 1) { // if first bit is 0 -> shortform
    length = start[0];
    *err = 0;
  } else {
    int lengthOfLength = start[0] & 0x7F; // remove bit indicating longform

    if(lengthOfLength > std::distance(start, end)){ //array is too short
      *err = 1;
      return 0;
    }
    if (lengthOfLength > 4 &&
        start[lengthOfLength - 4] > 0x00) { // only support up to 4 bytes, more is
                                          // not necessary for this project
      *err = 1;
      return 0;
    }
    int startOfLength = lengthOfLength - 3;
    if (startOfLength < 0) {
      startOfLength = 1;
    }
    for (int i = startOfLength; i <= lengthOfLength; i++) {
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
void SkipTags(std::vector<unsigned char>::iterator &start, int n, int *err,
              std::vector<unsigned char>::iterator end) { // TODO: test

  int i = 0;
  int jumpLength = 1;
  while (i < n) {
    if(std::distance(start, end) < jumpLength){
      *err = 1;
      return;
    }
    int length =
        GetLength(start + jumpLength, err, end) + // TODO: fix to use vector
        GetLengthOfLength(start + jumpLength, err, end);
    if (*err != 0) {
      *err = 1;
      return;
    }

    jumpLength += length + 1; // +1 for tag
    i++;
  }
  start = start + jumpLength - 1; // -1 to get index of tag instead of length
}

// /** TODO: update to use vector
//  * @brief Increase lenght of BER encoded data by n
//  * @param start start of the length in char array
//  * @param array whole envelope
//  * @param n number of bytes to increase
//  */
void IncreaseLength4Bytes(std::vector<unsigned char>::iterator &start, int n,
                          int *err, std::vector<unsigned char>::iterator end) {

  int length = GetLength(start, err, end) + n;

  start[1] = length >> 24;
  start[2] = length >> 16;
  start[3] = length >> 8;
  start[4] = length;
}

void AppendLenght4Bytes(std::vector<unsigned char> &start, int value) {
  start.push_back(0x84); // size
  start.push_back(value >> 24);
  start.push_back(value >> 16);
  start.push_back(value >> 8);
  start.push_back(value);
}

/**
 * @brief writes int in BER LDAP format to char array
 *
 * @param s start of the string in char array
 * @param value int to be written
 * @return -1 if error, 0 if success
 */
int WriteIntAppend(std::vector<unsigned char> &s, int value) {
  if (value < 0) {
    return -1;
  }
  s.push_back(BER_INT_C);
  if (value < 0x100) {
    s.push_back(0x01); // length
    s.push_back(value);
    return 3;
  } else if (value < 0x10000) {
    s.push_back(0x02); // length
    s.push_back(value >> 8);
    s.push_back(value);
    return 4;
  } else if (value < 0x1000000) {
    s.push_back(0x03); // length
    s.push_back(value >> 16);
    s.push_back(value >> 8);
    s.push_back(value);
    return 5;
  } else if (value < 0x100000000) {
    s.push_back(0x04); // length
    s.push_back(value >> 24);
    s.push_back(value >> 16);
    s.push_back(value >> 8);
    s.push_back(value);
    return 6;
  } else {
    return -1;
  }
  return -1;
}

int HowManyBytesWillIntUse(int value) {
  if (value < 0) {
    return -1;
  }
  if (value < 0x100) {
    return 3;
  } else if (value < 0x10000) {
    return 4;
  } else if (value < 0x1000000) {
    return 5;
  } else if (value < 0x100000000) {
    return 6;
  } else {
    return -1;
  }
  return -1;
}

/**
 * @brief go into tag
 * @param start  char array starting with tag from which to skip tags
 * @param err error code
 * @return pointer to next tag
 */
void GoIntoTag(std::vector<unsigned char>::iterator &start, int *err,
               std::vector<unsigned char>::iterator end) {

  int length = GetLengthOfLength(start + 1, err, end) + 1; // +1 for tag
  if (*err != 0) {
    *err = 1;
    return;
  }

  start += length; // return pointer to next tag
}