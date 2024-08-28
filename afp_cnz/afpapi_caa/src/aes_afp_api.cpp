#include "aes_afp_apiinternal.h"
#include "aes_afp_api.h"
#include "aes_afp_api_tracer.h"

AES_AFP_TRACE_DEFINE(AES_AFP_Api);

AES_AFP_Api::AES_AFP_Api() : internal_(NULL)
{
   internal_ = new aes_afp_apiinternal(this);
}


AES_AFP_Api::~AES_AFP_Api()
{
   if (internal_)
   {
      delete internal_;
      internal_ = NULL;
   }
}



//## Other Operations (implementation)
//## Operation: transferQueueDefined%3EACEE3C0109; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	fileExist()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::transferQueueDefined(std::string& transferQueue,
                                               std::string& destinationSet,
                                               bool &manual,
					       std::string& transferQueueDn)
{
   if (internal_)
   {
      return internal_->transferQueueDefined(transferQueue,
         destinationSet,
         manual,
	 transferQueueDn);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: getTransferState%3EACEE3C0113
//## Semantics:
//	----------------------------------------------------------------------------
//	getTransferState()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::getTransferState(std::string& filename,
                                           std::string& destinationSet,
                                           AES_GCC_Filestates& transferState)
{
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
}

//## Operation: getTransferStateEx%3EACEE3C0117
//## Semantics:
//	----------------------------------------------------------------------------
//	getFileStatusEx()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::getTransferStateEx(std::string& filename,
                                             AES_GCC_Filestates& status,
                                             std::string& destinationSet,
                                             int listOrder,
                                             std::string& reportDate,
                                             std::string& deleteDate,
                                             std::string& manualStart,
                                             std::string& manualStop,
                                             std::string& failDate,
                                             std::string& archiveDate,
                                             int& reasonForFailed,
                                             bool& isDirectory)
{
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
}

//## Operation: setTransferState%3EACEE3C0124
//## Semantics:
//	----------------------------------------------------------------------------
//	setTransferState()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::setTransferState(std::string& filename,
                                           std::string& destinationSet,
                                           AES_GCC_Filestates transferState)
{
   if (internal_)
   {
      return internal_->setTransferState(filename, destinationSet, transferState);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: listFile%3EACEE3C0128; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	listFile()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::listFile(std::string& transferQueue,
                                   std::string& destinationSet,
                                   std::stringstream& outStream)
{
   if (internal_)
   {
      return internal_->listFile(transferQueue, destinationSet, outStream);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: listNewestFile%3EACEE3C0131; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	listYoungestSubfile()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::listNewestFile(std::string& transferQueue,
                                         std::string& destinationSet,
                                         std::stringstream& outStream)
{
   if (internal_)
   {
      return internal_->listNewestFile(transferQueue, destinationSet, outStream);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: listOldestFile%3EACEE3C0135; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	listOldestSubfile()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::listOldestFile(std::string& transferQueue,
                                         std::string& destinationSet,
                                         std::stringstream& outStream)
{
   if (internal_)
   {
      return internal_->listOldestFile(transferQueue, destinationSet, outStream);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: listAllFiles%3EACEE3C013E; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	listAllSubfiles()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::listAllFiles(std::string& transferQueue,
                                       std::stringstream& outStream)
{
   if (internal_)
   {
      return internal_->listAllFiles(transferQueue, outStream);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

unsigned int AES_AFP_Api::listAllFilesShort(std::string& transferQueue,
                                       std::stringstream& outStream)
{
   if (internal_)
   {
      return internal_->listAllFilesShort(transferQueue, outStream);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}


//## Operation: listDestinationSets%3EACEE3C0145; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	listDests()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::listDestinationSets(std::string& transferQueue,
                                              std::stringstream& outStream)
{
   if (internal_)
   {
      return internal_->listDestinationSets(transferQueue, outStream);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: removeTransferQueue%3EACEE3C0148; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	removeFileDestination()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::removeTransferQueue(std::string& transferQueue)
{
   if (internal_)
   {
      return internal_->removeTransferQueue(transferQueue);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: stopFiles%3EACEE3C014F; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	stopFiles()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::stopFiles(std::string& transferQueue,
                                    std::string& destinationSet)
{
   if (internal_)
   {
      return internal_->stopFiles(transferQueue, destinationSet);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: stopFile%3EACEE3C0159; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	stopFile()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::stopFile(std::string& transferQueue,
                                   std::string& destinationSet,
                                   std::string& filename)
{
   if (internal_)
   {
      return internal_->stopFile(transferQueue, destinationSet, filename);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: getAttributes%3EACEE3C015D; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	getAttr()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::getAttributes(std::string& transferQueue,
                                        std::string& destinationSet,
                                        std::stringstream& outStream)
{
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
}

//## Operation: setAttributes%3EACEE3C0166; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	setAttr()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::setAttributes(std::string& transferQueue,
                                        std::string& destinationSet,
                                        AES_GCC_Filestates defaultState,
                                        int removeDelay,
                                        bool removeBefore,
                                        int nrOfRetries,
                                        int retryTime)
{
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
}

//## Operation: addDestinationSet%3EACEE3C0177; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	createDestination()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::addDestinationSet(std::string& transferQueue,
                                            std::string& destinationSet,
                                            AES_GCC_Filestates defaultState,
                                            int removeDelay,
                                            bool removeBefore,
                                            int nrOfRetries,
                                            int retryTime)
{
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
}

//## Operation: removeDestinationSet%3EACEE3C0181; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	removeDestination()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::removeDestinationSet(std::string& transferQueue,
                                               std::string& destinationSet)
{
   if (internal_)
   {
      return internal_->removeDestinationSet(transferQueue, destinationSet);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: getErrorCodeText%3EACEE3C018B; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	getErrCodeText()
//	----------------------------------------------------------------------------
const char * AES_AFP_Api::getErrorCodeText(unsigned int returnCode)
{
   if (internal_)
   {
      return internal_->getErrorCodeText(returnCode);
   }
   else
   {
      return NULL;
   }
}

//## Operation: lockTransferQueue%3EACEE3C01EF; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	lockFile()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::lockTransferQueue(std::string& transferQueue,
                                            std::string& destinationSet,
                                            std::string& application)
{
   if (internal_)
   {
      return internal_->lockTransferQueue(transferQueue, destinationSet, application);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: unlockTransferQueue%3EACEE3C01F9; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	unlockTransferQueue()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::unlockTransferQueue(std::string& transferQueue,
                                              std::string& destinationSet,
                                              std::string& application)
{
   if (internal_)
   {
      return internal_->unlockTransferQueue(transferQueue, destinationSet, application);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: isTransferQueueLocked%3EACEE3C0221; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	isFileLocked()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::isTransferQueueLocked(std::string& transferQueue,
                                                std::string& destinationSet,
                                                bool& locked,
                                                std::string& application)
{
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
}

//## Operation: resendOneFailed%3EACEE3C0394; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	resendOneFailed()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::resendOneFailed(std::string& transferQueue,
                                          std::string& destinationSet,
                                          std::string& newDestinationSet)
{
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
}

//## Operation: resendAllFailed%3EACEE3C03A8; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	resendAllFailed()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::resendAllFailed(std::string& transferQueue)
{
   if (internal_)
   {
      return internal_->resendAllFailed(transferQueue);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: open%3EACEE3C03B2; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	open()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::open(std::string subsystem,
                               std::string applicationBlock)
{
   if (internal_)
   {
      return internal_->open(subsystem, applicationBlock);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: open%3EACEE3C03BC; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	open()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::open(std::string& transferQueue, int app)
{
   if (internal_)
   {
      return internal_->open(transferQueue, app);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: close%3EACEE3C03C6; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	close()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::close()
{
   if (internal_)
   {
      return internal_->close();
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: createTransferQueue%3EACEE3C03D0; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	createTransferQueue()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::createTransferQueue(std::string& transferQueue,
                                              std::string& destinationSet,
                                              std::string sourceDirectory,
                                              AES_GCC_Filestates defaultState,
                                              int removeDelay,
                                              bool removeBefore,
                                              int retryTimeInterval,
                                              int retryTimes,
                                              AES_AFP_Renametypes renameFile,
                                              std::string fileTemplate,
                                              std::string userGroup,
                                              std::string nameTag,
                                              long startupSequenceNumber,
                                              bool treatAsDefaultValue)
{
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
                                            nameTag,
                                            startupSequenceNumber,
                                            treatAsDefaultValue);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: event%3EACEE3C03DC; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	event()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::event(AES_GCC_Eventcodes& eventCode)
{
	(void)(eventCode);
	AES_AFP_TRACE_MESSAGE("eventCode = %d",eventCode);
	return 0;
}

//## Operation: getEvent%3EACEE3D0006; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	getEvent()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::getEvent(AES_GCC_Eventcodes& eventCode,
                                   std::string& filename)
{
   if (internal_)
   {
      return internal_->getEvent(eventCode, filename);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: getSourceDirectoryPath%3EACEE3D001A; C++
//## Semantics:
//	----------------------------------------------------------------------------
// getDirectoryPath()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::getSourceDirectoryPath(std::string& transferQueue,
                                                 std::string& sourceDirectory)
{
   if (internal_)
   {
      return internal_->getSourceDirectoryPath(transferQueue, sourceDirectory);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: setSourceDirectoryPath%3EACEE3D0024; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	setDirectoryPath()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::setSourceDirectoryPath(std::string& transferQueue,
                                                 std::string& sourceDirectory)
{
   if (internal_)
   {
      return internal_->setSourceDirectoryPath(transferQueue, sourceDirectory);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: sendFile%3EACEE3D0027; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	sendFile()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::sendFile(std::string filename,
                                   AES_GCC_Format transferMode,
                                   std::string fileMask,
                                   bool isDirectory,
                                   AES_GCC_Filestates fileState)
{
	(void)(fileState);
	AES_AFP_TRACE_MESSAGE("fileState = %d",fileState);
   if (internal_)
   {
      return internal_->sendFile(filename, transferMode, fileMask, isDirectory);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: lastReportedFile%3EACEE3D002E; C++
//## Semantics:
//	----------------------------------------------------------------------------
// lastReportedFile()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::lastReportedFile(std::string transferQueue,
                                           std::string& originalFileName,
                                           std::string& generatedFileName)
{
   if (internal_)
   {
      return internal_->lastReportedFile(transferQueue,
                                         originalFileName,
                                         generatedFileName);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: getDestinationSetList%3EACEE3D0038; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	getDestinationList()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::getDestinationSetList(std::list<std::string>& destinationSetList)
{
   if (internal_)
   {
      return internal_->getDestinationSetList(destinationSetList);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: getFileStatus%3EACEE3D0042; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	getFileStatus()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::getFileStatus(std::string filename,
                                        AES_GCC_Filestates& status)
{
   if (internal_)
   {
      return internal_->getFileStatus(filename, status);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: removeFile%3EACEE3D004C; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	removeFile()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::removeFile(std::string transferQueue,
                                     std::string filename)
{
   if (internal_)
   {
      return internal_->removeFile(transferQueue, filename);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: removeFile%3EACEE3D0056; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	removeFile()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::removeFile(std::string transferQueue,
                                     std::string destinationSet,
                                     std::string filename)
{
   if (internal_)
   {
      return internal_->removeFile(transferQueue, destinationSet, filename);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

//## Operation: sendFileManually%3EACEE3D005A; C++
//## Semantics:
//	----------------------------------------------------------------------------
//	sendFileManually()
//	----------------------------------------------------------------------------
unsigned int AES_AFP_Api::sendFileManually(std::string transferQueue,
                                           std::string userGroup,
                                           std::string filename,
                                           std::string sourceDirectory,
                                           std::string destinationSet,
                                           int retryTimes,
                                           int retryTimeInterval,
                                           bool isDirectory)
{
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
}
//	---------------------------------------------------------
//	       removeFileFromSourceDir()
//	------------------------------------------------------
unsigned int AES_AFP_Api::removeFileFromSourceDir(std::string transferQueue, std::string filename)	//for APZ21230/4-699
{
  //## begin aes_afp_api::removeFileFromSourceDir%3EACEE3D004C.body preserve=yes
	if (internal_)
		{
			return internal_->removeFileFromSourceDir(transferQueue, filename);
		}
	else
		{
			return AES_NOSERVERACCESS;
		}
  //## end aes_afp_api::removeFileFromSourceDir%3EACEE3D004C.body
}

//## Operation: removeFile%3EACEE3D0056; C++
//## Semantics:

//	---------------------------------------------------------
//	       removeSourceDir()
//	---------------------------------------------------------
unsigned int AES_AFP_Api::removeSourceDirFiles(std::string& transferQueue)	//for APZ21230/4-699
{
  //## begin aes_afp_api::removeSourceDir%3EACEE3C0148.body preserve=yes
  if (internal_)
    {
      return internal_->removeSourceDirFiles(transferQueue);
    }
  else
    {
      return AES_NOSERVERACCESS;
    }
  //## end aes_afp_api::removeSourceDir%3EACEE3C0148.body
}

unsigned int AES_AFP_Api::validateTransferQueueParam(
                                  std::string& nameTag,
                                  std::string fileTemplate,
                                  AES_GCC_Filestates defaultState,
                                  int removeDelay,
                                  int retryTimeInterval,
                                  int retryTimes,
								  //Start - HT50930
                                  int startupSequenceNumber
                                  //End - HT50930
                                  )

{
   if (internal_)
   {
	    return internal_->validateTransferQueueParam(
													nameTag,
													fileTemplate,
													defaultState,
													removeDelay,
                                                    retryTimeInterval,
													retryTimes,
													//Start - HT50930
													startupSequenceNumber
													//End - HT50930
													);
   }
   else
   {
      return AES_NOSERVERACCESS;
   }
}

unsigned int AES_AFP_Api::validateDestinationSet(std::string& destinationSet)
{
   if (internal_)
   {
           return internal_->validateDestinationSet(destinationSet);
   }
   else
   {
           return AES_NOSERVERACCESS;
   }
}

unsigned int AES_AFP_Api::validateTqName(std::string& transferQueue)
{
   if(internal_)
   {
           return internal_->validateTqName(transferQueue);
   }
   else
   {
           return AES_NOSERVERACCESS;
   }
}

unsigned int AES_AFP_Api::validateRenameTemplate( std::string& nameTag, std::string& templateString)
{
	if(internal_)
	{
		return internal_->validateRenameTemplate(nameTag,templateString);
	}
	else
	{
		return AES_NOSERVERACCESS;
	}

}

unsigned int AES_AFP_Api::checkDefaultStatus (unsigned int defaultState, std::string& destinationSet)
{
	if(internal_)
	{
		return internal_->checkDefaultStatus(defaultState,destinationSet);
	}
	else
	{
		return AES_NOSERVERACCESS;
	}
}
