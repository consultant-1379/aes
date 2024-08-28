//	********************************************************
//
//	 COPYRIGHT Ericsson 2015
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2015.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2015 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	********************************************************
#include "stubs/encryption_stub.h"
#include <string.h>

namespace cipher
{

const EVP_CIPHER * EVP_des_ede3_cfb()  {
 return 0;
}
 int EVP_CIPHER_CTX_init(const EVP_CIPHER_CTX * context) {
	return 1;
}

 int EVP_EncryptInit_ex(EVP_CIPHER_CTX * context , const EVP_CIPHER * cipher ,unsigned char * engine, const unsigned char * key, unsigned char* iv) {

	return 1;
}
 int EVP_DecryptInit_ex(EVP_CIPHER_CTX * context , const EVP_CIPHER * cipher ,unsigned char * engine, const unsigned char * key, unsigned char* iv) {
	return 1;
}
 int EVP_EncryptUpdate(EVP_CIPHER_CTX * context , unsigned char * ciphertext, int * tmpLen, const unsigned char * plaintext, int inputLength) {
	*tmpLen = inputLength;
	return 1;
}

 int EVP_DecryptUpdate(EVP_CIPHER_CTX * context , unsigned char * plaintext, int * tmpLen, const unsigned char * ciphertext, int cipherLength) {

	*tmpLen = cipherLength;
	return 1;
}

 int EVP_EncryptFinal_ex(EVP_CIPHER_CTX * context , unsigned char * ciphertext, int * tmplen) {
	 *tmplen = 0;
	return 1;
}
 int EVP_DecryptFinal_ex(EVP_CIPHER_CTX * context,  unsigned char * plaintext, int * tmplen) {
	 *tmplen = 0;
	return 1;
}
 int EVP_CIPHER_iv_length(const EVP_CIPHER * cipher){
	return 16;
}
 unsigned long ERR_get_error(){
	return 0;
}
 char * ERR_error_string(unsigned long err, char *buf){
	return 0;
}

 int EVP_CIPHER_CTX_cleanup(EVP_CIPHER_CTX * context){
	return 1;
}
 int RAND_bytes(unsigned char * buf , int len){
	return 1;
}
 int EVP_CIPHER_CTX_block_size(EVP_CIPHER_CTX * context){
	return 16;
}

}
