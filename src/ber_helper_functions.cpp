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

unsigned int ParseINT(std::vector<unsigned char>::iterator s, int *err,
                      std::vector<unsigned char>::iterator end) {
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


int GetLengthOfLength(std::vector<unsigned char>::iterator start, int *err,
                      std::vector<unsigned char>::iterator end) {

  if (std::distance(start, end) < 1) {
    *err = 1;
    return 0;
  }

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

std::vector<unsigned char> ToLowerCase(std::vector<unsigned char> input) {
  std::vector<unsigned char> result;
  for (unsigned long int i = 0; i < input.size(); i++) {
        result.push_back(std::tolower(input[i]));
  }
  return result;
}


int GetLength(std::vector<unsigned char>::iterator start, int *err,
              std::vector<unsigned char>::iterator end) {
  if (std::distance(start, end) < 1) {
    *err = 1;
    return 0;
  }

  int length = 0;
  if ((start[0] >> 7) != 1) { // if first bit is 0 -> shortform
    length = start[0];
    *err = 0;
  } else {
    int lengthOfLength = start[0] & 0x7F; // remove bit indicating longform

    if (lengthOfLength > std::distance(start, end)) { // array is too short
      *err = 1;
      return 0;
    }
    if (lengthOfLength > 4 && start[lengthOfLength - 4] >
                                  0x00) { // only support up to 4 bytes, more is
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


void SkipTags(std::vector<unsigned char>::iterator &start, int n, int *err,
              std::vector<unsigned char>::iterator end) { // TODO: test

  int i = 0;
  int jumpLength = 1;
  while (i < n) {
    if (std::distance(start, end) < jumpLength) {
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


void GoIntoTag(std::vector<unsigned char>::iterator &start, int *err,
               std::vector<unsigned char>::iterator end) {

  int length = GetLengthOfLength(start + 1, err, end) + 1; // +1 for tag
  if (*err != 0) {
    *err = 1;
    return;
  }

  start += length; // return pointer to next tag
}