#include <aes_afp_datablock.h>
// Class aes_afp_datablock 

aes_afp_datablock::aes_afp_datablock()

      : m_dType(DUMMY),
	m_nCount(0),
	removeDelay_(0),
        sendRetries_(0),
        retriesDelay_(0),
        removeBefore_(false),
        manual_(false),
        lock_(false),
        isDirectory_(false),
        afpRestarted_(false),
        fileGeneration_(""),
        transferMask_(""),
        rangeEnd_(""),
        listOperations_(""),
        rangeBegin_(""),
        transferQueue_(""),
        filename_(""),
        path_(""),
        strBuf_(""),
        username_(""),
        destinationSet_(""),
        sourceDirectory_(""),
        renameTemplate_(""),
        newDestinationSet_(""),
        userGroup_(""),
        generation_(""),
        application_(""),
        newFilename_(""),
	originalfilename_(""),                                      //HU96961
        sendDate_(""),
        readyDate_(""),
        deleteDate_(""),
        noneDate_(""),
        archiveDate_(""),
        stopDate_(""),
        failedDate_(""),
        nameTag_(""),
        startupSequenceNumber_(-1),
	status_(AES_FSNONE),
        messageType_(UNKNOWN),
	errorCode_(AES_NOERRORCODE),
	transferFormat_(AES_DEFAULT),
        rename_(AES_RTUNKNOWN),
        dsdChannel_(NULL)

{

}

aes_afp_datablock::~aes_afp_datablock()
{

}

void aes_afp_datablock::setTransferQueue (std::string transferQueue)
{
  transferQueue_ = transferQueue;
}

std::string aes_afp_datablock::getTransferQueue ()
{
  return transferQueue_;
}

void aes_afp_datablock::setFilename (std::string filename)
{
  filename_ = filename;
}

std::string aes_afp_datablock::getFilename ()
{
  return filename_;
}

void aes_afp_datablock::setGeneration (std::string generation)
{
  generation_ = generation;
}

std::string& aes_afp_datablock::getGeneration ()
{

  return generation_;

}

void aes_afp_datablock::setDestinationSet (std::string destinationSet)
{
  destinationSet_ = destinationSet;
}

std::string& aes_afp_datablock::getDestinationSet ()
{
  return destinationSet_;
}

void aes_afp_datablock::setStatus (AES_GCC_Filestates status)
{
  status_ = status;
}

AES_GCC_Filestates aes_afp_datablock::getStatus () const
{
  return status_;
}

void aes_afp_datablock::setRemoveDelay (int removeDelay)
{
   removeDelay_ = removeDelay;
}

int aes_afp_datablock::getRemoveDelay ()const
{
  return removeDelay_;
}

void aes_afp_datablock::setReadyDate (std::string date)
{
  readyDate_ = date;
}

std::string aes_afp_datablock::getReadyDate ()const
{
  return readyDate_;
}

void aes_afp_datablock::setSendDate (std::string date)
{
  sendDate_ = date;
}

std::string aes_afp_datablock::getSendDate ()const
{
  return sendDate_;
}

void aes_afp_datablock::setFailedDate (std::string date)
{
    failedDate_ = date;
}

std::string aes_afp_datablock::getFailedDate ()const
{
  return failedDate_;
}

void aes_afp_datablock::setDeleteDate (std::string date)
{
  deleteDate_ = date;
}

std::string aes_afp_datablock::getDeleteDate ()const
{
  return deleteDate_;
}

void aes_afp_datablock::setNoneDate (std::string date)
{
  noneDate_ = date;
}

std::string aes_afp_datablock::getNoneDate ()const
{
  return noneDate_;
}

void aes_afp_datablock::setArchiveDate (std::string date)
{
  archiveDate_ = date;
}
std::string aes_afp_datablock::getArchiveDate ()const
{
   return archiveDate_;
 }

void aes_afp_datablock::setStoppedDate (std::string date)
{
	stopDate_ = date;
}

std::string aes_afp_datablock::getStoppedDate ()const
{
	return stopDate_;
}

std::string& aes_afp_datablock::getStrBuf ()
{
	return strBuf_;
}

void aes_afp_datablock::setSendRetries (int retries)
{
  sendRetries_ = retries;
}
std::string& aes_afp_datablock::getOriginalFileName()
{
	return originalfilename_;                                 //HU96961
}

void aes_afp_datablock::setOriginalFileName(std::string orgFileName)
{ 
	originalfilename_ = orgFileName;			  //HU96961
}
int aes_afp_datablock::getSendRetries ()const
{
  return sendRetries_;
}

void aes_afp_datablock::setMsgType (aes_afp_msgtypes type)
{
  messageType_ = type;
}

aes_afp_msgtypes aes_afp_datablock::getMsgType ()const
{
  return messageType_;
}

void aes_afp_datablock::setErrorCode (AES_GCC_Errorcodes code)
{
  errorCode_ = code;
}

AES_GCC_Errorcodes aes_afp_datablock::getErrorCode ()const
{
  return errorCode_;
}

void aes_afp_datablock::setFullPath (std::string path)
{
  path_ = path;
}

std::string& aes_afp_datablock::getFullPath ()
{
  return path_;
}

void aes_afp_datablock::setRetriesDelay (int retriesDelay)
{
  retriesDelay_ = retriesDelay;
}

int aes_afp_datablock::getRetriesDelay ()const
{
  return retriesDelay_;
}

void aes_afp_datablock::setRemoveBefore ()
{
  removeBefore_ = true;
}

void aes_afp_datablock::unsetRemoveBefore ()
{
	removeBefore_ = false;
}

bool aes_afp_datablock::getRemoveBefore ()const
{
  return removeBefore_;
}

void aes_afp_datablock::setManual ()
{
  manual_ = true;
}

bool aes_afp_datablock::getManual ()const
{
  return manual_;
}

void aes_afp_datablock::setLock ()
{
  lock_ = true;
}

void aes_afp_datablock::unsetLock ()
{
  lock_ = false;
}

bool aes_afp_datablock::getLock ()const
{
  return lock_;
}

void aes_afp_datablock::setRangeBegin (std::string rangeBegin)
{
	rangeBegin_ = rangeBegin;
}

std::string& aes_afp_datablock::getRangeBegin ()
{
	return rangeBegin_;
}

void aes_afp_datablock::setRangeEnd (std::string rangeEnd)
{
	rangeEnd_ = rangeEnd;
}

std::string& aes_afp_datablock::getRangeEnd ()
{
	return rangeEnd_;
}

void aes_afp_datablock::setNewDestinationSet (std::string newDestinationSet)
{
	newDestinationSet_ = newDestinationSet;
}

std::string& aes_afp_datablock::getNewDestinationSet ()
{
	return newDestinationSet_;
}

void aes_afp_datablock::setFileRename (AES_AFP_Renametypes rename)
{
	rename_ = rename;
}

AES_AFP_Renametypes aes_afp_datablock::getFileRename ()const
{
	return rename_;
}
void aes_afp_datablock::setFileTemplate (std::string renameTemplate)
{
	renameTemplate_ = renameTemplate;
}

std::string& aes_afp_datablock::getFileTemplate ()
{
	return renameTemplate_;
}

void aes_afp_datablock::setSourceDirectory (std::string sourceDirectory)
{
	sourceDirectory_ = sourceDirectory;
}

std::string& aes_afp_datablock::getSourceDirectory ()
{
	return sourceDirectory_;
}

void aes_afp_datablock::setFormat (AES_GCC_Format format)
{
	transferFormat_ = format;
}

AES_GCC_Format& aes_afp_datablock::getFormat ()
{
	return transferFormat_;
}
void aes_afp_datablock::setMask (std::string mask)
{
	transferMask_ = mask;
}

std::string& aes_afp_datablock::getMask ()
{
	return transferMask_;
}

void aes_afp_datablock::setDirectoryFlag ()
{
	isDirectory_ = true;
}

bool aes_afp_datablock::getDirectoryFlag ()const
{
 	return isDirectory_;
}

void aes_afp_datablock::setDsdChannel (ACS_DSD_Session* channel)
{
	dsdChannel_ = channel;
}

ACS_DSD_Session* aes_afp_datablock::getDsdChannel ()
{
	return dsdChannel_;
}

void aes_afp_datablock::setAfpRestarted (bool restarted)
{
	afpRestarted_ = restarted;
}

bool aes_afp_datablock::getAfpRestarted ()const
{
	return afpRestarted_;
}

void aes_afp_datablock::setListString (std::string listString)
{
	listOperations_ = listString;
}

std::string& aes_afp_datablock::getListString ()
{
	return listOperations_;
}

void aes_afp_datablock::setUsername (std::string username)
{
	username_ = username;
}

std::string& aes_afp_datablock::getUsername ()
{
	return username_;
}

void aes_afp_datablock::setUserGroup (std::string userGroup)
{
	userGroup_ = userGroup;
}

std::string& aes_afp_datablock::getUserGroup ()
{
	return userGroup_;
}

void aes_afp_datablock::setApplication (std::string application)
{
	application_ = application;
}

std::string& aes_afp_datablock::getApplication ()
{
	return application_;
}


void aes_afp_datablock::setNewFilename (std::string newFilename)
{
	newFilename_ = newFilename;
}

std::string& aes_afp_datablock::getNewFilename ()
{
	return newFilename_;
}

void aes_afp_datablock::setNameTag (std::string nameTag)
{
	nameTag_ = nameTag;
}

std::string& aes_afp_datablock::getNameTag ()
{
 	return nameTag_;
}

void aes_afp_datablock::setStartupSequenceNumber (long startupSequenceNumber)
{
	startupSequenceNumber_ = startupSequenceNumber;
}

long aes_afp_datablock::getStartupSequenceNumber ()const
{
	return startupSequenceNumber_;
}
