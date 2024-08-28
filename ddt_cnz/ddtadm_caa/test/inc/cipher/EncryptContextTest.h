#ifndef INC_CIPHER_ENCRYPTCONTEXTTEST_H_
#define INC_CIPHER_ENCRYPTCONTEXTTEST_H_

#include "cute_suite.h"
#include "stubs/encryption_stub.h"

void encrypt16ByteData();
void decrypt16ByteData();
void cipherLengthCheck();
void plaintextLenCheck();
void encrypt160ByteData();
void decrypt160ByteData();

extern cute::suite make_suite_cipherHandlerTest();
#endif //INC_CIPHER_ENCRYPTCONTEXTTEST_H_
