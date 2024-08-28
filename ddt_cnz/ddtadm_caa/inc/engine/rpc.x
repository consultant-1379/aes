/******************************************************************************
//
// NAME
//      rpc.x
//
//*****************************************************************************/

 enum answerCode
 {
     HANDLED   =0,
     UNHANDLED =1
 };

 struct lastTransactionAnswer
 {
     answerCode  code;
     unsigned long  lastTransactionID;
 };
 
 struct BlockInfo
 {
     string theSourceID<>;
   	 unsigned long theBlockID;
     unsigned char theBlockData<>;
   
 };
 
  
 program BGW_CDRB_RECEIVER
 {
   version A
   {
    	lastTransactionAnswer LASTTRANSACTION (string) = 1;
     
    	 answerCode PUT(BlockInfo) = 2;
    
   } = 1;
   
 } = BGW_RPC_PROG_NUMBER;
