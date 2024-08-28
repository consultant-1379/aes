
//******************************************************************************
// 
// .NAME
//  	TransDest
// .LIBRARY 3C++
// .PAGENAME TransDest
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE transdest.h

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
// 	<General description of the class>

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 0506

// AUTHOR 
// 	1999-03-24 by UAB/I/LD  Ulf Andersson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//			070925	XTBOLAR Updated to solve TR's HI45644, HI46429, HI47573, and HI48040.
//			071010	XTBOLAR Updated to solve TR HI51265

// .LINKAGE
//	<link information: libraries needed and order>

// .SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************

#ifndef TRANSDEST_H 
#define TRANSDEST_H
#include <string>
#include <list>           // INGO3 GOH qabhall
#include <map>
#include <transferagent.h>
#include <aes_cdh_destination.h>
#include <aes_cdh_destinationset.h>
#include <transdestset.h>
// INGO3 GOH qabhall start
#include <cmdhandler.h>
#include <aes_gcc_eventhandler.h>
#include <aes_gcc_event.h>
#include <ace/Task.h>
#include <ace/ACE.h>

// INGO3 GOH qabhall end

#include <readypoll.h>		//For CDH Ready Directory Polling Functionality.	
#include <notification.h>           // 020212, File Notification
#include <resendnotification.h>     // 051122, Resend File Notification
#include <AES_DBO_DataBlock.h>
using namespace std;


#define APG_DEFAULT_VIRTUAL_DIR_PATH	"/data/opt/ap/nbi/"
#define AES_DATA_TRANSFER_DIR_NAME	"data_transfer"
#define ENABLE 'T'             // TR HX18084
#define DISABLE 'F'            // TR HX18084


class ReadyPoll;

//##ModelId=3DE49C1101F3
class TransDest : public ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>
{
public:

	//##ModelId=3DE49C130367
    typedef enum       // INGO3 GOH qabhall
    {
      TRDMSG_DELETE,
      TRDMSG_SENDDONEOK,
      TRDMSG_SENDFILE,
      TRDMSG_SENDBLOCK,
	  TRDMSG_CHECKCON,
	  //HI58671
	  TRDMSG_CHECKDESTAVAIL
	  //HI58671
    } trdMessageCode;
	
	string PollVdName_;
	string PollDest_; 

	//##ModelId=3DE49C1303CB
    struct setEvMsg     // INGO3 GOH qabhall
    {
		//##ModelId=3DE49C140020
      trdMessageCode cmdC;
    };

    void shutDown(void);

    //##ModelId=3DE49C110274
    TransDest();

	//##ModelId=3DE49C11027E
    ~TransDest();

	//##ModelId=3DE49C110288
    ACE_INT32 open(void);

    ACE_INT32 close(ACE_UINT64 flag);

	//##ModelId=3DE49C110292
    ACE_INT32 svc(void);

	//##ModelId=3DE49C1102B0
    AES_CDH_ResultCode checkConnection(bool doCheck);

	void checkCon(void);

	inline bool isInstanceClosed () {return (ptrans_ ? ptrans_->isInstanceClosed() : false);}; 

	//HI58671
	void CheckDestAvailable(void);
	AES_CDH_ResultCode checkDestinations();
	AES_CDH_ResultCode GetDestAvailable();
	//HI58671

	//##ModelId=3DE49C1102BA
    AES_CDH_ResultCode checkConnectionManually();     // File Notification

	//##ModelId=3DE49C1102C4
    AES_CDH_ResultCode checkNotification();           // File Notification

	//##ModelId=3DE49C1102CE
    bool isNotificationDefined();

	//##ModelId=3DE49C1102CF
	AES_CDH_ResultCode define(const string destName, 
                             const string transType, 
                             ACE_INT32 &argc,
                             ACE_TCHAR* argv[],
                             const string& userGroup,
                             const bool define,
                             const bool recovery,
			     const string& destRdn,
			     const string destSetName ="");

	//##ModelId=3DE49C1102EC
	AES_CDH_ResultCode changeAttr(const string transType, 
								  ACE_INT32 &argc, ACE_TCHAR* argv[]);

	//##ModelId=3DE49C1102F8
	AES_CDH_ResultCode getAttr( AES_CDH_Destination::destAttributes &attr );

   AES_CDH_ResultCode getSingleAttr( const ACE_TCHAR* opt, void* value );


	//##ModelId=3DE49C11030A
    AES_CDH_ResultCode sendFile(const string& fileName, 
								const string& remoteSubDirName,
                                const string& newFileName,
                                const AES_CDH_DestinationSet::transferMode trMode,
                                const string& fileMask,
                                const bool isDir);


	//##ModelId=3DE49C11033C
	AES_CDH_ResultCode sendRecordFile(const string& streamName,
                                     const string& streamId, 
                                     AES_DBO_DataBlock*& block,
                                     CmdHandler *cmdHdlr,
                                     const ACE_UINT64 ticks);


	//##ModelId=3DE49C110382
	AES_CDH_ResultCode remove(void);

	//##ModelId=3DE49C11038C
    string getDestinationName(void);
    
	//##ModelId=3DE49C1103A1
    AES_CDH_ResultCode transactionBegin();

	//##ModelId=3DE49C1103AA
    AES_CDH_ResultCode transactionEnd(ACE_UINT32 &applBlockNr);

	//##ModelId=3DE49C1103B4
    AES_CDH_ResultCode transactionCommit(ACE_UINT32 &applBlockNr);

	//##ModelId=3DE49C1103BE
    AES_CDH_ResultCode getLastCommittedBlock(ACE_UINT32 &applBlockNr);

	//##ModelId=3DE49C1103C8
    AES_CDH_ResultCode transactionTerminate();

	//##ModelId=3DE49C1103D2
    AES_CDH_ResultCode setAPIClosed();

	//##ModelId=3DE49C110396
    TransferAgent::TransferAgentAttributes   getAgentType(void );

	//##ModelId=3DE6223D014D
    void setDestSetName(const string & destSetName);

    //##ModelId=3DF8ADFB0273
	string getDestSetName(void );

	//##ModelId=3E01B62D03D0
    const string getUserGroup() const;

    //##ModelId=3E2EB62C01E9
	void switchAgent(bool stopTransfer);

	
	ReadyPoll *GetPollObj();

	bool GetPollingStatus();

	void getDestinationRdn( string& destRdn );

private:

	//##ModelId=3DEB69B501BF
	string dest_;
	string m_destRdn;

	//##ModelId=3DEB69D00394
	string destSetName_;
	//##ModelId=3DE49C12006D
	TransferAgent* ptrans_;
	//##ModelId=3DE49C12008B
   TransferAgent* ptransForCheckConnection_;

	//##ModelId=3E7997330248
	TransferAgent* tmpPtransForCheckConnection_;
	Notification* pnotif_;              // ptr to the notification 'service', 020212
	ReadyPoll*  PollObj;				 //For CDH Ready Directory Polling Functionality.
	bool isPollingTrue;
	bool notificationRunning;           // true if notification has been started



	//##ModelId=3E01ADF70098
   string usergrp_;

   //bool recovery_;
/*// Destination Set Alarm start
   struct TransDestData {
      bool priOk;
      bool secOk;
      bool backOK;
      string priDestName;
      string secDestName;
      string backDestName;
      class TransDestSet *tds;
   };*/

   //static OSF_Mutex destpairMX_;
  /* static map<string, TransDestData> destPair_;
   typedef pair<string, TransDestData> DestSet_Pair;

// Destination Set Alarm end*/

// File Notification start

	//##ModelId=3DE49C1200B3

	//##ModelId=3DE49C1200DB
    ACE_Thread_Manager* threadManager_;

	//##ModelId=3DE49C120103

    //##ModelId=3DE49C1201CB
    ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy> notifQueue;
    
	//##ModelId=3DE49C120121
    ACE_UINT32 parseNotifParams(ACE_INT32 &argc, ACE_TCHAR *argv[],
                         std::string &connectType,
                         std::string &notifAddress,
                         std::string &notifPort,
                         std::string &responseTime,
                         std::string &eventType);



	//##ModelId=3DE49C1201E9
    ACE_UINT32 checkNotifChangeAttr(ACE_INT32 &argc, ACE_TCHAR *argv[],
                             std::string &connectType,
                             std::string &notifAddress,
                             std::string &notifPort,
                             std::string &responseTime,
                             std::string &eventType);

        //##ModelId=3DE49C120243
    bool readParameters(string &path1, string &path2);

        //##ModelId=3DE49C12026B
    void makeDataBaseDestPath(string &dbPath, const string &aesDataDir,
                              const string &cdhRootDir, const string &dest);

        //##ModelId=3DE49C12029D
    void removeDataBaseDestDir(const string &dbPath);

// File Notification end

// Resend File Notification start

    static ACE_Mutex resNotifMX_;

    static ACE_Thread_Manager* resendThreadManager_;

    static bool resendNotificationRunning_;

    static ACE_INT32 numberOfNotificationDestinations_;

    static ResendNotification* presendnotif_;

    static AES_CDH_ResultCode StartResendNotification(void);


    static void StopResendNotification(void);

// Resend File Notification end


        //##ModelId=3DE49C120316
    ACE_Mutex connMX_;                      // INGO3 GOH 010822 qabhall
        //##ModelId=3DE49C12033E
    ACE_Mutex eventSubscriberMX_;           // INGO3 GOH 010822 qabhall

        //##ModelId=3DFD9A250135
    AES_CDH_ResultCode connectionResult_;   // INGO3 GOH 010822 qabhall

        //##ModelId=3DE49C1203D4
    bool exitThread_;

        //##ModelId=3DE49C130262
    void handleMessage(setEvMsg *data);

    //##ModelId=3DE49C130295
    AES_CDH_ResultCode checkConnection(char setAlarm = ENABLE); // TR HX18084

        //HI58671
        AES_CDH_ResultCode DestAvailResult_;
        //HI58671

        //##ModelId=3DE49C11029D


	ACE_Thread_Mutex checkConnMutex;


};


#endif


