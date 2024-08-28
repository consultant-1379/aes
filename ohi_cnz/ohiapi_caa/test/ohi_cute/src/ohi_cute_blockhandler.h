#include "cute_suite.h"
#include "aes_ohi_blockhandler2.h"
#include "aes_ohi_eventcodes.h"

class AES_OHI_CUTE_BlockHandler
{
public:
	static cute::suite make_suite_aes_cute_blockhandler();
	static void attach();
	static void attachTwice();
	static void attachMaxStreamSize();
	static void getLastCommittedBlockNoBeforeAttach();
	static void getLastCommittedBlockNoTwice();
	static void getLastCommittedBlockNoAfterTransactionBegin();
	static void getLastCommittedBlockNo();
	static void transactionBeginBeforeAttach();
	static void transactionBegin();
	static void sendBeforeAttach();
	static void sendBeforeTransactionBegin();
	static void send();
	static void transactionTerminateBeforeTransactionBegin();
	static void transactionTerminate();
	static void transactionEndBeforeTransactionBegin();
	static void transactionEnd();
	static void transactionCommitBeforeTransactionEnd();
	static void transactionCommit();
	static void detachBeforeAttach();
	static void detachBeforeTransactionEnd();
	static void detach();
	static void getEventBlockNo();
	static void getEvent();

};


