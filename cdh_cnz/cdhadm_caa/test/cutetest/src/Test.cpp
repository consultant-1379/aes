/**
 * @file Test.cpp
 * It consists function definition to run the cdh test suites
 */

/* include section */
/* inclusion of ACE library*/
#include <ace/ACE.h>

#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
//#include "cute_cdh_filedestset.h"
#include "cute_cdh_cmd_handler.h"
//#include "ACS_TRA_Logging.h"

/**
 * @brief
 * Is a function to call the cdh test suites
 * No arguments
 *@return void
 */
void runSuite(){
	//TODO add your test here
	cute::ide_listener lis;
//	cute::makeRunner(lis)(CuteFileDestSet::make_suite_cute_cdh_filedestset(),"Running File Destination Test Suite\n");
	cute::makeRunner(lis)(Cute_Command_Handler::make_suite_cute_command_handler(),"Running File Destination Test Suite\n");
}/* end runSuite() */

/**
 * @brief
 * main function to call the runSuite() function
 * @return int
 * returns 0
 */
int main(){
//	ACS_TRA_Logging cute_logging;
//	cute_logging.Open("AES_CUTE");
    runSuite();
    return 0;
}
