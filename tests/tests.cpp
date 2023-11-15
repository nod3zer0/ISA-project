#include <gtest/gtest.h>
#include "inc/BerEnumObject.h"
#include "inc/BerIntObject.h"
#include "inc/BerObject.h"
#include "inc/BerParser.h"
#include "inc/BerSequenceObject.h"
#include "inc/BerSetObject.h"
#include "inc/BerStringObject.h"
#include "inc/DatabaseObject.h"
#include "inc/FilterObject.h"
#include "inc/ldap_comunication.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>



// test for berParser int
TEST(berParserTestInt,BasicTests){
    std::vector<unsigned char> berRepresentation;
    berRepresentation.push_back(0x02);
    berRepresentation.push_back(0x01);
    berRepresentation.push_back(0x01);
    int err = 0;
    BerIntObject* bo = (BerIntObject*)ParseBerObject(berRepresentation.begin(), &err,berRepresentation.end());

    EXPECT_EQ(err,0);
    EXPECT_EQ(bo->getValue(),1);
    EXPECT_EQ(bo->getLenght(),3);
    EXPECT_EQ(bo->getBerObjectType(), berIntObject);
}

/*

create search request with missing message ID in this format: `echo -e '\x80' | nc host port`
echo -e '\x30 \x03 \x02 \x01 \x01 \x04 \x00 \x0a \x01 \x00 \x0a \x01 \x00 \x01 \x01 \xff' | nc localhost 389
*/

