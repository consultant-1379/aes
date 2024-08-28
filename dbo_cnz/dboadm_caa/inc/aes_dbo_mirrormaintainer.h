//******************************************************************************
// 
// .NAME
//      MirrorMaintainer - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME MirrorMaintainer
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE MirrorMaintainer.h

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
//      2002-06-26 by UAB/UKB/AU  Mats Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//          020626  qabmnnn First draft

// .LINKAGE
//      <link information: libraries needed and order>

// .SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************

#ifndef AES_DBO_MIRRORMAINTAINER_H
#define AES_DBO_MIRRORMAINTAINER_H


#include "aes_dbo_mirrorqueue.h"

#include <ace/ACE.h>
#include <ace/Task.h>

#include <string>



class ACE_Process_Mutex;

class mirrorFileStateRecord 
{
 public:
    std::string streamId_;
    void* stateP_;
    uint32_t* curFile_;
    uint32_t* curBlock_;
};

class aes_dbo_mirrormaintainer : public ACE_Task_Base
{
  private:

	static const uint32_t MAX_BLADE_NUMBER = 64U;

	bool running_;
    std::string tq_;
    std::string tqPath_;
    std::string cfgTqPath_;
    std::string cfgTqFolder_;
    std::string m_ProcessMutexName;

    unsigned int delay_;
    aes_dbo_mirrorqueue queue_;

    void* m_stateP_;
    unsigned int * m_curFile_;
    unsigned int * m_curBlock_;
    bool m_multiCPsystem;
    bool exit_;
    bool exitForced_;
    ACE_Process_Mutex* stateMutex_;
    mirrorFileStateRecord msList[MAX_BLADE_NUMBER];

    /**
	 *  checkDirs  method: This method is used to check the dbo directories.
	 */
    void checkDirs();

    /**
	 *  removeDir  method: This method is used to remove the dbo directories.
	 */
    void removeDir(const std::string &path);

    /**
	 *  openMirrorStateFile  method: This method opens the state file.
	 */
    ACE_HANDLE openMirrorStateFile(const std::string& stateFile, bool& fileCreated);

public:
    /**
         * @brief
         * constructor with one parameter
         */
    aes_dbo_mirrormaintainer(const std::string &tq);
    /**
         * @brief
         * destructor 
         */
    virtual ~aes_dbo_mirrormaintainer();
    /**
         *  create  method: This method is used to create a mirror maintainer.
         */
    void create();
    /**
         *  create  method: This method is used to create a mirror maintainer with a remove delay.
         *  @param  delay : unsigned int
         */
    void create(unsigned int delay);
    /**
         *  remove  method: This method is used to remove a mirror maintainer.
         */
    void remove();
    /**
         *  abort method: This method is used to abort a mirror maintainer.
         */
    void abort();
    /**
         *  svc  method: This method is used to execute a mirror maintainer.
         */
    virtual int svc();
    /**
         *  changeDelay method: This method is used to change delay.
         *  @param  delay : unsigned int
         */
    void changeDelay(unsigned int delay);

    /**
         *  newFile method: This method is used to start a new file.
         *  @param  fName : constant string
         */
    unsigned int newFile(const std::string &fName);

    /**
         *  closeFile method: This method is used to close a  file.
         *  @param  fName : const string
         */
    unsigned int closeFile(const std::string &fName);
    /**
         *  checkDataArea  method: This method is used to check the data area.
         */
    unsigned int checkDataArea();
    /**
         *  makeDir method: This method is used to make a directory.
         *  @param  dir : const string
         */
    void makeDir(const std::string &dir);
    /**
         *  running method: This method is used to check if multiple CP is running.
         */
    bool running() const  { return running_; }
    /**
     	 *  forceExit method: This method is used to force the thread exit.
     	 */
    void forceExit();
};

#endif
