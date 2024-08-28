//	COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson Utvecklings AB, Sweden.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson Utvecklings AB or
//	in
//	accordance with the terms and conditions stipulated in
//	the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	DESCRIPTION:
//
//	ERROR HANDLING
//
//	      General rule:
//	      The error handling is specified for each method.
//
//	      No methods initiate or send error reports unless
//	specified.
//
//	DOCUMENT NO
//	      190 89-CAA 109 xxxx
//
//	AUTHOR
//	      1999-04-	UAB/I/LN           UABxxxx
//
//	 REVISION
//	       PA1  2001-06-13  UABxxxx  Prel version.
//         PA2  2001-08-08  QABLAKE EM fix pattern for 4096 1..FF,0..FF -> 1..FF,1..FF
//                            EM "<"  -> "<=" 
//                            Turn around after 9999 ->0000
//		   RxA  2001-09-xx  QABMSOL
//
//	LINKAGE
//
//	SEE ALSO
#include <string>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <ace/ACE.h>
#include <time.h>

#include "threadgen.h"
#include "aes_ohi_filehandler.h"
#include "aes_ohi_blockhandler2.h"
#include "aes_ohi_format.h"

/*************************************************************************************************
  N O T E !!!   N O T E !!!   N O T E !!!   N O T E !!!   N O T E !!!   N O T E !!!   N O T E !!! 
 *************************************************************************************************/
// The flag FULLFUNC enables full functionality towards other component in system.
// The flag is commented out during development, when there is no full test environment. 

#define FULLFUNC

/*************************************************************************************************
  N O T E !!!   N O T E !!!   N O T E !!!   N O T E !!!   N O T E !!!   N O T E !!!   N O T E !!! 
 *************************************************************************************************/

// Used for setting retry interval for reattaching to block-, dir- or file handler (milliseconds)
#define RETRY_TIME 10000

static ACE_INT32 retry_counter=0;
static ACE_INT32 retry_counter2=0;

typedef struct 
{
	ACE_TCHAR data;
	ACE_INT32 nr1;
	ACE_INT32 nr2;
}
filePattData;

const ACE_INT32 MaxFiles = 100;
filePattData fp[MaxFiles];

struct fil
{
        struct dirent* fFindFileData;
	DIR* fhFind;
	std::string rpath,cpath;
};

struct commonData
{
	ACE_HANDLE hMutex;
};

struct commonData common;

struct fil f[100]; 

DIR* dip;
struct dirent* dit;

using namespace std;

void Thrd(void* lp) 
{
	threadGen* pInfo = (threadGen*) lp;
	pInfo->threadStart();
}

bool CopyFile(const char* src, const char* dest)
{
   string cmd = string("cp ") + string(src) + " " + string(dest);
   FILE *fp;
   int status;
   char path[PATH_MAX] = { 0 };

   fp = popen(cmd.c_str(), "r");
   if (fp == NULL)
       return false;

   while(fgets(path, PATH_MAX, fp) != NULL) ;

   status = pclose(fp);
   return WIFEXITED(status);
}

// Class threadGen 
/*******************************************************************************
 * threadGen() - Default constructor.                                          *
 *******************************************************************************/
threadGen::threadGen()
:aes_ohi_extFileHandler2_r1("TST","TEST"), sendFile_(false), debugEnabled_(false), theMutexCondition(theRecursiveThreadMutex) {}

/*******************************************************************************
 * threadGen() - Constructor.                                                  *
 *******************************************************************************/
threadGen::threadGen ( ACE_INT32 recLength, 
					   ACE_INT32 nrOfRec, 
					   ACE_INT32 transActLength, 
					   ACE_INT32 freq, 
					   const std::string evText,
					   ACE_INT32 fDestNr, 
					   ACE_INT32 transtype, 
					   const std::string newpath,
					   const std::string streamID )
: aes_ohi_extFileHandler2_r1("TST","TEST"), name_(0), partOfDir_(0), lastSentFileNr_(0), m_nNumCP_Block(-1), sendFile_(false),
  sendDir_(false), sendBlock_(false), suspendAtStart_(true), terminated_(false), 
  debugEnabled_(false), nrOfSentBytes_(0), nrOfTransactionSentBytes_(0), theMutexCondition(theRecursiveThreadMutex)
{
	
	
	this->frequency_ =	freq;
	this->recordLength_ =	recLength;
	this->transActLength_ =	transActLength;
	this->nrOfRecords_ =	nrOfRec;
        this->eventText_=	evText;
	this->streamID_=	streamID;
	this->fileDestNr_ =	fDestNr;
	this->transType_ =	transtype;
	this->path_ =		newpath;

        ace_thr_mgr = ACE_Thread_Manager::instance();
        int grp_id = ace_thr_mgr->spawn((ACE_THR_FUNC)Thrd,
                        (void*)this ,
                        THR_NEW_LWP | THR_DETACHED,
                        &hThread_,
                        0,
                        ACE_DEFAULT_THREAD_PRIORITY,
                        -1,
                        0,
                        ACE_DEFAULT_THREAD_STACKSIZE); 
	
	if ( grp_id == -1 )
	{
		cerr << "ERROR: Cannot create thread" << endl;
                string s = "Cannot create thread";
		logfile((char*)s.c_str());
	}
	
	ACE_OS::thread_mutex_init(&MsgCriticalSection);

}

/*******************************************************************************
 * threadGen() - Destructor.                                                   *
 *******************************************************************************/
threadGen::~threadGen()
{
}

/*******************************************************************************
 * fillPattern() - Make records consisting of a known pattern.                 *
 *******************************************************************************/
void threadGen::fillPattern ( char* writebuff, 
							  ACE_INT32 fileNr, 
							  ACE_INT32 recordLength )
{
	ACE_INT32 i = 0;
    
	if (fp[fileNr].nr1 >9999)  // ..nr1 is the yatsi internal number
		fp[fileNr].nr1 = 0;   // ..nr2 is the sequence number that is check at decoding

    if (fp[fileNr].nr2 >9999)  // 010808 EM
		fp[fileNr].nr2 = 0;   // 010920 MASO Changed 0 -> 1
	
	sprintf(writebuff,"<%04u %04u",fp[fileNr].nr1,fp[fileNr].nr2);
	
	ACE_INT32 j = 10;
	
	ACE_TCHAR d1 = fp[fileNr].data;

	do
	{
		for(i=0; i<10; i++)
		{
 			writebuff[j++] = d1;
			if(j > recordLength-6) break;
		}

		d1 = (d1 % 255) +1;  // 010808 EM 1..FF,1..FF
	} while (j <= recordLength-6);// 010808 EM "<"  -> "<=" 
	
	j = 1;

	for(i = recordLength-5; i<recordLength-1; i++)
	{
		writebuff[i] = writebuff[j++];
	}
	
	writebuff[recordLength-1] = '>';
}

/*******************************************************************************
 * generateFile() -                                                            *
 *******************************************************************************/
bool threadGen::GenerateFile ( const std::string path, 
							   const std::string FileName, 
							   ACE_INT32 nrOfRecords, 
							   ACE_INT32 recordLength,
							   bool& exist )
{

	ACE_HANDLE fileref = ACE_INVALID_HANDLE;
	char* writeBuff = 0;
	ACE_INT32 fileNr=fileDestNr_;
	fp[fileDestNr_].data = 1;

	char myPath[1024] = { 0 };
	strcat(myPath, path.c_str());

	if (!(streamID_.empty()))
	{
		strcat(myPath,  streamID_.c_str() );
		strcat(myPath, "_" );
		strcat(myPath,  FileName.c_str());
	}
	else
	{
		strcat(myPath, FileName.c_str());
	}


	bool result = false;
	exist = false;
	
        if ( (fileref = open(myPath, O_WRONLY|O_CREAT|O_TRUNC|O_BINARY , S_IRWXU | S_IRWXO | S_IRWXG)) != ACE_INVALID_HANDLE)
	{
		writeBuff = new char[recordLength];
		
		for(ACE_INT32 i=0; i<nrOfRecords; i++)
		{
			fillPattern(writeBuff,fileDestNr_,recordLength);

			if(write(fileref,writeBuff,recordLength) != -1)
			{
				fp[fileNr].nr1++;
				fp[fileNr].nr2++;
			}
			else
				exist=true;
		}
		
		fp[fileNr].data++;
		delete [] writeBuff;
		writeBuff = 0;
		close(fileref);
		result = true;
	}
	else 
	{
		perror("error in open in else" );
		ACE_INT32 tmp = ACE_OS::last_error();
		exist=(tmp==EEXIST);
	}
	
	return result;
}

/*******************************************************************************
 * threadStart() -                                                             *
 *******************************************************************************/
bool threadGen::threadStart ()
{
	  //Wait until the start is resumed.
        theRecursiveThreadMutex.acquire();

        theMutexCondition.wait(theRecursiveThreadMutex); 
        theRecursiveThreadMutex.release();

	AES_OHI_FileHandler* myFileSender;	
	aes_ohi_blockHandler2_r1* myBlockSender;
	aes_ohi_directoryHandler_r1* myDirSender;
	
	std::string fileName("");
	std::string fullname("");
	std::string transt("");
        std::string fDest = "GOHSTDEST";
	std::string temppath("");
	ACE_TCHAR subname[10] = { 0 };
	bool exist = false;
	bool lastsentfile = false;
	bool sendPathInConstructor = false;
	ACE_TCHAR dirId[10];
	ACE_TCHAR threadnumber[3];
	ACE_INT32 fileNo =0;
	ACE_INT32 dirnr =1;
	ACE_UINT32 gohstBlockNo = 0;
        ACE_UINT32  reply = 0;
	ACE_UINT32  blockNumber;
	static ACE_INT64 recCounter = 0;
	bool notForceDetach = true;
	bool transTerm = false;
	
	ACE_OS::itoa(fileDestNr_,threadnumber,10);
	fileName = threadnumber;
	fDest = fDest + fileName;    //GOHSTDEST + nr of dest

    /**************************************
	 *     SENDING BLOCKS                 *
	 **************************************/

	if (sendBlock_)
	{
		if (m_nNumCP_Block <= 0)
		{
			if ( this->getDebugMode() )
			{
				cout << "\nMODE: Sending blocks in " << (ifoRpc_?"IFO":"BGW") << " mode ..." << endl << endl;
			}

			// Create instance
			try
			{
				myBlockSender = new aes_ohi_blockHandler2_r1("TST", "TEST", fDest.c_str(), eventText_.c_str(), streamID_.c_str());
			}
			catch (...)
			{
				cerr << "ERROR: Cannot create myBlockSender" << endl << endl;
			}

			while (terminated_ == false)
			{
#ifdef FULLFUNC
				reply = myBlockSender->attach();
#endif
	            if (reply != 0)
				{
					cerr << "ERROR: Failed to try to attach blockhandler" << endl;
					retry_counter2 = 1;
					while( (retry_counter2 <= 3) && (reply != 0) )
					{
						cout << "Trying to reattach blockhandler in " << RETRY_TIME/1000 << " seconds...\n" << endl;
						ACE_OS::sleep(RETRY_TIME/1000);

#ifdef FULLFUNC		
						reply = myBlockSender->attach();						
#endif				
						retry_counter2++;
					}
					if (reply !=0)
					{
						cerr << "ERROR: Check that GOH is running before try run GOHST again" << endl;
						break;
					}
				}
				notForceDetach = true;
				while ((terminated_ == false) && notForceDetach)
				{
					transTerm = false;
#ifdef FULLFUNC			
					reply = myBlockSender->getLastCommitedBlockNo(blockNumber);			
#endif	     				
					// status set of the getLastCommitedBlockNo

					retry_counter2 = 1;
					while( (retry_counter2 <= 3) && (reply == 38) )// connect error, try again and see if contact is established
					{
						ACE_OS::sleep(RETRY_TIME/1000);

#ifdef FULLFUNC		
						reply = myBlockSender->getLastCommitedBlockNo(blockNumber);
#endif				
						retry_counter2++;
					}
				
					if ( (reply != 0) && (reply != 121) ) // try again and see if contact is established
					{
						break;
					}
			
					// the start block number is set
					if (reply == 0)  // last commited block is given
					{
						if (blockNumber <= 9998)
						{
							gohstBlockNo = blockNumber + 1;  //next block to be sent
						}
						else
						{
							gohstBlockNo = 0;       //first block/sequencenumber will be 0 if the last is 9999
						}
					}
					if (reply == 121) // No block is commited
					{
						gohstBlockNo = 1;  //First block will have no 1
					}        

					while ((terminated_ == false) && notForceDetach && (transTerm ==false))
					{
						char* writeBlock;
						fp[fileDestNr_].data = 1;
						fp[fileDestNr_].nr2 = gohstBlockNo;   // Set the sequencenumer same as block number /BJOF 020529
	
#ifdef FULLFUNC
						reply = myBlockSender->transactionBegin();
#endif
						if (reply != 0)
						{
							break;
						}
						if ( this->getDebugMode() )
						{
							cout << "INFO: transaction begin result = " << reply << endl; 
						}
					
						while ((nrOfTransactionSentBytes_ <= (transActLength_ *1024)) && (terminated_ == false) && (transTerm ==false) && (gohstBlockNo <= 9999) )	
						{
							writeBlock = new char[recordLength_];
							fillPattern ( writeBlock, fileDestNr_, recordLength_ );
#ifdef FULLFUNC		
							reply = myBlockSender->send((const char*)writeBlock, recordLength_, gohstBlockNo);
#endif                        
		       				if (reply == 5)
							{
								while((reply == 5) )
								{
#ifdef FULLFUNC		
									reply = myBlockSender->send(writeBlock, recordLength_, gohstBlockNo);
									ACE_OS::sleep(1);
#endif				
								}
							}

							if (reply !=0)
							{
#ifdef FULLFUNC
								reply = myBlockSender->transactionTerminate();
#endif
								if (reply == 0)
								{
									transTerm = true;
								}
								else
								{
									notForceDetach = false;
								}

								break;
							}

							if ( (this->getDebugMode()) && (reply=0) )
							{
								cout << "INFO: Sent blockno = " << gohstBlockNo << endl; 
							}
						
							// Write to logfile
							ACE_TCHAR sendlogBlock[30];
							sprintf(sendlogBlock,"Sending block %d size= %d", gohstBlockNo, recordLength_);
							logfile(sendlogBlock);
							fp[fileDestNr_].nr1++;
							fp[fileDestNr_].nr2++;
						
							// Store nr of sent bytes
							this->addSentBytes( recordLength_ );
							// Store nr of sent bytes within the transaction
							this->addTransactionSentBytes( recordLength_ );

							// Print trace of current block status
							recCounter++;

							if ( this->getDebugMode() )
							{
								cout << "INFO: Last sent block = " << fp[fileDestNr_].nr1 
									 << "\n      Nr. of records sent = " << recCounter << endl << endl;
							}

							ACE_OS::thread_mutex_lock(&MsgCriticalSection);
							ACE_OS::thread_mutex_unlock(&MsgCriticalSection);
							ACE_OS::sleep(frequency_/1000); //file generating frequency

	                        if(writeBlock)   // If block is created, remove it!
							{
								delete []writeBlock;
								writeBlock = 0;
							}
						
							gohstBlockNo ++;
						}// send while

						if ( transTerm == false )
						{
#ifdef FULLFUNC	
							reply= myBlockSender->transactionEnd(blockNumber);
#endif                        
							if (reply != 0) // terminate if unsuccessfull trans end
							{
#ifdef FULLFUNC
								reply = myBlockSender->transactionTerminate();
#endif
								if (reply == 0)
								{
									transTerm = true;
								}							
								else
								{
									notForceDetach = false;
								}
	
								break;						
							}
						}

						clearTransactionSentBytes();

						if ( this->getDebugMode() )
						{
							cout << "INFO: transaction end blockno = " << gohstBlockNo << endl; 
						}

						if ( transTerm == false )
						{
#ifdef FULLFUNC	
							reply = myBlockSender->transactionCommit(blockNumber);
#endif													
							retry_counter2 = 1;

							while( (retry_counter2 <= 3) && (reply == 38) )// connect error, try again and see if contact is established
							{
								ACE_OS::sleep(RETRY_TIME/1000);

#ifdef FULLFUNC		
								reply = myBlockSender->transactionCommit(blockNumber);
#endif

								retry_counter2++;
							}

							if (reply == 0)
							{
								if (blockNumber >= 9999 )
								{
									gohstBlockNo = 0;
								}
								else
								{
									gohstBlockNo = blockNumber + 1;
								}
							}

							if (reply != 0) // terminate if unsuccessfull transend
							{
#ifdef FULLFUNC
								reply = myBlockSender->transactionTerminate();
#endif
								if (reply == 0)
								{
									transTerm = true;
								}
								else
								{
									notForceDetach = false;
								}

								break;
							}
						}

						if ( this->getDebugMode() )
						{
							cout << "INFO: transaction commit blockno = " << gohstBlockNo << endl; 
						}
					}  //while transaction begin
				}//while commit
					
#ifdef FULLFUNC
				reply = myBlockSender->detach();
#endif

#ifdef FULLFUNC				
			} //while attach

			if(myBlockSender)   // Cleanup	
			{
				delete myBlockSender;
				myBlockSender = 0;
			}
#endif
		}//if (m_nNumCP_Block <= 0)
		else
		{
			ACE_INT32 nActiveCP = 0;

			struct structInfoCPBlock * pStructInfoCPBlock = NULL;
			pStructInfoCPBlock = new (struct structInfoCPBlock[m_nNumCP_Block]);

			for (ACE_INT32 ss = 0; ss < m_nNumCP_Block; ss++)
			{
				ACE_TCHAR strMom[5];
				memset (strMom, 0, 5);
				ACE_OS::itoa(ss + 1, strMom, 10);

				pStructInfoCPBlock[ss].strIdName = "CP";
				pStructInfoCPBlock[ss].strIdName += strMom;

				pStructInfoCPBlock[ss].myBlockSender = NULL;
			}

			while (terminated_ == false)
			{
				if ( this->getDebugMode() )
				{
					cout << "\nMODE: Sending blocks in " << (ifoRpc_?"IFO":"BGW") << " mode ..." << endl << endl;
				}

				// Create instance
				try
				{
					if (pStructInfoCPBlock[nActiveCP].myBlockSender == NULL)
						pStructInfoCPBlock[nActiveCP].myBlockSender = new aes_ohi_blockHandler2_r1("TST", "TEST", fDest.c_str(), eventText_.c_str(), pStructInfoCPBlock[nActiveCP].strIdName.c_str());
				}
				catch (...)
				{
					cerr << "ERROR: Cannot create myBlockSender" << endl << endl;
				}
				ACE_INT32 nCountLoop = 0;

				while ((terminated_ == false) && (nCountLoop < 10))
				{
#ifdef FULLFUNC
					reply = pStructInfoCPBlock[nActiveCP].myBlockSender->attach();
#endif
				    if (reply != 0)
					{
						cerr << "ERROR: Failed to try to attach blockhandler" << endl;
						retry_counter2 = 1;
						while( (retry_counter2 <= 3) && (reply != 0) )
						{
							cout << "Trying to reattach blockhandler in " << RETRY_TIME/1000 << " seconds...\n" << endl;
							ACE_OS::sleep(RETRY_TIME/1000);

#ifdef FULLFUNC		
							reply = pStructInfoCPBlock[nActiveCP].myBlockSender->attach();						
#endif				
							retry_counter2++;
						}
						if (reply !=0)
						{
							cerr << "ERROR: Check that GOH is running before try run GOHST again" << endl;
							break;
						}
					}
					notForceDetach = true;
					while ((terminated_ == false) && (notForceDetach) && (nCountLoop < 10))
					{
						transTerm = false;
#ifdef FULLFUNC			
						reply = pStructInfoCPBlock[nActiveCP].myBlockSender->getLastCommitedBlockNo(blockNumber);			
#endif						

						retry_counter2 = 1;
						while( (retry_counter2 <= 3) && (reply == 38) )// connect error, try again and see if contact is established
						{
							ACE_OS::sleep(RETRY_TIME/1000);

#ifdef FULLFUNC		
							reply = pStructInfoCPBlock[nActiveCP].myBlockSender->getLastCommitedBlockNo(blockNumber);
#endif				
							retry_counter2++;
						}

						if ( (reply != 0) && (reply != 121) ) // try again and see if contact is established
						{
							break;
						}
			
						// the start block number is set
						if (reply == 0)  // last commited block is given
						{
							if (blockNumber <= 9998)
							{
								gohstBlockNo = blockNumber + 1;  //next block to be sent
							}
							else
							{
								gohstBlockNo = 0;       //first block/sequencenumber will be 0 if the last is 9999
							}
						}
						if (reply == 121) // No block is commited
						{
							gohstBlockNo = 1;  //First block will have no 1
						} 

						while ((terminated_ == false) && notForceDetach && (transTerm == false)&& (nCountLoop < 10))
						{
							ACE_TCHAR *writeBlock;
							fp[fileDestNr_].data = 1;
							fp[fileDestNr_].nr2 = gohstBlockNo;   // Set the sequencenumer same as block number /BJOF 020529
	
#ifdef FULLFUNC
							reply = pStructInfoCPBlock[nActiveCP].myBlockSender->transactionBegin();
#endif
							if (reply != 0)
							{
								break;
							}
							if ( this->getDebugMode() )
							{
								cout << "INFO: transaction begin result = " << reply << endl; 
							}
					
							while ((nrOfTransactionSentBytes_ <= (transActLength_ *1024)) && (terminated_ == false) && (transTerm ==false) && (gohstBlockNo <= 9999) && (nCountLoop < 10))	
							{
								writeBlock = new char[recordLength_];
								fillPattern ( writeBlock, fileDestNr_, recordLength_ );
#ifdef FULLFUNC		
								reply = pStructInfoCPBlock[nActiveCP].myBlockSender->send(writeBlock, recordLength_, gohstBlockNo);						
								ACE_OS::sleep(1000/1000);
#endif                        
		       					if (reply == 5)
								{
									while((reply == 5) )
									{
#ifdef FULLFUNC		
										reply = pStructInfoCPBlock[nActiveCP].myBlockSender->send(writeBlock, recordLength_, gohstBlockNo);						
										ACE_OS::sleep(1000/1000);
#endif				
									}
								}

								if (reply !=0)
								{
#ifdef FULLFUNC
									reply = pStructInfoCPBlock[nActiveCP].myBlockSender->transactionTerminate();
#endif
									if (reply == 0)
									{
										transTerm = true;
									}
									else
									{
										notForceDetach = false;
									}

									break;
								}

								if ( (this->getDebugMode()) && (reply=0) )
								{
									cout << "INFO: Sent blockno = " << gohstBlockNo << endl; 
								}
						
								// Write to logfile
								ACE_TCHAR sendlogBlock[30];
								sprintf(sendlogBlock,"Sending block %d size= %d", gohstBlockNo, recordLength_);
								logfile(sendlogBlock);
								fp[fileDestNr_].nr1++;
								fp[fileDestNr_].nr2++;
						
								// Store nr of sent bytes
								this->addSentBytes( recordLength_ );
								// Store nr of sent bytes within the transaction
								this->addTransactionSentBytes( recordLength_ );

								// Print trace of current block status
								recCounter++;

								if ( this->getDebugMode() )
								{
									cout << "INFO: Last sent block = " << fp[fileDestNr_].nr1 
										 << "\n      Nr. of records sent = " << recCounter << endl << endl;
								}

								ACE_OS::thread_mutex_lock(&MsgCriticalSection);
								ACE_OS::thread_mutex_unlock(&MsgCriticalSection);
								ACE_OS::sleep(frequency_/1000); //file generating frequency

								if(writeBlock)   // If block is created, remove it!
								{
									delete []writeBlock;
									writeBlock = 0;
								}
						
								gohstBlockNo ++;
								nCountLoop++;
							}// send while

							if ( transTerm == false )
							{
#ifdef FULLFUNC	
								reply= pStructInfoCPBlock[nActiveCP].myBlockSender->transactionEnd(blockNumber);
#endif                        
								if (reply != 0) // terminate if unsuccessfull trans end
								{
#ifdef FULLFUNC
									reply = pStructInfoCPBlock[nActiveCP].myBlockSender->transactionTerminate();
#endif
									if (reply == 0)
									{
										transTerm = true;
									}							
									else
									{
										notForceDetach = false;
									}
	
									break;						
								}
							}

							clearTransactionSentBytes();

							if ( this->getDebugMode() )
							{
								cout << "INFO: transaction end blockno = " << gohstBlockNo << endl; 
							}

							if ( transTerm == false )
							{
#ifdef FULLFUNC	
								reply = pStructInfoCPBlock[nActiveCP].myBlockSender->transactionCommit(blockNumber);
#endif													
								retry_counter2 = 1;

								while( (retry_counter2 <= 3) && (reply == 38) )// connect error, try again and see if contact is established
								{
									ACE_OS::sleep(RETRY_TIME/1000);

#ifdef FULLFUNC		
									reply = pStructInfoCPBlock[nActiveCP].myBlockSender->transactionCommit(blockNumber);
#endif

									retry_counter2++;
								}

								if (reply == 0)
								{
									if (blockNumber >= 9999 )
									{
										gohstBlockNo = 0;
									}
									else
									{
										gohstBlockNo = blockNumber + 1;
									}
								}

								if (reply != 0) // terminate if unsuccessfull transend
								{
#ifdef FULLFUNC
									reply = pStructInfoCPBlock[nActiveCP].myBlockSender->transactionTerminate();
#endif
									if (reply == 0)
									{
										transTerm = true;
									}
									else
									{
										notForceDetach = false;
									}

									break;
								}
							}

							if ( this->getDebugMode() )
							{
								cout << "INFO: transaction commit blockno = " << gohstBlockNo << endl; 
							}
						}  //while transaction begin
					}//while commit
					
#ifdef FULLFUNC
					reply = pStructInfoCPBlock[nActiveCP].myBlockSender->detach();
#endif

#ifdef FULLFUNC		
				} //while attach

#endif

				if (nCountLoop >= 10)
					nCountLoop = 0;

				nActiveCP++;

				if (nActiveCP >= m_nNumCP_Block)
					nActiveCP = 0;

			} //while terminated == false

			for (ACE_INT32 ss = 0; ss < m_nNumCP_Block; ss++)
			{
				if (pStructInfoCPBlock[ss].myBlockSender != NULL)
				{
					delete pStructInfoCPBlock[ss].myBlockSender;
					pStructInfoCPBlock[ss].myBlockSender = NULL;
				}
			}

			delete [] pStructInfoCPBlock;

		}//else if (m_nNumCP_Block <= 0)
	} //if (sendBlock_)
		

   	/**************************************
	 *     SENDING DIRECTORIES            *
	 **************************************/
	else if (sendDir_)
	{
		path_ = createPath(path_);
		
		path_ = "/data/aes/data";

		try
		{
			myDirSender = new aes_ohi_directoryHandler_r1("TST","TEST",fDest.c_str(),eventText_.c_str(),path_.c_str());
		}
		catch (...)
		{
			cerr << "ERROR: Cannot create myDirSender" << endl << endl;
		}

#ifdef FULLFUNC
		
REATTACH_DIRHANDLER:
		if ( showResult( "Attach directory sender", myDirSender->attach() ) )  
		{
			char pathBuff[1024] = { 0 };
			showResult( "GetDirectoryPath()", myDirSender->getDirectoryPath(pathBuff) ); 
			if ( this->getDebugMode() )
			{
				cout << "INFO: getDirectoryPath() returned " << myDirSender->getDirectoryPath(pathBuff);  // @@
			}
			path_ = pathBuff;
#endif

			// Create and send directories. Directores temp0001, temp002 etc. are
			// created. These directories contains x files each, where x is specified
			// by options -a
			if (partOfDir_ != 0)
			{
				if ( this->getDebugMode() )
					cout << "MODE: Creating & sending directories." << endl << endl;

				fp[fileDestNr_].nr2 = 1;  // Set recordnumber to start with 0001

				while (terminated_ == false)
				{
					sprintf(dirId,"%04u",dirnr++);
					temppath = path_  + "/" + "temp" + dirId;
					temppath = createPath(temppath);
					temppath += "/";

					for (ACE_INT32 i =1; i<=partOfDir_; i++)
					{
						sprintf(subname,"%04u",++fileNo);
						fullname = subname;
						if(!GenerateFile(temppath,fullname,nrOfRecords_,recordLength_,exist))
							--i;   //try again
						fullname = "temp" + string(dirId);
					}

					if(!exist)
					{
#ifdef FULLFUNC			
						if(transType_ == 0)  
						{
							if (showResult("Send directory in binary mode", myDirSender->send(fullname.c_str(), AES_OHI_BINARY, sendMask_.c_str())))
							{}
						}
						else if(transType_ != 0)
						{
							if (showResult("Send directory in binary mode", myDirSender->send(fullname.c_str(),AES_OHI_ASCII, sendMask_.c_str())))
							{}
						}
												 			
#endif
						ACE_TCHAR sendlogdir[35] = "Sending directory temp ";
						strcat(sendlogdir,dirId);
						logfile(sendlogdir);
					
						// Store nr of sent bytes
						this->addSentBytes( recordLength_ );

						ACE_OS::thread_mutex_lock(&MsgCriticalSection);
						ACE_OS::thread_mutex_unlock(&MsgCriticalSection);
						ACE_OS::sleep(frequency_/1000); //file generating frequency
					}
				}
#ifdef FULLFUNC
				showResult( "Detach directory sender", myDirSender->detach() );  
#endif
				if(myDirSender)
				{
					delete myDirSender;
					myDirSender = 0;
				}
			}
			// Sending already existing directory/directory structure
			else
			{
				if ( this->getDebugMode() )
					cout << "MODE: Sending already existing directory." << endl << endl;

				path_ += "/";
				refPath_ = checkPath(refPath_);
				std::string s = this->lastDir(refPath_);
				refPath_ += "/";

				if( true)
				{
					s = "GOHST";
#ifdef FULLFUNC				
					if(transType_ == 0)
					{
						if (showResult("Send directory in binary mode", myDirSender->send(s.c_str(), AES_OHI_BINARY, sendMask_.c_str())))
						{}
					}
					else if(transType_ != 0)
					{
						if (showResult("Send directory in ascii mode", myDirSender->send(s.c_str(), AES_OHI_ASCII, sendMask_.c_str())))
						{}
					}
#endif
					ACE_TCHAR sendlog[50] = "Sending";
					strcat(sendlog, " directory ");
					strcat(sendlog, s.c_str());
					logfile(sendlog);
				
					// End Deleted from qmicsal
				}
				// Directory to be sent did not exist
				else
				{
					ACE_TCHAR sendDirFailure[100];
					sprintf(sendDirFailure,"ERROR: Directory %s does NOT exist.", refPath_.c_str());//, s.c_str());
					cerr << "ERROR: Directory " << refPath_.c_str() << " does NOT exist." << endl << endl;
					logfile(sendDirFailure);
				}
					
#ifdef FULLFUNC				
				showResult( "Detach directory sender", myDirSender->detach() );
#endif				
				if(myDirSender)
				{
					delete myDirSender;
					myDirSender = 0;
				}
			}
			
#ifdef FULLFUNC			
		} //if
		else
		{
			cerr << "ERROR: Failed to attach dirhandler" << endl;
			
			while(retry_counter<=3)
			{
				retry_counter++;
				cout << "Trying to reattach dirhandler in " << RETRY_TIME/1000 << " seconds...\n";
				ACE_OS::sleep(RETRY_TIME/1000);
				goto REATTACH_DIRHANDLER;
			}
		}
#endif
	}	


    /**************************************
	 *     SEND SPECIFIED FILE            *
	 **************************************/
	else if(sendFile_)  // Send specified file
	{
		if ( this->getDebugMode() )
			cout << "MODE: Sending specified file." << endl << endl;

		try
		{
			while(this->getDebugMode())
			{
			}
			myFileSender = new AES_OHI_FileHandler("TEST_TOOL", "GOHST", fDest.c_str(), eventText_.c_str(), path_.c_str());
			// Last arg. path_ removed 
		}
		catch (...)
		{
			cerr << "ERROR: Cannot create myFileSender" << endl << endl;
		}
		
#ifdef FULLFUNC		
		
REATTACH_SPECFILEHANDLER:
		
		if (showResult("Attach file spec. sender", myFileSender->attach())) 
		{
			char strPath[1024] = { 0 };
			showResult("getDirectoryPath()", myFileSender->getDirectoryPath(strPath));   

			if ( this->getDebugMode() )
			{
				cout << "INFO: getDirectoryPath() returned " << myFileSender->getDirectoryPath(strPath) ;  // @@
				cout << "[path_=" << path_.c_str() << "]" << endl;  // @@
			}
			path_ = strPath;
#endif	

			path_ = createPath(path_);
			path_ += "/";
			refPath_ = checkPath(refPath_);
			std::string fileRes = this->copy_file(path_, refPath_);

			ACE_TCHAR sendToLog[100];
	
			if( fileRes.empty() )
			{
				sprintf(sendToLog,"ERROR: Failed to send requested file. File not available in %s", path_.c_str());
				logfile(sendToLog);
			}
			else 
			{
				std::string file = path_ + fileRes;

				sprintf(sendToLog,"Sending file %s ", file.c_str());

				logfile(sendToLog);

				if ( this->getDebugMode() )
					cout << " Sending " << file.c_str() << endl << endl;

#ifdef FULLFUNC	
				if(transType_ == 0)
				{
					if (showResult("Send file in binary mode", myFileSender->send(fileRes.c_str(), AES_OHI_BINARY)))
					{}
				}
				else if(transType_ != 0)
				{
					if (showResult("Send file in ascii mode", myFileSender->send(fileRes.c_str(), AES_OHI_ASCII)))
					{}
				}
#endif
				
				while (terminated_ == false)   // @@
			
				ACE_OS::thread_mutex_lock(&MsgCriticalSection);
				ACE_OS::thread_mutex_unlock(&MsgCriticalSection);

			} // else (fileRes != empty)
		
#ifdef FULLFUNC			
		showResult("Detach file spec. sender", myFileSender->detach());
#endif
		if(myFileSender)
		{
			delete myFileSender; 
			myFileSender = 0;
		}
			
#ifdef FULLFUNC				
		}  // if attach
		else
		{
			cerr << "ERROR: Failed to attach filehandler" << endl;
			
			while( retry_counter <= 3 )
			{
				retry_counter++;
				cout << "Trying to reattach filehandler in " << RETRY_TIME/1000 << " seconds...\n";
				ACE_OS::sleep(RETRY_TIME/1000);
				goto REATTACH_SPECFILEHANDLER;
			}
		} 
#endif		
	}  // else if(sendFile)


   	/**************************************
	 *     CREATE & SEND FILES            *
	 **************************************/
	else  
	{
		if ( this->getDebugMode() )
			cout << "MODE: Creating & sending files." << endl << endl;

		fp[fileDestNr_].nr2 = 1;   //Set id of file to start with

		
		path_ = createPath(path_);

		std::string old_path_ = path_;

		ACE_INT32 i = this->fileDestNr_;
		std::string path_K = "/data/aes/data/GOHST/GOHSTDEST";
		char buffer[10];

		sprintf(buffer, "%d", i );

		path_K += string(buffer);

		path_K = createPath(path_K);
		

CREATE_FILEHANDLER:
		
		try
		{
			if ( this->getDebugMode() )
				cout << "INFO: Creating new filehandler with ";

			if(sendPathInConstructor)
			{
				cout << " calling file handler constructor with file destination " << fDest.c_str() << "event text = " << eventText_.c_str() << "path = "<< path_K.c_str() << endl;

				myFileSender = new AES_OHI_FileHandler( "TEST_TOOL", "GOHST", fDest.c_str(), eventText_.c_str(), path_K.c_str());
				
			}
			else
			{
				myFileSender = new AES_OHI_FileHandler( "TEST_TOOL", "GOHST", fDest.c_str(),eventText_.c_str() , path_K.c_str());

				if ( this->getDebugMode() )
					cout << "4 args. " << endl;
			}
		}
		catch (...)
		{
			cerr << "ERROR: Cannot create myFileSender" << endl << endl;
		}
		
#ifdef FULLFUNC		
		
REATTACH_FILEHANDLER:
		
		if (showResult("Attach file sender", myFileSender->attach())) 
		{
			if (showResult("getLastSentFile()", myFileSender->getLastSentFile((char*)fullname.c_str())))
			{
				lastsentfile = true;
				strcpy(subname,fullname.c_str());		
				fileNo = atoi(subname); 
			}
			
			std::string tPath;
			char strtPath[1024] = { 0 };

			showResult("getDirectoryPath()", myFileSender->getDirectoryPath(strtPath));
			tPath = strtPath;

			// use the default path.
			if(!tPath.empty())
			{
				path_ = tPath;
				cout << "GOHST> ";
			}
			else  
			{
				// Since no path is defined by AFP, use gohsts default path. Terminate this fileHandler object
				// and create a new one submitting the gohst default path to AFP.

				cout << "INFO: Path (tPath) was empty, using gohst's default path..." << endl;
		
				// Detach & delete current myFileSender object
				showResult( "Detach file sender", myFileSender->detach() );

				if(myFileSender)
				{
					delete myFileSender; 
					myFileSender = 0;
				}
			
				cout << "INFO: Removed current filehandler object..." << endl;

				// Set to use constructor with given path
				sendPathInConstructor = true;
	
				goto CREATE_FILEHANDLER;
			}

#endif	
			path_ = path_ + "/";
			old_path_ = old_path_ + "/";

			
			while (terminated_ == false)
			{
				sprintf(subname,"%04u",++fileNo);
				fullname = subname;

				
				
				if(!GenerateFile(path_, fullname, nrOfRecords_, recordLength_, exist))
					exist = true;   //try to generate the file again
				if(!exist)
				{
#ifdef FULLFUNC	

/////////////////////////////////////////
					std::string fullPath_2;
					if (!(streamID_.empty()))
					{
						fullPath_2 = streamID_ + "_" + fullname;
					}
					else
					{
						fullPath_2 = fullname;
					}

					std::string strFullPath_2 = fullPath_2;

					
					std::string strFileExisting = old_path_ + strFullPath_2;
					std::string strFileDestination = path_K + "/" + strFullPath_2;
					ACE_OS::rename(strFileExisting.c_str(), strFileDestination.c_str());
					


/////////////////////////////////////////

					if(transType_ == 0)
					{
						//AES_OHI_Format sendFormat = AES_OHI_BINARY;	
						//if (showResult("Send files in binary mode", myFileSender->send(fullname.c_str(), AES_OHI_BINARY)))
						if (showResult("Send files in binary mode", myFileSender->send(strFullPath_2.c_str(), AES_OHI_BINARY)))
						{}
					}
					else if(transType_ != 0)
					{
						if (showResult("Send files in ascii mode", myFileSender->send(strFullPath_2.c_str(), AES_OHI_ASCII)))
						{}
					}
#endif
					lastSentFileNr_ = fileNo;
					ACE_OS::sleep(frequency_/1000);
					ACE_TCHAR sendlog[20] = "Sending";
					strcat(sendlog, " file ");
					strcat(sendlog,subname);
					logfile(sendlog);

					// Store nr of sent bytes
					this->addSentBytes( recordLength_ );

					ACE_OS::thread_mutex_lock(&MsgCriticalSection);
					ACE_OS::thread_mutex_unlock(&MsgCriticalSection);
				}
			}  //while
			
#ifdef FULLFUNC			
			showResult("Detach file sender", myFileSender->detach());
#endif
			if(myFileSender)
			{
				delete myFileSender; 
				myFileSender = 0;
			}
			
#ifdef FULLFUNC				
		}
		else
		{
			cerr << "ERROR: Failed to attach filehandler" << endl;
			
			while( retry_counter <= 3 )
			{
				retry_counter++;
				cout << "Trying to reattach filehandler in " << RETRY_TIME/1000 << " seconds...\n";
				ACE_OS::sleep(RETRY_TIME/1000);
				goto REATTACH_FILEHANDLER;
			}
		} 
#endif		
		
	} // else create & send files 
	
	retry_counter = 0;
	return 0;
}

/*******************************************************************************
 * resume() -                                                                  *
 *******************************************************************************/
void threadGen::resume ()
{
	  //Acquire the mutex.
        theRecursiveThreadMutex.acquire();

        //Signal the condition variable.
        theMutexCondition.signal();

        //Release the mutex.
        theRecursiveThreadMutex.release();
}

/*******************************************************************************
 * suspend() -                                                                 *
 *******************************************************************************/
void threadGen::suspend ()
{
}

/*******************************************************************************
 * terminate() -                                                               *
 *******************************************************************************/
void threadGen::terminate ()
{
	terminated_ = true;
}

/*******************************************************************************
 * logfile() -                                                                 *
 *******************************************************************************/
void threadGen::logfile( char* logtext )
{
	std::string templog;
	templog = logPath_ + "/log.txt";
	ACE_TCHAR *writelogBuff;
	ACE_TCHAR buf[20];
	time_t stCreate;
        struct tm* time1;
        time(&stCreate);
        time1 = localtime(&stCreate);
        ACE_OS::sprintf(buf,
                        ACE_TEXT("%04d-%02d-%02d  %02d:%02d:%02d"),
                        time1->tm_year+1900,
                        (time1->tm_mon+1)%13,
                        time1->tm_mday,
                        time1->tm_hour,
                        time1->tm_min,
                        time1->tm_sec);

	
	writelogBuff = new char[strlen(logtext) + 39];
	
	ACE_OS::sprintf(writelogBuff, "%s GOHSTDEST%d %s ", time, fileDestNr_, logtext);  // %02d
	 
	ofstream skrivfil(templog.c_str(), ios::app);
	skrivfil  << writelogBuff << endl;
	skrivfil.close();
	
	delete [] writelogBuff;
	writelogBuff = 0;
	templog = "";
}

/*******************************************************************************
 * copy_dir_tree() - Copies defined directory structure into gohst directory.  *
 *******************************************************************************/
bool threadGen::copy_dir_tree( std::string refpath, std::string s)
{
	static ACE_INT32 t=0, j=0;

	std::string str,     // error message
		        rPath,   // reference path 
				gPath;   // gohst path

	if(!s.empty())
	{
		if(ACE_OS::mkdir((path_+s).c_str(),ACE_DEFAULT_DIR_PERMS) == -1)
		{
			str = "Could not copy direcory " + path_ + s;
			logfile(const_cast<char*>(str.c_str()));
		}
		path_ += s;
		path_ += "/";
	}
		
        dip = opendir(refpath.c_str());

          if (dip == NULL)
		return false;
	
        while( (dit = readdir(dip)) != NULL)
	{
                if (!strcmp(dit->d_name,".") ||!strcmp(dit->d_name,".."))
                     continue;

                string filePath = refpath + string(dit->d_name);
                struct stat buf;
                stat(filePath.c_str(), &buf);
		
		if(S_ISDIR(buf.st_mode))
		{
			gPath = path_ + string(dit->d_name);
			
			if(ACE_OS::mkdir(gPath.c_str(),ACE_DEFAULT_DIR_PERMS) == -1)
			{
				str = "Could not copy direcory " + gPath;
				logfile(const_cast<char*>(str.c_str()));
			}
		}
		else //if(fileDirExist != 0)
		{
			rPath = refpath + string(dit->d_name);
			gPath = path_ + string(dit->d_name);
			
			if(CopyFile(rPath.c_str(), gPath.c_str()) == false)
			{
				str = "Could not copy file " + gPath;
				logfile(const_cast<char*>(str.c_str()));
			}
		}
		
		if (S_ISDIR(buf.st_mode))
		{
			f[t].rpath = refpath;
			f[t].cpath = path_;
			refpath = refpath + string(dit->d_name) + "/";
			path_ = path_ + string(dit->d_name) + "/";
			f[t].fFindFileData = dit;
			f[t].fhFind = dip;
			j++;
			t++;
			copy_dir_tree(refpath, "");
		}
	}  // while
	
	while(j > 0)
	{
                dit = readdir(f[j-1].fhFind);
		
		if(dit != NULL)
		{
                    struct stat buf;
                    string temp = f[j-1].rpath + string(dit->d_name);
                    stat(temp.c_str(), &buf);
		    if (S_ISDIR(buf.st_mode))
		    {
			gPath = f[j-1].cpath + string(dit->d_name); //FindFileData.cFileName;
				
			if(ACE_OS::mkdir(gPath.c_str(),ACE_DEFAULT_DIR_PERMS) == -1)
			{
				str = "Could not copy directory " +gPath;
				logfile(const_cast<char*>(str.c_str()));
			}
		   }
		   else
		   {
			rPath = f[j-1].rpath + string(dit->d_name); //FindFileData.cFileName;
			gPath = f[j-1].cpath + string(dit->d_name); //FindFileData.cFileName;
				
			if(CopyFile(rPath.c_str(),gPath.c_str()) == false)
			{
				str = "Could not copy file " +gPath;
				logfile(const_cast<char*>(str.c_str()));
			}
		  }   // else
			
		 if (S_ISDIR(buf.st_mode))
	         {
			f[j-1].fFindFileData = dit; //FindFileData;
			refpath = f[j-1].rpath + string(dit->d_name) + "/";
			path_ = f[j-1].cpath + string(dit->d_name) + "/";
			copy_dir_tree(refpath, "");
		}
			
		}  // if (fileDirExist != 0)
		else
		{
			j -- ;
			t -- ;
		} // else
	} // while(j != 0)

	return true;
}

/*******************************************************************************
 * copy_file() -                                                               *
 *******************************************************************************/
std::string threadGen::copy_file( std::string destination, 
								  std::string source )
{
	std::string str,   // String that contains information written to log file
		        sourceFile;   // String containing file name of the source file
	ACE_INT32 i;	
	for(i = source.length(); i >= 0; i--)
	{
		if(source[i] == '/')
			break;
	}
        i++;
	sourceFile = source.substr(i, source.length()-i);
	destination += sourceFile;

	if(CopyFile(source.c_str(), destination.c_str()) == false)
	{
		str = "Could not copy file " + source;
		logfile(const_cast<ACE_TCHAR*>(str.c_str()));
		return "";
	}
	else
	{
		str = "File " + source + " copied into " + destination;
		logfile(const_cast<char*>(str.c_str()));
		return sourceFile;
	}
}

/*******************************************************************************
 * showResult() -                                                              *
 *******************************************************************************/
bool threadGen::showResult( std::string operation,
						    ACE_UINT32 res )
{
	if ( res == AES_OHI_NOERRORCODE )
		return true;
	else
	{
		ACE_TCHAR *logMsg;
		logMsg = new char[100];
		ACE_OS::sprintf(logMsg,"ERROR(%d): %s %s", res, operation.c_str(), getErrCodeText(res) );
		this->logfile(logMsg);
		if(logMsg)
		{
			delete logMsg;
			logMsg=0;
		}

		return false;
	} 
}

/*******************************************************************************
 * createPath() -                                                              *
 *******************************************************************************/
std::string threadGen::createPath( std::string p )
{
	p = checkPath(p);
	ACE_INT32 pos =4;
	std::string temp;
	
	while(pos < p.length())
	{
		pos = p.find_first_of("/",pos);
	
		if (pos == -1)
			pos = p.length();
		
		temp = p.substr(0,pos);
		ACE_OS::mkdir(temp.c_str(),ACE_DEFAULT_DIR_PERMS);
		pos += 1;
	}

	return temp;
}

/*******************************************************************************
 * checkPath() -                                                               *
 *******************************************************************************/
std::string threadGen::checkPath( std::string p )
{
	#if 0
	ACE_INT32 pos = 0;
	std::string temp = p;
	
	while(pos < p.length() || pos != string::npos)
	{
		pos = p.find("/",pos);
		
		if(pos != string::npos)
		{
			p.replace(pos,1,"/");
			pos ++;
		}
	}

	pos = 0;
	
	while(pos < p.length() || pos != string::npos)
	{
		pos = p.find("/",pos);
		
		if(pos != string::npos)
		{
			if(p[pos+1] != '/')
			{
				p.insert(pos+1,"/");
				pos ++;
			}
			else 
				pos ++;
			
			pos ++;
		}
	}
	
	pos = 0;
	
	while(p.find("/",pos) != string::npos)
	{
		pos = p.find("/",pos);

		if(pos != string::npos)
			p.erase(pos,1);
	}
	
	if(temp != p)
	{
		temp += " changed to ";
		temp += p;
		logfile(const_cast<ACE_TCHAR *>(temp.c_str()));
	}
	#endif
	return p;
}

/*******************************************************************************
 * lastDir() -                                                                 *
 *******************************************************************************/
std::string threadGen::lastDir( std::string p )
{
	ACE_INT32 pos = -1;
        ACE_INT32 i;
	for(i=p.length(); i>=0; --i)
	{
		if(p[i] == '/')
		{
			pos = i;
			break;
		}
	}

	return p.substr(i+1, p.length()-i-1);
}

//Fråga varför detta inte var bortkommenterat, fick errors anropet ???????
/*******************************************************************************
 * listDefDest() -                                                             *
 *******************************************************************************/
/*bool threadGen::listDefDest()
{
	std::list<std::string> fileDestList;
	std::list<std::string>::iterator itl;

	if ( showResult( "fileDestDefined", getFileDestinations(fileDestList) ) )
	{
		for ( itl = fileDestList.begin(); itl != fileDestList.end(); itl++ )
			cout << (*itl)  <<endl;
		return true;		
	}
	return false; 
}*/
