#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "ohi_cute_blockhandler.h"
#include <iostream>

using namespace std;

void AES_OHI_CUTE_BlockHandler::attach()
{
    	AES_OHI_BlockHandler2* myBlockSender = NULL;
    	myBlockSender = new AES_OHI_BlockHandler2(
    			"mySystem","myApplname","myTq","myEvent","myStreamid");
    	unsigned int reply = 0;
	cout<<"Calling the attach API of AES_OHI_BlockHandler2 "<<endl;
	reply = myBlockSender->attach();
	if ( reply == 0 )
	{
	    cout<<"Result of attach API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
	}
	ASSERTM("Failed to attach the transfer queue ",reply == 0);
	delete myBlockSender;
	myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::attachTwice()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        cout<<"Calling the attach API of AES_OHI_BlockHandler2 twice"<<endl;
        reply = myBlockSender->attach();
        reply = myBlockSender->attach();
        if ( reply == 0 )
        {
            cout<<"Result of attach API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to attach the transfer queue twice",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::attachMaxStreamSize()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001");
        unsigned int reply = 0;
        cout<<"Calling the attach API of AES_OHI_BlockHandler2 "<<endl;
        reply = myBlockSender->attach();
        if ( reply == 0 )
        {
            cout<<"Result of attach API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to attach the transfer queue ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::getLastCommittedBlockNoBeforeAttach()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        cout<<"Calling the getLastCommittedBlockNo API without calling attach API of AES_OHI_BlockHandler2 "<<endl;
        reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        if ( reply == 0 )
        {
            cout<<"Result of getLastCommittedBlockNo API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to get the last committed block no before attach ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::getLastCommittedBlockNoTwice()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        cout<<"Calling the getLastCommittedBlockNo API of AES_OHI_BlockHandler2 twice"<<endl;
        reply = myBlockSender->attach();
        reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        if ( reply == 0 )
        {
            cout<<"Result of getLastCommittedBlockNo API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to get the last committed block no calling twice ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::getLastCommittedBlockNoAfterTransactionBegin()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        cout<<"Calling the getLastCommittedBlockNo API after transactionBegin of AES_OHI_BlockHandler2 "<<endl;
        reply = myBlockSender->attach();
        reply = myBlockSender->transactionBegin();
	cout<<"reply is "<<reply<<endl;
        reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        if ( reply == 0 )
        {
            cout<<"Result of getLastCommittedBlockNo API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to get the last committed block no after transactionBegin",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::getLastCommittedBlockNo()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        cout<<"Calling the getLastCommittedBlockNo API of AES_OHI_BlockHandler2 "<<endl;
        reply = myBlockSender->attach();
	reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        if ( reply == 0 )
        {
            cout<<"Result of getLastCommittedBlockNo API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to get the last committed block no ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}


void AES_OHI_CUTE_BlockHandler::transactionBeginBeforeAttach()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
	cout<<"Calling the transactionBegin API without calling attach API of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->transactionBegin();
	if ( reply == 0)
	{
	    cout<<"Result of transactionBegin API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to begin the transaction",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::transactionBegin()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        cout<<"Calling the transactionBegin API after calling attach API of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->attach();
	reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        reply = myBlockSender->transactionBegin();
        if ( reply == 0)
        {
            cout<<"Result of transactionBegin API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to begin the transaction",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}
void AES_OHI_CUTE_BlockHandler::sendBeforeAttach()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        const char* myBlock = "My simple data block";
        unsigned int noOfBytes = 20;
        unsigned int reply = 0;
        unsigned int blnr = 1;
        cout<<"Calling the send API without calling the attach API of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->send(myBlock, noOfBytes, blnr);
        if ( reply == 0)
        {
            cout<<"Result of send API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to send the transaction before calling the attach ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}
void AES_OHI_CUTE_BlockHandler::sendBeforeTransactionBegin()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        const char* myBlock = "My simple data block";
        unsigned int noOfBytes = 20;
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        unsigned int blnr = 1;
        cout<<"Calling the send API before calling the transaction begin of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->attach();
        reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        reply = myBlockSender->send(myBlock, noOfBytes, blnr);
        if ( reply == 0)
        {
            cout<<"Result of send API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to send the transaction without calling transactionBegin",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::send()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
	const char* myBlock = "My simple data block";
	unsigned int noOfBytes = 20;
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        unsigned int blnr = 1;
        cout<<"Calling the send API of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->attach();
        reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        reply = myBlockSender->transactionBegin();
	reply = myBlockSender->send(myBlock, noOfBytes, blnr);
        if ( reply == 0)
        {
            cout<<"Result of send API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to send the transaction",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::transactionTerminateBeforeTransactionBegin()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        cout<<"Calling the transactionTerminate API before calling transactionBegin API of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->attach();
        reply = myBlockSender->transactionTerminate();
        if ( reply == 0)
        {
            cout<<"Result of transactionTerminate API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to terminate the transaction before calling transaction begin",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::transactionTerminate()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        cout<<"Calling the transactionTerminate API of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->attach();
	reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        reply = myBlockSender->transactionBegin();
        reply = myBlockSender->transactionTerminate();
        if ( reply == 0)
        {
            cout<<"Result of transactionTerminate API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;        }
        ASSERTM("Failed to terminate the transaction ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::transactionEndBeforeTransactionBegin()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
	unsigned int blockNo = 0;
        cout<<"Calling the transactionEnd API before calling transactionBegin API of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->attach();
        reply = myBlockSender->transactionEnd(blockNo);
        if ( reply == 0)
        {
            cout<<"Result of transactionEnd API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;      
	}
        ASSERTM("Failed to end the transaction before calling transaction begin",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::transactionEnd()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
	unsigned int blockNo = 0;
        cout<<"Calling the transactionEnd API of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->attach();
	reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        reply = myBlockSender->transactionBegin();
        reply = myBlockSender->transactionEnd(blockNo);
        if ( reply == 0)
        {
            cout<<"Result of transactionEnd API of AES_OHI_BlockHandler2 is SUCCESS"<<endl; 
	}
        ASSERTM("Failed to terminate the transaction ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::transactionCommitBeforeTransactionEnd()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        cout<<"Calling the transactionCommit API before calling the transactionEnd of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->attach();
	reply = myBlockSender->transactionCommit(blockNo);
        if ( reply == 0)
        {
            cout<<"Result of transactionCommit API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to commit the transaction ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::transactionCommit()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        cout<<"Calling the transactionCommit API of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->attach();
	reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        reply = myBlockSender->transactionBegin();
        reply = myBlockSender->transactionEnd(blockNo);
        reply = myBlockSender->transactionCommit(blockNo);
        if ( reply == 0)
        {
            cout<<"Result of transactionCommit API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to commit the transaction ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::detachBeforeAttach()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        cout<<"Calling the detach before attach API of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->detach();
        if ( reply == 0)
        {
            cout<<"Result of detach API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to detach the transaction ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::detachBeforeTransactionEnd()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        cout<<"Calling the detach before transactionEnd API of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->attach();
	reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        reply = myBlockSender->transactionBegin();
        reply = myBlockSender->detach();
        if ( reply == 0)
        {
            cout<<"Result of detach API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to detach the transaction ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::detach()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        cout<<"Calling the detach API of AES_OHI_BlockHandler2"<<endl;
        reply = myBlockSender->attach();
	reply = myBlockSender->getLastCommitedBlockNo(blockNo);
        reply = myBlockSender->transactionBegin();
        reply = myBlockSender->transactionEnd(blockNo);
        reply = myBlockSender->transactionCommit(blockNo);
        reply = myBlockSender->detach();
        if ( reply == 0)
        {
            cout<<"Result of detach API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to detach the transaction ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}


void AES_OHI_CUTE_BlockHandler::getEventBlockNo()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        unsigned int blockNo = 0;
        ACE_Semaphore * handle = NULL;
	AES_OHI_Eventcodes eventCode;
        cout<<"Calling the getEventHandle API of AES_OHI_BlockHandler2"<<endl;
        handle = myBlockSender->getEventHandle();
	reply = myBlockSender->getEvent(eventCode);
	reply = myBlockSender->getEventBlockNo(blockNo);
        if ( reply == 0)
        {
            cout<<"Result of getHandleEvent API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to getEventHandle the transaction ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

void AES_OHI_CUTE_BlockHandler::getEvent()
{
        AES_OHI_BlockHandler2* myBlockSender = NULL;
        myBlockSender = new AES_OHI_BlockHandler2(
                        "mySystem","myApplname","myTq","myEvent","myStreamid");
        unsigned int reply = 0;
        ACE_Semaphore * handle = NULL;
        AES_OHI_Eventcodes eventCode;
        cout<<"Calling the getEvent API of AES_OHI_BlockHandler2"<<endl;
        handle = myBlockSender->getEventHandle();
        reply = myBlockSender->getEvent(eventCode);
        if ( reply == 0)
        {
            cout<<"Result of getEvent API of AES_OHI_BlockHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to getHandle the transaction ",reply == 0);
        delete myBlockSender;
        myBlockSender = NULL;
}

cute::suite AES_OHI_CUTE_BlockHandler::make_suite_aes_cute_blockhandler()
{
	cute::suite s;
	s.push_back(CUTE(attach));
	s.push_back(CUTE(attachTwice));
	s.push_back(CUTE(attachMaxStreamSize));
	s.push_back(CUTE(getLastCommittedBlockNoBeforeAttach));
	s.push_back(CUTE(getLastCommittedBlockNoTwice));
	s.push_back(CUTE(getLastCommittedBlockNoAfterTransactionBegin));
	s.push_back(CUTE(getLastCommittedBlockNo));
	s.push_back(CUTE(transactionBeginBeforeAttach));
	s.push_back(CUTE(transactionBegin));
	s.push_back(CUTE(sendBeforeAttach));
	s.push_back(CUTE(sendBeforeTransactionBegin));
	s.push_back(CUTE(send));
	s.push_back(CUTE(transactionTerminateBeforeTransactionBegin));
	s.push_back(CUTE(transactionTerminate));
	s.push_back(CUTE(transactionEndBeforeTransactionBegin));
	s.push_back(CUTE(transactionEnd));
	s.push_back(CUTE(transactionCommitBeforeTransactionEnd));
	s.push_back(CUTE(transactionCommit));
	s.push_back(CUTE(detachBeforeAttach));
	s.push_back(CUTE(detachBeforeTransactionEnd));
	s.push_back(CUTE(detach));
	s.push_back(CUTE(getEventBlockNo));
	s.push_back(CUTE(getEvent));
	return s;
}
