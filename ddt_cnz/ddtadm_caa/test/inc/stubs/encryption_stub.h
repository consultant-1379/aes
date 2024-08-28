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

#ifndef INC_ENCRYPTION_STUB_H_
#define INC_ENCRYPTION_STUB_H_
#include <stdlib.h>
#include <boost/make_shared.hpp>
#include <cipher/contextinterface.h>

namespace cipher
{
	class StrategyInterface
	{
	public:
		StrategyInterface(){;}
		virtual ~StrategyInterface(){;}
	};
namespace DES3
{
	class Strategy : public StrategyInterface
	{
	public:
		Strategy(){;}
		virtual ~Strategy(){;}
	};
}
	class CipherManager
	{
	public:
		CipherManager(){;}
		virtual ~CipherManager(){;}
		void shutdown(){;}
		void setCipherStrategy(boost::shared_ptr<cipher::StrategyInterface> strat){ ;}
		boost::shared_ptr<cipher::ContextInterface>  getEncryptContext() { return boost::shared_ptr<cipher::ContextInterface>(); }

	};


struct evp_cipher_st
{
	void *cipherFun(void);
} /* EVP_CIPHER */;
typedef struct evp_cipher_st EVP_CIPHER;

struct evp_cipher_ctx_st
{
	const EVP_CIPHER *cipher;
} /* EVP_CIPHER_CTX */;
typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;

const  EVP_CIPHER * EVP_des_ede3_cfb();
int EVP_CIPHER_CTX_init(const EVP_CIPHER_CTX * context);
int EVP_EncryptInit_ex(EVP_CIPHER_CTX * context , const EVP_CIPHER * cipher ,unsigned char * engine,const unsigned char * key, unsigned char* iv);
int EVP_DecryptInit_ex(EVP_CIPHER_CTX * context , const EVP_CIPHER * cipher ,unsigned char * engine,const unsigned char * key, unsigned char* iv);
int EVP_EncryptUpdate(EVP_CIPHER_CTX * context , unsigned char * ciphertext, int * tmpLen, const unsigned char * plaintext, int inputLength);
int EVP_DecryptUpdate(EVP_CIPHER_CTX * context , unsigned char * plaintext, int * tmpLen, const unsigned char * ciphertext, int cipherLength);
int EVP_EncryptFinal_ex(EVP_CIPHER_CTX * context , unsigned char * ciphertext, int * tmplen);
int EVP_DecryptFinal_ex(EVP_CIPHER_CTX * context,  unsigned char * plaintext, int * tmplen);
int EVP_CIPHER_iv_length(const EVP_CIPHER * cipher);
unsigned long ERR_get_error();
char * ERR_error_string(unsigned long err, char *buf);
int EVP_CIPHER_CTX_cleanup(EVP_CIPHER_CTX * context);
int RAND_bytes(unsigned char * buf , int len);
int EVP_CIPHER_CTX_block_size(EVP_CIPHER_CTX * context);

}
#endif /* INC_ENCRYPTION_STUB_H_ */
