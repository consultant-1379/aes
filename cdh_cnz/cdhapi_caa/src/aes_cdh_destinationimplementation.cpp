/*=================================================================== */
/**
   @file   aes_cdh_destinationimplementation.cpp

   Class method implementationn for AES_CDH_DestinationImplementation type module.

   This module contains the implementation of class declared in
   the aes_cdh_destinationimplementation.h module

   This program receives parameters sent from programs
   cdhdef and cdhls. The parameters is then analysed and
   sent further to method sendCmd in class Destination.
   Cdhdef is calling method define and cdhls is calling
   method getAttr. The different parts is described below.

   cdhdef/define : The arguments are supplyed in the argument
   list when the program is activated. (example prompt> cdhdef
   -d dest1 -a address -r c:main -t iforpc). The arguments -d
   (destination name) and  and -a (host address) are required.
   The arguments -a and -t (transfer type (IFORPC for example))
   are checked and stored in the variables "destination" and
   "transtype". Default value for -t is IFORPC. The arguments
   -a and -t are removed from the argument list. The variables
   destination, transtype and the remaining arguments are sent
   further to method sendCmd in class destination.
   

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011   XCHEMAD     Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ace/OS.h>
#include <ace/ACE.h>
#include <ace/Get_Opt.h>
#include <ace/Task_T.h>
#include <ace/Message_Block.h>
#include <ACS_APGCC_Command.H>
#include <ACS_APGCC_Cmd_Client.H>
#include <ACS_APGCC_DSD.H>
#include <aes_cdh_destinationimplementation.h>
#include <aes_cdh_cmdclient.h>
#include <servr.h>
#include <ctype.h>
#include <aes_cdh_result.h>
#include <aes_cdh_asynchreceiver.h>
#include "aes_cdh_tracer.h"

AES_CDH_TRACE_DEFINE(AES_CDH_DestinationImplementation);

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: AES_CDH_DestinationImplementation
=================================================================== */
AES_CDH_DestinationImplementation::AES_CDH_DestinationImplementation() 
    : destinationName(""), lastErrorCode(AES_CDH_RC_OK)
{
	asynchReceiver = NULL;
	sessionIsOpen = false;
	closeDone = true;
	ACE::init();
	threadManager = new ACE_Thread_Manager;
}

/*===================================================================
   ROUTINE: AES_CDH_DestinationImplementation
=================================================================== */
AES_CDH_DestinationImplementation::AES_CDH_DestinationImplementation(
    const string destName) : destinationName(destName), lastErrorCode(AES_CDH_RC_OK)
{
	asynchReceiver = NULL;
	sessionIsOpen = false;
	closeDone = true;
	ACE::init();
	threadManager = new ACE_Thread_Manager;
}

/*===================================================================
   ROUTINE: ~AES_CDH_DestinationImplementation
=================================================================== */
AES_CDH_DestinationImplementation::~AES_CDH_DestinationImplementation() 
{
	// Wait some to let the last thread start
	ACE_Time_Value timeVal = ACE_OS::gettimeofday () + ACE_Time_Value(1);
	threadManager->wait(&timeVal);
	if(! sessionIsOpen)
	{
		ACS_APGCC_Command    cmd;
		cmd.cmdCode = ServR::CMD_STOPTASKS;
		cmd.data[0] = (int)ACE_OS::getpid();
		cmd.data[1] = (long)this;

		ACS_APGCC_Cmd_Client<ACS_APGCC_DSD_Connector,ACS_APGCC_DSD_Stream,ACS_APGCC_DSD_Addr>
		client("AES_CDH_server:AES"); // create the client

		client.execute(cmd);  // send to server

	}
	else
	{
		//check if rewrite this
		//       if(! closeDone)
		//          close();
	}

	try
	{
		delete threadManager;
	}
	catch(...)
	{}
}

/*===================================================================
   ROUTINE: exists
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationImplementation::exists(void)
{
	int r_code;

	AES_CDH_TRACE_MESSAGE("Entering AES_CDH_Destination::exists %s",destinationName.c_str());

	vector<AES_CDH_Destination::destAttributes> attr;

	r_code = list(destinationName, attr);

	lastErrorCode = r_code;

	AES_CDH_TRACE_MESSAGE("Exiting AES_CDH_Destination::exists %d",r_code);

	return AES_CDH_Result(r_code).code();
}

/*===================================================================
   ROUTINE: getError
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationImplementation::getError(void)
{
	return AES_CDH_Result(lastErrorCode.value()).code();
}

/*===================================================================
   ROUTINE: getErrorText
=================================================================== */
void AES_CDH_DestinationImplementation::getErrorText(string &errText)
{
	errText = AES_CDH_Result(lastErrorCode.value()).errorText();
}

/*===================================================================
   ROUTINE: getAttr
=================================================================== */
AES_CDH_Result AES_CDH_DestinationImplementation::getAttr(int argc, char* argv[],
			            vector<AES_CDH_Destination::destAttributes> &attr)
{
	// Clear the vector before using it
	// It might contain old destination data or garbage
	attr.clear();
	switch(argc)
	{
	case 1:
	{
		AES_CDH_TRACE_MESSAGE("Entering AES_CDH_Destination::getAttr %d",argc);
		break;
	}
	case 2:
	{
		AES_CDH_TRACE_MESSAGE("Entering AES_CDH_Destination::getAttr %d %s ",argc,argv[1]);
		break;
	}
	case 3:
	{
		AES_CDH_TRACE_MESSAGE("Entering AES_CDH_Destination::getAttr %d %s %s",argc,argv[1],argv[2]);
		break;
	}
	case 4:
	{
		AES_CDH_TRACE_MESSAGE("Entering AES_CDH_Destination::getAttr %d %s %s %s",argc,argv[1],argv[2],argv[3]);
		break;
	}
	default:
	{
		AES_CDH_TRACE_MESSAGE("Entering AES_CDH_Destination::getAttr %d %s %s %s %s",argc,argv[1],argv[2],argv[3],argv[4]);
		break;
	}
	}
	int r_code = parse_cdhls(argc, argv, attr);
	AES_CDH_TRACE_MESSAGE( "Exiting AES_CDH_Destination::getAttr %d ",r_code);
	return AES_CDH_Result(r_code);
}

/*===================================================================
   ROUTINE: list
=================================================================== */
int AES_CDH_DestinationImplementation::list(string destname,
                    vector<AES_CDH_Destination::destAttributes>& attr)
{
	ACS_APGCC_Command    cmd;
	AES_CDH_Destination::destAttributes tmpattr;
	int r_code = AES_CDH_RC_OK;
	cmd.clear();
	cmd.cmdCode = ServR::CMD_LIST;
	cmd.data[0] = destname;

	ACS_APGCC_Cmd_Client<ACS_APGCC_DSD_Connector,ACS_APGCC_DSD_Stream,ACS_APGCC_DSD_Addr>
	client("AES_CDH_server:AES"); // create the client

	r_code = client.execute(cmd);  // send to server
	if (r_code != 0)
	{
		r_code = AES_CDH_RC_NOSERVER;
	}
	else
	{
		r_code = cmd.result;
	}
	if (r_code == AES_CDH_RC_OK)
	{
		for ( int j = 0; j < cmd.numOfData(); )
		{
			tmpattr.otherAttrs.clear();
			tmpattr.destName     = (std::string)cmd.data[j++];
			tmpattr.destSetName	 = (std::string)cmd.data[j++];
			tmpattr.destPath	 = (std::string)cmd.data[j++];
			tmpattr.transferType = (std::string)cmd.data[j++];
			int loop = cmd.data[j++];
			for ( int i = 0; i < loop; i++ )
			{
				tmpattr.otherAttrs.push_back((string)cmd.data[j++]);
			}
			attr.push_back( tmpattr );
		}
	}

	return r_code;
}

/*===================================================================
   ROUTINE: parse_cdhls
=================================================================== */
int AES_CDH_DestinationImplementation::parse_cdhls(int argc, char* argv[],
                    vector<AES_CDH_Destination::destAttributes>& attr)
{
	int exitcode = AES_CDH_RC_INCUSAGE;
	int opt;
	string destName = "";

	ACE_Get_Opt getopt(argc, argv, "");

	while ((opt = getopt()) != EOF)
	{
		switch (opt)
		{
		case '?':
			{
				return AES_CDH_RC_INCUSAGE;
			}

		default:
		{
			return AES_CDH_RC_INTPROGERR;
		}
		}
	}

	if (getopt.optind + 1 < argc)
		return AES_CDH_RC_INCUSAGE;

	if (getopt.optind < argc)
	{
		destName = argv[getopt.optind];
	}

	exitcode = list(destName,attr);

	return exitcode;
}









