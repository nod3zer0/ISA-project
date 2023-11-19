/**
 * @file ber_constants.h
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @brief Constants that are used in LDAP BER
 * @date 2023-11-19
 */

#ifndef BER_CONSTANTS_H
#define BER_CONSTANTS_H
const unsigned int BER_TAG_LENGTH = 1;
const unsigned int BER_LENGTH_OF_LENGTH_TAG = 1;
const unsigned int BER_4BYTE_LENGTH_LENGTH = 4;
const unsigned int BER_EXTENDED_RESPONSE_C = 0x78;
const unsigned int BER_BIND_REQUEST_C = 0x60;
const unsigned int BER_BIND_RESPONSE_C = 0x61;
const unsigned int BER_SEARCH_REQUEST_C = 0x63;
const unsigned int BER_SEARCH_RESULT_ENTRY_C = 0x64;
const unsigned int BER_SEARCH_RESULT_DONE_C = 0x65;
const unsigned int BER_UNBIND_REQUEST_C = 0x42;

const unsigned int BER_BOOL_C = 0x01;
const unsigned int BER_INT_C = 0x02;
const unsigned int BER_INT_4BYTES_C = 0x84;
const unsigned int BER_OCTET_STRING_C = 0x04;
const unsigned int BER_ENUM_C = 0x0A;
const unsigned int BER_SEQUENCE_C = 0x30;
const unsigned int BER_SET_C = 0x31;

//constants --- result codes

const unsigned int BER_LDAP_SUCCESS = 0x00;
const unsigned int BER_LDAP_PROTOCOL_ERROR = 0x02;
const unsigned int BER_LDAP_UNAVAILABLE = 0x34;
const unsigned int BER_LDAP_SIZE_LIMIT_EXCEEDED = 0x04;
const unsigned int BER_LDAP_AUTH_METHOD_NOT_SUPPORTED = 0x07;



#endif