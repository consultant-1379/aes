
#include <string>
#include <list>
#include <iostream>
#include "aes_ohi_blockhandler2.h"
#include "aes_ohi_errorcodes.h"

unsigned int reply;
unsigned int blockNumber;

int main()
{
    //define
    AES_OHI_BlockHandler2* myBlockSender = NULL;
    //create a block
    const char* myBlock = "My simple data block";
    unsigned int nrOfBytes = 20;
    //create instance
    myBlockSender = new AES_OHI_BlockHandler2(
    "mySystem","myApplname","myTq","myEvent","myStreamid");
    //attach to the transfer queue
    reply = myBlockSender->attach();
    //getlastCommitedBlockNo
    reply = myBlockSender->getLastCommitedBlockNo(blockNumber);
    //transactionBegin
    reply = myBlockSender->transactionBegin();
    //send 10 blocks
    unsigned int blnr = 1;
    for (int i=1; i<11; i++)
    {
	reply = myBlockSender->send(myBlock,nrOfBytes,blnr);
	blnr++;
    }
    //transactionEnd
    reply = myBlockSender->transactionEnd(blockNumber);
    //transactionCommit
    reply = myBlockSender->transactionCommit(blockNumber);
    //detach
    reply = myBlockSender->detach();
    //delete object
    if(myBlockSender != NULL)
    {
        delete myBlockSender;
	myBlockSender = NULL;
    }

    return 0;
}
