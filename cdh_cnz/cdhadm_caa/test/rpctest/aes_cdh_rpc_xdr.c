#include <windows.h>
#include <d32-rpc.h>
#include "aes_cdh_rpc.h"


BOOL WINAPI xdr_AES_IFO_BLOCK (XDR *xdrs, AES_IFO_BLOCK *objp)
{
	if (!xdr_string(xdrs, &objp->mainfile_name, AES_IFO_FILE_NAME_SIZE)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->subfile_no)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->record_no)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->maxsize)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->sent_indicator)) {
		return (FALSE);
	}
	if (!xdr_bytes(xdrs, (char **)&objp->block.block_val, (u_int *)&objp->block.block_len, AES_IFO_BLOCK_SIZE)) {
		return (FALSE);
	}
	return (TRUE);
}




BOOL WINAPI xdr_answerCode (XDR *xdrs, answerCode *objp)
{
	if (!xdr_enum(xdrs, (enum_t *)objp)) {
		return (FALSE);
	}
	return (TRUE);
}




BOOL WINAPI xdr_lastTransactionAnswer (XDR *xdrs, lastTransactionAnswer *objp)
{
	if (!xdr_answerCode(xdrs, &objp->code)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->lastTransactionID)) {
		return (FALSE);
	}
	return (TRUE);
}




BOOL WINAPI xdr_BlockInfo (XDR *xdrs, BlockInfo *objp)
{
	if (!xdr_string(xdrs, &objp->theSourceID, ~0)) {
		return (FALSE);
	}
	if (!xdr_u_long(xdrs, &objp->theBlockID)) {
		return (FALSE);
	}
	if (!xdr_array(xdrs, (char **)&objp->theBlockData.theBlockData_val, (u_int *)&objp->theBlockData.theBlockData_len, ~0, sizeof(u_char), (xdr_proc) xdr_u_char)) {
		return (FALSE);
	}
	return (TRUE);
}




BOOL WINAPI xdr_NewString (XDR *xdrs, NewString *objp)
{
	if (!xdr_string(xdrs, objp, ~0)) {
		return (FALSE);
	}
	return (TRUE);
}


