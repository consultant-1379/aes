
//******************************************************************************
// 
// .NAME
//  	TransDestSet
// .LIBRARY 3C++
// .PAGENAME TransDestSet
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE transdestset.h

// .COPYRIGHT
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
// 	This class describes a destination set in the server part of CDH.

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 0506

// AUTHOR 
// 	2002-12-12 by UAB/UKY/AU  Gunnar Andersson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//			070925	XTBOLAR Updated to solve TR's HI45644, HI46429, HI47573, and HI48040.

// .LINKAGE
//	<link information: libraries needed and order>

// .SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************

#ifndef TRANSDESTSET_H 
#define TRANSDESTSET_H

#include <string>
#include <transdest.h>
#include <cmdhandler.h>
#include <aes_cdh_destinationset.h>
#include <ace/ACE.h>
#include <ace/Monotonic_Time_Policy.h>

using namespace std;

//##ModelId=3DE4E65503E4
class TransDestSet : public ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy> 
{

 public:
	//##ModelId=3DE4E70A013B
	TransDestSet();

	//##ModelId=3DE4E80C02A5
	~TransDestSet();

	//##ModelId=3DE5D40501C2
	AES_CDH_ResultCode define(const string& destSetName, 
                              const string& primaryDestName, 
                              const string& secondaryDestName, 
                              const string& backupDestName, 
							  const string &userGroup,
							  const string &destSetRdn);

	//##ModelId=3DEB655603A1
	AES_CDH_ResultCode getAttr(AES_CDH_DestinationSet::destSetAttributes& attr);

	//##ModelId=3DEB67E1039D
	AES_CDH_ResultCode removeDestination(const string& destName);

    //##ModelId=3DE5EB900024
	string getDestinationSetName(void ) const;
	
	void getDestSetRdn(string& destSetRdn ) ;

	//##ModelId=3DE731D5001D
	AES_CDH_ResultCode checkDestinationSetType(AES_CDH_DestinationSet::TQConnectAttributes& realAttr) const;

	//##ModelId=3DEC7D890355
	AES_CDH_ResultCode changeAttr(const string& primaryDestName, 
                                  const string& secondaryDestName, 
                                  const string& backupDestName);

	//##ModelId=3DEC85DA028E
	AES_CDH_ResultCode switchDestination(const string& destName);

	//##ModelId=3DEF423A02BB
	AES_CDH_ResultCode sendFile(CmdHandler* cmdHdlr, const ACE_UINT64 ticks, ACE_UINT32 pid, ACE_UINT32 destSetObj, 
                                const string fileName, 
                                const string remoteSubDirName, 
                                const string newFileName, 
                                const string userUnique, 
                                const AES_CDH_DestinationSet::transferMode trMode, 
                                const string fileMask, const bool isDir);

	//##ModelId=3DF08D410066
	AES_CDH_ResultCode setEventHandler(const string& transferQueueName, 
                                       const string& alarmText, 
                                       CmdHandler* cmdHdlr) const;

	//##ModelId=3DF5F06800E0
	AES_CDH_ResultCode removeEventHandler(CmdHandler* cmdHdlr) const;

	//##ModelId=3DF091D900CB
	AES_CDH_ResultCode sendRecordFile(const string& streamName, 
                                      const ACE_UINT32 streamSize, 
                                      const ACE_UINT32 recordLength, 
                                      const string& mainFileName, 
                                      string fileName, 
                                      ACE_UINT64 recordsSent, 
                                      CmdHandler* cmdHdlr,
                                      const ACE_UINT64 ticks) ;

	//##ModelId=3DF4B7F401F1
	AES_CDH_ResultCode getLastCommittedBlock(ACE_UINT32& applBlockNr);

	//##ModelId=3DF4BAA801D8
	AES_CDH_ResultCode transactionBegin();

	//##ModelId=3DF5ED08019C
	AES_CDH_ResultCode transactionEnd(ACE_UINT32& applBlockNr);

	//##ModelId=3E0184A100D5
	AES_CDH_ResultCode transactionCommit(ACE_UINT32& applBlockNr);

	//##ModelId=3E018614001A
	AES_CDH_ResultCode transactionTerminate(void );

	//##ModelId=3DF5F3E20035
	AES_CDH_ResultCode setAPIClosed();

    //##ModelId=3DFDEFA60050
	bool verifyUserGroup(const string& userGroup) const;

	AES_CDH_ResultCode removeDestinationSet();

	void checkAllConnections();
	
	//HI58671
	bool NoDestAvailable ();
	//HI58671

	//##ModelId=3DEC93B30213
	TransDest* activeDest;
	bool TempConVar;

private:

	//##ModelId=3DFDC470007F
    typedef enum       // INGO3 GOH qabhall
    {
      TRDMSG_DELETE,
      TRDMSG_SENDDONEOK,
      TRDMSG_SENDFILE,
      TRDMSG_SENDBLOCK,
    } trdMessageCode;

	//##ModelId=3DF9ECF40342
	struct setEvMsg 
	{
		//##ModelId=3DFDC4FC003A
		trdMessageCode cmdC;

        //##ModelId=3DFDC532039F
		CmdHandler* cmdHdnlr;

        ACE_UINT64 ticks;

		//##ModelId=3DFDC556031F
		string sData[6];

		//##ModelId=3DF9ECF40343
		ACE_UINT32 iData[5];

		//##ModelId=3DF9ECF40344
		ACE_UINT64 lData[1];
	};

    //##ModelId=3E1AF21E03B1
	struct UsedDestinations 
	{
		//##ModelId=3E1AF3FC00A1
		bool usedPrimDest;
		//##ModelId=3E1AF443027A
		bool usedSecDest;
		//##ModelId=3E1AF4540166
		bool usedBackDest;
	};

    //##ModelId=3DF99DC90032
	AES_CDH_DestinationSet::TQConnectAttributes type;

	//##ModelId=3DEB69E90390
	string name;

	//##ModelId=3DEC7E0D0390
	TransDest* primaryDest;

	//##ModelId=3DEC7E4A024E
	TransDest* secondaryDest;

	//##ModelId=3DEC7E6C0080
	TransDest* backupDest;

	//##ModelId=3DFDE9EA004F
	string usergrp;

	//##ModelId=3E00990701AA
	bool exitThread;

	//##ModelId=3E27F8E70084
	bool transferInProgress;

	//##ModelId=3E3021EF03C4
	TransDest* newActiveDest;

	//##ModelId=3E2E6B4401BB
	TransDest** changeDestPtr;

	//##ModelId=3E3020E40293
	UsedDestinations usedDests;

	string m_destSetRdn;

    //##ModelId=3E31678A0228
	ACE_Thread_Mutex transDestMutex;

	//##ModelId=3DEF470700FF
	void handleMessage(setEvMsg* data);

	//##ModelId=3DEF498F00A7
	AES_CDH_ResultCode intSendFile(CmdHandler* cmdHdlr, const ACE_UINT64 ticks, ACE_UINT32 pid,
                                   ACE_UINT32 destSetObj, const string fileName, 
                                   const string remoteSubDirName, 
                                   const string newFileName, 
                                   const string userUnique, 
                                   const AES_CDH_DestinationSet::transferMode trMode, 
                                   const string fileMask, const bool isDir);

	//##ModelId=3DF4ADFD00EF
	AES_CDH_ResultCode intSendRecordFile(const string& streamName, 
                                         const ACE_UINT32 streamSize, 
                                         const ACE_UINT32 recordLength, 
                                         const string& mainFileName, 
                                         string fileName, 
                                         ACE_UINT64 recordsSent, 
                                         CmdHandler* cmdHdlr,
                                         const ACE_UINT64 ticks) ;

    //##ModelId=3E008F4A0141
	ACE_UINT32 open(void );

	//##ModelId=3E008F5903BA
	ACE_INT32 svc(void );

	//##ModelId=3E008F66028C
	void shutDown(void );

	//Switch destination to another destination in the dest 
	//set. If a destination fails, other destinations are 
	//only tried once / file or dir.
	//##ModelId=3E1AEFAC003F
	bool switchDestAuto();
	
	//##ModelId=3E2E66C80264
	void switchDestManual(void);

   // Debug purpose
   const ACE_TCHAR* GetDateTimeString();
};

#endif

