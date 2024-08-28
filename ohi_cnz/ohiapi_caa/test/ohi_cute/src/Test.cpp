#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Test.h"
#include "ohi_cute_blockhandler.h"
#include "ohi_cute_filehandler.h"
#include "ohi_cute_directoryhandler.h"
#include "ohi_cute_extfilehandler.h"

void runSuite()
{
	cute::suite s;
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
	cute::makeRunner(lis)(AES_OHI_CUTE_BlockHandler::make_suite_aes_cute_blockhandler(),"Block Handler Test Suite\n");
	cute::makeRunner(lis)(AES_OHI_CUTE_FileHandler::make_suite_aes_cute_filehandler(),"File Handler Test Suite\n");
	cute::makeRunner(lis)(AES_OHI_CUTE_DirectoryHandler::make_suite_aes_cute_directoryhandler(),"Directory Handler Test Suite\n");
	cute::makeRunner(lis)(AES_OHI_CUTE_ExtFileHandler::make_suite_aes_cute_extfilehandler(),"Ext File Handler Test Suite\n");
}

int main()
{
	runSuite();
	return 0;
}


