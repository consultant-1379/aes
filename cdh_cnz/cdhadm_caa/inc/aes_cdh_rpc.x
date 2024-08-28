/******************************************************************************
//
// NAME
//      aes_cdh_rpc.x
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//  An RPC XDR source file to generate the IFO and BGw RPC interface.

// DOCUMENT NO
//  190 89-CAA 109 0316

// AUTHOR 
// 	1999-04-15 by I/LD Gunnar Andersson

// SEE ALSO 
// 	-
//
//*****************************************************************************/
/*
* Define 1 procedure:
*
* AES_IFO_OutPut()	takes: 
*
*			mainfile name of the IFO-file,
*			subfile number,
*			record number,
*			maxsize of subfile,
*			sent indicator,
*			data block.
*
*			returns:
*
*			AES_IFO_OK if action was successful
*			AES_IFO_NOK if action was NOT successful
*/

const AES_IFO_OK = 0;
const AES_IFO_NOK = 1;

const AES_IFO_NOT_SENT = 0;
const AES_IFO_MAYBE_SENT = 1;

const AES_IFO_FILE_NAME_SIZE = 512;

const AES_IFO_BLOCK_SIZE = 32768;

struct AES_IFO_BLOCK
{
        string mainfile_name<AES_IFO_FILE_NAME_SIZE>;
        unsigned long subfile_no;
        unsigned long record_no;
        unsigned long maxsize;
        unsigned long sent_indicator;
        opaque block<AES_IFO_BLOCK_SIZE>;
};


program AES_IFO_PROG
{
    version AES_IFO_VERS
    {
  	unsigned long AES_IFO_OutPut(struct AES_IFO_BLOCK) = 1;

    } = 1;

} = 0x2df001f0;

 enum answerCode
 {
     HANDLED   =0,
     UNHANDLED =1
 };
 struct lastTransactionAnswer
 {
     answerCode         code;
     unsigned long      lastTransactionID;
 };
 struct BlockInfo
 {
     string theSourceID<>;
     /* A unique identification of the source.       *
     * Each rpc client that sends data to the same   *
     * rpc server must have an unique source id.     */
     unsigned long theBlockID;
     /* A unique block identifier  (4 bytes)         *
     * Restarts at 0 when maximum value (4294967295) *
     * has been reached                              */
     unsigned char theBlockData<>;
     /* CDR block data                               */
 };
 typedef string NewString<>;
 program BGW_CDRB_RECEIVER
 {
   version A
   {
    lastTransactionAnswer
    LASTTRANSACTION (NewString) = 1;
    /***************************************************
     PARAMETERS:
     *theSourceID - unique identification of the source
     *RETURN VALUE:
     *A struct containing an answer code which indicates
     *if the call was successful or not. IF the call was
     *successful, lastTransactionID will contain the ID
     *of the last transferred block for theSourceID
     *(or 0 if there were no previous block transferred)
     ***************************************************/
 
     answerCode PUT(BlockInfo) = 2;
     /***************************************************
     *PARAMETERS:
     *theBlock - the block to transfer to the destination
     *RETURN VALUE:
     *answerCode - can be HANDLED or UNHANDLED
    ****************************************************/
   } = 1;
 } = BGW_RPC_PROG_NUMBER;

