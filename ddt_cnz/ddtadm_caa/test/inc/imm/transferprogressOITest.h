#include "cute_suite.h"
#include "imm/transferprogress_oi.h"

class TransferProgressOITest
{
	static void updateRuntimeSuccess();
	static void updateRuntimeFailure();
	static void modifyRuntimeAttributeSuccess();
	static void modifyRuntimeAttributeFailure();
	static imm::TransferProgress_OI  transferProgressImpl;
public:
	static cute::suite make_suite_transferProgressOITest();

};

