
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
int writeInt(unsigned char *s, int value);

int ParseLength(unsigned char *start, int *err);

int getLengthLength(unsigned char *start, int *err);

unsigned char *skipTags(unsigned char *start, int n, int *err);

unsigned char *goIntoTag(unsigned char *start, int *err);