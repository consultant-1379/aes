
//******************************************************************************
// 
// .NAME
//      aes_dbo_clientsession - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME aes_dbo_clientsession
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE <filename>

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
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
//      <General description of the class>

// .ERROR HANDLING
//
//      General rule:
//      The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.

// DOCUMENT NO
//      19089-CAA 109 0319

// AUTHOR 
//      1999-11-10 by UAB/I/LN  Urban Sderberg

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//          010530  qabhall Changed due to new requirements INGO3 GOH.

// .LINKAGE
//      <link information: libraries needed and order>

// .SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************

#ifndef AES_DBO_CLIENTSESSION_H
#define AES_DBO_CLIENTSESSION_H

#include "aes_dbo_tqdatabase.h"
#include <aes_dbo_daemon.h>

#include <aes_gcc_log.h>

#include <aes_cdh_resultcode.h>

#include <ACS_APGCC_Command.H>
//#include <ACS_APGCC_Daemon.H>
#include <ACS_APGCC_DSD.H>

#include <ace/ACE.h>
#include "ace/Task.h"

#include <string>


class aes_dbo_clientsession  : public ACE_Task_Base
{
private:
    ACE_Recursive_Thread_Mutex hSendMX;
    aes_dbo_tqsession *tqSession_;
    bool tqOwner_;
    string destName;
    FileRecord fRec;
    /**
        @brief  m_cmdObj : APGCC_Command object
    */
    ACS_APGCC_Command cmd;

    int stopEvent_;
    /**
        @brief  m_cmdStreamIO : DSD stream object
    */
    ACS_APGCC_DSD_Stream m_cmdStreamIO;

    /**
         *  onOpen  method: This method is called when a TQ session is already open.
         *  @param  cmd          : ACS_APGCC_Command
     */    
    void onOpen(ACS_APGCC_Command &cmd);
    /**
         *  onClose  method: This method is called when a TQ session is closed.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onClose(ACS_APGCC_Command &cmd);
    /**
         *  onExists method: This method is checks if the TQ  exists in the database.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onExists(ACS_APGCC_Command &cmd);
    /**
         *  onExistsDN  method: This method is checks if the DN of TQ  exists in the database.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onExistsDN(ACS_APGCC_Command &cmd);
    /**
         *  onDefine method: This method is checks if the TQ defination exists in the database.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onDefine(ACS_APGCC_Command &cmd);
     /**
         *  onList method: This method is checks if the TQ list is in the database.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onList(ACS_APGCC_Command &cmd);
    /**
         *  onDelete method: This method is checks if the TQ  is deleted.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onDelete(ACS_APGCC_Command &cmd);
    /**
         *  onChangeDest method: This method is invoked when destination is changed.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onChangeDest(ACS_APGCC_Command &cmd);
    /**
         *  onChangeMirror method: This method is invoked when mirroring is changed.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onChangeMirror(ACS_APGCC_Command &cmd);
     /**
         *  onChangeAll method: This method is invoked if mirroring and remove delay is changed.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onChangeAll(ACS_APGCC_Command &cmd);
    /**
         *  onTBegin method: This method is invoked when a transaction begins.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onTBegin(ACS_APGCC_Command &cmd);
    /**
         *  onTEnd method: This method is invoked when a transaction ends.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onTEnd(ACS_APGCC_Command &cmd);
    /**
         *  onTTerminate method: This method is invoked when a transaction is terminated.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onTTerminate(ACS_APGCC_Command &cmd);
    /**
         *  onTCommit method: This method is invoked when a transaction is commited.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onTCommit(ACS_APGCC_Command &cmd);
    /**
         *  onGetLast method: This method is used to get the last commited block number.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onGetLast(ACS_APGCC_Command &cmd);
    /**
         *  onMirrorFileBeg method: This method is used for a begin new mirrored file.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onMirrorFileBeg(ACS_APGCC_Command &cmd);
    /**
         *  onMirrorFileEnd method: This method is used for a end mirrored file.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onMirrorFileEnd(ACS_APGCC_Command &cmd);
    /**
         *  onRemoveMirror  method: This method is invoked  when mirror is disabled.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onRemoveMirror(ACS_APGCC_Command &cmd);
    /**
         *  onIsTQAttached  method: This method is used to check if a TQ is attached or not.
         *  @param  cmd          : ACS_APGCC_Command
     */
    void onIsTQAttached(ACS_APGCC_Command& cmd);

public:
    /**
         * @brief
         * constructor 
         */
    aes_dbo_clientsession(int& stopEvent);
    /**
        * @brief
        * destructor
        */
    virtual ~aes_dbo_clientsession();
    /**
         *  open  method: This method is used to open a client session.
         *  @param  session_     : ACS_APGCC_Daemon
         *  @param  cmd_         : ACS_APGCC_Command
         *  @return bool: True/False
         */
    /**
       @brief  This method initializes a task and prepare it for execution
    */
    virtual int open (void *args = 0);

    /**
         *   svc method: This method is used to execute a client session.
         */
    virtual int svc();
    /**
         *   sendReply method: This method is used to send reply.
         *   @param  cmd_     : ACS_APGCC_Command
         */
    int sendReply(ACS_APGCC_Command& cmd);

    // called at 'svc' thread exit, due to the parent implementation
    virtual int close(u_long);

    /**
       @brief  This method get the DSD stream
    */
    ACS_APGCC_DSD_Stream& getStream() { return m_cmdStreamIO;}

};


#endif
