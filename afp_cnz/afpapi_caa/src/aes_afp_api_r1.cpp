//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3EAD028E007D.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3EAD028E007D.cm

//## begin module%3EAD028E007D.cp preserve=no
//	INCLUDE aes_afp_api_r1.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
//	   All rights reserved.
//	   The Copyright to the computer program(s) herein
//	   is the property of Ericsson Utvecklings AB, Sweden.
//	   The program(s) may be used and/or copied only with
//	   the written permission from Ericsson Utvecklings AB or
//	   in accordance with the terms and conditions stipulated
//	   in the agreement/contract under which the program(s)
//	   have been supplied.
//
//	DESCRIPTION:
//
//	ERROR HANDLING
//	   General rule:
//	   The error handling is specified for each method.
//	   No methods initiate or send error reports unless
//	   specified.
//
//	DOCUMENT NO
//	   19089-CAA 109 0505
//
//	AUTHOR
//	   2003-04-28  DAPA
//
//	REVISION
//	   A 2003-04-28 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3EAD028E007D.cp

//## Module: aes_afp_api_r1%3EAD028E007D; Package body
//## Subsystem: AFP::afpapi_caa::src%37DF37FE018C
//## Source file: Z:\ntaes\afp\afpapi_caa\src\aes_afp_api_r1.cpp

//## begin module%3EAD028E007D.additionalIncludes preserve=no
//## end module%3EAD028E007D.additionalIncludes

//## begin module%3EAD028E007D.includes preserve=yes
//## end module%3EAD028E007D.includes

#include "aes_afp_apiinternal.h"
#include "aes_afp_api_r3.h"


//## begin module%3EAD028E007D.declarations preserve=no
//## end module%3EAD028E007D.declarations

//## begin module%3EAD028E007D.additionalDeclarations preserve=yes
//## end module%3EAD028E007D.additionalDeclarations


// Class aes_afp_api_r1 

aes_afp_api_r1::aes_afp_api_r1()
  //## begin aes_afp_api_r1::aes_afp_api_r1%3D04AB2C00F3_const.hasinit preserve=no
      : internal_(NULL)
  //## end aes_afp_api_r1::aes_afp_api_r1%3D04AB2C00F3_const.hasinit
  //## begin aes_afp_api_r1::aes_afp_api_r1%3D04AB2C00F3_const.initialization preserve=yes
  //## end aes_afp_api_r1::aes_afp_api_r1%3D04AB2C00F3_const.initialization
{
  //## begin aes_afp_api_r1::aes_afp_api_r1%3D04AB2C00F3_const.body preserve=yes
	internal_ = new aes_afp_apiinternal(this);
  //## end aes_afp_api_r1::aes_afp_api_r1%3D04AB2C00F3_const.body
}


aes_afp_api_r1::~aes_afp_api_r1()
{
  //## begin aes_afp_api_r1::~aes_afp_api_r1%3D04AB2C00F3_dest.body preserve=yes
	delete internal_;
  //## end aes_afp_api_r1::~aes_afp_api_r1%3D04AB2C00F3_dest.body
}



//## Other Operations (implementation)
//## Operation: transferQueueDefined%3D04AB91003A; C++
//## Semantics:
//	---------------------------------------------------------
//	       fileExist()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::transferQueueDefined (std::string& transferQueue, std::string& destinationSet, bool &manual, std::string& tranferQueueDn)
{
  //## begin aes_afp_api_r1::transferQueueDefined%3D04AB91003A.body preserve=yes
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
  //## end aes_afp_api_r1::transferQueueDefined%3D04AB91003A.body
}

//## Operation: getTransferState%3D04AB910044
//## Semantics:
//	---------------------------------------------------------
//	       getTransferState()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::getTransferState (std::string& filename, std::string& destinationSet, AES_GCC_Filestates& transferState)
{
  //## begin aes_afp_api_r1::getTransferState%3D04AB910044.body preserve=yes
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
  //## end aes_afp_api_r1::getTransferState%3D04AB910044.body
}

//## Operation: getTransferStateEx%3D04AB91004E
//## Semantics:
//	---------------------------------------------------------
//	       getFileStatusEx()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::getTransferStateEx (std::string& filename, AES_GCC_Filestates& status, std::string& destinationSet, int listOrder, std::string& reportDate, std::string& deleteDate, std::string& manualStart, std::string& manualStop, std::string& failDate, std::string& archiveDate, int& reasonForFailed, bool& isDirectory)
{
  //## begin aes_afp_api_r1::getTransferStateEx%3D04AB91004E.body preserve=yes
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
  //## end aes_afp_api_r1::getTransferStateEx%3D04AB91004E.body
}

//## Operation: setTransferState%3D04AB91005B
//## Semantics:
//	---------------------------------------------------------
//	    setTransferState()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::setTransferState (std::string& filename, std::string& destinationSet, AES_GCC_Filestates transferState)
{
  //## begin aes_afp_api_r1::setTransferState%3D04AB91005B.body preserve=yes
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
  //## end aes_afp_api_r1::setTransferState%3D04AB91005B.body
}

//## Operation: listFile%3D04AB91005F; C++
//## Semantics:
//	---------------------------------------------------------
//	       listFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::listFile (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream)
{
  //## begin aes_afp_api_r1::listFile%3D04AB91005F.body preserve=yes
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
  //## end aes_afp_api_r1::listFile%3D04AB91005F.body
}

//## Operation: listNewestFile%3D04AB91006C; C++
//## Semantics:
//	---------------------------------------------------------
//	       listYoungestSubfile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::listNewestFile (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream)
{
  //## begin aes_afp_api_r1::listNewestFile%3D04AB91006C.body preserve=yes
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
  //## end aes_afp_api_r1::listNewestFile%3D04AB91006C.body
}

//## Operation: listOldestFile%3D04AB910076; C++
//## Semantics:
//	---------------------------------------------------------
//	       listOldestSubfile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::listOldestFile (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream)
{
  //## begin aes_afp_api_r1::listOldestFile%3D04AB910076.body preserve=yes
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
  //## end aes_afp_api_r1::listOldestFile%3D04AB910076.body
}

//## Operation: listAllFiles%3D04AB91007A; C++
//## Semantics:
//	---------------------------------------------------------
//	       listAllSubfiles()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::listAllFiles (std::string& transferQueue, std::strstream& outStream)
{
  //## begin aes_afp_api_r1::listAllFiles%3D04AB91007A.body preserve=yes
  if (internal_)
    {
      return internal_->listAllFiles(transferQueue,
                                    outStream);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r1::listAllFiles%3D04AB91007A.body
}

//## Operation: listDestinationSets%3D04AB910080; C++
//## Semantics:
//	---------------------------------------------------------
//	       listDests()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::listDestinationSets (std::string& transferQueue, std::strstream& outStream)
{
  //## begin aes_afp_api_r1::listDestinationSets%3D04AB910080.body preserve=yes
  if (internal_)
    {
      return internal_->listDestinationSets(transferQueue,
																					outStream);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r1::listDestinationSets%3D04AB910080.body
}

//## Operation: removeTransferQueue%3D04AB91008D; C++
//## Semantics:
//	---------------------------------------------------------
//	       removeFileDestination()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::removeTransferQueue (std::string& transferQueue)
{
  //## begin aes_afp_api_r1::removeTransferQueue%3D04AB91008D.body preserve=yes
  if (internal_)
    {
      return internal_->removeTransferQueue(transferQueue);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r1::removeTransferQueue%3D04AB91008D.body
}

//## Operation: stopFiles%3D04AB910095; C++
//## Semantics:
//	---------------------------------------------------------
//	       stopFiles()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::stopFiles (std::string& transferQueue, std::string& destinationSet)
{
  //## begin aes_afp_api_r1::stopFiles%3D04AB910095.body preserve=yes
  if (internal_)
    {
      return internal_->stopFiles(transferQueue,
                                 destinationSet);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r1::stopFiles%3D04AB910095.body
}

//## Operation: stopFile%3D04AB91009E; C++
//## Semantics:
//	---------------------------------------------------------
//	       stopFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::stopFile (std::string& transferQueue, std::string& destinationSet, std::string& filename)
{
  //## begin aes_afp_api_r1::stopFile%3D04AB91009E.body preserve=yes
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
  //## end aes_afp_api_r1::stopFile%3D04AB91009E.body
}

//## Operation: getAttributes%3D04AB9100AB; C++
//## Semantics:
//	---------------------------------------------------------
//	       getAttr()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::getAttributes (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream)
{
  //## begin aes_afp_api_r1::getAttributes%3D04AB9100AB.body preserve=yes
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
  //## end aes_afp_api_r1::getAttributes%3D04AB9100AB.body
}

//## Operation: setAttributes%3D04AB9100B2; C++
//## Semantics:
//	---------------------------------------------------------
//	       setAttr()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::setAttributes (std::string& transferQueue, std::string& destinationSet, AES_GCC_Filestates defaultState, int removeDelay, bool removeBefore, int nrOfRetries, int retryTime)
{
  //## begin aes_afp_api_r1::setAttributes%3D04AB9100B2.body preserve=yes
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
  //## end aes_afp_api_r1::setAttributes%3D04AB9100B2.body
}

//## Operation: addDestinationSet%3D04AB9100BC; C++
//## Semantics:
//	---------------------------------------------------------
//	       createDestination()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::addDestinationSet (std::string& transferQueue, std::string& destinationSet, AES_GCC_Filestates defaultState, int removeDelay, bool removeBefore, int nrOfRetries, int retryTime)
{
  //## begin aes_afp_api_r1::addDestinationSet%3D04AB9100BC.body preserve=yes
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
  //## end aes_afp_api_r1::addDestinationSet%3D04AB9100BC.body
}

//## Operation: removeDestinationSet%3D04AB9100C6; C++
//## Semantics:
//	---------------------------------------------------------
//	       removeDestination()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::removeDestinationSet (std::string& transferQueue, std::string& destinationSet)
{
  //## begin aes_afp_api_r1::removeDestinationSet%3D04AB9100C6.body preserve=yes
  if (internal_)
    {
      return internal_->removeDestinationSet(transferQueue,
																						destinationSet);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r1::removeDestinationSet%3D04AB9100C6.body
}

//## Operation: getErrorCodeText%3D04AB9100D0; C++
//## Semantics:
//	---------------------------------------------------------
//	       getErrCodeText()
//	---------------------------------------------------------
const char * aes_afp_api_r1::getErrorCodeText (unsigned int returnCode)
{
  //## begin aes_afp_api_r1::getErrorCodeText%3D04AB9100D0.body preserve=yes
  if (internal_)
    {
      return internal_->getErrorCodeText(returnCode);
    }
  else
    {
      return NULL;
    }
  //## end aes_afp_api_r1::getErrorCodeText%3D04AB9100D0.body
}

//## Operation: lockTransferQueue%3D04AB910152; C++
//## Semantics:
//	---------------------------------------------------------
//	       lockFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::lockTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string& application)
{
  //## begin aes_afp_api_r1::lockTransferQueue%3D04AB910152.body preserve=yes
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
  //## end aes_afp_api_r1::lockTransferQueue%3D04AB910152.body
}

//## Operation: unlockTransferQueue%3D04AB91015C; C++
//## Semantics:
//	---------------------------------------------------------
//	       unlockTransferQueue()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::unlockTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string& application)
{
  //## begin aes_afp_api_r1::unlockTransferQueue%3D04AB91015C.body preserve=yes
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
  //## end aes_afp_api_r1::unlockTransferQueue%3D04AB91015C.body
}

//## Operation: isTransferQueueLocked%3D04AB910160; C++
//## Semantics:
//	---------------------------------------------------------
//	       isFileLocked()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::isTransferQueueLocked (std::string& transferQueue, std::string& destinationSet, bool& locked, std::string& application)
{
  //## begin aes_afp_api_r1::isTransferQueueLocked%3D04AB910160.body preserve=yes
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
  //## end aes_afp_api_r1::isTransferQueueLocked%3D04AB910160.body
}

//## Operation: resendOneFailed%3D04AB91016A; C++
//## Semantics:
//	---------------------------------------------------------
//	       resendOneFailed()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::resendOneFailed (std::string& transferQueue, std::string& destinationSet, std::string& newDestinationSet)
{
  //## begin aes_afp_api_r1::resendOneFailed%3D04AB91016A.body preserve=yes
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
  //## end aes_afp_api_r1::resendOneFailed%3D04AB91016A.body
}

//## Operation: resendAllFailed%3D04AB910170; C++
//## Semantics:
//	---------------------------------------------------------
//	       resendAllFailed()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::resendAllFailed (std::string& transferQueue)
{
  //## begin aes_afp_api_r1::resendAllFailed%3D04AB910170.body preserve=yes
	if (internal_)
    {
      return internal_->resendAllFailed(transferQueue);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api_r1::resendAllFailed%3D04AB910170.body
}

//## Operation: open%3D04AB910184; C++
//## Semantics:
//	---------------------------------------------------------
//	       open()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::open (std::string subsystem, std::string applicationBlock)
{
  //## begin aes_afp_api_r1::open%3D04AB910184.body preserve=yes
	if (internal_)
		{
			return internal_->open(subsystem, applicationBlock);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::open%3D04AB910184.body
}

//## Operation: open%3D04AB910187; C++
//## Semantics:
//	---------------------------------------------------------
//	       open()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::open (std::string& transferQueue)
{
  //## begin aes_afp_api_r1::open%3D04AB910187.body preserve=yes
	if (internal_)
		{
			return internal_->open(transferQueue);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::open%3D04AB910187.body
}

//## Operation: close%3D04AB91018E; C++
//## Semantics:
//	---------------------------------------------------------
//	    close()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::close ()
{
  //## begin aes_afp_api_r1::close%3D04AB91018E.body preserve=yes
	if (internal_)
		{
			return internal_->close();
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::close%3D04AB91018E.body
}

//## Operation: createTransferQueue%3D04AB910198; C++
//## Semantics:
//	---------------------------------------------------------
//	       createFileDestination()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::createTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string sourceDirectory, AES_GCC_Filestates defaultState, int removeDelay, bool removeBefore, int retryTimeInterval, int retryTimes, AES_AFP_Renametypes renameFile, std::string fileTemplate, std::string userGroup)
{
  //## begin aes_afp_api_r1::createTransferQueue%3D04AB910198.body preserve=yes
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
																					 userGroup);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::createTransferQueue%3D04AB910198.body
}

//## Operation: event%3D04AB9101A3; C++
//## Semantics:
//	---------------------------------------------------------
//	       event()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::event (AES_GCC_Eventcodes& eventCode)
{
  //## begin aes_afp_api_r1::event%3D04AB9101A3.body preserve=yes
	return 0;
  //## end aes_afp_api_r1::event%3D04AB9101A3.body
}

//## Operation: getEvent%3D04AB9101A5; C++
//## Semantics:
//	---------------------------------------------------------
//	       getEvent()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::getEvent (AES_GCC_Eventcodes& eventCode, std::string& filename)
{
  //## begin aes_afp_api_r1::getEvent%3D04AB9101A5.body preserve=yes
	if (internal_)
		{
			return internal_->getEvent(eventCode, filename);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::getEvent%3D04AB9101A5.body
}

//## Operation: getSourceDirectoryPath%3D04AB9101AC; C++
//## Semantics:
//	---------------------------------------------------------
//	       getDirectoryPath()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::getSourceDirectoryPath (std::string& transferQueue, std::string& sourceDirectory)
{
  //## begin aes_afp_api_r1::getSourceDirectoryPath%3D04AB9101AC.body preserve=yes
	if (internal_)
		{
			return internal_->getSourceDirectoryPath(transferQueue, sourceDirectory);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::getSourceDirectoryPath%3D04AB9101AC.body
}

//## Operation: setSourceDirectoryPath%3D04AB9101B6; C++
//## Semantics:
//	---------------------------------------------------------
//	       setDirectoryPath()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::setSourceDirectoryPath (std::string& transferQueue, std::string& sourceDirectory)
{
  //## begin aes_afp_api_r1::setSourceDirectoryPath%3D04AB9101B6.body preserve=yes
	if (internal_)
		{
			return internal_->setSourceDirectoryPath(transferQueue, sourceDirectory);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::setSourceDirectoryPath%3D04AB9101B6.body
}

//## Operation: sendFile%3D04AB9101C0; C++
//## Semantics:
//	---------------------------------------------------------
//	       sendFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::sendFile (std::string filename, AES_GCC_Format transferMode, std::string fileMask, bool isDirectory, AES_GCC_Filestates fileState)
{
  //## begin aes_afp_api_r1::sendFile%3D04AB9101C0.body preserve=yes
	if (internal_)
		{
			return internal_->sendFile(filename, transferMode, fileMask, isDirectory);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::sendFile%3D04AB9101C0.body
}

//## Operation: lastReportedFile%3D04AB9101CA; C++
//## Semantics:
//	---------------------------------------------------------
//	       lastReportedFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::lastReportedFile (std::string transferQueue, std::string& originalFileName, std::string& generatedFileName)
{
  //## begin aes_afp_api_r1::lastReportedFile%3D04AB9101CA.body preserve=yes
	if (internal_)
		{
			return internal_->lastReportedFile(transferQueue, originalFileName, generatedFileName);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::lastReportedFile%3D04AB9101CA.body
}

//## Operation: getDestinationSetList%3D04AB9101CE; C++
//## Semantics:
//	---------------------------------------------------------
//	       getDestinationList()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::getDestinationSetList (std::list<std::string>& destinationSetList)
{
  //## begin aes_afp_api_r1::getDestinationSetList%3D04AB9101CE.body preserve=yes
	if (internal_)
		{
			return internal_->getDestinationSetList(destinationSetList);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::getDestinationSetList%3D04AB9101CE.body
}

//## Operation: getFileStatus%3D04AB9101D4; C++
//## Semantics:
//	---------------------------------------------------------
//	       getFileStatus()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::getFileStatus (std::string filename, AES_GCC_Filestates& status)
{
  //## begin aes_afp_api_r1::getFileStatus%3D04AB9101D4.body preserve=yes
	if (internal_)
		{
			return internal_->getFileStatus(filename, status);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::getFileStatus%3D04AB9101D4.body
}

//## Operation: removeFile%3D04AB9101DE; C++
//## Semantics:
//	---------------------------------------------------------
//	       removeFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::removeFile (std::string transferQueue, std::string filename)
{
  //## begin aes_afp_api_r1::removeFile%3D04AB9101DE.body preserve=yes
	if (internal_)
		{
			return internal_->removeFile(transferQueue, filename);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::removeFile%3D04AB9101DE.body
}

//## Operation: removeFile%3D04AB9101E1; C++
//## Semantics:
//	---------------------------------------------------------
//	       removeFile()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::removeFile (std::string transferQueue, std::string destinationSet, std::string filename)
{
  //## begin aes_afp_api_r1::removeFile%3D04AB9101E1.body preserve=yes
	if (internal_)
		{
			return internal_->removeFile(transferQueue, destinationSet, filename);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api_r1::removeFile%3D04AB9101E1.body
}

//## Operation: sendFileManually%3D04AB9101F2; C++
//## Semantics:
//	---------------------------------------------------------
//	       sendFileManually()
//	---------------------------------------------------------
unsigned int aes_afp_api_r1::sendFileManually (std::string transferQueue, std::string userGroup, std::string filename, std::string sourceDirectory, std::string destinationSet, int retryTimes, int retryTimeInterval, bool isDirectory)
{
  //## begin aes_afp_api_r1::sendFileManually%3D04AB9101F2.body preserve=yes
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
  //## end aes_afp_api_r1::sendFileManually%3D04AB9101F2.body
}

// Additional Declarations
  //## begin aes_afp_api_r1%3D04AB2C00F3.declarations preserve=yes
  //## end aes_afp_api_r1%3D04AB2C00F3.declarations

//## begin module%3EAD028E007D.epilog preserve=yes
//## end module%3EAD028E007D.epilog
