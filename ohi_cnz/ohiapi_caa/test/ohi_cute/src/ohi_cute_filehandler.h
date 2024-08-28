#include "cute_suite.h"
#include "aes_ohi_filehandler.h"

class AES_OHI_CUTE_FileHandler
{
public:
	static cute::suite make_suite_aes_cute_filehandler();
	static void vAttach();
	static void vGetDirectoryPath();
	static void vSend();
	static void vSetTransferState();
	static void vGetTransferState();
	static void vGetTransferStateEx();
	static void vGetLastSentFile();
	static void vGetEventFileName();
	static void vHandleEvent();
	static void vGetEventHandle();
	static void vGetEvent();
	static void vDetach();
	static void vGetErrCodeText();

	static AES_OHI_FileHandler * pFileHandler;
};



