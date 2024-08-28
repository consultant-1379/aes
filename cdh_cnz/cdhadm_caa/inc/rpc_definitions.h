#ifndef RPC_H
#define RPC_H

#include <stdio.h>
#include <rpc/rpc.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <aes_cdh_rpc.h>

#ifndef RPC_SUCCESSFUL
#define RPC_SUCCESSFUL           0
#endif

#define BGW_RPC_PROG_NUMBER         0x2df001f1



#ifdef __cplusplus
extern "C" {
#endif
	typedef CLIENT HCLIENT;
	typedef CLIENT *PHCLIENT;
#if 0
	bool_t xdr_AES_IFO_BLOCK (XDR *xdrs, AES_IFO_BLOCK *objp)
	{
		if (!xdr_string(xdrs, &objp->mainfile_name, AES_IFO_FILE_NAME_SIZE))
			return FALSE;

		if (!xdr_u_long(xdrs, &objp->subfile_no))
			return FALSE;

		if (!xdr_u_long(xdrs, &objp->record_no))
			return FALSE;

		if (!xdr_u_long(xdrs, &objp->maxsize))
			return FALSE;

		if (!xdr_u_long(xdrs, &objp->sent_indicator))
			return FALSE;

		if (!xdr_bytes(xdrs, (char **)&objp->block.block_val, (u_int *)&objp->block.block_len, AES_IFO_BLOCK_SIZE))
			return FALSE;

			return TRUE;
	}

	bool_t xdr_answerCode(XDR *xdrs, answerCode *objp)
	{
		if (!xdr_enum(xdrs, (enum_t *)objp))
			return FALSE;

		return TRUE;
	}

 	bool_t xdr_BlockInfo(XDR *xdrs, BlockInfo *objp)
	{
		if (!xdr_string(xdrs, &objp->theSourceID, ~0))
			return FALSE;

		if (!xdr_u_long(xdrs, &objp->theBlockID))
			return FALSE;

		if (!xdr_array(xdrs, (char **)&objp->theBlockData.theBlockData_val, (u_int *)&objp->theBlockData.theBlockData_len, ~0, sizeof(u_char), (xdrproc_t) xdr_u_char))
			return FALSE;
		return TRUE;
	}

	bool_t xdr_lastTransactionAnswer(XDR *xdrs, lastTransactionAnswer *objp)
	{
		if (!xdr_answerCode(xdrs, &objp->code))
			return FALSE;

		if (!xdr_u_long(xdrs, &objp->lastTransactionID))
			return FALSE;

		return TRUE;
	}

	 bool_t xdr_NewString(XDR *xdrs, NewString *objp)
	{
		if (!xdr_string(xdrs, objp, ~0))
			return FALSE;

		return TRUE;
	}
#endif
#ifdef __cplusplus
	}
#endif

#endif
