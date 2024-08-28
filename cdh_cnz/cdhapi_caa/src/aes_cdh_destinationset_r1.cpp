/*=================================================================== */
/**
   @file   aes_cdh_destinationset_r1.cpp

   Class method implementationn for AES_CDH_DestinationSet_R1 type module.

   This module contains the implementation of class declared in
   the aes_cdh_destinationset_r1.h module

   This program receives parameters sent from programs
   cdhdslsw. The parameters is then analysed and sent further
   to method sendCmd in class DestinationSet.
   Cdhdsls is calling method getAttr

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
#include <aes_cdh_destinationset.h>
#include <aes_cdh_destinationsetimplementation.h>

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: AES_CDH_DestinationSet_R1
=================================================================== */
AES_CDH_DestinationSet_R1::AES_CDH_DestinationSet_R1() 
{
    implementation = new AES_CDH_DestinationSetImplementation;
}

/*===================================================================
   ROUTINE: AES_CDH_DestinationSet_R1
=================================================================== */
AES_CDH_DestinationSet_R1::AES_CDH_DestinationSet_R1(const std::string &destSetName) 
{
    implementation = new AES_CDH_DestinationSetImplementation(destSetName);
}

/*===================================================================
   ROUTINE: ~AES_CDH_DestinationSet_R1
=================================================================== */
AES_CDH_DestinationSet_R1::~AES_CDH_DestinationSet_R1() 
{
    delete implementation;
}

/*===================================================================
   ROUTINE: open
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::open(void)
{
    return implementation->open(*this);
}

/*===================================================================
   ROUTINE: close
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::close(void)
{
    return implementation->close();
}

/*===================================================================
   ROUTINE: setEventSubscription
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::setEventSubscription(
    const std::string& transferQueue,  
    const std::string& alarmText)
{
    return implementation->setEventSubscription(transferQueue, alarmText);
}

/*===================================================================
   ROUTINE: removeEventSubscription
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::removeEventSubscription(void)
{
    return implementation->removeEventSubscription();
}

/*===================================================================
   ROUTINE: exists
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::exists(void)
{
    return implementation->exists();
}

/*===================================================================
   ROUTINE: exists
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::exists(
    const TQConnectAttributes attr)
{
    return implementation->exists(attr);
}

/*===================================================================
   ROUTINE: sendFile
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::sendFile(
    const std::string &fileName, 
    const std::string remoteSubDirName,
    const std::string newFileName,
    const std::string userUnique,
    transferMode trMode,
    const std::string fileMask,
    bool isDir,
    bool retryAfterRestart)
{
    return implementation->sendFile(fileName, remoteSubDirName, newFileName,
                                    userUnique, trMode, fileMask,
                                    isDir, retryAfterRestart);
}

/*===================================================================
   ROUTINE: stopSendFile
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::stopSendFile(
    const std::string &fileName, 
    const std::string remoteSubDirName,
    const std::string newFileName)
{
    return implementation->stopSendFile(fileName, remoteSubDirName, newFileName);
}

/*===================================================================
   ROUTINE: sendRecordFile
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::sendRecordFile(
    std::string streamName,
    std::string streamId)
{
    return implementation->sendRecordFile(*this, streamName, streamId);
}

/*===================================================================
   ROUTINE: getError
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::getError(void)
{
    return implementation->getError();
}

/*===================================================================
   ROUTINE: getErrorText
=================================================================== */
void AES_CDH_DestinationSet_R1::getErrorText(std::string& errText)
{
    implementation->getErrorText(errText);
}

/*===================================================================
   ROUTINE: getAttr
=================================================================== */
AES_CDH_Result AES_CDH_DestinationSet_R1::getAttr(int argc, char* argv[],
                    std::vector<AES_CDH_DestinationSet::destSetAttributes>& attr)
{
    return AES_CDH_DestinationSetImplementation::getAttr(argc, argv, attr);
}

/*===================================================================
   ROUTINE: transactionBegin
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::transactionBegin()
{
    return implementation->transactionBegin();
}

/*===================================================================
   ROUTINE: transactionEnd
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::transactionEnd(
                                            unsigned int &applBlockNr)
{
    return implementation->transactionEnd(applBlockNr);
}

/*===================================================================
   ROUTINE: transactionCommit
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::transactionCommit(
                                            unsigned int &applBlockNr)
{
    return implementation->transactionCommit(applBlockNr);
}

/*===================================================================
   ROUTINE: getLastCommittedBlock
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::getLastCommittedBlock(
                                            unsigned int &applBlockNr)
{
    return implementation->getLastCommittedBlock(applBlockNr);
}
/*===================================================================
   ROUTINE: transactionTerminate
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSet_R1::transactionTerminate()
{
    return implementation->transactionTerminate();
}

/*===================================================================
   ROUTINE: isSessionOpen
=================================================================== */
bool AES_CDH_DestinationSet_R1::isSessionOpen()
{
    return implementation->isSessionOpen();
}
