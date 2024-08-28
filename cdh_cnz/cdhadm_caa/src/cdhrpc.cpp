
//******************************************************************************
//
// NAME
//      cdhrpc.cpp
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//  A common API for different RPC types.

// DOCUMENT NO
//  190 89-CAA 109 0507

// AUTHOR 
//  2002-12-13 by UAB/UKY/AU Hans-Erik Nilsson

// SEE ALSO 
//  CDHProtocol
//
//******************************************************************************

#ifdef WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4231)
#endif

#include <bgwrpc.h>
#include <ace/Get_Opt.h>
#include <ace/Synch.h>
#include <ace/OS.h>
#include <servr.h>
#include <string>
#include <cdhrpc.h>
#include <transferagent.h>
#include <ACS_TRA_trace.h>
#include <fstream>
#include <stdio.h>
#include <event.h>
#include <sstream>
#include <map>
#include <utility>
#include <string.h>
#include <AES_DBO_DataBlock.h>
#include <destinationalarm.h>
//! Common Class include for Getting Active Node
#include <ACS_APGCC_Util.H>
#include <ACS_APGCC_CommonLib.h>
#include <aes_gcc_variable.h>
#include <parameter.h>
#include <aes_cdh_common.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

using namespace std;

multimap< string, CDHRPC* > CDHRPC::masterCopyMap;
ACE_Thread_Mutex CDHRPC::masterCopyMutex;

AES_CDH_TRACE_DEFINE(AES_CDH_CDHRPC);

#if 0      // Not used anywhere in the code
int CDHRPC::GetHWVersion()
{
	ACE_INT32 nVersion;
	char myhwVersion[200];
	int myLen=10;
	ACS_APGCC_HwVer_ReturnTypeT myReturnCode;
	ACS_APGCC_CommonLib myAPGCCCommonLib;
	myReturnCode = myAPGCCCommonLib.GetHWVersion (myhwVersion, myLen );
	if (ACE_OS::strcmp(myhwVersion,"APG43") == 0)
	{
		nVersion = 1;
	}
	else if(ACE_OS::strcmp(myhwVersion,"APG43/2") == 0)
	{
		nVersion = 2;
	}
	else
	{
		nVersion = -1;
	}
	return nVersion;
};
#endif

//******************************************************************************
//  CDHRPC()
//******************************************************************************
//##ModelId=3DE49B4801FD
CDHRPC::CDHRPC():
paramError(false),
switchState(AGENT_OK),
TQrecv(NULL),
bCease(true),
bTerminate(false),
apiClosed(false),
hEvAbortRead(0),
hEvInstanceClosed(0),
streamOpened(false),
retrySendRecord(false),
bytesRead(0),
parentCopy(NULL),
continueRetries(false)
{
	hEvAbortRead = new (std::nothrow) ACE_Manual_Event();
	hEvInstanceClosed = new (std::nothrow) ACE_Manual_Event();
}




//******************************************************************************
//  ~CDHRPC()
//******************************************************************************
//##ModelId=3DE49B480210
CDHRPC::~CDHRPC()
{
	delete hEvInstanceClosed;
	delete hEvAbortRead;
}


//******************************************************************************
//  define()
//******************************************************************************
//##ModelId=3DE49B48021A
AES_CDH_ResultCode CDHRPC::define(int& argc,
                                  char* argv[],
                                  const bool define,
                                  const bool recovery)
{
	(void)define;
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	pair< string, CDHRPC* > masterCopyPair;
	multimap< string, CDHRPC* >::iterator itr;

	hostAddress = "";

	if (paramError == true)
		return AES_CDH_RC_INTPROGERR;
	returnCode = parseRPC(argc, argv);
	cleanUpArgv(argc, argv, 1);

	if (returnCode == AES_CDH_RC_OK && argc != 1)
	{
		if (!recovery)
			returnCode = AES_CDH_RC_INCUSAGE;
	}

	if (returnCode == AES_CDH_RC_OK)
	{
		parentCopy = this;
		(void) masterCopyMutex.acquire_write();
		itr = masterCopyMap.find(destinationName);
		if (itr != masterCopyMap.end())
		{
			(*itr).second = this;
		}
		else
		{
			masterCopyPair.first = destinationName;
			masterCopyPair.second = this;
			masterCopyMap.insert(masterCopyPair);
		}
		(void) masterCopyMutex.release();
	}

	return returnCode;
}


//******************************************************************************
//  remove()
//******************************************************************************
AES_CDH_ResultCode CDHRPC::remove(void)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	return AES_CDH_RC_OK;   /* Nothing to do */
}


//******************************************************************************
//  sendFile()
//******************************************************************************
AES_CDH_ResultCode CDHRPC::sendFile(const string& fileName,
                                    const string& remoteSubDirName,
                                    const string& newFileName,
                                    AES_CDH_DestinationSet::transferMode trMode) // INGO3 GOH qabhall
{
	(void)fileName;
	(void)remoteSubDirName;
	(void)newFileName;
	(void)trMode;
	return AES_CDH_RC_INCUSAGE;
}  

//******************************************************************************
//  sendRecordFile()
//******************************************************************************
AES_CDH_ResultCode CDHRPC::sendRecordFile(const string& streamName,
                                          const string& streamId,
                                          AES_DBO_DataBlock*& block,
                                          CmdHandler *cmdHdlr,
                                          const ACE_UINT64 ticks)
{
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	AES_CDH_ResultCode old_returnCode = AES_CDH_RC_OK;

	int blockRetryDelay_int;
	bool keepTrying = true;

	// start fresh
	if (continueRetries == false)
	{
		blockSendRetries_int_done = 0;

		// default values
		if (blockSendRetries == "")
		{
			blockSendRetries_int = 0;
			blockSendRetries_int_todo = 0;
			blockRetryDelay_int = 1000;
		}
		else
		{
			// non-default values
			blockSendRetries_int = atoi(blockSendRetries.c_str());
			blockSendRetries_int_todo = atoi(blockSendRetries.c_str());

			if (blockRetryDelay == "")
			{
				blockRetryDelay_int = 1000;
			}
			else
			{
				blockRetryDelay_int = atoi(blockRetryDelay.c_str());
			}
		}
	}
	else
	{
		// continue decrementing retries
		blockSendRetries_int_todo = (blockSendRetries_int - blockSendRetries_int_done);

		if (blockRetryDelay == "")
		{
			blockRetryDelay_int = 1000;
		}
		else
		{
			blockRetryDelay_int = atoi(blockRetryDelay.c_str());
		}

		AES_CDH_TRACE_MESSAGE("CDHRPC::SendRecordFile(..), Continue with OLD values");
	}

	AES_CDH_TRACE_MESSAGE( "startvalue blockSendRetries_int %d", blockSendRetries_int);
	AES_CDH_TRACE_MESSAGE( "startvalue blockSendRetries_int_done %d", blockSendRetries_int_done);
	AES_CDH_TRACE_MESSAGE( "startvalue blockSendRetries_int_todo %d", blockSendRetries_int_todo);
	AES_CDH_TRACE_MESSAGE( "blockRetryDelay_int %d", blockRetryDelay_int);

	while (keepTrying)
	{
		returnCode = intSendRecordFile(streamName, streamId, block, cmdHdlr, ticks);

		switch (returnCode)
		{
		//If there is a switch ordered, then break out.
		case AES_CDH_RC_SWITCH:
			keepTrying = false;
			break;
			// transfer OK
		case AES_CDH_RC_OK:
			continueRetries = false;
			keepTrying = false;
			AES_CDH_TRACE_MESSAGE("CDHRPC::SendRecordFile(..), AES_CDH_RC_OK");
			break;
			// transfer failed
		case AES_CDH_RC_CONNECTERR:
		case AES_CDH_RC_SENDERR:
			old_returnCode = returnCode;
			--blockSendRetries_int_todo;
			++blockSendRetries_int_done;

			// ... and we have retries left, wait a while before next retry
			if (blockSendRetries_int_todo > -1)
			{
				ACE_Time_Value tv(0,blockRetryDelay_int*1000); // milliseconds
				ACE_OS::sleep(tv);
				continueRetries = true;
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::SendRecordFile(..), AES_CDH_RC_CONNECT-/SENDERR, blockSendRetries_int_todo &gt; -1");
				AES_CDH_TRACE_MESSAGE("CDHRPC::SendRecordFile(..), AES_CDH_RC_CONNECT-/SENDERR, blockSendRetries_int_todo &gt; -1");
			}
			else
			{
				// ... and we have used all retries
				// transfer will be initiated to another destination if there is an unused one in the destination set
				keepTrying = false;
				continueRetries = false;
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::SendRecordFile(..), AES_CDH_RC_CONNECT-/SENDERR, blockSendRetries_int_todo = -1");
				AES_CDH_TRACE_MESSAGE("CDHRPC::SendRecordFile(..), AES_CDH_RC_CONNECT-/SENDERR, blockSendRetries_int_todo = -1");
			}
			break;

		default:
			keepTrying = false;
			--blockSendRetries_int_todo;
			++blockSendRetries_int_done;

			// ... and we have retries left
			// we will continue decrementing retries for this destination next time block transfer is initiated
			if (blockSendRetries_int_todo > -1)
			{
				continueRetries = true;
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::SendRecordFile(), DEFAULT, blockSendRetries_int_todo > -1 return value: %d", returnCode);
				AES_CDH_TRACE_MESSAGE("CDHRPC::SendRecordFile(), DEFAULT, blockSendRetries_int_todo > -1 return value: %d", returnCode);
			}
			else
			{
				// ... and we have used all retries
				// transfer will be initiated to another destination if there is an unused one in the destination set
				continueRetries = false;
				returnCode = old_returnCode;

				AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::SendRecordFile(), DEFAULT, blockSendRetries_int_todo = -1 return value: %d", returnCode);
			}
			break;
		} // end switch

		AES_CDH_TRACE_MESSAGE( "whileloop blockSendRetries_int %d", blockSendRetries_int);
		AES_CDH_TRACE_MESSAGE( "whileloop blockSendRetries_int_done %d", blockSendRetries_int_done);
		AES_CDH_TRACE_MESSAGE( "whileloop blockSendRetries_int_todo %d", blockSendRetries_int_todo);
	} //end while

	apiClosed = false;
	return AES_CDH_Result(returnCode).code();
}

//******************************************************************************
//  sendEvent()
//******************************************************************************
void CDHRPC::sendEvent(CmdHandler *cmdHdlr, const ACE_UINT32 ticks)
{
	ServR::cmdHandlerListMX_.acquire();
	if (ServR::validCmdHandler(cmdHdlr, ticks))
	{
		cmdHdlr->sendRecordFileReply(AES_CDH_RC_OK, ""/*fileName*/, 0/*recordsSent*/);
	}
	ServR::cmdHandlerListMX_.release();
	DestinationAlarm::instance()->cease(this->destinationName, this->destinationPtr->getDestSetName(), DestinationAlarm::AlarmRecord::ok);
}

//******************************************************************************
//  intSendRecordFile()
//******************************************************************************
//##ModelId=3DE49B48024D
AES_CDH_ResultCode CDHRPC::intSendRecordFile(const string& streamName,
                                          const string& streamId,
                                          AES_DBO_DataBlock*& block,
                                          CmdHandler *cmdHdlr,
                                          const ACE_UINT32 ticks)
{

	// streamId       = Transfer Queue name
	// streamId       = if "EMPTY", 'Clustername_TQ_Destination' shall be used
	//                  else it replaces 'Clustername_TQ_Destination'

	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	unsigned int GCC_rc = 0;
	AES_CDH_TRACE_MESSAGE("Entering...");
	AES_CDH_TRACE_MESSAGE("streamName= %s", streamName.c_str());
	AES_CDH_TRACE_MESSAGE("streamId= %s", streamId.c_str());

	if ( TQrecv == NULL )
	{
		AES_CDH_TRACE_MESSAGE("calling tqmanager open");
		AES_DBO_TQManager::instance()->openRx(streamName, TQrecv);
	}

	// Open the channel
	GCC_rc = TQrecv->openTransfer();
	if (GCC_rc == AES_BUFFERERROR)
	{
		TQrecv->closeTransfer(); // HM83829 Close the synchronization objects if open function fails for shared memory
		TQrecv->destroy(); // HM83829
		TQrecv = NULL;
		//CNI55_1_1137
		AES_CDH_LOG(LOG_LEVEL_ERROR, "CDHRPC::intSendRecordFile(AES_BUFFERERROR) - returning AES_CDH_RC_STREAMERROR,BladeID = %s DESTINATION = %s GCC_rc = %u",streamId.c_str(),destinationName.c_str(),GCC_rc);
		AES_CDH_TRACE_MESSAGE("CDHRPC::intSendRecordFile(AES_BUFFERERROR) - returning AES_CDH_RC_STREAMERROR,BladeID = %s DESTINATION = %s GCC_rc = %u",streamId.c_str(),destinationName.c_str(),GCC_rc);
		return AES_CDH_RC_STREAMERROR;
	}


	// ++
	int argc;
	char* argv[9];
	char optionF[] = "-f";
	char optionM[] = "-m";
	char optionN[] = "-n";
	char optionO[] = "-o";
	char maxSize[20] = "0";

	char tmpSubFileName[256 + 1];
	char tmpStreamName[256 + 1];
	char tmpMainFileName[256 + 1];

	strcpy(tmpMainFileName, streamId.c_str());
	strcpy(tmpSubFileName, streamId.c_str());
	strcpy(tmpStreamName, streamName.c_str());

	argv[1] = optionF;
	argv[2] = tmpMainFileName;
	argv[3] = optionN;
	argv[4] = tmpSubFileName;
	argv[5] = optionM;
	argv[6] = maxSize;
	argv[7] = optionO;
	argv[8] = tmpStreamName;
	argc = 9;

	unsigned int blockFromOpen = 0;
	//CNI-55_1_1173
	AES_CDH_TRACE_MESSAGE("BEFORE this->open function, BladeID = %s DESTINATION = %s",streamId.c_str(),destinationName.c_str());
	returnCode = this->open(argc, argv, blockFromOpen);
	//CNI-55_1_1173
	AES_CDH_TRACE_MESSAGE( "AFTER this->open function, BladeID = %s DESTINATION = %s returnCode = %d",streamId.c_str(),destinationName.c_str(),returnCode);
	if (returnCode != AES_CDH_RC_OK)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "CDHRPC::intSendRecordFile OPEN FAILED ,BladeID = %s DESTINATION = %s returnCode = %d",streamId.c_str(),destinationName.c_str(),returnCode);
		AES_CDH_TRACE_MESSAGE("CDHRPC::intSendRecordFile OPEN FAILED ,BladeID = %s DESTINATION = %s returnCode = %d",streamId.c_str(),destinationName.c_str(),returnCode);
		this->close();
		TQrecv->closeTransfer();
		ACE_Time_Value tv(0,100*1000);
		ACE_OS::sleep(tv);
		return returnCode;
	}
	AES_CDH_TRACE_MESSAGE("OPEN SUCCESS - ATTACH DONE IN CDH.. , BladeID = %s DESTINATION = %s returnCode = %d",streamId.c_str(),destinationName.c_str(),returnCode);
	while (!bTerminate && (ServR::isStopEventSignalled == false) )
	{
		if (bCease == true)
		{
			if ( block == NULL || blockFromOpen == block->blockNr_)
			{
				if(block != NULL)
				{
					delete block;
					block = NULL;
				}
				if (switchState == AGENT_STOP)
				{
					this->close();
					TQrecv->closeTransfer();
					switchState = AGENT_OK;
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile AGENT_STOP STATE returning AES_CDH_RC_SWITCH, BladeID = %s DESTINATION = %s bCease = %u",streamId.c_str(),destinationName.c_str(),bCease);
					return AES_CDH_RC_SWITCH;
				}

				GCC_rc = TQrecv->recv(block, 1000, hEvAbortRead);

			}
			else
			{
				GCC_rc = AES_NOERRORCODE;
			}

			if (GCC_rc == AES_NOERRORCODE)
			{
				AES_CDH_TRACE_MESSAGE("TQrecv->recv() SUCCESS Sending Block, BladeID = %s DESTINATION = %s bCease = %u GCC_rc = %u",streamId.c_str(),destinationName.c_str(),bCease,GCC_rc);
				returnCode = sendRecord( block->getData(), block->length_, block->blockNr_ );

				if (returnCode == AES_CDH_RC_SKIPPED)
				{
					delete block;
					block = NULL;
				}
				else if (returnCode != AES_CDH_RC_OK)
				{
					AES_CDH_TRACE_MESSAGE("CDHRPC::intSendRecordFile (sendRecord FAILED) , BladeID = %s DESTINATION = %s bCease = %u block->length_ = %u block->blockNr_ = %u\nreturnCode = %d",streamId.c_str(),destinationName.c_str(),bCease,block->length_,block->blockNr_,returnCode);
					this->close();
					TQrecv->closeTransfer();
					delete block;
					block = NULL;
					//sleep(500);
					return returnCode;
				}
				else
				{
					bCease = false;
					delete block;
					block = NULL;
					this->sendEvent(cmdHdlr, ticks);
				}
			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile TQrecv->recv() FAILED, BladeID = %s DESTINATION = %s bCease = %u GCC_rc = %u",streamId.c_str(),destinationName.c_str(),bCease,GCC_rc);
				switch (GCC_rc)
				{
				case AES_NOSERVERACCESS:
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile() stream returned AES_NOSERVERACCESS");
					break;

				case AES_BUFFERERROR:
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile() stream returned AES_BUFFERERROR");
					this->close();
					TQrecv->closeTransfer();
					TQrecv->destroy();
					TQrecv = NULL;
					return AES_CDH_RC_STREAMERROR;
					break;

				case AES_TIMEOUT:
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile() stream returned AES_TIMEOUT");

					// The call to recv from the stream above has timed out.
					// Check if the application has closed the API
					if (apiClosed == true)
					{
						AES_CDH_LOG(LOG_LEVEL_INFO,"CDHRPC::intSendRecordFile(..) apiClosed == true");
						AES_CDH_LOG(LOG_LEVEL_INFO, "CDHRPC::intSendRecordFile(..), streamName= %s", streamName.c_str());
						AES_CDH_LOG(LOG_LEVEL_INFO, "CDHRPC::intSendRecordFile(..), streamId= %s", streamId.c_str());
						apiClosed= false;
						this->close();
						TQrecv->closeTransfer();
						TQrecv->destroy();
						TQrecv = NULL;
						delete block;
						block = NULL;
						bTerminate = false;
						return returnCode;
					}

					break;

				case AES_BUFFEREMPTY:
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile() stream returned AES_BUFFEREMPTY");
					break;

				case AES_CATASTROPHIC:
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile() stream returned AES_CATASTROPHIC");
					this->close();
					TQrecv->closeTransfer();
					TQrecv->destroy();
					TQrecv = NULL;
					return AES_CDH_RC_ERROR;
					break;

				default:
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile() Unknown return value from stream");
					break;
				}
			}
		}
		else
		{

			// This is the normal case
			while (! bTerminate && (ServR::isStopEventSignalled == false) )
			{
				if (switchState == AGENT_STOP)
				{
					this->close();
					TQrecv->closeTransfer();
					bCease = true;
					//block = NULL;
					switchState = AGENT_OK;
					//Start - CNI-55_1_1173
					AES_CDH_TRACE_MESSAGE("(AGENT_STOP STATE) returning AES_CDH_RC_SWITCH, BladeID = %s DESTINATION = %s bCease = %u",streamId.c_str(),destinationName.c_str(),bCease);
					return AES_CDH_RC_SWITCH;
				}
				//Start - CNI-55_1_1173			
				AES_CDH_TRACE_MESSAGE("BEFORE TQrecv->recv() function, BladeID = %s DESTINATION = %s bCease = %u",streamId.c_str(),destinationName.c_str(),bCease);
				if ( (GCC_rc = TQrecv->recv(block, 1000, hEvAbortRead)) == AES_NOERRORCODE)
				{
					//Start - CNI-55_1_1173				
					AES_CDH_TRACE_MESSAGE("TQrecv->recv() SUCCESS Sending Block, BladeID = %s DESTINATION = %s bCease = %u GCC_rc = %u",streamId.c_str(),destinationName.c_str(),bCease,GCC_rc);
					returnCode = sendRecord( block->getData(), block->length_, block->blockNr_ );

					if (returnCode != AES_CDH_RC_OK && returnCode != AES_CDH_RC_SKIPPED)
					{
						this->close();
						TQrecv->closeTransfer();
						bCease = true;
						delete block;
						block = NULL;
						//Start - CNI-55_1_1173				
						AES_CDH_LOG(LOG_LEVEL_ERROR,"CDHRPC::intSendRecordFile ELSE - ERROR sendRecord FAILED, BladeID = %s DESTINATION = %s bCease = %u returnCode = %d",streamId.c_str(),destinationName.c_str(),bCease,returnCode);
						//End - CNI-55_1_1173
						return returnCode;
					}

					delete block;
					block = NULL;
				}
				else
				{
					//Start - CNI-55_1_1173				
					AES_CDH_LOG(LOG_LEVEL_ERROR, "CDHRPC::intSendRecordFile TQrecv->recv() FAILED ,BladeID = %s DESTINATION = %s bCease = %u GCC_rc = %u",streamId.c_str(),destinationName.c_str(),bCease,GCC_rc);
					//End - CNI-55_1_1173

					switch (GCC_rc)
					{
					case AES_NOSERVERACCESS:
						AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile() stream returned AES_NOSERVERACCESS");
						break;

					case AES_BUFFERERROR:
						AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile() stream returned AES_BUFFERERROR");
						this->close();
						TQrecv->closeTransfer();
						TQrecv->destroy();
						TQrecv = NULL;
						bCease = true;      // ++
						return AES_CDH_RC_STREAMERROR;
						break;

					case AES_TIMEOUT:
						AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile() stream returned AES_TIMEOUT");

						// The call to recv from the stream above has timed out.
						// Check if the application has closed the API
						if (apiClosed == true)
						{
							AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile()AES_TIMEOUT Apiclosed == true");
							apiClosed= false;
							this->close();
							TQrecv->closeTransfer();
							TQrecv->destroy();
							TQrecv = NULL;
							bCease = true;
							delete block;
							block = NULL;
							bTerminate = false;
							return returnCode;
						}

						break;

					case AES_BUFFEREMPTY:
						AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile() stream returned AES_BUFFEREMPTY");
						break;

					case AES_CATASTROPHIC:
						AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile() stream returned AES_CATASTROPHIC");
						this->close();
						TQrecv->closeTransfer();
						TQrecv->destroy();
						TQrecv = NULL;
						return AES_CDH_RC_ERROR;
						break;

					default:
						AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDHRPC::intSendRecordFile() Unknown return value from stream");
						break;
					}
				} // else
			} // while
		} // else
	} // while

	this->close();
	AES_CDH_TRACE_MESSAGE("RPC-client is closed");

			TQrecv->closeTransfer();
	TQrecv->destroy();
	TQrecv = NULL;
	AES_CDH_TRACE_MESSAGE("TQ is closed");

			if (block)
			{
				delete block;
				block = NULL;
			}

	if( ServR::isStopEventSignalled == true )
	{
		returnCode = AES_CDH_RC_TASKSTOPPED;
	}

	if (bTerminate == true)
	{
		bTerminate = false;
		returnCode = AES_CDH_RC_TASKSTOPPED;
	}

	bCease = true;
	AES_CDH_TRACE_MESSAGE("return value: %u", returnCode);

	this->changeAfterTerminate();

	AES_CDH_TRACE_MESSAGE("Leaving");
	return returnCode;
}


//******************************************************************************
//  sendDirectory()            
//******************************************************************************

//##ModelId=3DE49B48027F
AES_CDH_ResultCode CDHRPC::sendDirectory(const string& dirName,      // INGO3 GOH qabhall 010510
                                         const string& remoteSubDirName,
                                         const string& newDirName,
                                         AES_CDH_DestinationSet::transferMode trMode,
                                         const string& fileMask) 
{
	(void)dirName;
	(void)remoteSubDirName;
	(void)newDirName;
	(void)trMode;
	(void)fileMask;
	return AES_CDH_RC_INCUSAGE;
}


//******************************************************************************
//  parseRPC()
//******************************************************************************
//##ModelId=3DE49B490280
AES_CDH_ResultCode CDHRPC::parseRPC(int& argc, char* argv[])
{
	AES_CDH_TRACE_MESSAGE("Entering");
	int opt;
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	int aFlag = 0;
	int mFlag = 0;
	int sFlag = 0;

	ACE_Get_Opt getopt(argc, argv, "a:m:s:");

	while ((opt = getopt()) != EOF)      
	{
		switch (opt) 
		{
		case 'a':
		{
			if (argv[getopt.optind-1][0] == '-')
				return AES_CDH_RC_INCUSAGE;

			hostAddress = getopt.optarg;
			if(AES_CDH_Common_Util::isIPv6Address(hostAddress))		//IPv6_feature:: IPv6 format length limits
			{
				if ((hostAddress.length() < 2) ||
						(hostAddress.length() > 39))
				{
					return AES_CDH_RC_UNREAS; // host_address name too long or not specified
				}
			}
			else
			{														//IPv6_feature:: IPv4 format length limits
				if ((hostAddress.length() < 1) ||
						(hostAddress.length() > 30))
				{
					return AES_CDH_RC_UNREAS; // host_address name too long or not specified
				}
			}

			delete [] argv[getopt.optind-2];
			argv[getopt.optind-2] = NULL;
			delete [] argv[getopt.optind-1];
			argv[getopt.optind-1] = NULL;
			aFlag++;
			break;
		}

		case 'm':
		{
			blockRetryDelay = getopt.optarg;

			if(blockRetryDelay.find_first_not_of("0123456789") != string::npos)
				return AES_CDH_RC_UNREAS;

			int blockRetryDelay_int = atoi(blockRetryDelay.c_str());
			if((blockRetryDelay_int > 10000) || (blockRetryDelay_int < 200))
				return AES_CDH_RC_UNREAS;

			delete [] argv[getopt.optind-2];
			argv[getopt.optind-2] = NULL;
			delete [] argv[getopt.optind-1];
			argv[getopt.optind-1] = NULL;
			mFlag++;
			break;
		}

		case 's':
		{
			blockSendRetries = getopt.optarg;

			if(blockSendRetries.find_first_not_of("0123456789") != string::npos)
				return AES_CDH_RC_UNREAS;

			int blockSendRetries_int = atoi(blockSendRetries.c_str());
			if((blockSendRetries_int > 100) || (blockSendRetries_int < 0))
				return AES_CDH_RC_UNREAS;

			delete [] argv[getopt.optind-2];
			argv[getopt.optind-2] = NULL;
			delete [] argv[getopt.optind-1];
			argv[getopt.optind-1] = NULL;
			sFlag++;
			break;
		}
		case '?':
		{
			if ((getopt.optind + 1) < argc)
			{
				if (argv[getopt.optind][0] != '-' && argv[getopt.optind + 1][0] == '-')
					getopt.optind++;
			}
			break;
		}
		default:
		{
			return AES_CDH_RC_INTPROGERR;
		}
		}
	}

	if (aFlag > 1 || mFlag > 1 || sFlag > 1 )
		returnCode = AES_CDH_RC_INCUSAGE;

	if (aFlag == 0)
		returnCode = AES_CDH_RC_INCUSAGE;

	cleanUpArgv(argc, argv, getopt.optind);

	AES_CDH_TRACE_MESSAGE("Leaving");
	return returnCode;
}  

//******************************************************************************
//  transactionBegin()
//******************************************************************************
//##ModelId=3DE49B4802A8
AES_CDH_ResultCode CDHRPC::transactionBegin()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	return AES_CDH_RC_INCUSAGE;
}


//******************************************************************************
//  transactionEnd()
//******************************************************************************
//##ModelId=3DE49B4802B1
AES_CDH_ResultCode CDHRPC::transactionEnd(unsigned int &translatedBlockNr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	(void)translatedBlockNr;
	return AES_CDH_RC_INCUSAGE;
}


//******************************************************************************
//  transactionCommit()
//******************************************************************************
//##ModelId=3DE49B4802C5
AES_CDH_ResultCode CDHRPC::transactionCommit(unsigned int &applBlockNr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	(void)applBlockNr;
	return AES_CDH_RC_INCUSAGE;
}


//******************************************************************************
//  getLastCommittedBlock()
//******************************************************************************
//##ModelId=3DE49B4802CF
AES_CDH_ResultCode CDHRPC::getLastCommittedBlock(unsigned int &applBlockNr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	(void)applBlockNr;
	return AES_CDH_RC_INCUSAGE;
}


//******************************************************************************
//  transactionTerminate()
//******************************************************************************
//##ModelId=3DE49B4802D9
AES_CDH_ResultCode CDHRPC::transactionTerminate()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	return AES_CDH_RC_INCUSAGE;
}


//******************************************************************************
//  setAPIClosed()
//******************************************************************************
//##ModelId=3DE49B4802DA
AES_CDH_ResultCode CDHRPC::setAPIClosed()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	apiClosed = true;

	// signal to abort any ongoing read operations
	AES_CDH_TRACE_MESSAGE("Signal to abort any ongoing read operations");
	hEvAbortRead->signal();

	AES_CDH_TRACE_MESSAGE("Waiting for InstanceClosed Event");
	hEvInstanceClosed->wait();

	hEvAbortRead->reset();
	apiClosed = false;
	AES_CDH_TRACE_MESSAGE("Leaving");
	return AES_CDH_RC_OK;
}


//******************************************************************************
//  changeAttr()
//******************************************************************************
//##ModelId=3DE49B4802E3
AES_CDH_ResultCode CDHRPC::changeAttr(TransferAgent *agent)
{
	(void) agent;
	return AES_CDH_RC_OK;
}


//******************************************************************************
//  getAgentType()
//******************************************************************************
//##ModelId=3DE49B48029E
TransferAgent::TransferAgentAttributes CDHRPC::getAgentType()
{
	return TransferAgent::BLOCK;
}


//******************************************************************************
//  getVDDestPath()
//******************************************************************************
//##ModelId=3E3E765A0096
std::string CDHRPC::getVDDestPath()
{
	std::string emptyStr;
	return emptyStr;
}

//******************************************************************************
//  readVDPath()
//******************************************************************************
AES_CDH_ResultCode CDHRPC::readVDPath( std::string& path)
{
	(void)path;
	return AES_CDH_RC_INCUSAGE;
}
