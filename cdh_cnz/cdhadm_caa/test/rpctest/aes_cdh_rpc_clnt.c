#include <windows.h>
#include <d32-rpc.h>
#include "aes_cdh_rpc.h"

/* Default timeout can be changed using clnt_control() */
struct timeval TIMEOUT = {25, 0};

int aes_ifo_output_1(struct AES_IFO_BLOCK *argp, u_long *res, void *clnt)
{
	return (clnt_call ((HCLIENT *) clnt, AES_IFO_OutPut, (xdr_proc) xdr_AES_IFO_BLOCK,
	        argp, (xdr_proc) xdr_u_long, res, TIMEOUT));
}


int lasttransaction_1(NewString *argp, lastTransactionAnswer *res, void *clnt)
{
	return (clnt_call ((HCLIENT *) clnt, LASTTRANSACTION, (xdr_proc) xdr_NewString,
	        argp, (xdr_proc) xdr_lastTransactionAnswer, res, TIMEOUT));
}


int put_1(BlockInfo *argp, answerCode *res, void *clnt)
{
	return (clnt_call ((HCLIENT *) clnt, PUT, (xdr_proc) xdr_BlockInfo,
	        argp, (xdr_proc) xdr_answerCode, res, TIMEOUT));
}

