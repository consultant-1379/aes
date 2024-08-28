//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3D04AD2700FC.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3D04AD2700FC.cm

//## begin module%3D04AD2700FC.cp preserve=no
//	INCLUDE aes_afp_api_r2.h
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
//	 B 2003-04-28 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3D04AD2700FC.cp

//## Module: aes_afp_api_r2%3D04AD2700FC; Package body
//## Subsystem: AFP::afpapi_caa::src%37DF37FE018C
//## Source file: Z:\ntaes\afp\afpapi_caa\src\aes_afp_api_r2.cpp

//## begin module%3D04AD2700FC.additionalIncludes preserve=no
//## end module%3D04AD2700FC.additionalIncludes

//## begin module%3D04AD2700FC.includes preserve=yes
//## end module%3D04AD2700FC.includes

#include "aes_afp_apiinternal.h"
#include "aes_afp_api_r3.h"


//## begin module%3D04AD2700FC.declarations preserve=no
//## end module%3D04AD2700FC.declarations

//## begin module%3D04AD2700FC.additionalDeclarations preserve=yes
//## end module%3D04AD2700FC.additionalDeclarations


// Class aes_afp_api_r2 

aes_afp_api_r2::aes_afp_api_r2()
  //## begin aes_afp_api_r2::aes_afp_api_r2%3EACEE03032E_const.hasinit preserve=no
      : internal_(NULL)
  //## end aes_afp_api_r2::aes_afp_api_r2%3EACEE03032E_const.hasinit
  //## begin aes_afp_api_r2::aes_afp_api_r2%3EACEE03032E_const.initialization preserve=yes
  //## end aes_afp_api_r2::aes_afp_api_r2%3EACEE03032E_const.initialization
{
  //## begin aes_afp_api_r2::aes_afp_api_r2%3EACEE03032E_const.body preserve=yes
	internal_ = new aes_afp_apiinternal(this);
  //## end aes_afp_api_r2::aes_afp_api_r2%3EACEE03032E_const.body
}


aes_afp_api_r2::~aes_afp_api_r2()
{
  //## begin aes_afp_api_r2::~aes_afp_api_r2%3EACEE03032E_dest.body preserve=yes
	delete internal_;
  //## end aes_afp_api_r2::~aes_afp_api_r2%3EACEE03032E_dest.body
}



//## Other Operations (implementation)
//## Operation: transferQueueDefined%3EACEE3C0109; C++
//## Semantics:
//	---------------------------------------------------------
//	       fileExist()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::transferQueueDefined (std::string& transferQueue, std::string& destinationSet, bool &manual, std::string& tranferQueueDn)
{
  //## begin aes_afp_api_r2::transferQueueDefined%3EACEE3C0109.body preserve=yes
	if (internal_)
    {
      return internal_->transferQueueDefined(transferQueue,
																						destinationSet,
																						manual,

				     tranferQueueDn);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::transferQueueDefined%3EACEE3C0109.body
}

//## Operation: getTransferState%3EACEE3C0113
//## Semantics:
//	---------------------------------------------------------
//	       getTransferState()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::getTransferState (std::string& filename, std::string& destinationSet, AES_GCC_Filestates& transferState)
{
  //## begin aes_afp_api_r2::getTransferState%3EACEE3C0113.body preserve=yes
	if (internal_)
    {
      return internal_->getTransferState(filename,
                                        destinationSet,
                                        transferState);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::getTransferState%3EACEE3C0113.body
}

//## Operation: getTransferStateEx%3EACEE3C0117
//## Semantics:
//	---------------------------------------------------------
//	       getFileStatusEx()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::getTransferStateEx (std::string& filename, AES_GCC_Filestates& status, std::string& destinationSet, int listOrder, std::string& reportDate, std::string& deleteDate, std::string& manualStart, std::string& manualStop, std::string& failDate, std::string& archiveDate, int& reasonForFailed, bool& isDirectory)
{
  //## begin aes_afp_api_r2::getTransferStateEx%3EACEE3C0117.body preserve=yes
	if (internal_)
		{
			return internal_->getTransferStateEx(filename,
																					status,
																					destinationSet,
																					listOrder,
																					reportDate,
																					deleteDate,
																					manualStart,
																					manualStop,
																					failDate,
																					archiveDate,
																					reasonForFailed,
																					isDirectory);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::getTransferStateEx%3EACEE3C0117.body
}

//## Operation: setTransferState%3EACEE3C0124
//## Semantics:
//	---------------------------------------------------------
//	    setTransferState()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::setTransferState (std::string& filename, std::string& destinationSet, AES_GCC_Filestates transferState)
{
  //## begin aes_afp_api_r2::setTransferState%3EACEE3C0124.body preserve=yes
  if (internal_)
    {
      return internal_->setTransferState(filename,
                                        destinationSet,
                                        transferState);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::setTransferState%3EACEE3C0124.body
}

//## Operation: listFile%3EACEE3C0128; C++
//## Semantics:
//	---------------------------------------------------------
//	       listFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::listFile (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream)
{
  //## begin aes_afp_api_r2::listFile%3EACEE3C0128.body preserve=yes
  if (internal_)
    {
      return internal_->listFile(transferQueue,
                                destinationSet,
                                outStream);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::listFile%3EACEE3C0128.body
}

//## Operation: listNewestFile%3EACEE3C0131; C++
//## Semantics:
//	---------------------------------------------------------
//	       listYoungestSubfile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::listNewestFile (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream)
{
  //## begin aes_afp_api_r2::listNewestFile%3EACEE3C0131.body preserve=yes
  if (internal_)
    {
      return internal_->listNewestFile(transferQueue,
                                      destinationSet,
                                      outStream);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::listNewestFile%3EACEE3C0131.body
}

//## Operation: listOldestFile%3EACEE3C0135; C++
//## Semantics:
//	---------------------------------------------------------
//	       listOldestSubfile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::listOldestFile (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream)
{
  //## begin aes_afp_api_r2::listOldestFile%3EACEE3C0135.body preserve=yes
  if (internal_)
    {
      return internal_->listOldestFile(transferQueue,
                                      destinationSet,
                                      outStream);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::listOldestFile%3EACEE3C0135.body
}

//## Operation: listAllFiles%3EACEE3C013E; C++
//## Semantics:
//	---------------------------------------------------------
//	       listAllSubfiles()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::listAllFiles (std::string& transferQueue, std::strstream& outStream)
{
  //## begin aes_afp_api_r2::listAllFiles%3EACEE3C013E.body preserve=yes
  if (internal_)
    {
      return internal_->listAllFiles(transferQueue,
                                    outStream);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::listAllFiles%3EACEE3C013E.body
}

//## Operation: listDestinationSets%3EACEE3C0145; C++
//## Semantics:
//	---------------------------------------------------------
//	       listDests()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::listDestinationSets (std::string& transferQueue, std::strstream& outStream)
{
  //## begin aes_afp_api_r2::listDestinationSets%3EACEE3C0145.body preserve=yes
  if (internal_)
    {
      return internal_->listDestinationSets(transferQueue,
																					outStream);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::listDestinationSets%3EACEE3C0145.body
}

//## Operation: removeTransferQueue%3EACEE3C0148; C++
//## Semantics:
//	---------------------------------------------------------
//	       removeFileDestination()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::removeTransferQueue (std::string& transferQueue)
{
  //## begin aes_afp_api_r2::removeTransferQueue%3EACEE3C0148.body preserve=yes
  if (internal_)
    {
      return internal_->removeTransferQueue(transferQueue);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::removeTransferQueue%3EACEE3C0148.body
}

//## Operation: stopFiles%3EACEE3C014F; C++
//## Semantics:
//	---------------------------------------------------------
//	       stopFiles()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::stopFiles (std::string& transferQueue, std::string& destinationSet)
{
  //## begin aes_afp_api_r2::stopFiles%3EACEE3C014F.body preserve=yes
  if (internal_)
    {
      return internal_->stopFiles(transferQueue,
                                 destinationSet);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::stopFiles%3EACEE3C014F.body
}

//## Operation: stopFile%3EACEE3C0159; C++
//## Semantics:
//	---------------------------------------------------------
//	       stopFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::stopFile (std::string& transferQueue, std::string& destinationSet, std::string& filename)
{
  //## begin aes_afp_api_r2::stopFile%3EACEE3C0159.body preserve=yes
	if (internal_)
    {
      return internal_->stopFile(transferQueue,
                                destinationSet,
																filename);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::stopFile%3EACEE3C0159.body
}

//## Operation: getAttributes%3EACEE3C015D; C++
//## Semantics:
//	---------------------------------------------------------
//	       getAttr()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::getAttributes (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream)
{
  //## begin aes_afp_api_r2::getAttributes%3EACEE3C015D.body preserve=yes
  if (internal_)
    {
      return internal_->getAttributes(transferQueue,
																		destinationSet,
																		outStream);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::getAttributes%3EACEE3C015D.body
}

//## Operation: setAttributes%3EACEE3C0166; C++
//## Semantics:
//	---------------------------------------------------------
//	       setAttr()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::setAttributes (std::string& transferQueue, std::string& destinationSet, AES_GCC_Filestates defaultState, int removeDelay, bool removeBefore, int nrOfRetries, int retryTime)
{
  //## begin aes_afp_api_r2::setAttributes%3EACEE3C0166.body preserve=yes
  if (internal_)
    {
      return internal_->setAttributes(transferQueue,
																			destinationSet,
																			defaultState,
																			removeDelay,
																			removeBefore,
																			nrOfRetries,
																			retryTime);
    }
  else
    {
			return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::setAttributes%3EACEE3C0166.body
}

//## Operation: addDestinationSet%3EACEE3C0177; C++
//## Semantics:
//	---------------------------------------------------------
//	       createDestination()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::addDestinationSet (std::string& transferQueue, std::string& destinationSet, AES_GCC_Filestates defaultState, int removeDelay, bool removeBefore, int nrOfRetries, int retryTime)
{
  //## begin aes_afp_api_r2::addDestinationSet%3EACEE3C0177.body preserve=yes
  if (internal_)
    {
      return internal_->addDestinationSet(transferQueue,
                                         destinationSet,
                                         defaultState,
                                         removeDelay,
                                         removeBefore,
                                         nrOfRetries,
                                         retryTime);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::addDestinationSet%3EACEE3C0177.body
}

//## Operation: removeDestinationSet%3EACEE3C0181; C++
//## Semantics:
//	---------------------------------------------------------
//	       removeDestination()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::removeDestinationSet (std::string& transferQueue, std::string& destinationSet)
{
  //## begin aes_afp_api_r2::removeDestinationSet%3EACEE3C0181.body preserve=yes
  if (internal_)
    {
      return internal_->removeDestinationSet(transferQueue,
																						destinationSet);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::removeDestinationSet%3EACEE3C0181.body
}

//## Operation: getErrorCodeText%3EACEE3C018B; C++
//## Semantics:
//	---------------------------------------------------------
//	       getErrCodeText()
//	---------------------------------------------------------
const char * aes_afp_api_r2::getErrorCodeText (unsigned int returnCode)
{
  //## begin aes_afp_api_r2::getErrorCodeText%3EACEE3C018B.body preserve=yes
  if (internal_)
    {
      return internal_->getErrorCodeText(returnCode);
    }
  else
    {
      return NULL;
    }
  //## end aes_afp_api_r2::getErrorCodeText%3EACEE3C018B.body
}

//## Operation: lockTransferQueue%3EACEE3C01EF; C++
//## Semantics:
//	---------------------------------------------------------
//	       lockFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::lockTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string& application)
{
  //## begin aes_afp_api_r2::lockTransferQueue%3EACEE3C01EF.body preserve=yes
	if (internal_)
    {
      return internal_->lockTransferQueue(transferQueue,
																					destinationSet,
																					application);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::lockTransferQueue%3EACEE3C01EF.body
}

//## Operation: unlockTransferQueue%3EACEE3C01F9; C++
//## Semantics:
//	---------------------------------------------------------
//	       unlockTransferQueue()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::unlockTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string& application)
{
  //## begin aes_afp_api_r2::unlockTransferQueue%3EACEE3C01F9.body preserve=yes
	if (internal_)
    {
      return internal_->unlockTransferQueue(transferQueue,
																						destinationSet,
																						application);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::unlockTransferQueue%3EACEE3C01F9.body
}

//## Operation: isTransferQueueLocked%3EACEE3C0221; C++
//## Semantics:
//	---------------------------------------------------------
//	       isFileLocked()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::isTransferQueueLocked (std::string& transferQueue, std::string& destinationSet, bool& locked, std::string& application)
{
  //## begin aes_afp_api_r2::isTransferQueueLocked%3EACEE3C0221.body preserve=yes
	if (internal_)
    {
      return internal_->isTransferQueueLocked(transferQueue,
																							destinationSet,
																							locked,
																							application);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::isTransferQueueLocked%3EACEE3C0221.body
}

//## Operation: resendOneFailed%3EACEE3C0394; C++
//## Semantics:
//	---------------------------------------------------------
//	       resendOneFailed()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::resendOneFailed (std::string& transferQueue, std::string& destinationSet, std::string& newDestinationSet)
{
  //## begin aes_afp_api_r2::resendOneFailed%3EACEE3C0394.body preserve=yes
	if (internal_)
    {
      return internal_->resendOneFailed(transferQueue,
																				destinationSet,
																				newDestinationSet);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::resendOneFailed%3EACEE3C0394.body
}

//## Operation: resendAllFailed%3EACEE3C03A8; C++
//## Semantics:
//	---------------------------------------------------------
//	       resendAllFailed()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::resendAllFailed (std::string& transferQueue)
{
  //## begin aes_afp_api_r2::resendAllFailed%3EACEE3C03A8.body preserve=yes
	if (internal_)
    {
      return internal_->resendAllFailed(transferQueue);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r2::resendAllFailed%3EACEE3C03A8.body
}

//## Operation: open%3EACEE3C03B2; C++
//## Semantics:
//	---------------------------------------------------------
//	       open()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::open (std::string subsystem, std::string applicationBlock)
{
  //## begin aes_afp_api_r2::open%3EACEE3C03B2.body preserve=yes
	if (internal_)
		{
			return internal_->open(subsystem, applicationBlock);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::open%3EACEE3C03B2.body
}

//## Operation: open%3EACEE3C03BC; C++
//## Semantics:
//	---------------------------------------------------------
//	       open()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::open (std::string& transferQueue)
{
  //## begin aes_afp_api_r2::open%3EACEE3C03BC.body preserve=yes
	if (internal_)
		{
			return internal_->open(transferQueue);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::open%3EACEE3C03BC.body
}

//## Operation: close%3EACEE3C03C6; C++
//## Semantics:
//	---------------------------------------------------------
//	    close()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::close ()
{
  //## begin aes_afp_api_r2::close%3EACEE3C03C6.body preserve=yes
	if (internal_)
		{
			return internal_->close();
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::close%3EACEE3C03C6.body
}

//## Operation: createTransferQueue%3EACEE3C03D0; C++
//## Semantics:
//	---------------------------------------------------------
//	       createTransferQueue()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::createTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string sourceDirectory, AES_GCC_Filestates defaultState, int removeDelay, bool removeBefore, int retryTimeInterval, int retryTimes, AES_AFP_Renametypes renameFile, std::string fileTemplate, std::string userGroup, std::string nameTag)
{
  //## begin aes_afp_api_r2::createTransferQueue%3EACEE3C03D0.body preserve=yes
	if (internal_)
		{
			return internal_->createTransferQueue(transferQueue,
																					 destinationSet,
																					 sourceDirectory,
																					 defaultState,
																					 removeDelay,
																					 removeBefore,
																					 retryTimeInterval,
																					 retryTimes,
																					 renameFile,
																					 fileTemplate,
																					 userGroup,
																					 nameTag);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::createTransferQueue%3EACEE3C03D0.body
}

//## Operation: event%3EACEE3C03DC; C++
//## Semantics:
//	---------------------------------------------------------
//	       event()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::event (AES_GCC_Eventcodes& eventCode)
{
  //## begin aes_afp_api_r2::event%3EACEE3C03DC.body preserve=yes
	return 0;
  //## end aes_afp_api_r2::event%3EACEE3C03DC.body
}

//## Operation: getEvent%3EACEE3D0006; C++
//## Semantics:
//	---------------------------------------------------------
//	       getEvent()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::getEvent (AES_GCC_Eventcodes& eventCode, std::string& filename)
{
  //## begin aes_afp_api_r2::getEvent%3EACEE3D0006.body preserve=yes
	if (internal_)
		{
			return internal_->getEvent(eventCode, filename);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::getEvent%3EACEE3D0006.body
}

//## Operation: getSourceDirectoryPath%3EACEE3D001A; C++
//## Semantics:
//	---------------------------------------------------------
//	       getDirectoryPath()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::getSourceDirectoryPath (std::string& transferQueue, std::string& sourceDirectory)
{
  //## begin aes_afp_api_r2::getSourceDirectoryPath%3EACEE3D001A.body preserve=yes
	if (internal_)
		{
			return internal_->getSourceDirectoryPath(transferQueue, sourceDirectory);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::getSourceDirectoryPath%3EACEE3D001A.body
}

//## Operation: setSourceDirectoryPath%3EACEE3D0024; C++
//## Semantics:
//	---------------------------------------------------------
//	       setDirectoryPath()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::setSourceDirectoryPath (std::string& transferQueue, std::string& sourceDirectory)
{
  //## begin aes_afp_api_r2::setSourceDirectoryPath%3EACEE3D0024.body preserve=yes
	if (internal_)
		{
			return internal_->setSourceDirectoryPath(transferQueue, sourceDirectory);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::setSourceDirectoryPath%3EACEE3D0024.body
}

//## Operation: sendFile%3EACEE3D0027; C++
//## Semantics:
//	---------------------------------------------------------
//	       sendFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::sendFile (std::string filename, AES_GCC_Format transferMode, std::string fileMask, bool isDirectory, AES_GCC_Filestates fileState)
{
  //## begin aes_afp_api_r2::sendFile%3EACEE3D0027.body preserve=yes
	if (internal_)
		{
			return internal_->sendFile(filename, transferMode, fileMask, isDirectory);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::sendFile%3EACEE3D0027.body
}

//## Operation: lastReportedFile%3EACEE3D002E; C++
//## Semantics:
//	---------------------------------------------------------
//	       lastReportedFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::lastReportedFile (std::string transferQueue, std::string& originalFileName, std::string& generatedFileName)
{
  //## begin aes_afp_api_r2::lastReportedFile%3EACEE3D002E.body preserve=yes
	if (internal_)
		{
			return internal_->lastReportedFile(transferQueue, originalFileName, generatedFileName);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::lastReportedFile%3EACEE3D002E.body
}

//## Operation: getDestinationSetList%3EACEE3D0038; C++
//## Semantics:
//	---------------------------------------------------------
//	       getDestinationList()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::getDestinationSetList (std::list<std::string>& destinationSetList)
{
  //## begin aes_afp_api_r2::getDestinationSetList%3EACEE3D0038.body preserve=yes
	if (internal_)
		{
			return internal_->getDestinationSetList(destinationSetList);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::getDestinationSetList%3EACEE3D0038.body
}

//## Operation: getFileStatus%3EACEE3D0042; C++
//## Semantics:
//	---------------------------------------------------------
//	       getFileStatus()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::getFileStatus (std::string filename, AES_GCC_Filestates& status)
{
  //## begin aes_afp_api_r2::getFileStatus%3EACEE3D0042.body preserve=yes
	if (internal_)
		{
			return internal_->getFileStatus(filename, status);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::getFileStatus%3EACEE3D0042.body
}

//## Operation: removeFile%3EACEE3D004C; C++
//## Semantics:
//	---------------------------------------------------------
//	       removeFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::removeFile (std::string transferQueue, std::string filename)
{
  //## begin aes_afp_api_r2::removeFile%3EACEE3D004C.body preserve=yes
	if (internal_)
		{
			return internal_->removeFile(transferQueue, filename);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::removeFile%3EACEE3D004C.body
}

//## Operation: removeFile%3EACEE3D0056; C++
//## Semantics:
//	---------------------------------------------------------
//	       removeFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::removeFile (std::string transferQueue, std::string destinationSet, std::string filename)
{
  //## begin aes_afp_api_r2::removeFile%3EACEE3D0056.body preserve=yes
	if (internal_)
		{
			return internal_->removeFile(transferQueue, destinationSet, filename);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::removeFile%3EACEE3D0056.body
}

//## Operation: sendFileManually%3EACEE3D005A; C++
//## Semantics:
//	---------------------------------------------------------
//	       sendFileManually()
//	---------------------------------------------------------
unsigned int aes_afp_api_r2::sendFileManually (std::string transferQueue, std::string userGroup, std::string filename, std::string sourceDirectory, std::string destinationSet, int retryTimes, int retryTimeInterval, bool isDirectory)
{
  //## begin aes_afp_api_r2::sendFileManually%3EACEE3D005A.body preserve=yes
	if (internal_)
		{
			return internal_->sendFileManually(transferQueue,
																				userGroup,
																				filename,
																				sourceDirectory,
																				destinationSet,
																				retryTimes,
																				retryTimeInterval,
																				isDirectory);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r2::sendFileManually%3EACEE3D005A.body
}

// Additional Declarations
  //## begin aes_afp_api_r2%3EACEE03032E.declarations preserve=yes
  //## end aes_afp_api_r2%3EACEE03032E.declarations

//## begin module%3D04AD2700FC.epilog preserve=yes
//## end module%3D04AD2700FC.epilog
