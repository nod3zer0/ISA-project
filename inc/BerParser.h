/**
 * @file BerParser.h
 * @author Rene Ceska xceska06 (xceska06@stud.fit.vutbr.cz)
 * @brief Parser for BER LDAP
 * @date 2023-11-19
 */

#ifndef BERPARSER_H
#define BERPARSER_H
#include "inc/BerObject.h"
#include "inc/ber_constants.h"
#include "inc/ber_helper_functions.h"
#include "inc/BerEnumObject.h"
#include "inc/BerStringObject.h"
#include "inc/BerSetObject.h"
#include "inc/BerSequenceObject.h"
#include "inc/BerSetObject.h"
#include "inc/BerIntObject.h"
#include "inc/BerBoolObject.h"
#include "inc/BerUndefinedObject.h"
#include <vector>

/**
 * @brief Parses BER and converts it to BerObject
 *
 * @param start  start of the BER
 * @param err 1 if error, 0 if success
 * @param end end of the array
 * @return BerObject*
 */
BerObject *ParseBerObject(std::vector<unsigned char>::iterator start,
                          int *err,std::vector<unsigned char>::iterator end);

#endif