/*=================================================================== */
/******************************************************************************
@file notificationrecord.cpp

Class method implementation for notificationrecord.h

DESCRIPTION
Facilitating notification records.

ERROR HANDLING
General rule:

The error handling is specified for each method.
No methods initiate or send error reports unless specified.

@version 1.1.1

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------

N/A       26/09/2011     xbhadur       Initial Release
=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <notificationrecord.h>

/*===================================================================
   ROUTINE:NotificationRecord
=================================================================== */
NotificationRecord::NotificationRecord() :fileSize(""), messageNumber(0), timeStamp(0)
{
    storeFileName = "";
}

/*===================================================================
   ROUTINE:NotificationRecord
=================================================================== */
NotificationRecord::NotificationRecord(const NotificationRecord &nr) 
    :fileSize(""), messageNumber(0), timeStamp(0)
{
    fileSize = nr.fileSize;
    timeStamp = nr.timeStamp;
    messageNumber = nr.messageNumber;
    storeFileName = "";
}


/*===================================================================
   ROUTINE:~NotificationRecord
=================================================================== */
NotificationRecord::~NotificationRecord()
{
}

/*===================================================================
   ROUTINE:getStoreFileName
=================================================================== */
std::string NotificationRecord::getStoreFileName() const
{
    return storeFileName;
}

/*===================================================================
   ROUTINE:getFileSize
=================================================================== */
std::string NotificationRecord::getFileSize() const
{
    return fileSize;
}


/*===================================================================
   ROUTINE:getMessageNumber
=================================================================== */
ACE_UINT32 NotificationRecord::getMessageNumber() const
{
    return messageNumber;
}

/*===================================================================
   ROUTINE:setStoreFileName
=================================================================== */
void NotificationRecord::setStoreFileName(const std::string &name)
{
    storeFileName.assign(name);
}

/*===================================================================
   ROUTINE:setFileSize
=================================================================== */
void NotificationRecord::setFileSize(const std::string &size)
{
    fileSize.assign(size);
}

/*===================================================================
   ROUTINE:setMessageNumber
=================================================================== */
void NotificationRecord::setMessageNumber(const ACE_UINT32 n)
{
    messageNumber = n;
}

/*===================================================================
   ROUTINE:setTime
=================================================================== */
void NotificationRecord::setTime(time_t stamp)
{
    timeStamp = stamp;
}

/*===================================================================
   ROUTINE:getTime
=================================================================== */
time_t NotificationRecord::getTime() const
{
    return timeStamp;
}
