#include "cute_suite.h"
#include "aes_ohi_extfilehandler2.h"

class AES_OHI_CUTE_ExtFileHandler
{
public:
	static cute::suite make_suite_aes_cute_extfilehandler();
	static void attach();
	static void detach();
	static void fileTransferQueueDefined();
	static void getEvent();
	static void removeFile();
};


