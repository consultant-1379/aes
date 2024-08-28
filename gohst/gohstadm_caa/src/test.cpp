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
//         RxA  2001-       QABMSOL
//			
//	LINKAGE
//
//	SEE ALSO
//

#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <ace/ACE.h>

#include <test.h>
#include <threadgen.h>

#define RESCUE 1

using namespace std;


/*******************************************************************************
 * Test() - Constructor.                                                       *
 *******************************************************************************/
Test::Test(ACE_TCHAR * pCPName)
:continue_(false), destNr_(0), recLength_(1024), nrOfRec_(100), transActLength_(100), freq_(1000), 
 threadNumber_(0), transType_(2), amount_(0), amount_block_(-1),
 status_(9), eventText_("EventText"), mask_("*.*") 
{
	streamID_ = pCPName;
        gohstDefaultPath_ = "/opt/ap/aes/bin/";
	logDefaultPath_ = "/var/log/gohst/";

	if (!(streamID_.empty()))
	{
		gohstDefaultPath_ += "_";
		gohstDefaultPath_ += pCPName;

		logDefaultPath_ += "_";
		logDefaultPath_ += pCPName;
	}

 	s_ = new Supervisor(pCPName);
	// End modified from qmicsal
	
	if (RESCUE)
	{
		std::string rescuePath = "";
		rescuePath += s_->getDefaultDisc();

		// Modified from qmicsal 21/02/2007
		if (!(streamID_.empty()))
		{
			rescuePath += "gohst_rescue/gohst_" + streamID_ + ".rec";
		}
		else
		{
			rescuePath += "gohst_rescue/gohst.rec";
		}
		// End Modified from qmicsal

		ifstream in( rescuePath.c_str() );
		ACE_TCHAR msg[100];
		
		if(!in)
		{
		}
		else
		{
			in.close();

			cout << "INFO: Previous gohst session were not terminated correctly \n";
			cout << "      last time. Trying to restore previous gohst session.\n";
			cout << "      (Enter \"list\" to check status)\n\n";
			cout << "      If a new gohst session is wanted enter \"exit\"\n";
			cout << "      and restart gohst." << endl << endl;

			s_->recoverFromRescueFile();
			
			for(ACE_INT32 i = 0;i < s_->getNumberOfDest(); i++)
			{
				destNr_ = i+1;
				tg_[destNr_] = new threadGen(s_->getIntOptions(destNr_-1, 3), s_->getIntOptions(destNr_-1, 4), s_->getIntOptions(destNr_-1, 5), s_->getIntOptions(destNr_-1, 2), s_->getStrOptions(destNr_-1, 5), destNr_, s_->getIntOptions(destNr_-1, 6), s_->getStrOptions(destNr_-1, 1), s_->getStrOptions(destNr_-1, 7));
                                tg_[destNr_]->suspend();
				tg_[destNr_]->sendDir_ = false;
				tg_[destNr_]->sendMask_ = s_->getStrOptions(destNr_-1, 3);
				s_->setStrOptions(destNr_-1, 2, tg_[destNr_]->createPath(s_->getStrOptions(destNr_-1, 2)));
				tg_[destNr_]->logPath_ = s_->getStrOptions(destNr_ -1 , 2);
				sprintf(msg, "Destination %d restarted from file", destNr_);
				
				if(s_->getStrOptions(i, 6) != "IFORPC")
				{
					tg_[destNr_]->ifoRpc_ = 0;   //BGW

					if(s_->getStrOptions(destNr_-1, 6) == "BGWRPC")
						tg_[destNr_]->sendBlock_ = true;
					else
						tg_[destNr_]->sendBlock_ = false;
				}
				else
				{
					tg_[destNr_]->ifoRpc_ = 1;   //IFO
					tg_[destNr_]->sendBlock_ = true;
				}
				
				tg_[destNr_]->m_nNumCP_Block = s_->getIntOptions(destNr_-1, 8);

				tg_[destNr_]->logfile(msg);	
				
				if(s_->getStrOptions(i, 4) == "Started")
				{
					tg_[destNr_]->resume();
				}
				
				if(s_->getStrOptions(i, 4) == "Paused")
				{
					tg_[destNr_]->resume();
					tg_[destNr_]->suspend();
				}
				status_ = 2;
			}
		} // else
	}
}

/*******************************************************************************
 * ~Test() - Destructor.                                                       *
 *******************************************************************************/
Test::~Test()
{
	if(s_)
	{
		delete s_;
		s_=0;
	}
}

/*******************************************************************************
 * getCommand()                                                                *
 *******************************************************************************/
bool Test::getCommand ()
{
	ACE_INT32 k = 0;
	cin.getline(command_, 98);
	    
	if( strstr(command_, "def")!=NULL || strstr(command_, "DEF")!=NULL )
		k=1;
	else if( (strstr(command_, "list")!=NULL || strstr(command_, "LIST")!=NULL) && (strlen(command_)==4 || strlen(command_)==7 ) )
		k=2;
	else if( strstr(command_, "pause")!=NULL || strstr(command_, "PAUSE")!=NULL )
		k=3;
	else if( strstr(command_, "start")!=NULL || strstr(command_, "START")!=NULL )
		k=4;
	else if( strstr(command_, "stop")!=NULL || strstr(command_, "STOP")!=NULL )
		k=5;
	else if( (strstr(command_, "exit")!=NULL || strstr(command_, "EXIT")!=NULL) && strlen(command_) == 4)
		k=6;
	else if( (strstr(command_, "help")!=NULL || strstr(command_, "HELP")!=NULL) && strlen(command_) == 4)
		k=7;
	else if( strstr(command_, "dir")!=NULL || strstr(command_, "DIR")!=NULL )
		k=8;
	else if( (strstr(command_, "block")!=NULL || strstr(command_, "BLOCK")!=NULL ) && (strlen(command_)==5 || strlen(command_)==10) )
		k=9;
	else if( strstr(command_, "file")!=NULL || strstr(command_, "FILE")!=NULL ) 
		k=10;
	else if( (strstr(command_, "clear")!=NULL || strstr(command_, "CLEAR")!=NULL) && strlen(command_) == 5)
		k=11;
	else if( (strstr(command_, "debug on")!=NULL || strstr(command_, "DEBUG ON")!=NULL) && strlen(command_) == 8)
		k=12;
	else if( (strstr(command_, "debug off")!=NULL || strstr(command_, "DEBUG OFF")!=NULL) && strlen(command_) == 9)
		k=13;
	else if( (strstr(command_, "version")!=NULL || strstr(command_, "VERSION")!=NULL) && strlen(command_) == 7)
		k=14;
	else if( (strstr(command_, "info")!=NULL || strstr(command_, "INFO")!=NULL) && strlen(command_) == 4)
		k=15;
	else
		k=-1;

	switch(k)
	{
	case 1:  //def
		{
			ACE_INT32 i = 0;

			while(( command_[i] != ' ') && (command_[i] != '\0'))
				i++;
			
			if (( i == 3 && command_[4]=='-') || ACE_OS::strlen(command_)==3 )
			{
				if(status_ == 0 || status_ == 3 || status_ == 5 || status_ == 7 || status_ == 8 || status_ == 9)
				{
					if(checkParam(command_))
					{
						status_ = 0;   // Def ok -> status_ = 0
						gohstdef(command_);
					}
					else
					{
						if(status_ != 0)
							status_ = 9;
						goto DEF_ERROR;
					}
					
					if(threadNumber_ <= s_->getNumberOfDest())
					{
						if(threadNumber_ == 0)
						{
							//threadNumber_ = 0;
							continue_ = true;
							destNr_ = 0;
							
							while(destNr_<s_->getNumberOfDest())
							{
								destNr_++;
								tg_[destNr_] = new threadGen(s_->getIntOptions(destNr_-1, 3), s_->getIntOptions(destNr_-1, 4), s_->getIntOptions(destNr_-1, 5), s_->getIntOptions(destNr_-1, 2), s_->getStrOptions(destNr_-1, 5), destNr_, s_->getIntOptions(destNr_-1, 6), s_->getStrOptions(destNr_-1, 1), s_->getStrOptions(destNr_-1, 7));
                                                                tg_[destNr_]->suspend();
								tg_[destNr_]->sendDir_ = false;
								tg_[destNr_]->sendMask_ = s_->getStrOptions(destNr_-1, 3);
                                                                cout << "DEEP : strOpt = " << s_->getStrOptions(destNr_-1, 1);
								s_->setStrOptions(destNr_-1, 1, tg_[destNr_]->createPath(s_->getStrOptions(destNr_-1, 1)));  // SMÄLL
								tg_[destNr_]->path_ = s_->getStrOptions(destNr_-1 , 1);
								s_->setStrOptions(destNr_-1, 2, tg_[destNr_]->createPath(s_->getStrOptions(destNr_-1, 2)));
								tg_[destNr_]->logPath_ = s_->getStrOptions(destNr_-1 , 2);
								tg_[destNr_]->logfile(command_);
								tg_[destNr_]->m_nNumCP_Block = s_->getIntOptions(destNr_-1, 8);
							}
							destNr_ = 0;
						}
						else
						{
							tg_[threadNumber_] = new threadGen(s_->getIntOptions(threadNumber_-1, 3), s_->getIntOptions(threadNumber_-1, 4), s_->getIntOptions(threadNumber_-1, 5), s_->getIntOptions(threadNumber_-1, 2), s_->getStrOptions(threadNumber_-1, 5), threadNumber_, s_->getIntOptions(threadNumber_-1, 6), s_->getStrOptions(threadNumber_-1, 1), s_->getStrOptions(threadNumber_-1, 7));
                                                        tg_[threadNumber_]->suspend();
							tg_[threadNumber_]->sendDir_ = false;
							tg_[threadNumber_]->sendMask_ = s_->getStrOptions(threadNumber_-1, 3);
							s_->setStrOptions(threadNumber_-1, 1, tg_[threadNumber_]->createPath(s_->getStrOptions(threadNumber_-1, 1)));
							tg_[threadNumber_]->path_ = s_->getStrOptions(threadNumber_-1 , 1);
							s_->setStrOptions(threadNumber_-1, 2, tg_[threadNumber_]->createPath(s_->getStrOptions(threadNumber_-1, 2)));
							tg_[threadNumber_]->logPath_ = s_->getStrOptions(threadNumber_-1 , 2);
							tg_[threadNumber_]->logfile(command_);
							tg_[threadNumber_]->m_nNumCP_Block = s_->getIntOptions(threadNumber_-1, 8);
							threadNumber_ = 0;
						}
						
						status_ = 0;
					}
					else
						cerr << "ERROR: Illegal destination" << endl;
					
					if(((strstr(command_, "-e"))!=NULL) || ((strstr(command_, "-p"))!=NULL) || ((strstr(command_, "-m"))!=NULL) ||
						((strstr(command_, "-E"))!=NULL) || ((strstr(command_, "-P"))!=NULL) || ((strstr(command_, "-M"))!=NULL) )
					{
						delete [] tmpBuffer_;
						tmpBuffer_ = 0;
					}
				}	
			}
			else
			{
DEF_ERROR:
				cerr << "ERROR: Wrong command (type help for valid commands)" << endl;
				continue_ = true;
			}

			break;
		}
	case 2: //list
		{
			ACE_INT32 i    = 0,
				temp = 0;
			
			while(( command_[i] != ' ') && (command_[i] != '\0'))
				i++;
			
			if ( (i==4 && ACE_OS::strlen(command_)==4 && status_!=9) || (ACE_OS::strlen(command_)==7 && command_[5]=='-' && (command_[6]=='l'||command_[6]=='L') ) )
			{
				while(command_[i] != '\0')
				{
					if(command_[i] == '-')
					{
						if(command_[++i] == 'l' || command_[i] == 'L')
							temp = 1;
					}
					i++;
				} // while
				
				i = 1;

				while(i<=s_->getNumberOfDest())
				{
					s_->setStrOptions(i-1, 1, tg_[i]->path_);
					s_->setIntOptions(i-1, 6, tg_[i]->lastSentFileNr_);
					i++;
				} // while
				
				if(temp==1)
					s_->printex();
				else
					s_->print();
			}
			else if(status_ == 9)
			{
				cerr << "ERROR: No destinations defined" << endl;
			}
			else
			{
				cerr << "ERROR: Wrong command (type help for valid commands)" << endl;
			}
			
			continue_ = true;	
			destNr_ = 0;
			break;
		}
	case 3: //pause 
		{
			ACE_INT32 i = 0;
			while(( command_[i] != ' ') && (command_[i] != '\0'))
				i++;
			
			if (i == 5)
			{
				if (status_ == 2 || status_ == 4 || status_ == 5 || status_ == 6 || status_ == 8)
				{
					if(checkParam(command_))
					{
						status_ = 1;   
						gohstdef(command_);
					}
					else
					{
						goto PAUSE_ERROR;
					}
					
					if(threadNumber_ == 0)
					{
						for(ACE_INT32 i = 1;i<=s_->getNumberOfDest();i++)
						{
							if( s_->getStrOptions(i-1, 4) != "Paused")
							{
								tg_[i]->suspend();
								tg_[i]->frequency_ = s_->getIntOptions(i-1, 2);
								s_->setStrOptions(i-1, 4, "Paused");
								tg_[i]->logfile(command_);
							}
						}
					}
					else
					{
						tg_[threadNumber_]->suspend();
						tg_[threadNumber_]->frequency_ = s_->getIntOptions(threadNumber_-1, 2);
						tg_[threadNumber_]->logfile(command_);
						s_->setStrOptions(threadNumber_-1, 4, "Paused");
						threadNumber_ = 0;
						status_ = 6;
					}
				}
				else				
				{
PAUSE_ERROR:
				cerr << "ERROR: Wrong command (type help for valid commands)" << endl;
				}
			}
			
			continue_ = true;
			break;
		}
	case 4: //start
		{
			ACE_INT32 i = 0;
			while(( command_[i] != ' ') && (command_[i] != '\0'))
				i++;
			
			if (i == 5)
			{
				if(checkParam(command_))
				{
					gohstdef(command_);
				}
				else
				{
					goto START_ERROR;
				}
				
				if(threadNumber_ == 0)
				{
					if(status_ == 9 || status_ == 3)
					{
						for(ACE_INT32 i = 1;i<=s_->getNumberOfDest();i++)
						{
							tg_[i] = new threadGen(s_->getIntOptions(i-1, 3), s_->getIntOptions(i-1, 4), s_->getIntOptions(i-1, 5), s_->getIntOptions(i-1, 2), s_->getStrOptions(i-1, 5), i, s_->getIntOptions(i-1, 6), s_->getStrOptions(i-1, 1), s_->getStrOptions(i-1, 7));
                                                        tg_[i]->suspend();
							tg_[i]->sendDir_ = false;
							tg_[i]->sendMask_ = s_->getStrOptions(i-1, 3);
							s_->setStrOptions(i-1, 2, tg_[i]->createPath(s_->getStrOptions(i-1, 2)));
							tg_[i]->logPath_ = s_->getStrOptions(i -1 , 2);
							
							if(s_->getStrOptions(i-1, 6) != "IFORPC")
							{
								tg_[i]->ifoRpc_ = 0;   //BGW
								if(s_->getStrOptions(i-1, 6) == "BGWRPC")
									tg_[i]->sendBlock_ = true;
								// Add from qmicsal 21/02/2007
								else
									tg_[i]->sendBlock_ = false;
								// End Add from qmicsal
							}
							else
							{
								tg_[i]->ifoRpc_ = 1;   //IFO
								tg_[i]->sendBlock_ = true;
							}
							
							tg_[i]->logfile(command_);
							tg_[i]->resume();   // 1 -> i
							s_->setStrOptions(i-1, 4, "Started");
						} // for
						
						status_ = 2;
					}
					else
					{
						for(ACE_INT32 i = 1;i<=s_->getNumberOfDest();i++)
						{
							if(s_->getStrOptions(i-1, 4) != "Started")
							{
								tg_[i]->logfile(command_);	
								tg_[i]->resume();
								s_->setStrOptions(i-1, 4, "Started");
								status_ = 2;
							}
						}
					}
				}
				else
				{
					if (status_ ==5)
					{
						tg_[threadNumber_] = new threadGen(s_->getIntOptions(threadNumber_-1, 3), s_->getIntOptions(threadNumber_-1, 4), s_->getIntOptions(threadNumber_-1, 5), s_->getIntOptions(threadNumber_-1, 2), s_->getStrOptions(threadNumber_-1, 5), threadNumber_, s_->getIntOptions(threadNumber_-1, 6), s_->getStrOptions(threadNumber_-1, 1), s_->getStrOptions(threadNumber_-1, 7));
                                                tg_[threadNumber_]->suspend();
						tg_[threadNumber_]->sendDir_ = false;
						tg_[threadNumber_]->sendMask_ = s_->getStrOptions(threadNumber_-1, 3);
						s_->setStrOptions(threadNumber_-1, 2, tg_[threadNumber_]->createPath(s_->getStrOptions(threadNumber_-1, 2)));
						tg_[threadNumber_]->logPath_ = s_->getStrOptions(threadNumber_ -1 , 2);
						
						if(s_->getStrOptions(threadNumber_-1, 6) != "IFORPC")
						{
							tg_[threadNumber_]->ifoRpc_ = 0;   //BGW
							if(s_->getStrOptions(threadNumber_-1, 6) == "BGWRPC")
								tg_[threadNumber_]->sendBlock_ = true;
							// Add from MIC
							else
								tg_[threadNumber_]->sendBlock_ = false;
							// End From MIC
						}
						else
						{
							tg_[threadNumber_]->ifoRpc_ = 1;   //IFO
							tg_[threadNumber_]->sendBlock_ = true;
						}
						
						tg_[threadNumber_]->logfile(command_);
						tg_[threadNumber_]->resume();
						s_->setStrOptions(threadNumber_-1, 4, "Started");
						threadNumber_ = 0;
						status_ = 4;
					}
					else
					{
						tg_[threadNumber_]->resume();
						tg_[threadNumber_]->logfile(command_);
						s_->setStrOptions(threadNumber_-1, 4, "Started");
						threadNumber_ = 0;
						status_ = 4;
					}
				}
			}
			else
			{
START_ERROR:
			cerr << "ERROR:	Wrong command (type help for valid commands)" << endl;
			}
			
			continue_ = true;
			break;
		}
	case 5: //stop
		{
			ACE_INT32 i = 0;
			
			while(( command_[i] != ' ') && (command_[i] != '\0'))
				i++;
			
			if (i == 4)
			{
				ACE_OS::sleep(2);
				if (status_ == 2 || status_ == 4 || status_ == 5 || status_ == 6 || status_ == 8)
				{
					if(checkParam(command_))
					{
						status_ = 2;
						gohstdef(command_);
					}
					else
					{
						goto STOP_ERROR;
					}
					
					if(threadNumber_==0)
					{
						for(ACE_INT32 i = 1;i <= s_->getNumberOfDest(); i++)
						{
							if(s_->getStrOptions(i-1, 4) != "Stopped")
							{
								tg_[i]->terminate();
								tg_[i]->logfile(command_);
								s_->setStrOptions(i-1, 4, "Stopped");
								status_ = 3;
							}
						}
					}
					else
					{
						tg_[threadNumber_]->terminate();
						tg_[threadNumber_]->logfile(command_);
						s_->setStrOptions(threadNumber_-1, 4, "Stopped");
						threadNumber_ = 0;
						status_ = 5;
					}
				}
				else
				{
					cerr << "ERROR: Wrong command (type help for valid commands)" << endl;
				}
			}
			else
			{
STOP_ERROR:
			cerr << "ERROR: Wrong command (type help for valid commands)" << endl;
			}
			
			destNr_ = 0;
			continue_ = true;
			break;
		}
	case 6: //exit
		{
			ACE_INT32 i = 0;
			
			while(( command_[i] != ' ') && (command_[i] != '\0'))
				i++;
			
			if (i == 4)
			{
				for (ACE_INT32 i = 1; i<= s_->getNumberOfDest(); i++)
				{
					if(status_ != 9)
					{
						tg_[i]->logfile(command_);
						delete tg_[i];
						tg_[i] = 0;
					}
				}

				continue_ = false;
				
				if (RESCUE)
				{
					s_->deleteRescueFile();
				}
				
				continue_ = false;
			}
			else if(i != 4)
			{
				cerr << "ERROR: Wrong command (type help for valid commands)" << endl;
				continue_ = true;
			}
			
			break;
		}
	case 7: //help
		{
			ACE_INT32 i = 0;
			
			while(( command_[i] != ' ') && (command_[i] != '\0'))
				i++;
			
			if (i == 4)
			{
				ACE_INT32 in = 0;
				bool showHelp = true;
				helpSession_ = true;				
								
				while(helpSession_)
				{
					if(showHelp)
					{
						cout << "\n_____ GOHSTHELP __________________________________________________________\n\n";
						cout << "Commands available in GOHST:\n\n";
						cout << " block    : Defines that blocks will be sent. BGWRPC is default, use \n";
						cout << "            'block -ifo' for IFORPC." << endl;
						cout << " clear    : Clears and delete the default gohst directory.\n";
						cout << " def      : Used to define destination or change parameter values.\n";
						cout << "            (Parameters that not are changed with def receives default \n";
						cout << "            values)." << endl;
						cout << " dir      : Generate and send directories. If dir <ref. path> is used an \n";
						cout << "            already existing directory will be sent.\n";
						cout << " end      : Exit gohsthelp\n";
						cout << " examples : Examples of the most common tasks performed by GOHST.\n";
						cout << " exit     : Ends the GOHST session." << endl;
						cout << " file     : Defines that a specified file will be sent. The path to\n";
						cout << "            the file must be specified (file <file path>).\n";
						cout << " list     : Lists current values for parameters. \n";
						cout << "            (use list -l for extended list)." << endl;
						cout << " pause    : Pauses the file/directory generation." << endl;
						cout << " start    : Activates/reactivates the file-, block or directory generation." << endl;
						cout << " stop     : Stops the present file generation.\n";
						cout << "            (To start after a stop is performed a new def is required)." << endl;
						cout << " version  : Displays a popup box with gohst version data.\n" << endl;
						cout << "For further information about one of above commands, enter the command\n";
						cout << "at gohsthelp prompt below ('end' will terminate gohsthelp).\n";
						cout << "___________________________________________________________________________" << endl;
						cout << "\nGOHSTHELP> ";
					}
					
					cin.getline( helpCmd_, 24 );
									
					if( ( strstr(helpCmd_, "end")!=NULL || strstr(helpCmd_, "END")!=NULL ) && strlen(helpCmd_) == 3)
						in=0;
					else if( ( ACE_OS::strstr(helpCmd_,"def")!=NULL || ACE_OS::strstr(helpCmd_, "DEF")!=NULL ) && ACE_OS::strlen(helpCmd_) == 3)
						in=1;
					else if( ( ACE_OS::strstr(helpCmd_, "list")!=NULL || ACE_OS::strstr(helpCmd_, "LIST")!=NULL ) && ACE_OS::strlen(helpCmd_) == 4)
						in=2;
					else if( ( ACE_OS::strstr(helpCmd_, "pause")!=NULL || ACE_OS::strstr(helpCmd_, "PAUSE")!=NULL  ) && ACE_OS::strlen(helpCmd_) == 5)
						in=3;
					else if( ( ACE_OS::strstr(helpCmd_, "start")!=NULL || ACE_OS::strstr(helpCmd_, "START")!=NULL ) && ACE_OS::strlen(helpCmd_) == 5)
						in=4;
					else if( ( ACE_OS::strstr(helpCmd_, "stop")!=NULL || ACE_OS::strstr(helpCmd_, "STOP")!=NULL ) && ACE_OS::strlen(helpCmd_) == 4)
						in=5;
					else if( ( ACE_OS::strstr(helpCmd_, "exit")!=NULL || ACE_OS::strstr(helpCmd_, "EXIT")!=NULL ) && ACE_OS::strlen(helpCmd_) == 4)
						in=6;
					else if( ( ACE_OS::strstr(helpCmd_, "dir")!=NULL || ACE_OS::strstr(helpCmd_, "DIR")!=NULL ) && ACE_OS::strlen(helpCmd_) == 3)
						in=7;
					else if( ( ACE_OS::strstr(helpCmd_, "block")!=NULL || ACE_OS::strstr(helpCmd_, "BLOCK")!=NULL ) && ACE_OS::strlen(helpCmd_) == 5)
						in=8;
					else if( ( ACE_OS::strstr(helpCmd_, "examples")!=NULL || ACE_OS::strstr(helpCmd_, "EXAMPLES")!=NULL ) && ACE_OS::strlen(helpCmd_) == 8)
						in=9;
					else if( ( ACE_OS::strstr(helpCmd_, "help")!=NULL || ACE_OS::strstr(helpCmd_, "HELP")!=NULL ) && ACE_OS::strlen(helpCmd_) == 4)
						in=10;
					else if( ( ACE_OS::strstr(helpCmd_, "file")!=NULL || ACE_OS::strstr(helpCmd_, "FILE")!=NULL ) && ACE_OS::strlen(helpCmd_) == 4)
						in=11;
					else if( ( ACE_OS::strstr(helpCmd_, "version")!=NULL || ACE_OS::strstr(helpCmd_, "VERSION")!=NULL ) && ACE_OS::strlen(helpCmd_) == 7)
						in=12;
					else if( ( ACE_OS::strstr(helpCmd_, "clear")!=NULL || ACE_OS::strstr(helpCmd_, "CLEAR")!=NULL ) && ACE_OS::strlen(helpCmd_) == 5)
						in=13;
					else 
						in=-1;
					
					switch(in)
					{
					case -1:
						{
							cerr << "\nERROR: Wrong command (enter help for assistance)." << endl;
							cout << "\nGOHSTHELP> ";
							showHelp = false;
							break;
						}
					case 10:
						{
							showHelp = true;
							break;
						}
					default:
						{
							if(help(in) == 0)
							{
								showHelp = false;
								break;
							}
							else
							{
								showHelp = false;
								helpSession_ = false;
								break;
							}
						}
					} // switch
				} // helpSession
			}
			else
			{
				cerr << "ERROR: Wrong command (type help for valid commands)" << endl;
			}
			
			continue_ = true;  //2;
			break;
		}
	case 8: //dir
		{
			if(status_ != 9)
			{
				ACE_INT32 i = 0;
				while(( command_[i] != ' ') && (command_[i] != '\0'))
					i++;
				
				if (i == 3)
				{
					ACE_INT32 j = strlen(command_);
					
					if ((j != 3) && (j != 4))
					{
						j = -1;
						referencePath_ = new ACE_TCHAR[strlen(command_)];
						
						while((command_[i++] != '-') && (command_[i] != '\0') && (command_[i] != ' '))
						{
							referencePath_[++j] = command_[i];
						}
						
						referencePath_[++j] = '\0';
						ACE_OS::strstr(referencePath_," ");
						
						for(ACE_INT32 i = 1;i<=s_->getNumberOfDest();i++)
						{
							tg_[i]->refPath_ = referencePath_;
							tg_[i]->sendDir_ = true;
							tg_[i]->partOfDir_ = s_->getIntOptions(i-1, 1);  //amount of files   // -1
							tg_[i]->logfile(command_);
						}
						
						if(referencePath_)
						{
							delete [] referencePath_;
							referencePath_ = 0;
						}
					}
					else 
					{
						for(ACE_INT32 i = 1;i<=s_->getNumberOfDest();i++)
						{
							tg_[i]->sendDir_ = true;
							tg_[i]->partOfDir_ = s_->getIntOptions(i-1, 1); // -1
							tg_[i]->logfile(command_);
						}
					}
				}
				else
				{
					cerr << "ERROR: Wrong command (type help for valid commands)" << endl;
				}
				
				continue_ = true;	
			}
			else
			{
				cerr << "ERROR: Wrong command (cannot start with dir)" << endl;
				continue_ = true;	
			}
			break;
		}
	case 9: //block
		{
			if(status_ != 9)
			{
				ACE_INT32 i = 0;
				
				while(( command_[i] != ' ') && (command_[i] != '\0'))
					i++;
				
				if (i == 5)
				{
					if(checkParam(command_))
					{
						gohstdef(command_);
					}
					else
					{
						goto BLOCK_ERROR;
					}
					
					if(threadNumber_==0)
					{
						for(ACE_INT32 i = 1;i <= s_->getNumberOfDest(); i++)
						{
							tg_[i]->sendBlock_ = true;
							tg_[i]->logfile(command_);

							if( strstr(command_, "IFO")!=NULL || strstr(command_, "ifo")!=NULL )  // if ifo
							{
								cerr << "ERROR: IFO not supported, BGWRPC will be set" << endl; //INGO4
								s_->setStrOptions(i-1, 6, "BGWRPC"); //INGO4
								tg_[i]->ifoRpc_ = 0; //INGO4
							}
							else
							{
								s_->setStrOptions(i-1, 6, "BGWRPC");
								tg_[i]->ifoRpc_ = 0;
							}
						}
					}
					else
					{
						tg_[threadNumber_]->sendBlock_ = true;
						tg_[threadNumber_]->logfile(command_);
						
						if( strstr(command_, "IFO")!=NULL || strstr(command_, "ifo")!=NULL )  // if ifo
						{
							cerr << "ERROR: IFO not supported, BGWRPC will be set" << endl; //INGO4
							s_->setStrOptions(threadNumber_-1, 6, "BGWRPC"); //INGO4
							tg_[threadNumber_]->ifoRpc_ = 0;//INGO4
						}
						else
						{
							s_->setStrOptions(threadNumber_-1, 6, "BGWRPC");
							tg_[threadNumber_]->ifoRpc_ = 0;						
						}
						
						threadNumber_ = 0;
					}	
				} // if i==5
				else
				{
BLOCK_ERROR:
					cerr << "ERROR: Wrong command (type help for valid commands)" << endl;
				}
				continue_ = true;	
			}
			else
			{
				cerr << "ERROR: Wrong command (cannot start with block)" << endl;
				continue_ = true;	
			}
	 		break;
		}
	case 10:   // Send specified file
		{
			if(status_ != 9)
			{
				ACE_INT32 i = 0;
				while(( command_[i] != ' ') && (command_[i] != '\0'))
					i++;
				
				// Modified from qmicsal 21/02/2007
				// MIC if (i==4 && command_[6]!=':')
				if (i==4 && command_[6]==':')
				// End modified from qmicsal
				{
					ACE_INT32 j = ACE_OS::strlen(command_);
					
					if ((j != 4) && (j != 5))
					{
						j = -1;
						referencePath_ = new ACE_TCHAR[strlen(command_)];
						
						while((command_[i++] != '-') && (command_[i] != '\0') && (command_[i] != ' '))
						{
							referencePath_[++j] = command_[i];
						}
						
						referencePath_[++j] = '\0';
						ACE_OS::strstr(referencePath_," ");
						
						for(ACE_INT32 i = 1;i<=s_->getNumberOfDest();i++)
						{
							tg_[i]->refPath_ = referencePath_;
							tg_[i]->sendFile_ = true;
							tg_[i]->logfile(command_);
						}
						
						if(referencePath_)
						{
							delete [] referencePath_;
							referencePath_ = 0;
						}
					}
					else 
					{
						for(ACE_INT32 i = 1;i<=s_->getNumberOfDest();i++)
						{
							tg_[i]->sendFile_ = true;
							tg_[i]->logfile(command_);
						}
					}
				}
				else
				{
					cerr << "ERROR: Wrong command (type help for valid commands)" << endl;
				}
				
				continue_ = true;	
			}
			else
			{
				cerr << "ERROR:Wrong command (cannot start with file)" << endl;
			}
			
			continue_ = true;	
			break;
		}
		case 11:   // Clear directory
			{
				if(status_ != 9)
				{
					try
					{
						s_->clearDirectory(s_->getDefaultPath());
						cout << "INFO: Removed directory " << s_->getDefaultPath() << endl;
					}
					catch(...)
					{
						cerr << "ERROR: Failed to remove directory " << s_->getDefaultPath() << endl;
					}
				}
				else
				{
					cerr << "ERROR:Wrong command (cannot start with clear)" << endl;
				}
				
				continue_ = true;	
				break;
			}
	case 12:
		{
			if(status_ != 9)
			{
				s_->setDebugMode(true);
				
				if(threadNumber_ == 0)
				{
					for(ACE_INT32 i = 1; i<=s_->getNumberOfDest(); i++)
					{
						tg_[i]->setDebugMode(true);
					}
				}
				
				else
					tg_[threadNumber_]->setDebugMode(true);
			}
			else
			{
				cerr << "ERROR:Wrong command (cannot start with debug on)" << endl;
			}

			continue_ = true;	
			break;
		}
	case 13:
		{
			if(status_ != 9)
			{
				s_->setDebugMode(false);
				
				if(threadNumber_ == 0)
				{
					for(ACE_INT32 i = 1; i<=s_->getNumberOfDest(); i++)
					{
						tg_[i]->setDebugMode(false);
					}
				}
				
				else
					tg_[threadNumber_]->setDebugMode(false);
			}
			else
			{
				cerr << "ERROR:Wrong command (cannot start with debug on)" << endl;
			}
			
			continue_ = true;	
			break;
		}
	case 14:
		{
			// Present a popup box containing version information
			ACE_TCHAR buf[500]; 
			
  			ACE_OS::sprintf(buf, "\nProduct name:\t%s\nProduct nr:\t%s\nVersion:\t\t%s\nUpdated:\t\t%s\n", 
			        PRODUCTNAME, PRODUCTNUMBER, GOHSTVERSION, UPDATED);
                        printf("%s\n", buf);

			//MessageBox(NULL, buf, "GOHST Version information", MB_TOPMOST); //MB_ICONINFORMATION); // need to port?? not required
			continue_ = true;
			break;
		}
	case 15:
		{
			if(status_ != 9) 
			{
				cout << "INFO: Number of sent bytes: " << tg_[threadNumber_+1]->sentBytes() << endl;
			}
			else
				cerr << "ERROR:Wrong command (cannot start with debug on)" << endl;
			
			continue_ = true;	
			
			break;
		}
	default: 
		{
			cerr << "ERROR: Wrong command (type help for valid commands)" << endl;
			continue_ = true;
			break;
		}
		
	};

	return (continue_);
}


/*******************************************************************************
 * getCommand() - stores the parameter "command_´s" value in a new parameter,  *
 *                'newOption_'. newOption_ is deleted in the function gohstdef.*
 *******************************************************************************/
ACE_TCHAR* Test::parameter (ACE_TCHAR* kommando_, int& i)
{
	ACE_INT32 j = -1;
	i++;
	newOption_ = new ACE_TCHAR[ACE_OS::strlen(kommando_)];

	while((kommando_[++i] != '-') && (kommando_[i] != '\0'))
	{
		newOption_[++j] = kommando_[i];
	}
	
	newOption_[++j] = '\0';
	return (newOption_);
}

/*******************************************************************************
 * gohstdef() - Reads the parameter "kommando_" step by step. If it gets a '-'  *
 *              followed by l,s,f,d,x,a or e and the session is in "run-mode"  *
 *				(except x and f which can be run in "pause-mode" as well) the  *
 *				value is transformed from ascii to int by using the function   *
 *				"parameter", except e which is supposed to be a text. 'e' is   *
 *				read sign by sign and is stored (in eventText_) until the next *
 *				character is '\0'. If the character doesn't match any of the   *
 *				above, "Wrong parameter" is printed on the screen.             *
 *******************************************************************************/
void* Test::gohstdef (ACE_TCHAR* kommando_)
{
	ACE_INT32 i = 0;

	if(((ACE_OS::strstr(kommando_, "-X")==NULL) || (strstr(kommando_, "-x")==NULL)) && (ACE_OS::strstr(kommando_, "def")!=NULL)&& (ACE_OS::strstr(kommando_, "-")==NULL)) 
	{
		s_->defaultValues();
		s_->setNumberOfDest(1);
	}

	while(kommando_[i] != '\0')
	{
		if(kommando_[i] == '-')
		{
			switch (kommando_[++i])
			{
			// ***** a (amount of files) *****
			case 'a': //amount of files sent in one directory.	
			case 'A':
				{
					if (status_ == 0)
					{
						newOption_ = parameter(kommando_, i);
						
						if( this->checkIfNumeric(newOption_) )
							amount_ = atoi(newOption_);
						else
							amount_ = -1;   // Faulty!
						
						// Set -a for all defined destinations
						if(threadNumber_ == 0)
						{
							for(ACE_INT32 i = 0; i < s_->getNumberOfDest(); i++)   // numberOfDest_ 
								s_->setIntOptions(i, 1, amount_);
						}
						
						// Set -a for specfied destination
						else
							s_->setIntOptions(threadNumber_-1, 1, amount_);
						
						if( newOption_ )
						{
							delete [] newOption_;
							newOption_= 0;
						}
					}
					else
					{
						cerr << "ERROR: Wrong parameter  -" << kommando_[i] << " (type help for valid commands)"<< endl;
					}
					
					break;
				}

///////////////////////////////////
			// ***** b (amount of threads for block sender) *****
			case 'b': //amount of threads to sent block in multiCP.	
			case 'B':
				{
					if (status_ == 0)
					{
						newOption_ = parameter(kommando_, i);
						
						if( this->checkIfNumeric(newOption_) )
							amount_block_ = atoi(newOption_);
						else
							amount_block_ = -1;   // Faulty!
						
						// Set -a for all defined destinations
						if(threadNumber_ == 0)
						{
							for(ACE_INT32 i = 0; i < s_->getNumberOfDest(); i++)   // numberOfDest_ 
								s_->setIntOptions(i, 8, amount_block_);
						}
						
						// Set -a for specfied destination
						else
							s_->setIntOptions(threadNumber_-1, 8, amount_block_);
						
						if( newOption_ )
						{
							delete [] newOption_;
							newOption_= 0;
						}
					}
					else
					{
						cerr << "ERROR: Wrong parameter  -" << kommando_[i] << " (type help for valid commands)"<< endl;
					}
					
					break;
				}

///////////////////////////////////

			// ***** d (number of destinations) *****
			case 'd':	//destinations	
			case 'D':
				{
					if(status_ == 0)
					{
						s_->setNumberOfDest(s_->checkIntValue(this->threadNumber_, 0, atoi(parameter(kommando_, i))));
												
						if( newOption_ )
						{
							delete [] newOption_;
							newOption_= 0;
						}
					}
					else
					{
						cerr << "ERROR: Wrong parameter  -" << kommando_[i] << " (type help for valid commands)"<< endl;
					}
					
					break;
				}
			
			// ***** e (set eventtext) *****
			case 'e':
			case 'E':
				{
					if(status_ == 0)
					{
						ACE_INT32 j = -1;
						tmpBuffer_ = new ACE_TCHAR[strlen(kommando_)];
						while((kommando_[++i + 1] != '-') && (kommando_[i + 1] != '\0'))// fix for that it started with a blank
						{
							tmpBuffer_ [++j] = kommando_[i+1];// fix for that it started with a blank
						}

						tmpBuffer_ [++j] = '\0';
						ACE_OS::strstr(tmpBuffer_ , " ");
						eventText_ = tmpBuffer_;

						if(threadNumber_ ==0)
						{
							for(ACE_INT32 i = 0; i < s_->getNumberOfDest(); i++)    //numberOfDest_
							{
								s_->setStrOptions(i, 5, eventText_);
							}
						}
						else
							s_->setStrOptions(threadNumber_-1, 5, eventText_);
					}
					else
					{
						cerr << "ERROR: Wrong parameter  -" << kommando_[i] << " (type help for valid commands)"<< endl;
					}
					
					break;
				}

			// ***** f (frequency between creation of files)
			case 'f':
			case 'F':
				{
					newOption_ = parameter(kommando_, i);
					
					if( this->checkIfNumeric(newOption_) )
						freq_ = atoi(newOption_);
					else
						freq_ = -1;   // Faulty!
					
					// Set frequency for all defined destinations
					if(threadNumber_ ==0)
					{
						for(ACE_INT32 i = 0; i < s_->getNumberOfDest(); i++)   //numberOfDest_
							s_->setIntOptions(i, 2, freq_);
					}
					// Set frequency for specified destination
					else
						s_->setIntOptions(threadNumber_-1, 2, freq_);
					
					if(newOption_)
					{
						delete newOption_;
						newOption_= 0;
					}
					
					break;
				}

			// ***** i (RPC type) *****
			case 'i':
			case 'I':
				{
					if((strstr(kommando_, "ifo")!=NULL) || (strstr(kommando_, "ifo")!=NULL))
					{
						// Set RPC type for all defined destinations
						if(threadNumber_ ==0)
						{
							for(ACE_INT32 i = 0; i < s_->getNumberOfDest() ; i++)   // numberOfDest_
							{
								s_->setStrOptions(i, 6, "IFORPC");
							}
						}
						// Set RPC type for specified destination
						else
						{
							s_->setStrOptions(threadNumber_-1, 6, "IFORPC");
						}
					}
					break;
				}

			// ***** l (record length of created file) *****
			case 'l':	
			case 'L':
				{
					if(status_ == 0)
					{
						newOption_ = parameter(kommando_, i);
				
						if( this->checkIfNumeric(newOption_) )
							recLength_ = atoi(newOption_);
						else
							recLength_ = -1;   // Faulty!
												
						// Set record length for all defined destinations
						if(threadNumber_ ==0)
						{
							for(ACE_INT32 i = 0; i < s_->getNumberOfDest() ; i++)  // numberOfDest_
								s_->setIntOptions(i, 3, recLength_);
						}
						// Set record length for specified destination
						else
							s_->setIntOptions(threadNumber_-1, 3, recLength_);
						
						if( newOption_ )
						{
							delete [] newOption_;
							newOption_= 0;
						}
					}
					else
					{
						cerr << "ERROR: Wrong parameter  -" << kommando_[i] << " (type help for valid commands)"<< endl;
					}
					break;
				}
				
			// ***** m (mask) *****
			case 'm':
			case 'M':
				{
					if(status_ == 0)
					{
						ACE_INT32 j = -1;
						tmpBuffer_ = new ACE_TCHAR[strlen(kommando_)];
						i++;

						while((kommando_[i++] != '-') && (kommando_[i] != '\0') && (kommando_[i] != ' '))
						{
							tmpBuffer_ [++j] = kommando_[i];
						}

						tmpBuffer_ [++j] = '\0';
						ACE_OS::strstr(tmpBuffer_ , " ");
						mask_ = tmpBuffer_;

						// Set mask for all defined destinations
						if(threadNumber_ ==0)
						{
							for(ACE_INT32 i = 0; i < s_->getNumberOfDest(); i++)
							{
								s_->setStrOptions(i, 3, mask_);
							}
						}
						// Set mask for specified destination
						else
							s_->setStrOptions(threadNumber_-1, 3, mask_);
					}
					else
					{
						cerr << "ERROR: Wrong parameter  -" << kommando_[i] << " (type help for valid commands)"<< endl;
					}
					
					break;
				}

			// ***** p (path) *****
			case 'p':	
			case 'P':
				{
					if(status_ == 0)
					{
						ACE_INT32 j = -1;
						i++;
						if((kommando_[i] == 'l') || (kommando_[i] == 'L'))
						{
							i++;
							tmpBuffer_ = new ACE_TCHAR[strlen(kommando_)];

							while((kommando_[i++] != '-') && (kommando_[i] != '\0') && (kommando_[i] != ' '))
							{
								tmpBuffer_ [++j] = kommando_[i];
							}

							tmpBuffer_ [++j] = '\0';
							ACE_OS::strstr(tmpBuffer_ , " ");
							logDefaultPath_ = tmpBuffer_;
					
							// Set path for all defined destinations
							if(threadNumber_ ==0)
							{
								for(ACE_INT32 i = 0; i < s_->getNumberOfDest(); i++)
								{
									s_->setStrOptions(i, 2, logDefaultPath_);
								}
							}
							// Set path for specified destination
							else
								s_->setStrOptions(threadNumber_-1, 2, logDefaultPath_);
						}
						else
						{
							tmpBuffer_ = new ACE_TCHAR[strlen(kommando_)];

							while((kommando_[i++] != '-') && (kommando_[i] != '\0') && (kommando_[i] != ' '))
							{
								tmpBuffer_ [++j] = kommando_[i];
							}

							tmpBuffer_ [++j] = '\0';
							ACE_OS::strstr(tmpBuffer_ , " ");
							gohstDefaultPath_ = tmpBuffer_;

							if(threadNumber_ ==0)
							{
								for(ACE_INT32 i = 0; i < s_->getNumberOfDest(); i++)
								{
									s_->setStrOptions(i, 1, gohstDefaultPath_);
								}
							}
							else
								s_->setStrOptions(threadNumber_-1, 1, gohstDefaultPath_);
						}
					}
					else
					{
						cerr << "ERROR: Wrong parameter  -" << kommando_[i] << " (type help for valid commands)"<< endl;
					}
					break;
				}

			// ***** s (number of records in each file/block) *****
			case 's':
			case 'S':
				{
					if(status_ == 0)
					{
						newOption_ = parameter(kommando_, i);
						
						if( this->checkIfNumeric(newOption_) )
							nrOfRec_ = atoi(newOption_);
						else
							nrOfRec_ = -1;   // Faulty!
											
						// Set number of records for al defined destinations
						if(threadNumber_ ==0)
						{
							for(ACE_INT32 i = 0; i < s_->getNumberOfDest(); i++)
								s_->setIntOptions(i, 4, nrOfRec_);
						}
						// Set number of records for specified destination
						else
							s_->setIntOptions(threadNumber_-1, 4, nrOfRec_);
						
						if( newOption_ )
						{
							delete [] newOption_;
							newOption_= 0;
						}
					}
					else
					{
						cerr << "ERROR: Wrong parameter  -" << kommando_[i] << " (type help for valid commands)"<< endl;
					}
					break;
				}

			// ***** k streamID, used at block transfer *****
			case 'k':
			case 'K':
				{
					if(status_ == 0)
					{
						ACE_INT32 j = -1;
						tmpBuffer_ = new ACE_TCHAR[strlen(kommando_)];
						//while((kommando_[++i] != '-') && (kommando_[i] != '\0'))
						while((kommando_[++i + 1] != '-') && (kommando_[i+1] != '\0') && (kommando_[i+1] != ' ')) // fix for that the first  was a blank
						{
							//tmpBuffer_ [++j] = kommando_[i];
							tmpBuffer_ [++j] = kommando_[i+1]; // fix for that is started with a blank
						}
						tmpBuffer_ [++j] = '\0';
						ACE_OS::strstr(tmpBuffer_ , " ");
						streamID_ = tmpBuffer_;
						if(threadNumber_ ==0)
						{
							for(ACE_INT32 i = 0; i < s_->getNumberOfDest(); i++)    //numberOfDest_
							{
								s_->setStrOptions(i, 7, streamID_);
							}
						}
						else
							s_->setStrOptions(threadNumber_-1, 7, streamID_);
					}
					else
					{
						cerr << "ERROR: Wrong parameter  -" << kommando_[i] << " (type help for valid commands)"<< endl;
					}
					
					break;
				}

			// ***** z transaction length i Kilobytes, max amount of data within a transaction *****
			case 'z':
			case 'Z':
				{
					if(status_ == 0)
					{
						newOption_ = parameter(kommando_, i);
						
						if( this->checkIfNumeric(newOption_) )
							transActLength_ = atoi(newOption_);
						else
							transActLength_ = -1;   // Faulty!
											
						// Set transaction length for all destinations
						if(threadNumber_ ==0)
						{
							for(ACE_INT32 i = 0; i < s_->getNumberOfDest(); i++)
								s_->setIntOptions(i, 5, transActLength_);
						}
						// Set number of records for specified destination
						else
							s_->setIntOptions(threadNumber_-1, 5, transActLength_);
						
						if( newOption_ )
						{
							delete [] newOption_;
							newOption_= 0;
						}
					}
					else
					{
						cerr << "ERROR: Wrong parameter  -" << kommando_[i] << " (type help for valid commands)"<< endl;
					}
					break;
				}

			// ***** t (Transmission type ascii(1)/binary(0)) *****
			case 't':
			case 'T':
				{
					//ACE_INT32 j = -1;
					i++;

					while((kommando_[++i] != '-') && (kommando_[i] != '\0') && (kommando_[i] != ' '))
					{
						if(kommando_[i] == 'a' || kommando_[i] == '1')
						{
							transType_ = 1;
					
							// Set transmission type ascii for all defined destinations
							if(threadNumber_ ==0)
							{
								for(ACE_INT32 i = 0; i < s_->getNumberOfDest(); i++)
								{
									//s_->setIntOptions(i, 5, transType_);
									s_->setIntOptions(i, 6, transType_);
								}
							}
							// Set transmission type ascii for specified destination
							else
								//s_->setIntOptions(threadNumber_-1, 5, transType_);
								s_->setIntOptions(threadNumber_-1, 6, transType_);
						}
						else if(kommando_[i] == 'b' || kommando_[i] == '0')
						{
							transType_ = 0;

							// Set transmission type binary for all defined destinations
							if(threadNumber_ ==0)
							{
								for(ACE_INT32 i = 0; i < s_->getNumberOfDest(); i++)
								{
									//s_->setIntOptions(i, 5, transType_);
									s_->setIntOptions(i, 6, transType_);
								}
							}
							// Set transmission type binary for specified destination
							else
								//s_->setIntOptions(threadNumber_-1, 5, transType_);
								s_->setIntOptions(threadNumber_-1, 6, transType_);
						}
						else
							cerr << "ERROR: Wrong value : " << kommando_[i] << " (type help for valid commands)"<< endl;
					}
					break;
				}

			case 'x':	//destnumber x		
			case 'X':
				{
					newOption_ = parameter(kommando_, i);
					
					if( this->checkIfNumeric(newOption_) )
						threadNumber_ = atoi(newOption_);
					else
					{
						cerr << "ERROR: Wrong argument \"" << newOption_ << "\" to parameter -x " << newOption_ << " (type help for valid commands)"<< endl;
						threadNumber_ = 1;   // Faulty!
					}
					
					if( newOption_ )
					{
						delete [] newOption_;
						newOption_= 0;
					}
					
					break;
				}
				
			default:
				{
					cerr << "ERROR: Wrong parameter -" << kommando_[i] << " (type help for valid commands)"<< endl;
					return 0;
				}
				}
			}
			else 
				i++;
			
		} // if

		return 0;
}

/*******************************************************************************
 * help() - Explaines how and when to use the different commands and           *
 *          parameters that can be used in GOHST.                              *
 *******************************************************************************/
 ACE_INT32 Test::help (ACE_INT32 in)
 {
	 switch (in)
	 {
	 case 0:	//exit
		 {
			 return -1;
		 }
	
	 case 1:	//def
		 {
			 cout << "\n_____ def _________________________________________________________________\n\n";
			 cout << "-a\tAmount of files\tDefines the amount of files generated in\n";
			 cout << "\t\t\teach directory.\n";                      
			 cout << "\t\t\tDefault value: 0 files\n\n";
			 cout << "-d\tDestionations\tNumber of defined gohst destinations.\n";                          
			 cout << "\t\t\tDefault value: 1 destination\n\n";
			 cout << "-e\tEvent text\tInformation in case of failure.\n";     
			 cout << "\t\t\tDefault value: Default EventText\n\n";
			 cout << "-f\tFrequency\tThe rate of which files/blocks/directories \n";
			 cout << "\t\t\tare generated (ms).\n";
			 cout << "\t\t\tDefault value: 1000 ms\n\n";
			 cout << "-l\tRecord length\tThe amount of data in one record.\n"; 
			 cout << "\t\t\tDefault value: 1024 bytes\n\n";
			 
			 /* Deleted from qmicsal 21/02/2007
			 cout << "-k\tStreamID\tThe identification that is sent to the BGW or RPC server.\n"; 
			 */
			 // End deleted from qmicsal

			 cout << "\t\t\tDefault value: Empty, which means that DOMAIN_TQ_DEST is used by GOH\n\n";
			 cout << "-m\tMask\t\tSpecifies how the files will be sent.\n\n";
			 cout << "-p\tPath\t\tThe path to the generated files.\n\n";
			 cout << "-pl\tLogPath\t\tThe path to the logfile.\n\n";
			 cout << "-s\tSize\t\tThe amount of records in one file.\n";
			 cout << "\t\t\tDefault value: 100 records\n\n";
			 cout << "-t\tTransmission\tDefines if the file is binary or ascii.\n";
			 cout << "\ttype\t\tDefault value: b, i.e. binary\n\n";			 
			 cout << "-x\tDest number\tSpecifies a single thread (destination).\n"; 
			 cout << "\t\t\tDefault value: 0, i.e. all threads\n\n";
			 cout << "-z\tBlock transaction length\tMaximum lenght of a block transaction \n"; 
			 cout << "\t\t\tDefault value: 2000 KByte.\n\n";
			 cout << "___________________________________________________________________________" << endl;
			 break;
		 }
	 case 2: //list
		 {
			 cout << "\n_____ list ________________________________________________________________\n\n";
			 cout << "Lists current status of most common parameters (list -l views an\n";
			 cout << "extended list of parameters).\n";
			 cout << "___________________________________________________________________________" << endl;
			 break;
		 }
	 case 3: //pause
		 {
			 cout << "\n_____ pause _______________________________________________________________\n\n";
			 cout << "Pauses the file, block or directory generation. To\n";
			 cout << "reactivate the session, the start command is required.\n";
			 cout << "___________________________________________________________________________" << endl;
			 break;
		 }
	 case 4: //start				
		 {
			 cout << "\n_____ start _______________________________________________________________\n\n";
			 cout << "Activates the file, block or directory generation. Start is also used\n";
			 cout << "to restart the generation after a pause.\n";
			 cout << "If the last command performed was stop the start command starts the application\n";
			 cout << "with the last defined values.\n";
			 cout << "A specified gohstdestination can be started with start -x <dest number>\n";
			 cout << "(-x is the only parameter that can be used in this mode).\n";
			 cout << "___________________________________________________________________________" << endl;
			 break;
		 }
	 case 5: // stop
		 {
			 cout << "\n_____ stop ________________________________________________________________\n\n";
			 cout << "Stops the present file, block or directory generation.\n";
			 cout << "To activate a new session a new def or start command is required.\n";
			 cout << "A specified gohstdestination can be stopped by stop -x <dest number>\n";
			 cout << "(-x is the only parameter that can be used in this mode)." << endl;
			 cout << "___________________________________________________________________________" << endl;
			 break;
		 }
	 case 6: // exit
		 {
			 cout << "\n_____ exit ________________________________________________________________\n\n";
			 cout << "Ends the GOHST session." << endl;
			 cout << "___________________________________________________________________________" << endl;
			 break;
		 }
	 case 7: //dir
		 {
			 cout << "\n_____ dir _________________________________________________________________\n\n";
			 cout << "With this command the application will generate and send directories, or\n";
			 cout << "use dir <path> to set the path to the reference directory that you want to send.\n";
			 cout << "The operator must specify a path, NO default path set." << endl;
			 cout << "___________________________________________________________________________" << endl;
			 break;
		 }
	 case 8: //block
		 {
			 cout << "\n_____ block _______________________________________________________________\n\n";
			 cout << "With this command the application will generate and send blocks.\n";
			 cout << "RPC type in INGO4 can only be BGWRPC which is the default value." << endl;
			 cout << "___________________________________________________________________________" << endl;
			 break;
		 }
	 case 9: //examples
		 {
			 cout << "\n_____ EXAMPLES ____________________________________________________________\n";
			 cout << "For more detailed information see GOHST User Guide (1/19817 - CNL 105 140 Uen)\n\n";
			 cout << "_____ Send file ___________________________________________________________\n\n";
			 cout << "GOHST> def <additional parameters>\n";
			 cout << "GOHST> file <full path to file>\n";
			 cout << "GOHST> start\n";
			 cout << "GOHST> stop" << endl << endl;
			 cout << "_____ Generate & send files _______________________________________________\n\n";
			 cout << "GOHST> def <additional parameters>\n";
			 cout << "GOHST> start\n";
			 cout << "GOHST> stop" << endl << endl;
			 cout << "_____ Send directory ______________________________________________________\n\n";
			 cout << "GOHST> def <additional parameters>\n";
			 cout << "GOHST> dir <full path to directory>\n";
			 cout << "GOHST> start\n";
			 cout << "GOHST> stop" << endl << endl;
			 cout << "_____ Generate & send directories _________________________________________\n\n";
			 cout << "GOHST> def <additional parameters>\n";
			 cout << "GOHST> dir\n";
			 cout << "GOHST> start\n";
			 cout << "GOHST> stop" << endl << endl;
			 cout << "_____ Generate & send blocks ______________________________________________\n\n";
			 cout << "BGWRPC:\n";
			 cout << "GOHST> def <additional parameters>\n";
			 cout << "GOHST> block\n";
			 cout << "GOHST> start\n";
			 cout << "GOHST> stop" << endl << endl;
			 cout << "___________________________________________________________________________" << endl;
			 break;
		 }
	 case 11: // file
		 {
			 cout << "\n_____ file ________________________________________________________________\n\n";
			 cout << "With this command the application will send specified file only.\n";
			 cout << "User sets path to the file that will be send. See example for \n";
			 cout << "further information about this command.\n";
			 cout << "___________________________________________________________________________" << endl;
			 break;
		 }
	 case 12: // version
		 {
			 cout << "\n_____ version _____________________________________________________________\n\n";
			 cout << "This command displays a popup box containing information about\n";
			 cout << "the current gohst version.\n";
			 cout << "___________________________________________________________________________" << endl;
			 break;
		 }
 	 case 13: // clear
		 {
			 cout << "\n_____ clear _______________________________________________________________\n\n";
			 cout << "This command removes all files in local gohst directory and \n";
			 cout << "deletes the directory.\n";
			 cout << "___________________________________________________________________________" << endl;
			 break;
		 }
	 default:
		 {
			 cerr << "\nERROR: Wrong command";  // Should NOT happen
			 break;
		 }
	}
	
	cout << "\nGOHSTHELP> ";
	return 0; 
}

/*******************************************************************************
 * checkParam() - Check if parameter in command is valid.                      *
 *******************************************************************************/
bool Test::checkParam(ACE_TCHAR *commandToCheck)
{
 	std::string cmd = std::string(commandToCheck);
		 
	ACE_INT32 pos = 0, notOk = 0;

	//while(pos < cmd.length()) || (pos != string.npos))
	while( (cmd.length() > pos) || (pos != string::npos))
	{
		pos = cmd.find("-",pos);
		if(string::npos != pos)
		{
			if (!this->validParam(cmd[pos+1], cmd[pos+2]))
				notOk++;
			pos ++;
		}
	}

	if(notOk==0)
		return true;
	else
		return false;
 }

/*******************************************************************************
 * validParam() - Check if parameter is a valid parameter.                     *
 *******************************************************************************/
bool Test::validParam(ACE_TCHAR p1, ACE_TCHAR p2)
{
	// Check pattern: -'p1''p2' (where p1 is a valid option & p2 is a space (' ') )
	// Modified from qmicsal 21/02/2007 - deleted the k parameter
	if( (p1=='a'&&p2==' ')||(p1=='b'&&p2==' ')||(p1=='k'&&p2==' ')||(p1=='d'&&p2==' ')||(p1=='e'&&p2==' ')||(p1=='f'&&p2==' ')||(p1=='l'&&p2==' ')||(p1=='m'&&p2==' ')||(p1=='p'&&p2==' ')||(p1=='p'&&p2=='l')||(p1=='s'&&p2==' ')||(p1=='z'&&p2==' ')||(p1=='t'&&p2==' ')||(p1=='x'&&p2==' ')||(p1=='i'&&p2=='f')||
(p1=='A'&&p2==' ')||(p1=='B'&&p2==' ')||(p1=='K'&&p2==' ')||(p1=='D'&&p2==' ')||(p1=='E'&&p2==' ')||(p1=='F'&&p2==' ')||(p1=='L'&&p2==' ')||(p1=='M'&&p2==' ')||(p1=='P'&&p2==' ')||(p1=='P'&&p2=='L')||(p1=='S'&&p2==' ')||(p1=='Z'&&p2==' ')||(p1=='T'&&p2==' ')||(p1=='X'&&p2==' ')||(p1=='I'&&p2=='F'))
	// End modified from MIC
		return true;
	else
		return false;
}

/*******************************************************************************
 * checkIfNumeric() - Check buffert contains any non numeric characters.       *
 *******************************************************************************/
bool Test::checkIfNumeric(ACE_TCHAR* buf)
{
	ACE_INT32 nrOfNonNumeric = 0;
	
	for (ACE_INT32 i=0; i<ACE_OS::strlen(buf); i++)
	{
		if( buf[i]<48 || buf[i]>57)
			nrOfNonNumeric++;	
		if (buf[i] == 32)  // Except if space
			nrOfNonNumeric--;	
	}
	if(nrOfNonNumeric>0)  
		return false;
	else
		return true;
}
