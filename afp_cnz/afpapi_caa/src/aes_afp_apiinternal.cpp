//	INCLUDE aes_afp_apiinternal.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
//	  All rights reserved.
//	  The Copyright to the computer program(s) herein
//	  is the property of Ericsson Utvecklings AB, Sweden.
//	  The program(s) may be used and/or copied only with
//	  the written permission from Ericsson Utvecklings AB or
//	in
//	  accordance with the terms and conditions stipulated in
//	the
//	  agreement/contract under which the program(s) have
//	been
//	  supplied.
//
//	DESCRIPTION
//
//	ERROR HANDLING
//	 General rule:
//	 The error handling is specified for each method.
//	 No methods initiate or send error reports unless
//	specified.
//
//	DOCUMENT NO
//	 190 89-CAA 109 0505
//
//	AUTHOR
//	 2002-12-19 DAPA
//
//	REVISION
//	 A 2002-12-19 DAPA
//	 B 2003-04-30 DAPA
//
//	LINKAGE
//
//	SEE ALSO

#include "aes_afp_api.h"
#include "aes_afp_apitask.h"
#include "aes_afp_apiinternal.h"
#include "aes_ohi_errorcodes.h"
#include <ACS_CS_API_Common_R1.h>
#include <ACS_CS_API_NetworkElement_R1.h>
#include <fstream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <ACS_APGCC_Util.H>
#include "aes_afp_api_tracer.h"
#define APPLICATION_TYPE 1
#define STS_APPLICATION "sts"
AES_AFP_TRACE_DEFINE(AES_AFP_apiinternal);
typedef std::vector< std::string  > TemplateVector;

//	---------------------------------------------------------
//	       Constructor()
//	---------------------------------------------------------
aes_afp_apiinternal::aes_afp_apiinternal(AES_AFP_Api* apiPointer)
: MAX_REMOVEDELAY_(20160),
  MIN_REMOVEDELAY_(-1),
  MAX_NROFRETRIES_(100),
  MIN_NROFRETRIES_(-1),
  MAX_RETRYTIME_(59999),
  MIN_RETRYTIME_(10),
  MAX_DESTINATIONSET_LEN_(32),
  MAX_TRANSFERQUEUE_LEN_(32),
  MAX_FILENAME_LEN_(255),
  MAX_SOURCEDIRECTORY_LEN_(255),
  MAX_USERGROUP_LEN_(256),
  NOTALLOWEDCHARACTERS_("< > : \" / \\ | \?"),
  ALLOWEDCHARACTERS_("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-"),
  MAX_NAMETAG_LEN_(32),
  apiPtr_(0),
  convSession_(0),
  convService_(0),
  apiTask_(0)
{
	AES_AFP_TRACE_MESSAGE
("Constructor() R3\n");
	apiPtr_ = apiPointer;
        applicationSts = "";
}

aes_afp_apiinternal::~aes_afp_apiinternal()
{
}

//	---------------------------------------------------------
//	       fileExist()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::transferQueueDefined (std::string& transferQueue, 
		std::string& destinationSet,
		bool &manual,
		std::string& tranferQueueDn)
{
	AES_AFP_TRACE_MESSAGE("TQ %s, destinationSet %s", transferQueue.c_str(), destinationSet.c_str()) ;

	std::stringstream strStream;
	std::string dataString("");
	char tempbuf[128];
	int result(0);

	protocol_.clearValues();

	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);

	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}

	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);

	result = sendToServer(0, COMMAND, FINDTRANSFERQUEUE, 1, dataString);

	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(strStream);

	AES_AFP_TRACE_MESSAGE( "Received from server, result = %d", result);

	if (result == 0)
	{

		//spool filename
		strStream.getline(tempbuf,128,'\n');

		//userGroup
		strStream.getline(tempbuf,128,'\n');

		//spool destinationSet
		strStream.getline(tempbuf,128,'\n');

		//spool status
		strStream.getline(tempbuf,128,'\n');

		//spool removedelay
		strStream.getline(tempbuf,128,'\n');

		//get manual flag
		strStream.getline(tempbuf,4,'\n');

		int tmpInt = atoi(tempbuf);
		if (tmpInt == 0)
		{
			manual = false;
		}
		else
		{
			manual = true;
		}

		//rename type
		strStream.getline(tempbuf,128,'\n');

		//source dir
		strStream.getline(tempbuf,128,'\n');

		//transferQueueDn
		strStream.getline(tempbuf, 128, '\n');
		tranferQueueDn = tempbuf;

	}
	return result;
}

//## Operation: getTransferState%387C2C0400E3
//## Semantics:
//	---------------------------------------------------------
//	       getTransferState()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::getTransferState (std::string& filename, std::string& destinationSet, AES_GCC_Filestates& transferState)
{
	//## begin aes_afp_apiinternal::getTransferState%387C2C0400E3.body preserve=yes
	AES_AFP_TRACE_MESSAGE("TQ = %s Destination = %s", filename.c_str(), destinationSet.c_str());

	std::stringstream strStream;
	std::string dataString;
	char tempbuf[32];
	int result(0);
	std::string tmpOrder;
	std::string tmpSubname;
	std::string user;

	protocol_.clearValues();


	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}

	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	result = checkFilename(filename);
	if (result != 0)
	{
		return(result);
	}

	protocol_.addStringValue("FILENAME", filename);

	AES_AFP_TRACE_MESSAGE("State - Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, GETSTATUSFILE, 1, dataString);

	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(strStream);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	if (result == 0)
	{
		//strStream.freeze(true);

		strStream.getline(tempbuf,32,'\n');
		switch (atoi (tempbuf))
		{
		case AES_FSREADY		: transferState = AES_FSREADY; break;
		case AES_FSSEND			: transferState = AES_FSSEND; break;
		case AES_FSARCHIVE		: transferState = AES_FSARCHIVE; break;
		case AES_FSDELETE		: transferState = AES_FSDELETE; break;
		case AES_FSFAILED		: transferState = AES_FSFAILED; break;
		case AES_FSUSEDEFAULT   : transferState = AES_FSUSEDEFAULT; break;
		case AES_FSREMOVE       : transferState = AES_FSREMOVE; break;
		case AES_FSSTOPPED      : transferState = AES_FSSTOPPED; break;
		case AES_FSPENDING      : transferState = AES_FSPENDING; break;
		case AES_FSNONE			: transferState = AES_FSNONE; break;
		default					: transferState = AES_FSNONE;
		}

		//strStream.freeze(false);
	}

	return result;
	//## end aes_afp_apiinternal::getTransferState%387C2C0400E3.body
}

//## Operation: getTransferStateEx%3CF5FE8302BC
//## Semantics:
//	---------------------------------------------------------
//	       getFileStatusEx()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::getTransferStateEx (std::string& filename, AES_GCC_Filestates& status, std::string& destinationSet, int listOrder, std::string& reportDate, std::string& deleteDate, std::string& manualStart, std::string& manualStop, std::string& failDate, std::string& archiveDate, int& reasonForFailed, bool& isDirectory)
{
	(void)(isDirectory);
	//## begin aes_afp_apiinternal::getTransferStateEx%3CF5FE8302BC.body preserve=yes
	AES_AFP_TRACE_MESSAGE("TQ = %s, isDirectory = %d",filename.c_str(),isDirectory);

	std::stringstream strStream;
	std::string dataString;
	char tempbuf[1024];
	int result(0);
	std::string tmpFileName;
	std::string user;


	protocol_.clearValues();


	result = checkDestinationSetName(destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	protocol_.addStringValue("DESTINATIONSSET", destinationSet);

	result = checkFilename(filename);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	protocol_.addStringValue("FILENAME", filename);

	AES_AFP_TRACE_MESSAGE("Ex - Send to server");

	protocol_.getString(dataString);

	if (filename.empty() )
	{
		switch (listOrder)
		{
		case 0: result = sendToServer(0, COMMAND, GETSTATUSOLDFILE, 1, dataString); break;
		case 1: result = sendToServer(0, COMMAND, GETSTATUSNEWFILE, 1, dataString); break;
		default: result = sendToServer(0, COMMAND, GETSTATUSFILE, 1, dataString); break;
		}
	}
	else
	{
		switch (listOrder)
		{
		case 2: result = sendToServer(0, COMMAND, GETSTATUSFILE, 1, dataString); break;
		case 3: result = sendToServer(0, COMMAND, GETSTATUSFILE, 1, dataString); break;
		case 4: result = sendToServer(0, COMMAND, GETSTATUSNEXTFILE, 1, dataString); break;
		default: result = sendToServer(0, COMMAND, GETSTATUSFILE, 1, dataString); break;
		}
	}

	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(strStream);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	if (result == 0)
	{
		reasonForFailed = 0;

		strStream.getline(tempbuf,1024,'\n');
		switch (atoi (tempbuf))
		{
		case AES_FSREADY: status = AES_FSREADY; break;
		case AES_FSSEND: status = AES_FSSEND; break;
		case AES_FSARCHIVE: status = AES_FSARCHIVE; break;
		case AES_FSDELETE: status = AES_FSDELETE; break;
		case AES_FSFAILED: status = AES_FSFAILED; reasonForFailed = 10; break;
		case AES_FSUSEDEFAULT: status = AES_FSUSEDEFAULT; break;
		case AES_FSREMOVE: status = AES_FSREMOVE; break;
		case AES_FSSTOPPED: status = AES_FSSTOPPED; break;
		case AES_FSPENDING: status = AES_FSPENDING; break;
		case AES_FSNONE: status = AES_FSNONE; break;
		default: status = AES_FSNONE;
		}

		strStream.getline(tempbuf,1024,'\n');
		filename = tempbuf;

		strStream.getline(tempbuf,1024,'\n');
		destinationSet = tempbuf;

		strStream.getline(tempbuf,1024,'\n');
		reportDate = tempbuf;

		strStream.getline(tempbuf,1024,'\n');
		deleteDate = tempbuf;

		strStream.getline(tempbuf,1024,'\n');
		manualStart = tempbuf;

		strStream.getline(tempbuf,1024,'\n');
		manualStop = tempbuf;

		strStream.getline(tempbuf,1024,'\n');
		failDate = tempbuf;

		strStream.getline(tempbuf,1024,'\n');
		archiveDate = tempbuf;

	}

	return result;
	//## end aes_afp_apiinternal::getTransferStateEx%3CF5FE8302BC.body
}

//## Operation: setTransferState%387C2C0400F7
//## Semantics:
//	---------------------------------------------------------
//	    setTransferState()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::setTransferState (std::string& filename, std::string& destinationSet, AES_GCC_Filestates transferState)
{
	//## begin aes_afp_apiinternal::setTransferState%387C2C0400F7.body preserve=yes
	AES_AFP_TRACE_MESSAGE("File name = %s Destination = %s State = %d",
			filename.c_str(), destinationSet.c_str(),transferState);

	std::string dataString;
	std::string tmpOrder;
	std::string tmpSubname;
	int result(0);
	std::string user;

	protocol_.clearValues();
	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}

	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	result = checkFilename(filename);
	if (result != 0)
	{
		return(result);
	}

	protocol_.addStringValue("FILENAME", filename);

	result = checkDefaultStatus(transferState, destinationSet);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addIntValue("STATUS", transferState);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, SETSTATUSFILE, 1, dataString);

	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer();

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	return result;
	//## end aes_afp_apiinternal::setTransferState%387C2C0400F7.body
}

//## Operation: listFile%387C2C04010B; C++
//## Semantics:
//	---------------------------------------------------------
//	       listFile()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::listFile (std::string& transferQueue, std::string& destinationSet, std::stringstream& outStream)
{
	//## begin aes_afp_apiinternal::listFile%387C2C04010B.body preserve=yes
	AES_AFP_TRACE_MESSAGE("TQ = %s, destinationSet = %s", transferQueue.c_str(), destinationSet.c_str());

	int result(0);
	std::string dataString("");
	std::string user("");

	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}

	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, INFOONETRANSFERQUEUE, 1, dataString);

	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(outStream);
	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	if (result == 0)
	{
	}

	return result;
	//## end aes_afp_apiinternal::listFile%387C2C04010B.body
}


//## Operation: listNewestFile%387C2C040129; C++
//## Semantics:
//	---------------------------------------------------------
//	       listYoungestSubfile()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::listNewestFile (std::string& transferQueue, std::string& destinationSet, std::stringstream& outStream)
{
	//## begin aes_afp_apiinternal::listNewestFile%387C2C040129.body preserve=yes
	AES_AFP_TRACE_MESSAGE("TQ = %s", transferQueue.c_str());

	int result(0);
	std::string dataString;
	std::string user;

	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}

	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, FINDNEWESTFILE, 1, dataString);

	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(outStream);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	if (result == 0)
	{
		//			outStream.freeze();
	}

	return result;
	//## end aes_afp_apiinternal::listNewestFile%387C2C040129.body
}

//## Operation: listOldestFile%387C2C04013D; C++
//## Semantics:
//	---------------------------------------------------------
//	       listOldestSubfile()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::listOldestFile (std::string& transferQueue, std::string& destinationSet, std::stringstream& outStream)
{
	//## begin aes_afp_apiinternal::listOldestFile%387C2C04013D.body preserve=yes
	AES_AFP_TRACE_MESSAGE("TQ = %s", transferQueue.c_str());

	int result(0);
	std::string dataString("");
	std::string user("");

	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, FINDOLDESTFILE, 1, dataString);

	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(outStream);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	if (result == 0)
	{
	}

	return result;
	//## end aes_afp_apiinternal::listOldestFile%387C2C04013D.body
}

//## Operation: listAllFiles%387C2C040147; C++
//## Semantics:
//	---------------------------------------------------------
//	       listAllSubfiles()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::listAllFiles (std::string& transferQueue, std::stringstream& outStream)
{
	//## begin aes_afp_apiinternal::listAllFiles%387C2C040147.body preserve=yes
	AES_AFP_TRACE_MESSAGE("TQ = %s", transferQueue.c_str());

	std::string dataString;
	int result(0);
	std::string user;

	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, INFOALLTRANSFERQUEUES, 1, dataString);

	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("return code from sendToServer is %d",result );
	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(outStream);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	if (result == 0)
	{
	}

	return result;
	//## end aes_afp_apiinternal::listAllFiles%387C2C040147.body
}

unsigned int aes_afp_apiinternal::listAllFilesShort (std::string& transferQueue, std::stringstream& outStream)
{
        //## begin aes_afp_apiinternal::listAllFiles%387C2C040147.body preserve=yes
        AES_AFP_TRACE_MESSAGE("TQ = %s", transferQueue.c_str());

        std::string dataString;
        int result(0);
        std::string user;

        protocol_.clearValues();


        result = checkTqName(transferQueue);
        if (result != AES_NOERRORCODE)
        {
                return(result);
        }

        protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


        AES_AFP_TRACE_MESSAGE("Send to server");

        protocol_.getString(dataString);
        result = sendToServer(0, COMMAND, INFOALLTRANSFERQUEUESSHORT, 1, dataString);

        if (result != AES_NOERRORCODE)
        {
                return result;
        }

        AES_AFP_TRACE_MESSAGE("return code from sendToServer is %d",result );
        AES_AFP_TRACE_MESSAGE("Waiting for server");

        result = recvFromServer(outStream);

        AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

        return result;
        //## end aes_afp_apiinternal::listAllFiles%387C2C040147.body
}


//## Operation: listDestinationSets%387C2C04015B; C++
//## Semantics:
//	---------------------------------------------------------
//	       listDests()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::listDestinationSets (std::string& transferQueue, std::stringstream& outStream)
{
	//## begin aes_afp_apiinternal::listDestinationSets%387C2C04015B.body preserve=yes
	AES_AFP_TRACE_MESSAGE("TQ = %s", transferQueue.c_str());

	std::string dataString;
	int result(0);
	std::string user;

	protocol_.clearValues();
	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, INFODESTINATIONSET, 1, dataString);

	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(outStream);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	if (result == 0)
	{
	}

	return result;
	//## end aes_afp_apiinternal::listDestinationSets%387C2C04015B.body
}

//## Operation: removeTransferQueue%387C2C040179; C++
//## Semantics:
//	---------------------------------------------------------
//	       removeFile()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::removeTransferQueue (std::string& transferQueue)
{
	//## begin aes_afp_apiinternal::removeTransferQueue%387C2C040179.body preserve=yes
	AES_AFP_TRACE_MESSAGE("TQ = %s", transferQueue.c_str());

	std::string dataString;
	int result(0);
	std::string user;

	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);

	return result;
	//## end aes_afp_apiinternal::removeTransferQueue%387C2C040179.body
}

//## Operation: stopFiles%387C2C04018E; C++
//## Semantics:
//	---------------------------------------------------------
//	       stopFiles()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::stopFiles (std::string& transferQueue, std::string& destinationSet)
{
	//## begin aes_afp_apiinternal::stopFiles%387C2C04018E.body preserve=yes
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s", transferQueue.c_str(), destinationSet.c_str());

	std::string dataString;
	int result(0);
	std::string user;

	protocol_.clearValues();
	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, CDHSTOPSENDALL, 1, dataString);

	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer();

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	return result;
	//## end aes_afp_apiinternal::stopFiles%387C2C04018E.body
}

//	---------------------------------------------------------
//	       stopFile()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::stopFile (std::string& transferQueue, std::string& destinationSet, std::string& filename)
{
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s File to stop %s",
			transferQueue.c_str(), destinationSet.c_str(), filename.c_str());

	std::string dataString;
	int result(0);
	std::string user;


	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	result = checkFilename(filename);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("FILENAME", filename);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	return result;
	//## end aes_afp_apiinternal::stopFile%3B8F437200EB.body
}

//	---------------------------------------------------------
//	       getAttr()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::getAttributes (std::string& transferQueue, std::string& destinationSet, std::stringstream& outStream)
{
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s", transferQueue.c_str(), destinationSet.c_str());

	int result(0);
	std::string dataString;
	std::string user;

	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, GETTRANSFERQUEUEATTRIBUTES, 1, dataString);

	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(outStream);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	if (result == 0)
	{
	}

	return result;
}

//	---------------------------------------------------------
//	       setAttr()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::setAttributes (std::string& transferQueue, std::string& destinationSet, AES_GCC_Filestates defaultState, int removeDelay, bool removeBefore, int nrOfRetries, int retryTime)
{
	AES_AFP_TRACE_MESSAGE("TQ = %s, Destination set = %s State = %d Remove delay = %d Remove before = %s Retries = %d Retry time = %d",
			transferQueue.c_str(), destinationSet.c_str(), defaultState, removeDelay, removeBefore?"true":"false", nrOfRetries, retryTime);

	std::string dataString;
	int result(0);
	std::string user;

	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	result = checkDefaultStatus(defaultState, destinationSet);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addIntValue("STATUS", defaultState);

	result = checkRemoveDelay(removeDelay);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addIntValue("REMOVEDELAY", removeDelay);

	protocol_.addBoolValue("REMOVEBEFORE", removeBefore);

	result = checkNrOfRetries(nrOfRetries);
	if (result != 0)
	{
		return(result);
	}

	protocol_.addIntValue("SENDRETRIES", nrOfRetries);

	result = checkRetryTime(retryTime);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addIntValue("SENDRETRIESDELAY", retryTime);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, SETTRANSFERQUEUEATTRIBUTES, 1, dataString);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer();

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	return result;
}

//	---------------------------------------------------------
//	       createDestination()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::addDestinationSet (std::string& transferQueue, std::string& destinationSet, AES_GCC_Filestates defaultState, int removeDelay, bool removeBefore, int nrOfRetries, int retryTime)
{
	(void)(removeBefore);
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s State = %d Remove delay = %d Remove before = %s Retries = %d Retry delay = %d",
			transferQueue.c_str(), destinationSet.c_str(), defaultState, removeDelay, removeBefore?"true":"false", nrOfRetries, retryTime);

	std::string dataString;
	int result(0);
	std::string user;

	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	result = checkDefaultStatus(defaultState, destinationSet);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addIntValue("STATUS", defaultState);

	result = checkRemoveDelay(removeDelay);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addIntValue("REMOVEDELAY", removeDelay);

	result = checkNrOfRetries(nrOfRetries);
	if (result != 0)
	{
		return(result);
	}

	protocol_.addIntValue("SENDRETRIES", nrOfRetries);

	result = checkRetryTime(retryTime);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addIntValue("SENDRETRIESDELAY", retryTime);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, ADDDESTINATIONSET, 1, dataString);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer();

	AES_AFP_TRACE_MESSAGE("Rceived from server, result %d", result);

	return result;
}

//	---------------------------------------------------------
//	       removeDestination()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::removeDestinationSet (std::string& transferQueue, std::string& destinationSet)
{
	AES_AFP_TRACE_MESSAGE("TQ = %s, destinationSet = %s", transferQueue.c_str(), destinationSet.c_str());

	std::string dataString;
	int result(0);
	std::string user;

	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	return result;
	//## end aes_afp_apiinternal::removeDestinationSet%387C2C0401DE.body
}

//	---------------------------------------------------------
//	       getErrCodeText()
//	---------------------------------------------------------
const char * aes_afp_apiinternal::getErrorCodeText (unsigned int returnCode)
{
	AES_AFP_TRACE_MESSAGE("Get text for error code %d", returnCode);

	switch(returnCode)
	{
	case AES_NOERRORCODE:					return "";
	case AES_EXECUTEERROR:				return "Error when executing";
	case AES_INCORRECTCOMMAND:		return "Incorrect usage";
	case AES_PROCORDEREXIST:			return "FileTransferQueue already defined";
	case AES_NOPROCORDER:					return "Transfer queue not defined";
	case AES_FILENOTFOUND:				return "File or directory not found";
	case AES_FILENAMEINVALID:			return "Invalid transfer queue name";
	case AES_FILE_NAME_INVALID:
		AES_AFP_TRACE_MESSAGE("AES_FILE_NAME_INVALID:");
		return "Invalid transfer queue name";
	case AES_NOTCOMPOSITE:				return "File not composite";
	case AES_SENDITEMEXIST:				return "File or directory already reported";
	case AES_SENDITEMNOTREP:			return "File or directory not reported";
	case AES_NOSUCHITEM:					return "File or directory not found";
	case AES_SENDITEMNAMEINVAL:		return "Invalid file or directory name";
	case AES_DESTINATIONEXIST:		return "Destination set already exist";
	case AES_NODESTINATION:				return "Destination set does not exist";
	case AES_INVALIDDESTNAME:			return "Invalid destination set name";
	case AES_NOACCESS:						return "Internal program error: Access error";
	case AES_NOSERVERACCESS:			return "Internal program error: Unable to connect to server";
	case AES_CATASTROPHIC:				return "Internal program error: Program error has occurred";
	case AES_ILLEGALDELAYVALUE:		return "Illegal remove delay value";
	case AES_ILLEGALRETRYVALUE:		return "Illegal retry value";
	case AES_WRONGFILETYPE:				return "Wrong file type";
	case AES_ILLEGALTIMEVALUE:		return "Illegal retry interval value";
	case AES_ILLEGALSTATUSVALUE:	return "Invalid status value";
	case AES_ILLEGALDEFSTATUS:      return "status attribute of advancedFileTransferQueueInfo object should be set to DELETE when empty destination set has to be associated to";
	case AES_ERRORCHANGESTATUS:		return "Illegal status change";
	case AES_SUBFILENOTFOUND:			return "Subfile not found";
	case AES_NAMETAGINVALID:			return "Invalid nameTag";
	case AES_WRONGSTARTUPSEQUENCENUMBER:		return "Different startup sequence number for the same transfer queue name";
	case AES_TIMERNOTEXPERIED:		return "Timer not experied";
	case AES_ILLEGALSTARTUPSEQUENCENUMBER: return "Illegal combination of attributes"; //HT50930
	case AES_UNABLETOSTOPFILE:		return "Unable to end file or directory transfer";
	case AES_NOTMANUALLY:					return "File or directory is not manually initiated";
	case AES_FILEISPROTECTED:			return "FileTransferQueue is protected by";
	case AES_TQISOPEN:					return "Transfer in progress"; //HE74701
	case AES_TEMPLATEFAULT:				return "Template is invalid";
	case AES_NOTDIRECTORY:				return "Invalid directory name";
	case AES_APPLDIRNOTFOUND:			return "Directory not found";
	case AES_TOOMANYTQ:						return "Too many transfer queues defined to the same destination set name";
	case AES_EXCPROTDESTNAME:			return "Too many destination set names defined to transfer queues in Data Transfer function";
	case AES_EXCPROTDESTUSER:			return "Too many protections of the destination set name";
	case AES_MAXTQ:								return "Too many transfer queues defined in Data Transfer function";
	case AES_TQDESTSETCOMBINATION:return "The combination transfer queue and destination set does not exist";
	case AES_WRONGRENAMETYPE:			return "Different rename types for the same transfer queue name";
	case AES_ILLEGALRENAMETYPE:		return "Illegal rename identifier";
	case AES_INCONSISTENTDIR:			return "Different directories for the same transfer queue name";
	case AES_ILLEGALREMBEFORE:		return "Invalid remove before value";
	case AES_WRONGUSERGROUP:			return "Different user group for the same transfer queue name";
	case AES_WRONGTEMPLATE:				return "Different template for the same transfer queue name";
	case AES_WRONGNAMETAG:				return "Different name tag for the same transfer queue name";
	case AES_NOTALLOWEDVOLUME:		return "Not an allowed volume for transfer";
	case AES_USERNOTDEFINED:			return "User not member of specified user group";
	case AES_NOTAUTHORIZED:				return "User not authorized to perform specified action";
	case AES_NOTFILEDEST:					return "Destination set is not configured for file transfer";
	case AES_MULTIDESTSET:				return "If more than one destination set needs to be associated/deassociated then the attribute initiatingDesitnationSet/respondingDestinationSet needs to be valued separately ";
	case AES_REPLACEDESTSET:    		return "Destination set of a Transfer queue cannot be replaced";
	case AES_MAXDESTSETLIMIT:			return "More than three destination sets cannot be attached to a transfer queue";
	case AES_IEMPTYDESTSET:                  return "initiatingDestinationSet(-) cannot be added/replaced/deleted";
	case AES_REMPTYDESTSET:                  return "respondingDestinationSet(-) cannot be added/replaced/deleted";
	case AES_ILLITQEMPTYDESTSET:             return "Transfer queue is configured for auto deletion, destination set cannot be added to it";
	case AES_ILLRTQEMPTYDESTSET:             return "Transfer queue is configured for auto deletion, destination set cannot be added to it";
	case AES_ILLMODTQATTRS:   	return "Modification of transfer queue attributes not allowed";
	case AES_BACKUP_INPROGRESS:      return "Command not executed, AP backup in progress";
	case AES_ERRORUNKNOWN:	
		AES_AFP_TRACE_MESSAGE("AES_ERRORUNKNOWN:");
		return "Unknown error";
	case AES_INCUSAGE:                  return "Template is invalid, Blade ID element not allowed in this system configuration"; // TR HO99988
	case AES_INVALIDFILETQ:			return "Invalid FileTransferQueue";
	case AES_UNREAS_IDESTSETNAME:			return "Invalid initiatingDestinationSet";
	case AES_UNREAS_RDESTSETNAME:			return "Invalid respondingDestinationSet";
	case AES_NO_DESTSETNAME:			return "Either initiatingDestinationSet or respondingDestinationSet is mandatory";
	case AES_NAMETAG1MANDATORY:			return "nameTag1 is mandatory for nameTag2";
	case AES_IDESTSETALREADYATT:			return "initiatingDestinationSet is already attached to the transfer queue";
	case AES_RDESTSETALREADYATT:			return "respondingDestinationSet is already attached to the transfer queue";
	case AES_ILLEGALREMIDESTSET:			return "Removal of initiatingDestinationSet denied. Either initiatingDestinationSet or respondingDestinationSet is mandatory";
	case AES_ILLEGALREMRDESTSET:			return "Removal of respondingDestinationSet denied. Either initiatingDestinationSet or respondingDestinationSet is mandatory";
	case AES_NOIDESTINATION:				return "initiatingDestinationSet does not exist";
	case AES_NORDESTINATION:				return "respondingDestinationSet does not exist";
	case AES_FILETQREFERRED:				return "FileTransferQueue is protected";	
	case AES_NORESPDESTSETEXIST:			return "Incorrect Operation. No respondingDestinationSet is associated";
	case AES_NOINITDESTSETEXIST:			return "Incorrect Operation. No initiatingDestinationSet is associated";
	case AES_STSTQREFERRED:				return "FileTransferQueue is used by Measurement program, it cannot be deleted";	
	case AES_CPFTQREFERRED:				return "FileTransferQueue is used by CP-file, it cannot be deleted";	
	case AES_PDSTQREFERRED:				return "FileTransferQueue is used by PDS-file, it cannot be deleted";	
	case AES_ALOGTQREFERRED:			return "FileTransferQueue is used by ALOG-file, it cannot be deleted";
	case AES_TQNAMEINVALID:                         return "Invalid transfer queue name";
	case AES_COMMITERROR:                           return "Creation or Modification of multiple Transfer queues is not allowed in a single transaction commit"; 	
	default:											return "Unknown error";
	}
}

//	---------------------------------------------------------
//	       lockFile()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::lockTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string& application)
{
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s, application = %s",
			transferQueue.c_str(), destinationSet.c_str(), application.c_str());

	std::string dataString;
	int result(0);
	std::string user;

	toUpper(application);

	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addStringValue("DESTINATIONSET",destinationSet);

	protocol_.addStringValue("APPLICATION", application);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, LOCKTRANSFERQUEUE, 1, dataString);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer();

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	return result;
}

//	---------------------------------------------------------
//	       unlockTransferQueue()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::unlockTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string& application)
{
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s, application = %s",
			transferQueue.c_str(), destinationSet.c_str(), application.c_str());

	std::string dataString;
	int result(0);
	std::string user;

	toUpper(application);

	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}

	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	protocol_.addStringValue("APPLICATION", application);

	AES_AFP_TRACE_MESSAGE("Sending to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, UNLOCKTRANSFERQUEUE, 1, dataString);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer();

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	return result;
}

//	---------------------------------------------------------
//	       isTransferQueueLocked()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::isTransferQueueLocked (std::string& transferQueue, std::string& destinationSet, bool& locked, std::string& application)
{
	(void)(application);
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s, application = %s",
			transferQueue.c_str(), destinationSet.c_str(), application.c_str());

	std::stringstream strStream;
	int result(0);
	char tmpStr[512];
	std::string dataString;
	std::string tmpString;
	std::string user;

	protocol_.clearValues();
	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	AES_AFP_TRACE_MESSAGE("Sending to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, ISTRANSFERQUEUELOCKED, 1, dataString);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(strStream);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	if (result == 0)
	{
	}

	strStream.getline(tmpStr, 512, '\n');

	tmpString = tmpStr;
	if (tmpString == "1")
	{
		locked = true;
	}
	else
	{
		locked = false;
	}

	AES_AFP_TRACE_MESSAGE("TQ %s with destinationSet %s, locked = %s",
			transferQueue.c_str(), destinationSet.c_str(), locked?"true":"false");

	strStream.getline(tmpStr, 512, '\0');
	user = tmpStr;


	return result;
}

//	---------------------------------------------------------
//	       resendOneFailed()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::resendOneFailed (std::string& transferQueue, std::string& destinationSet, std::string& newDestinationSet)
{
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s, new destination set = %s",
			transferQueue.c_str(), destinationSet.c_str(), newDestinationSet.c_str());

	int result(0);
	std::string dataString;
	std::string user;


	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	result = checkDestinationSetName(newDestinationSet);
	if (result != 0)
	{
		return(result);
	}
	protocol_.addStringValue("NEWDESTINATIONSET", newDestinationSet);

	AES_AFP_TRACE_MESSAGE("Sending to server");

	protocol_.getString(dataString);
	return result;
	//## end aes_afp_apiinternal::resendOneFailed%3A6EA4370054.body
}

//	---------------------------------------------------------
//	       resendAllFailed()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::resendAllFailed (std::string& transferQueue)
{
	AES_AFP_TRACE_MESSAGE("TQ = %s", transferQueue.c_str());

	int result(0);
	std::string dataString;
	std::string user;


	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	return result;
}

//	---------------------------------------------------------
//	    checkTqName()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::checkTqName (std::string& transferQueue)
{
	AES_AFP_TRACE_MESSAGE("Checking TQ %s", transferQueue.c_str());

	if (transferQueue.length() > (ACE_UINT32)MAX_TRANSFERQUEUE_LEN_) 
	{
		AES_AFP_TRACE_MESSAGE("TQ name exceeds limit (%d chars), name length is %d",
				(int)MAX_TRANSFERQUEUE_LEN_, (int)transferQueue.size() );

		//return AES_FILENAMEINVALID;
		return AES_TQNAMEINVALID;
	}

	size_t index(0);

	index = transferQueue.find_first_not_of(ALLOWEDCHARACTERS_);
	if ( string::npos != index)
	{
		AES_AFP_TRACE_MESSAGE( "Invalid TQ name, incorrect character = %c", transferQueue[index] );
		//return AES_FILENAMEINVALID;
		return AES_TQNAMEINVALID;
	}
	const string legalChars2("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

	if(transferQueue.substr(0,1).find_first_not_of(legalChars2) != string::npos)
	{
		AES_AFP_TRACE_MESSAGE("Invalid TQ name, incorrect character = %c", transferQueue[index] );
		//return AES_FILENAMEINVALID;
		return AES_TQNAMEINVALID;
	}

	return AES_NOERRORCODE;
}

//	---------------------------------------------------------
//	    checkDestinationSetName()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::checkDestinationSetName (std::string& destinationSet)
{
	AES_AFP_TRACE_MESSAGE("Checking destination %s", destinationSet.c_str());

	if (destinationSet.length() > (ACE_UINT32)MAX_DESTINATIONSET_LEN_) 
	{
		AES_AFP_TRACE_MESSAGE("Destinationset name exeeds limit (%d chars), name length is %d",
				(int)MAX_DESTINATIONSET_LEN_, (int)destinationSet.size() );
		return AES_INVALIDDESTNAME;
	}

	int index(0);

	index = destinationSet.find_first_not_of(ALLOWEDCHARACTERS_);
	if (index != -1)
	{
		AES_AFP_TRACE_MESSAGE("Invalid destinationset name, incorrect character = %c",
				destinationSet[index] );
		return AES_INVALIDDESTNAME;
	}

	return AES_NOERRORCODE;
}

//	---------------------------------------------------------
//	    checkFilename()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::checkFilename (std::string& filename)
{
	AES_AFP_TRACE_MESSAGE("Checking filename %s", filename.c_str());

	if (filename.length() > (ACE_UINT32)MAX_FILENAME_LEN_) 
	{
		AES_AFP_TRACE_MESSAGE("Filename exeeds limit (%d chars), name length is %d",
				(int)MAX_FILENAME_LEN_, (int)filename.size() );
		return AES_SENDITEMNAMEINVAL;
	}

	int index(0);

	index = filename.find_first_of(NOTALLOWEDCHARACTERS_);
	if (index != -1)
	{
		AES_AFP_TRACE_MESSAGE("Invalid filename, incorrect character = %c", filename[index] );
		return AES_SENDITEMNAMEINVAL;
	}

	return AES_NOERRORCODE;
}

//	---------------------------------------------------------
//	       checkSourceDirectoryName()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::checkSourceDirectoryName (std::string& sourceDirectory)
{
	AES_AFP_TRACE_MESSAGE("Checking source directory %s", sourceDirectory.c_str());

	std::string tmpNotAllowed("<>\"\\|");

	if (sourceDirectory.length() > (ACE_UINT32)MAX_SOURCEDIRECTORY_LEN_) 
	{
		AES_AFP_TRACE_MESSAGE("Source directory name exeeds limit (%d chars), name length is %d",
				(int)MAX_SOURCEDIRECTORY_LEN_, (int)sourceDirectory.size() );
		return AES_NOTDIRECTORY;
	}

	int index(0);

	index = sourceDirectory.find_first_of(tmpNotAllowed);
	if (index != -1)
	{
		AES_AFP_TRACE_MESSAGE("Invalid source directory name, incorrect character = %c",
				sourceDirectory[index] );
		return AES_NOTDIRECTORY;
	}

	return AES_NOERRORCODE;
}

//	---------------------------------------------------------
//	    checkDefaultStatus()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::checkDefaultStatus (unsigned int defaultState, std::string& destinationSet)
{
	AES_AFP_TRACE_MESSAGE("Status to check = %d, destination set = %s",
			defaultState, destinationSet.c_str());

	if (destinationSet == "-" && defaultState != AES_FSDELETE)
	{
		AES_AFP_TRACE_MESSAGE("Incorrect status for destination set %s",
				destinationSet.c_str());
		return AES_ILLEGALSTATUSVALUE;
	}

	switch(defaultState)
	{
	case AES_FSREADY:
	{
		return AES_NOERRORCODE;
	}
	case AES_FSSEND:
	{
		return AES_NOERRORCODE;
	}
	case AES_FSARCHIVE:
	{
		return AES_NOERRORCODE;
	}
	case AES_FSDELETE:
	{
		return AES_NOERRORCODE;
	}
	case AES_FSFAILED:
	{
		return AES_NOERRORCODE;
	}
	case AES_FSUSEDEFAULT:
	{
		return AES_NOERRORCODE;
	}
	case AES_FSREMOVE:
	{
		return AES_NOERRORCODE;
	}
	case AES_FSSTOPPED:
	{
		return AES_NOERRORCODE;
	}
	case AES_FSPENDING:
	{
		return AES_NOERRORCODE;
	}
	case AES_FSNONE:
	{
		return AES_NOERRORCODE;
	}
	default:
	{
		AES_AFP_TRACE_MESSAGE("Illegal status value for destination set %s", destinationSet.c_str());
		return AES_ILLEGALSTATUSVALUE;
	}
	}
}

//	---------------------------------------------------------
//	    checkRemoveDelay()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::checkRemoveDelay (int& removeDelay)
{
	AES_AFP_TRACE_MESSAGE("Check remove delay value %d", removeDelay);

	if ((MIN_REMOVEDELAY_ > removeDelay) || (removeDelay > MAX_REMOVEDELAY_))
	{
		AES_AFP_TRACE_MESSAGE("Illegal delay value %d", removeDelay);
		return AES_ILLEGALDELAYVALUE;
	}

	return AES_NOERRORCODE;
}

//	---------------------------------------------------------
//	    checkNrOfRetries()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::checkNrOfRetries (int& nrOfRetries)
{
	AES_AFP_TRACE_MESSAGE("Check number of retries value %d", nrOfRetries);

	if ((MIN_NROFRETRIES_ > nrOfRetries) || (nrOfRetries > MAX_NROFRETRIES_))
	{
		AES_AFP_TRACE_MESSAGE("Illegal retry value %d", nrOfRetries);
		return AES_ILLEGALRETRYVALUE;
	}
	else
	{
		return AES_NOERRORCODE;
	}
}

//	---------------------------------------------------------
//	    checkRetryTime()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::checkRetryTime (int& retryTime)
{
	AES_AFP_TRACE_MESSAGE("Check retry time %d", retryTime);
	if ((MIN_RETRYTIME_ > retryTime) || (retryTime > MAX_RETRYTIME_))   
	{
		AES_AFP_TRACE_MESSAGE("Illegal retry time %d", retryTime);
		return AES_ILLEGALTIMEVALUE;
	}
	else
	{
		return AES_NOERRORCODE;
	}
}

//	---------------------------------------------------------
//	    checkRenameType()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::checkRenameType (AES_AFP_Renametypes type)
{
	AES_AFP_TRACE_MESSAGE("Check rename type %d", type);
	switch (type)
	{
	case AES_LOCAL:
	{
		return AES_NOERRORCODE;
	}
	case AES_REMOTE:
	{
		return AES_NOERRORCODE;
	}
	case AES_NONE:
	{
		return AES_NOERRORCODE;
	}
	default:
	{
		AES_AFP_TRACE_MESSAGE("Illegal rename type %d", type);
		return AES_ILLEGALRENAMETYPE;
	}
	}
}

//	---------------------------------------------------------
//	    checkTransferMode()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::checkTransferMode (AES_GCC_Format mode)
{
	AES_AFP_TRACE_MESSAGE("Check transfer mode %d", mode);
	switch (mode)
	{
	case AES_BINARY:
	{
		return AES_NOERRORCODE;
	}
	case AES_ASCII:
	{
		return AES_NOERRORCODE;
	}
	case AES_DEFAULT:
	{
		return AES_NOERRORCODE;
	}
	default:
	{
		AES_AFP_TRACE_MESSAGE("Illegal transfer mode %d", mode);
		return AES_INVALIDTRANSMODE;
	}
	}
}


//	---------------------------------------------------------
//	       checkNameTag()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::checkNameTag (std::string& nameTag)
{
	AES_AFP_TRACE_MESSAGE("Checking name tag %s", nameTag.c_str());
	int index(0);
	int i = 0;

	// uabmha: CNI 1135, updated with check for more than one asterisk etc.
	index = nameTag.find_first_of(NOTALLOWEDCHARACTERS_);
	if (index != -1)
	{
		AES_AFP_TRACE_MESSAGE("Invalid name tag, incorrect character = %c", nameTag[index] );
		return AES_NAMETAGINVALID;
	}
	if (nameTag.length() > (ACE_UINT32)MAX_NAMETAG_LEN_)
        {

                return AES_NAMETAGINVALID;
        }

	if ((nameTag[0] == '*') || (nameTag[(nameTag.size()-1)] == '*'))
	{
		// Asterisk first or last not allowed, no real multinametag then...
		// Should be safe to use [] operator as above because nameTag is
		// checked for min. length already in command parsing...

		return AES_NAMETAGINVALID;

	}
	// Count to see if more than one asterisk is specified in name tag...
	for (std::string::iterator p = nameTag.begin(); p != nameTag.end(); ++p)
	{
		if (*p == '*')
		{
			i++;
		}
	}
	if (i > 1)
	{
		AES_AFP_TRACE_MESSAGE("Invalid name tag, more than one asterisk");
		return AES_NAMETAGINVALID;
	}

	return AES_NOERRORCODE;
}

//	---------------------------------------------------------
//	       checkStartupSequenceNumber()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::checkStartupSequenceNumber (long startupSequenceNumber,std::string& fileTemplate)
{
	AES_AFP_TRACE_MESSAGE("Checking startup sequence number %ld FileTemplate %s", startupSequenceNumber, fileTemplate.c_str());

	if ( 0 != startupSequenceNumber)
	{  // If startupSequenceNumber is not a default value ...
		/* For HL79347: Check the number of occurence of both p and n. With the help of count
	check for StartupSequenceNumber for differnt range of 1/4/6/8.
		 */
		int pCount = 0;
		int nCount = 0;
		int count = 0;

		for (int index = 0;index < (int)fileTemplate.length(); index++)
		{
			if (fileTemplate[index] == 'p')
			{
				pCount++;
			}
			if (fileTemplate[index] == 'n')
			{
				nCount++;
			}
		}

		if ((pCount > 0 ) && (nCount > 0 ))
		{
			return AES_TEMPLATEFAULT;
		}

		count = (nCount > 0) ? nCount : pCount;

		switch (count)
		{
		case 1:
		case 4:
		{
			if ((startupSequenceNumber < 0) || (startupSequenceNumber > 9999))
			{
				AES_AFP_TRACE_MESSAGE("Illegal startup sequence number = %ld", startupSequenceNumber );
				return AES_ILLEGALSTARTUPSEQUENCENUMBER;
			}
			break;
		}
		case 6:
		{
			if ((startupSequenceNumber < 0) || (startupSequenceNumber > 999999))
			{
				AES_AFP_TRACE_MESSAGE("Illegal startup sequence number = %ld", startupSequenceNumber );
				return AES_ILLEGALSTARTUPSEQUENCENUMBER;
			}
			break;
		}
		case 8:
		{
			if ((startupSequenceNumber < 0) || (startupSequenceNumber > 99999999))
			{
				AES_AFP_TRACE_MESSAGE("Illegal startup sequence number = %ld", startupSequenceNumber );
				return AES_ILLEGALSTARTUPSEQUENCENUMBER;
			}
			break;
		}
		default:
		{
			AES_AFP_TRACE_MESSAGE("sequence number failed");
			return AES_TEMPLATEFAULT;
		}
		}
	}
	return AES_NOERRORCODE;
}

//	---------------------------------------------------------
//	       open()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::open (std::string subsystem, std::string applicationBlock)
{
	AES_AFP_TRACE_MESSAGE("Subsystem %s, application = %s", subsystem.c_str(), applicationBlock.c_str());

	int result = -1;
	std::string dataString("");
	std::string tmpErrString("");
	std::string tmpSubBlock("");
	std::string user("");

	applicationSts = applicationBlock;

	toUpper(subsystem);

	toUpper(applicationBlock);

	protocol_.clearValues();

	convService_ = new ACS_DSD_Client;
	convSession_ = new ACS_DSD_Session;

	ACS_DSD_Node node;

	convService_->get_local_node(node);

	result = convService_->connect(*convSession_, "AES_AFP_server", "AES", node.system_id, node.node_state);
	if (result  <  0 )
	{

		AES_AFP_TRACE_MESSAGE("Unable to connect to DSD. Error Code: %d. Error Description: %s", convService_->last_error(), convService_->last_error_text());

		tmpErrString = getErrorCodeText(AES_NOSERVERACCESS);
		putOnEventQueue(tmpErrString, AES_EVELOSTSERVER);
		apiTask_ = 0;
		delete convService_;
		convService_ = 0;
		delete convSession_;
		convSession_ = 0;
		return AES_NOSERVERACCESS;
	}

	AES_AFP_TRACE_MESSAGE("Connected to AES_AFP_Server");

	if (apiPtr_ != 0)
	{
		apiTask_ = new aes_afp_apitask(convSession_, this, apiPtr_);
	}
	else
	{
		AES_AFP_TRACE_MESSAGE("apiPtr_ is NULL, return AES_NOSERVERACCESS");
		return AES_NOSERVERACCESS;
	}


        if ( apiTask_->open() != 0 ) //for sts hang
        {
            AES_AFP_TRACE_MESSAGE("open(2) Unable to spawn apitask thread");
            convSession_->close();

            tmpErrString = getErrorCodeText(AES_NOSERVERACCESS);
            putOnEventQueue(tmpErrString, AES_EVELOSTSERVER);
            delete convService_;
            convService_ = 0;
            delete convSession_;
            convSession_ = 0;
            delete apiTask_;
            apiTask_ = 0;
            return AES_NOSERVERACCESS;
        }



	tmpSubBlock = subsystem;
	tmpSubBlock += "_" + applicationBlock;

	protocol_.addStringValue("APPLICATION", tmpSubBlock);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, OPENGENERIC, 1, dataString);
	if (result != AES_NOERRORCODE)
	{
		close();
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer();

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	if (result != AES_NOERRORCODE)
	{
		AES_AFP_TRACE_MESSAGE("Open failed, closing");
		close();
	}

	return result;
}

//	---------------------------------------------------------
//	       open()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::open (std::string& transferQueue, int applicationBlock)
{
	AES_AFP_TRACE_MESSAGE("TQ = %s", transferQueue.c_str());

	int result = 0;
	std::string errEvent;
	std::string dataString;
        std::string tmpErrString;
	std::string user;
        if (applicationBlock == APPLICATION_TYPE)
        {
	  AES_AFP_TRACE_MESSAGE("applicationBlock = %d", applicationBlock);
          {
             char tempBuff[200];
             sprintf(tempBuff,"open(1) applicationBlock = %d",applicationBlock);
          }

          applicationSts = STS_APPLICATION;
	}

	protocol_.clearValues();

	convService_ = new ACS_DSD_Client;
	convSession_ = new ACS_DSD_Session;

	ACS_DSD_Node node;
	convService_->get_local_node(node);

	result = convService_->connect(*convSession_, "AES_AFP_server", "AES", node.system_id, node.node_state);
	if (result < 0 )
	{
		AES_AFP_TRACE_MESSAGE("Unable to connect to DSD");
		errEvent = getErrorCodeText(AES_NOSERVERACCESS);
		putOnEventQueue(errEvent, AES_EVELOSTSERVER);

		delete convService_;
		convService_ = 0;
		delete convSession_;
		convSession_ = 0;

		return AES_NOSERVERACCESS;
	}

	AES_AFP_TRACE_MESSAGE("Connected to DSD");
	if (apiPtr_ != 0)
	{
		apiTask_ = new aes_afp_apitask(convSession_, this, apiPtr_);
	}
	else
	{
		return AES_NOSERVERACCESS;
	}

        if ( apiTask_->open() != 0 ) //for sts hang
        {
            AES_AFP_TRACE_MESSAGE("Unable to spawn apitask thread");
            convSession_->close();

            tmpErrString = getErrorCodeText(AES_NOSERVERACCESS);
            putOnEventQueue(tmpErrString, AES_EVELOSTSERVER);
            delete convService_;
            convService_ = 0;
            delete convSession_;
            convSession_ = 0;
            delete apiTask_;
            apiTask_ = 0;
            return AES_NOSERVERACCESS;
        }


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		close();
		return result;
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);

	AES_AFP_TRACE_MESSAGE("Send to server OPENTRANSFERQUEUE command (TRANSFERQUEUE=%s, USERNAME=%s)", transferQueue.c_str(), user.c_str());
	protocol_.getString(dataString);

	result = sendToServer(0, COMMAND, OPENTRANSFERQUEUE, 1, dataString);
	if (result != AES_NOERRORCODE)
	{
		close();
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");
	result = recvFromServer();

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	if (result != AES_NOERRORCODE)
	{
		AES_AFP_TRACE_MESSAGE("Failed to open, closing");
		close();
	}

	return result;
}

//	---------------------------------------------------------
//	       close()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::close()
{
	AES_AFP_TRACE_MESSAGE("close()");
	if (apiTask_ == 0)
	{
		AES_AFP_TRACE_MESSAGE("apiTask_ AES_NOERRORCODE");
		return AES_NOERRORCODE;
	}

	int result(0);
	std::string user;
	std::string dataString;

	protocol_.clearValues();

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, CLOSETRANSFERQUEUE, 1, dataString);
	if (result == AES_NOERRORCODE)
	{
		AES_AFP_TRACE_MESSAGE("Waiting for server");
		result = recvFromServer();
		AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);
	}

	int isApiTask = pthread_kill(apiTask_->getThreadId(), 0);
	if (isApiTask != ESRCH)
		apiTask_->endSvcLoop();
	AES_AFP_TRACE_MESSAGE("After endSvcLoop, waiting for ApiTask to join");
	pthread_join( apiTask_->getThreadId(), NULL);
	convSession_->close();

	AES_AFP_TRACE_MESSAGE("Waiting for api task to finish");

	if (isApiTask != ESRCH)
	{
		ACE_event_t HThreadApiTask;
		apiTask_->getHandle(HThreadApiTask);

		ACE_OS::event_wait(&HThreadApiTask);
	}

	AES_AFP_TRACE_MESSAGE("Api task finished");
	delete apiTask_;
	apiTask_ = 0;

	delete convSession_;
	convSession_ = 0;
	delete convService_;
	convService_ = 0;

	AES_AFP_TRACE_MESSAGE("Connection closed, returning result = %d", result);
	return result;
}

//	---------------------------------------------------------
//	       createTransferQueue()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::createTransferQueue (std::string& transferQueue,
		std::string& destinationSet,
		std::string& sourceDirectory,
		AES_GCC_Filestates defaultState,
		int removeDelay,
		bool removeBefore,
		int retryTimeInterval,
		int retryTimes,
		AES_AFP_Renametypes rename,
		std::string& fileTemplate,
		std::string& userGroup)
{
	(void)(userGroup);
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s Source directory = %s", transferQueue.c_str(), destinationSet.c_str(), sourceDirectory.c_str());
	AES_AFP_TRACE_MESSAGE("State %d Remove delay %d Remove before %s Retry time %d Retries %d Rename type %d File template %s User group %s", defaultState, removeDelay, removeBefore?"true":"false", retryTimeInterval, retryTimes, rename, fileTemplate.c_str(), userGroup.c_str());

	std::string dataString;
	int result(0);
	std::string user;


	toUpper(sourceDirectory);

	protocol_.clearValues();

	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	result = checkSourceDirectoryName(sourceDirectory);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("SOURCEDIRECTORY", sourceDirectory);


	result = checkDefaultStatus(defaultState, destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addIntValue("STATUS", defaultState);

	result = checkRemoveDelay(removeDelay);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addIntValue("REMOVEDELAY", removeDelay);

	protocol_.addBoolValue("REMOVEBEFORE", removeBefore);

	result = checkNrOfRetries(retryTimes);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addIntValue("SENDRETRIES", retryTimes);

	result = checkRetryTime(retryTimeInterval);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addIntValue("SENDRETRIESDELAY", retryTimeInterval);

	result = checkRenameType(rename);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addIntValue("RENAMETYPE", rename);

	protocol_.addStringValue("RENAMETEMPLATE", fileTemplate);

	return result;
}

//	---------------------------------------------------------
//	       createTransferQueue()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::createTransferQueue (std::string& transferQueue,
		std::string& destinationSet,
		std::string& sourceDirectory,
		AES_GCC_Filestates defaultState,
		int removeDelay,
		bool removeBefore,
		int retryTimeInterval,
		int retryTimes,
		AES_AFP_Renametypes rename,
		std::string& fileTemplate,
		std::string& userGroup,
		std::string& nameTag)
{
	(void)(userGroup);
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s Source directory = %s", transferQueue.c_str(), destinationSet.c_str(), sourceDirectory.c_str());
	AES_AFP_TRACE_MESSAGE("State %d Remove delay %d Remove before %s Retry time %d Retries %d Rename type %d File template %s User group %s Name tag %s", defaultState, removeDelay, removeBefore?"true":"false", retryTimeInterval, retryTimes, rename, fileTemplate.c_str(), userGroup.c_str(), nameTag.c_str());

	std::string dataString("");
	int result(0);
	std::string user("");

	toUpper(sourceDirectory);

	protocol_.clearValues();

	AES_AFP_TRACE_MESSAGE("User name recived from GCC %s", user.c_str());

	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);

	result = checkDestinationSetName(destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	result = checkSourceDirectoryName(sourceDirectory);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("SOURCEDIRECTORY", sourceDirectory);

	result = checkDefaultStatus(defaultState, destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addIntValue("STATUS", defaultState);

	result = checkRemoveDelay(removeDelay);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addIntValue("REMOVEDELAY", removeDelay);

	protocol_.addBoolValue("REMOVEBEFORE", removeBefore);

	result = checkNrOfRetries(retryTimes);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addIntValue("SENDRETRIES", retryTimes);

	result = checkRetryTime(retryTimeInterval);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addIntValue("SENDRETRIESDELAY", retryTimeInterval);

	result = checkRenameType(rename);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addIntValue("RENAMETYPE", rename);

	protocol_.addStringValue("RENAMETEMPLATE", fileTemplate);

	result = checkNameTag(nameTag);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("NAMETAG", nameTag);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);

	return result;
}

//	---------------------------------------------------------
//	       createTransferQueue()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::createTransferQueue (std::string& transferQueue,
		std::string& destinationSet,
		std::string& sourceDirectory,
		AES_GCC_Filestates defaultState,
		int removeDelay,
		bool removeBefore,
		int retryTimeInterval,
		int retryTimes,
		AES_AFP_Renametypes rename,
		std::string& fileTemplate,
		std::string& userGroup,
		std::string& nameTag,
		long startupSequenceNumber,
		bool treatAsDefaultValue)
{
	(void)userGroup;
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s Source directory = %s", transferQueue.c_str(), destinationSet.c_str(), sourceDirectory.c_str());

	std::string dataString;
	int result(0);
	std::string user;

	toUpper(sourceDirectory);

	protocol_.clearValues();


	AES_AFP_TRACE_MESSAGE("User name received from GCC %s", user.c_str());

	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	result = checkSourceDirectoryName(sourceDirectory);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addStringValue("SOURCEDIRECTORY", sourceDirectory);


	result = checkDefaultStatus(defaultState, destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addIntValue("STATUS", defaultState);

	result = checkRemoveDelay(removeDelay);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addIntValue("REMOVEDELAY", removeDelay);

	protocol_.addBoolValue("REMOVEBEFORE", removeBefore);

	result = checkNrOfRetries(retryTimes);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addIntValue("SENDRETRIES", retryTimes);

	result = checkRetryTime(retryTimeInterval);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}
	protocol_.addIntValue("SENDRETRIESDELAY", retryTimeInterval);
	result = checkRenameType(rename);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addIntValue("RENAMETYPE", rename);

	protocol_.addStringValue("RENAMETEMPLATE", fileTemplate);

	result = checkNameTag(nameTag);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("NAMETAG", nameTag);


	if (treatAsDefaultValue == false)
	{
		result = checkStartupSequenceNumber(startupSequenceNumber,fileTemplate);
		if (result != AES_NOERRORCODE)
		{
			return result;
		}

	}

	protocol_.addLongValue("STARTUPSEQUENCENUMBER", startupSequenceNumber);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	return result;
}

//	---------------------------------------------------------
//	    send()
//	---------------------------------------------------------
int aes_afp_apiinternal::send (char* msg, unsigned short& msgLen)
{
	AES_AFP_TRACE_MESSAGE("Sending to AES_AFP_Server");
	int result(0);

        ACE_Guard<ACE_Thread_Mutex> guard(sendMutex_);
	result = convSession_->sendf((void*)msg, msgLen, MSG_NOSIGNAL);

	if (result <= 0 )
	{
		AES_AFP_TRACE_MESSAGE("Sending to AES_AFP_Server failed");
		return -1;
	}

	return 0;
}

int aes_afp_apiinternal::recv (std::string& msg, unsigned int& msgLen)
{
	AES_AFP_TRACE_MESSAGE("Receiving from DSD");

	int result(0);
	unsigned short totdatasize(0);
	char tmpbuf[64000];
	char moredata('0');
	bool first(true);

	while (moredata != '1')
	{
		char recvBuf[64000];
		msgLen = 64000;

		result = convSession_->recv((char*)recvBuf, msgLen);
		if (result == false)
		{
			AES_AFP_TRACE_MESSAGE("Receiving from DSD failed, first time");
			result = convSession_->recv((char*)recvBuf, msgLen);
			if (result == false)
			{
				AES_AFP_TRACE_MESSAGE("Receiving from DSD failed, second time");
				return -1;
			}
		}

		totdatasize += (msgLen-1);

		recvBuf[msgLen + 4] = 0;

		moredata = recvBuf[4];

		if (first)
		{
			strcpy(tmpbuf, &recvBuf[6]);
		}
		else
		{
			strcpy(tmpbuf, &recvBuf[10]);
		}
		msg += tmpbuf;

		first = false;

	}

	msgLen = totdatasize;

	AES_AFP_TRACE_MESSAGE("Received %d bytes from DSD", msgLen);

	return AES_NOERRORCODE;
}

unsigned int aes_afp_apiinternal::getEvent (AES_GCC_Eventcodes& eventCode, std::string& filename)
{
	aes_afp_apimsg* eventMsg;
	ACE_Message_Block* osfMsg;

	eventQueue_.dequeue_head(osfMsg);
	eventMsg = (aes_afp_apimsg*)osfMsg;

	filename = eventMsg->getMsgString();
	eventCode = eventMsg->getEventCode();

	AES_AFP_TRACE_MESSAGE("Text = %s event code = %d", filename.c_str(), eventCode);
	osfMsg->release();

	return 0;
}

ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* aes_afp_apiinternal::getEventQueue ()
{
	return &eventQueue_;
}

ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* aes_afp_apiinternal::getCmdQueue ()
{
	return &cmdQueue_;
}

int aes_afp_apiinternal::putOnEventQueue(std::string& dataString, unsigned short message)
{
	aes_afp_apimsg* msg = new aes_afp_apimsg;
	dataString += '\0';

	AES_AFP_TRACE_MESSAGE("Text = %s message = %d", dataString.c_str(), message);

	msg->setMsgString(dataString);
	msg->setEventCode((AES_GCC_Eventcodes)message);
	eventQueue_.enqueue_head(msg);
	AES_GCC_Eventcodes errorcode = (AES_GCC_Eventcodes)message;

	if (apiPtr_ != 0)
	{
		apiPtr_->event(errorcode);
	}
	else
	{
		return AES_NOSERVERACCESS;
	}

	return AES_NOERRORCODE;
}

unsigned int aes_afp_apiinternal::getSourceDirectoryPath (std::string& transferQueue,
		std::string& sourceDirectory)
{
	AES_AFP_TRACE_MESSAGE("Get path for TQ %s", transferQueue.c_str());

	std::string dataString;
	std::stringstream strStream;
	std::string tmpString;
	int result(0);
	std::string user;


	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, GETSOURCEDIRECTORYPATH, 1, dataString);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");
	result = recvFromServer(strStream);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	if (result == AES_NOERRORCODE)
	{
	}

	strStream >> sourceDirectory;


	AES_AFP_TRACE_MESSAGE("Source directory for TQ %s is %s", transferQueue.c_str(), sourceDirectory.c_str());

	return result;
}

unsigned int aes_afp_apiinternal::setSourceDirectoryPath (std::string& transferQueue,
		std::string& sourceDirectory)
{
	AES_AFP_TRACE_MESSAGE("Set path %s for TQ %s", sourceDirectory.c_str(), transferQueue.c_str() );

	int result(0);
	std::string dataString;
	std::string tmpDirectory;
	std::string user;


	protocol_.clearValues();

	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);

	tmpDirectory = sourceDirectory.c_str() ;

	result = checkSourceDirectoryName(tmpDirectory);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("SOURCEDIRECTORY", sourceDirectory);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, SETSOURCEDIRECTORYPATH, 1, dataString);

	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer();

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	return result;
}

int aes_afp_apiinternal::recvFromServer (std::stringstream& strStream)
{
	ACE_Message_Block* osfMsg;
	aes_afp_apimsg* cmdMsg;
	aes_afp_msgtypes  afp_msgType;
	AES_GCC_Errorcodes errorCode;

	strStream.clear();
        cmdQueue_.dequeue_head(osfMsg);
	cmdMsg = (aes_afp_apimsg*)osfMsg;
	afp_msgType = cmdMsg->getMessage();

	std::string afpMsg = cmdMsg->getMsgString();	//qassore
	AES_AFP_TRACE_MESSAGE("cmdMsg->msg = %d", afp_msgType );
	AES_AFP_TRACE_MESSAGE("cmdMsg_msgString.size = %d", (int)afpMsg.length());

	strStream << cmdMsg->getMsgString();

	errorCode = cmdMsg->getErrorCode();
	osfMsg->release();

	return errorCode;
}

//	---------------------------------------------------------
//	       recvFromServer()
//	---------------------------------------------------------
int aes_afp_apiinternal::recvFromServer ()
{
	ACE_Message_Block* osfMsg = 0;
	aes_afp_apimsg* cmdMsg = 0;
	AES_GCC_Errorcodes errorCode = AES_NOERRORCODE;

	//cmdQueue_.dequeue_head(osfMsg);
        if (strcasecmp(applicationSts.c_str(), "sts" ) == 0)
	{
          ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout(cmdQueue_.gettimeofday());
          //timeout += ACE_Time_Value(0, 700000);
          timeout += ACE_Time_Value(3, 0);
          //WriteIntoAFPApi("recvFromServer() b4 timeout");
	  if (cmdQueue_.dequeue_head(osfMsg, &timeout) == -1)
          {
            //WriteIntoAFPApi("recvFromServer() after timeout");
            return AES_NOSERVERACCESS; 
          }
        }
        else
        {
	  cmdQueue_.dequeue_head(osfMsg);
        }
	if (osfMsg)
	{
		cmdMsg = (aes_afp_apimsg*)osfMsg;
		errorCode = cmdMsg->getErrorCode();
		osfMsg->release();
	}
	return errorCode;
}

int aes_afp_apiinternal::sendToServer (unsigned short endFlag, aes_afp_msgtypes msgKind, aes_afp_msgtypes message, unsigned short dataPart, std::string& data)
{
	int result(0);
	char buf[64000]= { 0 };
	unsigned short msgLen(1024);

	sprintf(buf, "%s%d\n%d\n%d\n%d\n%d\n", "rpch", endFlag, msgKind, message, AES_NOERRORCODE, dataPart);

	strcat(buf, data.c_str() );

	msgLen = strlen(buf);
	buf[msgLen] = 0;

	result = send(buf, msgLen);
	if (result == -1)
	{
		convSession_->close();
		return AES_NOSERVERACCESS;
	}

	return AES_NOERRORCODE;
}

//	---------------------------------------------------------
//	       sendFile()
//	---------------------------------------------------------
int aes_afp_apiinternal::sendFile (std::string& filename,
		AES_GCC_Format transferMode,
		std::string fileMask,
		bool isDirectory,
		AES_GCC_Filestates fileState)
{
	AES_AFP_TRACE_MESSAGE("Filename %s Transfer mode %d File mask %s isDirectory %s Status %d", filename.c_str(), transferMode, fileMask.c_str(), isDirectory?"true":"false", fileState);

	int result(0);
	std::string dataString;
	std::string tmpFilename;
	std::string user;

	protocol_.clearValues();

	result = checkFilename(filename);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("FILENAME", filename);

	protocol_.addIntValue("STATUS", fileState);

	result = checkTransferMode(transferMode);
	if (result != 0)
	{
		return result;
	}
	protocol_.addIntValue("TRANSFERMODE", transferMode);

	protocol_.addStringValue("TRANSFERMASK", fileMask);

	protocol_.addBoolValue("ISDIRECTORY", isDirectory);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, SENDOHIFILE, 1, dataString);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer();

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	return result;
}

//	---------------------------------------------------------
//	       lastReportedFile()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::lastReportedFile (std::string transferQueue,
		std::string& originalFilename,
		std::string& generatedFilename)
{
	AES_AFP_TRACE_MESSAGE("Find last reported file for TQ %s", transferQueue.c_str());

	int result(0);
	std::string dataString;
	std::stringstream strStream;
	std::string user;

	protocol_.clearValues();

	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, GETLASTREPORTEDFILE, 1, dataString);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(strStream);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);


	strStream >> originalFilename;
	strStream >> generatedFilename;


	AES_AFP_TRACE_MESSAGE("For TQ %s last reported original %s, generated %s", transferQueue.c_str(), originalFilename.c_str(), generatedFilename.c_str());

	return result;
}

//	---------------------------------------------------------
//	       toUpper()
//	---------------------------------------------------------
char* aes_afp_apiinternal::toUpper (char* str)
{
	int i = 0;

	while (str[i])
	{
		str[i] = toupper(str[i]);
		i++;
	}

	return str;
}

//	---------------------------------------------------------
//	       toUpper()
//	---------------------------------------------------------
void aes_afp_apiinternal::toUpper (std::string& str)
{
	std::string::iterator itr;

	for (itr = str.begin();itr != str.end(); ++itr)
	{
		(*itr) = toupper((*itr) );
	}
}

//	---------------------------------------------------------
//	       getDestinationList()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::getDestinationSetList (std::list<std::string>& destinationSetList)
{
	AES_AFP_TRACE_MESSAGE("getDestinationSetList()");

	int result(0);
	std::string dataString;
	std::stringstream strStream;
	std::string tmpString;
	std::string user;

	protocol_.clearValues();


	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, GETDESTINATIONSETLIST, 1, dataString);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(strStream);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);


	while (strStream.good() )
	{
		strStream >> tmpString;
		// HP32415 start
		if ( tmpString != "")
		{
			destinationSetList.push_back(tmpString);
		}
		// HP32415 end
	}


	return result;
}

//	---------------------------------------------------------
//	       getFileStatus()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::getFileStatus (std::string filename, AES_GCC_Filestates& status)
{
	AES_AFP_TRACE_MESSAGE("File name = %s", filename.c_str());

	int result(0);
	std::string dataString;
	std::stringstream strStream;
	std::string tmpString;
	std::string user;

	protocol_.clearValues();


	result = checkFilename(filename);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("FILENAME", filename);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	result = sendToServer(0, COMMAND, GETFILESTATUS, 1, dataString);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}

	AES_AFP_TRACE_MESSAGE("Waiting for server");

	result = recvFromServer(strStream);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);


	strStream >> tmpString;

	AES_AFP_TRACE_MESSAGE("File status is %s", tmpString.c_str());

	status = (AES_GCC_Filestates)atoi(tmpString.c_str() );


	return result;
}

//	---------------------------------------------------------
//	       removeFile()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::removeFile (std::string transferQueue, std::string filename)
{
	AES_AFP_TRACE_MESSAGE("Removing file %s from TQ %s", filename.c_str(), transferQueue.c_str());

	int result(0);
	std::string dataString;
	std::string user;


	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkFilename(filename);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("FILENAME", filename);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);

	return result;
}

//	---------------------------------------------------------
//	       removeFile()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::removeFile (std::string transferQueue, std::string destinationSet, std::string filename)
{
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s Filename = %s",
			transferQueue.c_str(), destinationSet.c_str(), filename.c_str());

	int result(0);
	std::string dataString;
	std::string user;


	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	result = checkFilename(filename);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("FILENAME", filename);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);

	return result;
}

//	---------------------------------------------------------
//	       sendFileManually()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::sendFileManually (std::string transferQueue, std::string userGroup, std::string filename, std::string sourceDirectory, std::string destinationSet, int retryTimes, int retryTimeInterval, bool isDirectory)
{
	AES_AFP_TRACE_MESSAGE("TQ %s Filename %s sourceDirectory %s destination set %s Retries = %d Retry time = %d Directory = %s",
			transferQueue.c_str(), filename.c_str(), sourceDirectory.c_str(), destinationSet.c_str(), retryTimes, retryTimeInterval, isDirectory?"true":"false");
	(void)userGroup;
	int result(0);
	std::string dataString;
	std::string user;
	protocol_.clearValues();


	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	result = checkDestinationSetName(destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("DESTINATIONSET", destinationSet);

	result = checkFilename(filename);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("FILENAME", filename);

	result = checkSourceDirectoryName(sourceDirectory);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("SOURCEDIRECTORY", sourceDirectory);

	result = checkNrOfRetries(retryTimes);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addIntValue("SENDRETRIES", retryTimes);

	result = checkRetryTime(retryTimeInterval);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addIntValue("SENDRETRIESDELAY", retryTimeInterval);

	protocol_.addBoolValue("MANUALINITIATED", true);
	protocol_.addBoolValue("ISDIRECTORY", isDirectory);

	return result;
}
//	---------------------------------------------------------
//	       removeSourceDirectory()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::removeSourceDirFiles (std::string& transferQueue)	//for APZ21230/4-699
{
	AES_AFP_TRACE_MESSAGE("TQ = %s", transferQueue.c_str());

	std::string dataString;
	int result(0);
	std::string user;

	protocol_.clearValues();

	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return(result);
	}

	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);


	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);

	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	return result;
}

//	---------------------------------------------------------
//	       removeFileFromSourceDir ()
//	---------------------------------------------------------
unsigned int aes_afp_apiinternal::removeFileFromSourceDir (std::string transferQueue, std::string filename)	//for APZ21230/4-699
{
	AES_AFP_TRACE_MESSAGE("Removing file %s from Source Directory for TQ %s", filename.c_str(), transferQueue.c_str());

	int result(0);
	std::string dataString("");
	std::string user("");
	////AES_GCC_User tmpUser;

	//toUpper(filename);

	protocol_.clearValues();

	//  //user = tmpUser.getName();

	result = checkTqName(transferQueue);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);

	//	//protocol_.addStringValue("USERNAME", user);

	result = checkFilename(filename);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	protocol_.addStringValue("FILENAME", filename);

	AES_AFP_TRACE_MESSAGE("Send to server");

	protocol_.getString(dataString);
	AES_AFP_TRACE_MESSAGE("Received from server, result = %d", result);

	return result;
}

unsigned int aes_afp_apiinternal::validateTqName(std::string& transferQueue)
{
	int result = AES_NOERRORCODE;
	result = checkTqName(transferQueue);
	if(result != AES_NOERRORCODE)
	{
		AES_AFP_TRACE_MESSAGE("TransferQueue validation failed");
		return result;
	}
	return result;
}


unsigned int aes_afp_apiinternal::validateDestinationSet(std::string& destinationSet)
{
	int result = AES_NOERRORCODE;
	result = checkDestinationSetName(destinationSet);
	if(result != AES_NOERRORCODE)
	{
		AES_AFP_TRACE_MESSAGE("Destination Set validation failed");
		return result;
	}
	return result;
}

unsigned int aes_afp_apiinternal::validateTransferQueueParam(
		std::string& nameTag,
		std::string& fileTemplate,
		AES_GCC_Filestates defaultState,
		int removeDelay,
		int retryTimeInterval,
		int retryTimes,
	    //Start - HT50930
	    int startupSequenceNumber
	    //End - HT50930
		)
{
	(void)fileTemplate;
	int result = AES_NOERRORCODE;
	AES_AFP_TRACE_MESSAGE("%d",defaultState);
// Fix for TR HQ17394
	int count = 0;
	size_t ipos = fileTemplate.find('F');
	while(ipos != string::npos)
	 {
	 	count++;
	 	ipos = fileTemplate.find('F',ipos+1);
	 }

	 if((count==2 && nameTag.find('*')==string::npos) || (count == 1 && nameTag.empty()))
	 {
	   return AES_NAMETAGINVALID;
  	 }
	 
	result = checkNameTag(nameTag);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	result = checkRemoveDelay(removeDelay);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	std::string destinationSet("");
	result = checkDefaultStatus(defaultState,destinationSet);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	result = checkNrOfRetries(retryTimes);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	result = checkRetryTime(retryTimeInterval);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	//Start - HT50930
	result = checkStartupSequenceNumber(startupSequenceNumber,fileTemplate);
	if (result != AES_NOERRORCODE)
	{
		return result;
	}
	//End - HT50930
	return result; 
}


unsigned int aes_afp_apiinternal::validateRenameTemplate(
		std::string& nameTag,
		std::string& templateString
)
{
	//## begin aes_afp_template::setTemplate%3BE13529026C.body preserve=yes
	unsigned int index(0);
	bool year(false);
	bool month(false);
	bool day(false);
	bool hour(false);
	bool min(false);
	bool sec(false);
	bool pers(false);
	bool ners(false);
	bool fsiz(false);
	bool orig(false);
	//bool tag(false);
	bool tagfirst(false);  // uabmha: CNI 1135
	bool tagsecond(false); // uabmha: CNI 1135

	int ipos = 0;
	//int totlen = 0;
	int part2len = 0;
	bool multiNameTag(false);
	//bool usePersNr_(false);
	std::string nameTagPart1,nameTagPart2;
	TemplateVector templateVector_;
	
	//totlen = nameTag.length();

	//
	if ((ipos = nameTag.find_first_of("*")) != (int)(std::string::npos))
	{
		// Asterisk found in string, split string into two name tags
		multiNameTag = true;

		// First calculate length of part2 (aaa*.cdr : 8 - 3 - 1 = 4 characters
		part2len = nameTag.length() - ipos - 1;

		// then copy from pos. 0 to ipos into nameTagPart1_ ...
		nameTagPart1 = nameTag.substr(0, ipos);
		// terminate with NULL;
		nameTagPart1[ipos] = NULL;

		// then copy from pos. after ipos to end into nameTagPart2_
		nameTagPart2 = nameTag.substr(ipos+1, part2len);
	}
	AES_AFP_TRACE_MESSAGE("templateString = %s",templateString.c_str());
	for (index=0;index < templateString.length();index++)
	{
		switch (templateString[index] )
		{
		case 'y':
		{
			if (year == true)
			{
				return AES_TEMPLATEFAULT;
			}
			year = true;
			index++;

			// Check for 2 digit year
			if (templateString[index] == 'y')
			{
				index++;
			}
			else
			{
				return AES_TEMPLATEFAULT;
			}
			if (templateString[index] == 'y')
			{
				index++;
			}
			else
			{
				// Two digit year
				templateVector_.push_back("TWOYEAR");
				index--;
				break;
			}

			// Check for 4 digit year
			if (templateString[index] == 'y')
			{
				templateVector_.push_back("FOURYEAR");
			}
			else
			{
				return AES_TEMPLATEFAULT;
			}
			break;
		}
		case 'm':
		{
			if (month == true)
			{
				return AES_TEMPLATEFAULT;
			}
			month = true;
			index++;
			if (templateString[index] == 'm')
			{
				templateVector_.push_back("MONTH");
			}
			else
			{
				return AES_TEMPLATEFAULT;
			}
			break;
		}
		case 'd':
		{
			if (day == true)
			{
				return AES_TEMPLATEFAULT;
			}
			day = true;
			index++;
			if (templateString[index] == 'd')
			{
				templateVector_.push_back("DAY");
			}
			else
			{
				return AES_TEMPLATEFAULT;
			}
			break;
		}
		case 'H':
		{
			if (hour == true)
			{
				return AES_TEMPLATEFAULT;
			}
			hour = true;
			index++;
			if (templateString[index] == 'H')
			{
				templateVector_.push_back("HOUR");
			}
			else
			{
				return AES_TEMPLATEFAULT;
			}
			break;
		}
		case 'M':
		{
			if (min == true)
			{
				return AES_TEMPLATEFAULT;
			}
			min = true;
			index++;
			if (templateString[index] == 'M')
			{
				templateVector_.push_back("MINUTE");
			}
			else
			{
				return AES_TEMPLATEFAULT;
			}
			break;
		}
		case 'S':
		{
			if (sec == true)
			{
				return AES_TEMPLATEFAULT;
			}
			sec = true;
			index++;
			if (templateString[index] == 'S')
			{
				templateVector_.push_back("SECOND");
			}
			else
			{
				return AES_TEMPLATEFAULT;
			}
			break;
		}
		case 'p':
		{
			if (pers == true || ners == true)
			{
				return AES_TEMPLATEFAULT;
			}
			pers = true;
			//usePersNr_ = true;
			// Check for one p
			index++;
			if (templateString[index] == 'p')
			{
				index++;
			}
			else
			{
				index--;
				templateVector_.push_back("ONEPERS");
				break;
			}
			if (templateString[index] == 'p')
			{
				index++;
			}
			else
			{
				//cout << "1" << endl;
				return AES_TEMPLATEFAULT;
			}
			// Check for four p
			if ((templateString[index] == 'p') && (templateString[index + 1] != 'p'))
			{
				templateVector_.push_back("FOURPERS");
				break;
			}
			else if ((templateString[index] == 'p') && (templateString[index + 1] == 'p'))
			{
				index += 2;
			}
			else
			{
				//cout << "2" << endl;
				return AES_TEMPLATEFAULT;
			}
			// For HL79347 : Check for six p
			if ((templateString[index] == 'p') && (templateString[index + 1] != 'p'))
			{
				templateVector_.push_back("SIXPERS");
				break;
			}
			else if ((templateString[index] == 'p') && (templateString[index + 1] == 'p'))
			{
				index += 2;
			}
			else
			{
				//cout << "3" << endl;
				return AES_TEMPLATEFAULT;
			}
			// For HL79347 : Check for eight p
			if ((templateString[index] == 'p') && (templateString[index + 1] != 'p'))
			{
				templateVector_.push_back("EIGHTPERS");
				break;
			}
			else
			{
				//cout << "4" << endl;
				return AES_TEMPLATEFAULT;
			}

			break;
		}
		case 'n':
		{
			if (ners == true || pers == true)
			{
				return AES_TEMPLATEFAULT;
			}
			ners = true;
			// Check for one n
			index++;
			if (templateString[index] == 'n')
			{
				index++;
			}
			else
			{
				index--;
				templateVector_.push_back("ONENERS");
				break;
			}

			if (templateString[index] == 'n')
			{
				index++;
			}
			else
			{
				return AES_TEMPLATEFAULT;
			}
			if (templateString[index] != 'n')
			{
				return AES_TEMPLATEFAULT;
			}
			// Check for four n
			if ((templateString[index] == 'n') && (templateString[index + 1] != 'n'))
			{
				templateVector_.push_back("FOURNERS");
				break;
			}
			else if ((templateString[index] == 'n') && (templateString[index + 1] == 'n'))
			{
				index += 2;
			}
			else
			{
				return AES_TEMPLATEFAULT;
			}
			// For HL79347 : Check for six n
			if ((templateString[index] == 'n') && (templateString[index + 1] != 'n'))
			{
				templateVector_.push_back("SIXNERS");
				break;
			}
			else if ((templateString[index] == 'n') && (templateString[index + 1] == 'n'))
			{
				index += 2;
			}
			else
			{
				return AES_TEMPLATEFAULT;
			}
			// For HL79347 : Check for eight n
			if ((templateString[index] == 'n') && (templateString[index + 1] != 'n'))
			{
				templateVector_.push_back("EIGHTNERS");
				break;
			}
			else
			{
				return AES_TEMPLATEFAULT;
			}
			break;
		}
		case 'z':
		{
			if (fsiz == true)
			{
				return AES_TEMPLATEFAULT;
			}
			fsiz = true;
			templateVector_.push_back("FILESIZE");
			break;
		}
		case 'f':
		{
			if (orig == true)
			{
				return AES_TEMPLATEFAULT;
			}
			orig = true;
			templateVector_.push_back("ORIGINAL");
			break;
		}
		case 'F':
		{
			// uabmha: CNI 1135, Added handling of 2 name tags
			if (tagsecond == true)
			{
				// No more than two F elements allowed
				// regardless of name tag content.
				//cout << "6" << endl;
				return AES_TEMPLATEFAULT;
			}

			else if (tagfirst == true)
			{
				if (multiNameTag == false)
				{
					// No asterisk present, so no second F allowed
					//cout << "5" << endl;
					return AES_TEMPLATEFAULT;
				}
				tagsecond = true;
				templateVector_.push_back("NAMETAG");
			}
			else
			{
				tagfirst = true;
				templateVector_.push_back("NAMETAG");
			}

			break;
		}// end of case 'F':
		/* START:: CNI 55/1-970:: introducing new option 'b' for blade id. if it is bc1 considers as '01' and if 
						it is cp01 considers as '91' */
		case 'b':
		{
			bool isMultipleCPSystem = false;
			ACS_CS_API_NS::CS_API_Result multiCP = ACS_CS_API_NetworkElement_R1::isMultipleCPSystem(isMultipleCPSystem);
			if (!((multiCP == ACS_CS_API_NS::Result_Success) && (isMultipleCPSystem)))
			return AES_INCUSAGE;
			break;
		}
		/* END of new option 'b' */
		default:
		{
			//cout << "7" << endl;
			return AES_TEMPLATEFAULT;
		}

		}// end of switch

	}// end of for

	if ((nameTag.length() > 0) && (tagfirst != true))
	{
		// Name tag is set, at least one F element should have existed
		//cout << "8" << endl;
		return AES_TEMPLATEFAULT;
	}
	if ((tagsecond != true) && (multiNameTag == true))
	{
		// Less than two F elements found, not enough when asterisk
		// is present in name tag...
		//cout << "9" << endl;
		return AES_TEMPLATEFAULT;
	}
	return AES_NOERRORCODE;
}

#if 0 
void WriteIntoAFPApi(string message)
{
 char tmp_msg[200];
 ofstream fin;
 string myProcessName("");
 ACS_APGCC::getProcessName(&myProcessName);
 if (strcmp(myProcessName.c_str(), "sts_provd") != 0 )
     return;
 string file_name("/var/log/acs/tra/afp_api.log");
 struct stat statbuf;
 int ret = stat(file_name.c_str(), &statbuf);
 if (ret == 0 && statbuf.st_size > 1024*1024*500 )
 {
    rename(file_name.c_str(), "/var/log/acs/tra/afp_api.log.1");
 }
 fin.open(file_name.c_str(), std::ofstream::out | std::ofstream::app);
 time_t t = time(NULL);
 struct tm tm = *localtime(&t);
 sprintf(tmp_msg,"%d-%d-%d %d:%d:%d\t%d\t", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, getpid());
 fin<<tmp_msg <<message.c_str()<<"\n";

 fin.close();
}
#endif

