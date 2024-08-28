/*=================================================================== */
/**
   @file   aes_dbo_service.cpp

   Class method implementation for AES_DBO_Service type module.

   This module contains the implementation of class declared in
   the aes_dbo_service.h module

   @version 1.0.0
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/10/2011   XCHEMAD     Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

// C++ header files

#include <aes_dbo_service.h>
#include <aes_dbo_ha_service.h>
#include <aes_dbo_server.h>
#include <aes_dbo_daemon.h>
#include <aes_dbo_macros.h>
//#include <aes_gcc_tracer.h>

//AES_GCC_TRACE_DEFINE(AES_DBO_Service);

AES_DBO_HA_Service *pdboObj  = 0;

aes_dbo_server *gServR = 0;

void sighandler(int signum)
{
	if( signum == SIGTERM || signum == SIGINT || signum == SIGTSTP )
	{
		_PRINT_DBG_L(aes_dbo_ns::S_P_LEVEL_0, printf("Received signal %d\n", signum));
		if( gServR != 0){
			gServR->StopEvent->signal();
			gServR->isStopEventSignalled = true;
			sleep(1); //Delay to handle APGCC reactor delay
			gServR->close();
		}
	}

	if(signum == SIGPIPE)
	{
		_PRINT_DBG_L(aes_dbo_ns::S_P_LEVEL_0, printf("SIGPIPE received\n"));
	}
	_PRINT_DBG_L(aes_dbo_ns::S_P_LEVEL_0, printf("Bye Bye...\n"));
}

void PrintUsage()
{
	cout<<"Usage:\naes_dbod -d for debug mode."<<endl;
}

ACE_THR_FUNC_RETURN DBO_serviceMain(void *ptr );

ACE_INT32 ACE_TMAIN(ACE_INT32 argc, ACE_TCHAR* argv[])
{
	if ( argc >  1)
	{
		if((argc == 2)&& (strcmp( argv[1], "-d") == 0))
		{
			cout << "STARTING...\n" << endl;
			//Register the signal handler

			struct sigaction sa;
			sa.sa_handler = sighandler;
			sa.sa_flags = 0;
			sigemptyset(&sa.sa_mask );

			if( sigaction(SIGINT, &sa, NULL ) == -1)
			{
				cout << "Error occurred while handling SIGINT" << endl;
				return -1;
			}
			if( sigaction(SIGTERM, &sa, NULL ) == -1)
			{
				cout << "Error occurred while handling SIGTERM" << endl;
				return -1;
			}
			if( sigaction(SIGTSTP, &sa, NULL ) == -1)
			{
				cout << "Error occurred while handling SIGTSTP" << endl;
				return -1;
			}
			_PRINT_DBG_L(aes_dbo_ns::S_P_LEVEL_0, printf("Starting DBO Service\n"));
			gServR = new (std::nothrow) aes_dbo_server();
			if(	gServR == 0  )
			{
					cout << " Memory Allocated failed for servR " << endl;
					return 0;
			}

			ACE_thread_t applicationThreadId = 0;

			int threadGroupId =
				ACE_Thread_Manager::instance()->spawn(&DBO_serviceMain,
				(void *)gServR ,
				THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
				&applicationThreadId,
				0,
				ACE_DEFAULT_THREAD_PRIORITY,
				-1,
				0,
				ACE_DEFAULT_THREAD_STACKSIZE);

			if (threadGroupId == -1)
			{
					cout << " Thread spawn failed. " << endl;
					delete gServR;
					gServR = 0;
					return 0;
			}

			gServR->open();
			gServR->start_service();
			if( applicationThreadId != 0 )
			{
				ACE_Thread_Manager::instance()->join( applicationThreadId );
			}

			delete gServR;
			gServR = 0;
		}
		else
		{
			PrintUsage();
		}
	}
	else if(argc == 1)
	{
		//HA Mode
		ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

		//Create an object of aes_dbo_ha_service class with user as root.
		pdboObj = new AES_DBO_HA_Service("aes_dbod", "root");

		//Check for Memory allocation
		if( pdboObj == 0 )
		{
			//AES_GCC_TRACE_MESSAGE("Failed to allocate AES_DBO_HA_Service!");
			return -1;
		}
		else
		{
			//Register with core middleware.
			errorCode = pdboObj->activate();
			if (errorCode == ACS_APGCC_HA_FAILURE)
			{
				cout<<"Activate failed \n";
				//AES_GCC_TRACE_MESSAGE("HA activation for aes_dbod failed!");
				delete pdboObj;
				pdboObj = 0;
				return -1;

			}
			else if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE)
			{
				//AES_GCC_TRACE_MESSAGE("HA Failed to close aes_dbod gracefully!");
				delete pdboObj;
				pdboObj = 0;
				return -1;

			}
			else if (errorCode == ACS_APGCC_HA_SUCCESS)
			{
				//AES_GCC_TRACE_MESSAGE("HA Gracefully closed aes_dbod!");
				delete pdboObj;
				pdboObj = 0;
				//AES_GCC_TRACE_MESSAGE("Leaving aes_dbod!");
				return 0;
			}
			else
			{
				//AES_GCC_TRACE_MESSAGE("Error occurred while integrating aes_dbod with HA!");
				delete pdboObj;
				pdboObj = 0;
				//AES_GCC_TRACE_MESSAGE("Leaving aes_dbod!");
				return -1;
			}

		}
	}

	//AES_GCC_TRACE_MESSAGE("Leaving aes_dbod!");
	return 0;
}
