/*=================================================================== */
/**
@file messageblock.cpp

Class method implementation for messageblock.h

DESCRIPTION
This class handles messages block sent between threads.
The source code is adapted for WinNT only.

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
#include "messageblock.h"
#include <ace/ACE.h>

/*===================================================================
   ROUTINE:MessageBlock
=================================================================== */
MessageBlock::MessageBlock(const ACE_TCHAR *data)
: ACE_Message_Block(data),
  mt(MT_EMPTYQUEUE),
  fileName(""),
  fileSize(""),
  msgNr(""),
  resetFlag(false)
{

}

/*===================================================================
   ROUTINE:MessageBlock
=================================================================== */
MessageBlock::MessageBlock(MessageType messType)
: mt(messType),
  fileName(""),
  fileSize(""),
  msgNr(""),
  resetFlag(false)
{

}

/*===================================================================
   ROUTINE:~MessageBlock
=================================================================== */
MessageBlock::~MessageBlock()
{

}

/*===================================================================
   ROUTINE:msgType
=================================================================== */
MessageType
MessageBlock::msgType(void)
{
	return mt;
}

/*===================================================================
   ROUTINE:msgType
=================================================================== */
void
MessageBlock::msgType(MessageType mtype)
{
	mt = mtype;
}

/*===================================================================
   ROUTINE:getFileName
=================================================================== */
string
MessageBlock::getFileName()
{
	return fileName;
}

/*===================================================================
   ROUTINE:setFileName
=================================================================== */
void
MessageBlock::setFileName(string &fName)
{
	fileName = fName;
}

/*===================================================================
ROUTINE:getFileSize
=================================================================== */
string
MessageBlock::getFileSize()
{
	return fileSize;
}

/*===================================================================
   ROUTINE:setFileSize
=================================================================== */
void
MessageBlock::setFileSize(string &fSize)
{
	fileSize = fSize;
}

/*===================================================================
   ROUTINE:getMsgNr
=================================================================== */
string
MessageBlock::getMsgNr(void)
{
	return msgNr;
}

/*===================================================================
   ROUTINE:setMsgNr
=================================================================== */
void
MessageBlock::setMsgNr(string &mNr)
{
	msgNr = mNr;
}

/*===================================================================
   ROUTINE:getResetFlag
=================================================================== */
bool
MessageBlock::getResetFlag(void)
{
        return resetFlag;
}

/*===================================================================
   ROUTINE:setResetFlag
=================================================================== */
void
MessageBlock::setResetFlag(bool rflag)
{
        resetFlag = rflag;
}

