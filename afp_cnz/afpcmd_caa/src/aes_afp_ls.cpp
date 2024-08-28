/*=================================================================== */
/**
@file aes_afp_ls.cpp

Class method implementation for aes_afp_ls.h

DESCRIPTION
The services provided by this facilitates listing of transfer ques.

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

N/A       8/11/2011     xbhadur       Initial Release
=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "aes_afp_ls.h"
#include "aes_afp_services.h"
#include <ACS_APGCC_CommonLib.h>
#include "aes_afp_api_tracer.h"
/*===================================================================
                        DECLARATION SECTION
=================================================================== */
// ACS Trace definition
AES_AFP_TRACE_DEFINE(aes_afp_Ls);



/*===================================================================
   ROUTINE:main
=================================================================== */

int main (int argc, char *argv [])
{
   aes_afp_ls myAFPLS(argc, argv);
   return myAFPLS.launch ();
}


/*===================================================================
   ROUTINE:aes_afp_ls
=================================================================== */
aes_afp_ls::aes_afp_ls (int argc, char** argv)
       : AFPTABLETEXT_("AFP TABLE"),
        REMBEFORETEXT_("REMOVE BEFORE"),
        RETRIESTEXT_("RETRIES"),
        RETRYINTERVALTEXT_("RETRY INTERVAL"),
        MANUALTEXT_("MANUAL"),
        TEMPLATETEXT_("TEMPLATE"),
        RENAMETEXT_("RENAME"),
        TQTEXT_("TRANSFER QUEUE"),
        DESTTEXT_("DESTINATION SET"),
        SOURCEDIRTEXT_("SOURCE DIRECTORY"),
        REMDELAYTEXT_("REMOVE DELAY"),
        REMTIMERTEXT_("REMOVE TIMER"),
        DEFSTATUSTEXT_("DEFAULT STATUS"),
        STATUSTEXT_("STATUS"),
        FILEDIRTEXT_("FILE OR DIRECTORY"),
        DIRTEXT_("DIR"),
        NOTEXT_("NO"),
        YESTEXT_("YES"),
        USERGROUPTEXT_("USER GROUP"),
        NAMETAGTEXT_("NAME TAG"),
        STARTUPSEQUENCENUMBERTEXT_("STARTUP SEQUENCE NUMBER"),
        PADDING34_("                                  "),
        PADDING32_("                                "),
        PADDING7_("       "),
        PADDING2_("  "),
        PADDING3_("   "),
        attributes_(false),
        longList_(false),
        allFiles_(false),
        firstFile_(false),
        lastFile_(false),
        header_(true),
        transferQueue_(""),
        destination_("")
{

	argC_ = argc;
	argV_ = argv;
}


/*===================================================================
   ROUTINE:~aes_afp_ls
=================================================================== */
aes_afp_ls::~aes_afp_ls()
{

}

/*===================================================================
   ROUTINE:parse
=================================================================== */
bool aes_afp_ls::parse ()
{
	AES_AFP_TRACE_MESSAGE("parse  called");
	int c;
	option_t option;
	optpair_t optlist [] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};

	aes_afp_getopt getopt (argC_, argV_, "alsoyq");

	// Parse the options

	while ((c = getopt ()) != EOF)
	{
			switch (c) 
				{
					case 'a':
						{
							option = ATTRIBUTES;
							break;
						}
					case 'l':
						{
							option = LONGLIST;
							break;
						}
					case 's':
						{
							option = ALLFILES;
							break;
						}
					case 'o':
						{
							option = FIRSTFILE;
							break;
						}
					case 'y':
						{
							option = LASTFILE;
							break;
						}
					case 'q':
						{
							option = NOHEADER;
							break;
						}
					case '?':
						{
							usage();
							return false;
						}
					default:
						{
							usage();
							return false;
						}
				}
			if (!optlist [option].opt_)
				{
					optlist [option].opt_ = c;
					optlist [option].arg_ = getopt.optarg_;
				}
			else
				{
					usage (); 
					return false;
				}
		} // while

  // Parse file name -----------------------------------------------------------

	if (argC_ > getopt.optind_)
		{
			transferQueue_ = argV_[getopt.optind_];
			getopt.optind_++;
		}  

  // Parse destination ---------------------------------------------------------

  if (argC_ > getopt.optind_)
		{
			destination_ = argV_[getopt.optind_];
			getopt.optind_++;
		}

  // Parse no more arguments ---------------------------------------------------

  if (argC_ != getopt.optind_)
		{
			usage (); 
			return false;
		}

  // Attributes option ---------------------------------------------------------

  attributes_ = optlist [ATTRIBUTES].opt_ ? true: false;

  // Longlist option -----------------------------------------------------------

  longList_ = optlist [LONGLIST].opt_ ? true: false;

  // List subfiles option ------------------------------------------------------
 
  allFiles_ = optlist [ALLFILES].opt_ ? true: false;

  // List subfiles option ------------------------------------------------------
 
  firstFile_ = optlist [FIRSTFILE].opt_ ? true: false;

  // List subfiles option ------------------------------------------------------
 
  lastFile_ = optlist [LASTFILE].opt_ ? true: false;

  // Quiet listing option ------------------------------------------------------

  header_ = optlist [NOHEADER].opt_ ? false: true;

  // Check syntax --------------------------------------------------------------

  if (allFiles_ + firstFile_ + lastFile_ > 1)
		{
			usage (); 
			return false;
		}  

	// If dest is empty?
	if (attributes_ && transferQueue_.empty() )
		{
			usage (); 
			return false;			
		}

	if ( (transferQueue_.empty() || destination_.empty() ) && (firstFile_ || lastFile_))
		{
			usage (); 
			return false;			
		}

  if (attributes_ && (longList_ || allFiles_ || firstFile_ || lastFile_))
		{
			usage (); 
			return false;
	  }

  if (header_ == false && ((attributes_ + longList_) < 1))
		{
			usage (); 
			return false;
	  }
  return true;

  //## end aes_afp_ls::parse%70E9BAF1FEED.body
}

/*===================================================================
   ROUTINE:execute
=================================================================== */
unsigned int aes_afp_ls::execute ()
{
  //## begin aes_afp_ls::execute%D06D6D7DFEED.body preserve=yes
	unsigned int result(0);

	// afpls [tq] [dest]
	if (longList_ == false && attributes_ == false && allFiles_ == false && firstFile_ == false && lastFile_ == false)
		{
			result = listTqOnly();
			if (result != AES_NOERRORCODE)
				{
					printErrorMessage(result);
				}
			return result;
		}

	// afpls -l [tq] [dest]
	if (longList_ == true && allFiles_ == false && firstFile_ == false && lastFile_ == false)
		{
			result = listTqOnlyLong();
			if (result != AES_NOERRORCODE)
				{
					printErrorMessage(result);
				}
			return result;
		}

	// afpls -s [tq] [dest]
	if (longList_ == false && allFiles_ == true)
		{
			result = listTqAndFiles();
			if (result != AES_NOERRORCODE)
				{
					printErrorMessage(result);
				}
			return result;
		}

	// afpls -o tq dest
	if (firstFile_ == true && longList_ == false)
		{
			result = listTqAndOldFile();
			if (result != AES_NOERRORCODE)
				{
					printErrorMessage(result);
				}
			return result;
		}

	// afpls -y tq dest
	if (lastFile_ == true && longList_ == false)
		{
			result = listTqAndYoungFile();
			if (result != AES_NOERRORCODE)
				{
					printErrorMessage(result);
				}
			return result;
		}

	// afpls -ls
	if (longList_ == true && allFiles_ == true)
		{
			result = listTqAndFilesLong();
			if (result != AES_NOERRORCODE)
				{
					printErrorMessage(result);
				}
			return result;
		}

	// afpls -ol
	if (longList_ == true && firstFile_ == true)
		{
			result = listTqAndOldFileLong();
			if (result != AES_NOERRORCODE)
				{
					printErrorMessage(result);
				}
			return result;
		}

		// afpls -yl
	if (longList_ == true && lastFile_ == true)
		{
			result = listTqAndYoungFileLong();
			if (result != AES_NOERRORCODE)
				{
					printErrorMessage(result);
				}
			return result;
		}

	// afpls -a tq dest
	if (attributes_ == true && longList_ == false && firstFile_ == false && longList_ == false && allFiles_ == false)
		{
			result = listAttr();
			if (result != AES_NOERRORCODE)
				{
					printErrorMessage(result);
				}
			return result;
		}

	// Nothing appropriate, error code = user unreliable ;-)
	usage(); 
	return false;
}

//## Operation: usage%2CBA4A7FFEED; C++
//## Semantics:

/*===================================================================
   ROUTINE:usage
=================================================================== */
void aes_afp_ls::usage ()
{
  //## begin aes_afp_ls::usage%2CBA4A7FFEED.body preserve=yes
	string abc="afpls";
	cout << "Incorrect usage"<< std::endl;
  cout << "Usage: "<<abc<<" [-l ][-s ][transferqueue [destinationset]]" << std::endl;
  cout << "       afpls -l [-q ][-s ][transferqueue [destinationset]]" << std::endl;
  cout << "       afpls [-l ] -o | -y transferqueue destinationset" << std::endl;
  cout << "       afpls -l [-q ] -o | -y transferqueue destinationset" << std::endl;
  cout << "       afpls -a [-q ] transferqueue destinationset" << std::endl;
  cout << std::endl<<std::endl;;
	
	exit(AES_INCORRECTCOMMAND);
  //## end aes_afp_ls::usage%2CBA4A7FFEED.body
}

//## Operation: listTqOnly%62A12982FEED; C++
//## Semantics:

/*===================================================================
   ROUTINE:listTqOnly
=================================================================== */
unsigned int aes_afp_ls::listTqOnly ()
{
	unsigned int result(0);
	std::stringstream outStream;

	if (transferQueue_.empty() == true)
		{
			AES_AFP_TRACE_MESSAGE("listTqOnly()\n List all\n");
			result = api_.listAllFilesShort(transferQueue_, outStream);
			if (result != AES_NOERRORCODE && result != AES_NOPROCORDER)
				{
					return result;
				}
		}
	else
		{
			AES_AFP_TRACE_MESSAGE("listTqOnly()\n List TQ %s\n", transferQueue_.c_str());
			result = api_.listFile(transferQueue_, destination_, outStream);
			if (result != AES_NOERRORCODE)
				{
					return result;
				}
		}
	printTqOnly(outStream);
	std::cout<<std::endl;

	return AES_NOERRORCODE;
}

/*===================================================================
   ROUTINE:printTqOnly
=================================================================== */
void aes_afp_ls::printTqOnly (std::stringstream& buf)
{
	char outbuf[512];
	unsigned int cntrlsign(0);
	std::string tq("");
	std::string savedTqName("");
	std::string remoteDestination("");
	Tqdata* tmpTqData = 0;

	std::multimap<std::string, Tqdata*, std::less<std::string> >tqMap;
	std::multimap<std::string, Tqdata*, std::less<std::string> >::iterator tqItr;
	std::pair<std::string, Tqdata*>tqPair;

	buf.getline(outbuf, 512, '\n');
	cntrlsign = atoi(outbuf);

	if (cntrlsign != PROCORDERSTART)
	{
		if (header_ == true)
		{
			std::cout << AFPTABLETEXT_ << std::endl;
		}
		return;
	}

	while (cntrlsign == PROCORDERSTART)
	{
			tmpTqData = new Tqdata;
			// TRANSFER QUEUE NAME ------------------------------
			buf.getline(outbuf, 512, '\n');
			tmpTqData->transferQueue_ = outbuf;

			// USER GROUP NAME ----------------------------------To be ported in future
			buf.getline(outbuf, 512, '\n');
			tmpTqData->userGroup_ = outbuf;


			// REMOTE DESTINATION NAME --------------------------
			buf.getline(outbuf, 512, '\n');
			tmpTqData->destinationSet_ = outbuf;

			// Skip rest of TQ data
			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');

			// Get new control sign
			buf.getline(outbuf, 512, '\n');
			cntrlsign = atoi(outbuf);

			while (cntrlsign == SENDITEMSTART)
				{
					// Skip senditem data
					buf.getline(outbuf, 512, '\n');
					buf.getline(outbuf, 512, '\n');
					buf.getline(outbuf, 512, '\n');
					buf.getline(outbuf, 512, '\n');
					buf.getline(outbuf, 512, '\n');

					// Get new cntrl sign
					buf.getline(outbuf, 512, '\n');
					cntrlsign = atoi(outbuf);
				}

			tqPair.first =	tmpTqData->transferQueue_;
			tqPair.second = tmpTqData;
			tqMap.insert(tqPair);
		}

	// PRINTOUT PART ----------------------------------------
	// Print header AFP TABLE

	if (header_ == true)
		{
			std::cout << AFPTABLETEXT_ << std::endl << std::endl;
			std::cout << std::setw(34) << std::left << TQTEXT_ <<std::endl;
			//USERGROUPTEXT_ << std::endl;
		}

	for (tqItr=tqMap.begin();tqItr!=tqMap.end();++tqItr)
		{
			if (savedTqName != (*tqItr).first)
				{
					// Print tq value
					cout.setf(ios::left, ios::adjustfield);
					//std::cout << std::setw(34) << std::left << (*tqItr).first << (*tqItr).second->userGroup_ << std::endl;
					std::cout << std::setw(34) << std::left << (*tqItr).first << std::endl;
					savedTqName = (*tqItr).first;
				}
		}
	for (tqItr = tqMap.begin(); tqItr != tqMap.end(); )
	{
		Tqdata *ptr = (*tqItr).second;
		tqMap.erase( tqItr++);
		delete ptr;
		ptr = 0;

	}
	tqMap.clear();
}

/*===================================================================
   ROUTINE:listTqOnlyLong
=================================================================== */
unsigned int aes_afp_ls::listTqOnlyLong ()
{
	unsigned int result(0);
	std::stringstream outStream;

	if (transferQueue_.empty() == true)
		{
			AES_AFP_TRACE_MESSAGE("listTqOnlyLong()\n List all\n");
			result = api_.listAllFilesShort(transferQueue_, outStream);
			if (result != AES_NOERRORCODE && result != AES_NOPROCORDER)
				{
					return result;
				}
		}
	else
		{
			AES_AFP_TRACE_MESSAGE("listTqOnlyLong()\n List TQ %s\n", transferQueue_.c_str());
			result = api_.listFile(transferQueue_, destination_, outStream);
			if (result != AES_NOERRORCODE)
				{
					return result;
				}
		}

	//outStream.freeze();
	printTqOnlyLong(outStream);
	std::cout<<std::endl;
	return AES_NOERRORCODE;
  //## end aes_afp_ls::listTqOnlyLong%5ADA9058FEED.body
}

//## Operation: printTqOnlyLong%8534DC1EFEED; C++
//## Semantics:

/*===================================================================
   ROUTINE:printTqOnlyLong
=================================================================== */
void aes_afp_ls::printTqOnlyLong (std::stringstream& buf)
{
	char outbuf[512];
	unsigned int cntrlsign(0);
	int tmpInt(0);
	std::string tq("");
	std::string savedTqName("");
	std::string remoteDestination("");
	std::string sourceDirectory("");

	int remoteRename(0);
	AES_GCC_Filestates remoteStatus;

	Tqdata* tmpTqData = 0;

	std::multimap<std::string, Tqdata*, std::less<std::string> >tqMap;
	std::multimap<std::string, Tqdata*, std::less<std::string> >::iterator tqItr;
	std::pair<std::string, Tqdata*>tqPair;

	buf.getline(outbuf, 512, '\n');
	cntrlsign = atoi(outbuf);

	if (cntrlsign != PROCORDERSTART)
	{
		if (header_ == true)
		{
			std::cout << AFPTABLETEXT_ << std::endl;
		}
		return;
	}

	while (cntrlsign == PROCORDERSTART)
	{
		tmpTqData = new Tqdata;
		// TRANSFER QUEUE NAME ------------------------------
		buf.getline(outbuf, 512, '\n');
		tmpTqData->transferQueue_ = outbuf;

		// USER GROUP ---------------------------------------

		buf.getline(outbuf, 512, '\n');
		tmpTqData->userGroup_ = outbuf;

		// REMOTE DESTINATION NAME --------------------------
		buf.getline(outbuf, 512, '\n');
		tmpTqData->destinationSet_ = outbuf;

		// DEFAULT STATUS -----------------------------------
		buf.getline(outbuf, 512, '\n');
		remoteStatus = (AES_GCC_Filestates)atoi(outbuf);
		tmpTqData->status_ = stateString(remoteStatus);

		// REMOVE DELAY -------------------------------------
		buf.getline(outbuf, 512, '\n');
		tmpTqData->removeDelay_ = atoi(outbuf);

		// MANUAL -------------------------------------------
		buf.getline(outbuf, 512, '\n');
		tmpInt = atoi(outbuf);
		if (tmpInt == 0)
		{
			tmpTqData->manual_ = false;
		}
		else
		{
			tmpTqData->manual_ = true;
		}

		// RENAME -------------------------------------------
		buf.getline(outbuf, 512, '\n');
		remoteRename = atoi(outbuf);
		if (remoteRename == 0)
		{
			tmpTqData->renameStringType_ = "LOCAL";
		}
		else if (remoteRename == 1)
		{
			tmpTqData->renameStringType_ = "REMOTE";
		}
		else
		{
			tmpTqData->renameStringType_ = "NONE";
		}


		// SOURCE DIRECTORY----------------------------------
		buf.getline(outbuf, 512, '\n');
		tmpTqData->sourceDirectory_ = outbuf;
		string tempPath = tmpTqData->sourceDirectory_; 
		if(!tempPath.empty())
		{
			tempPath = updateSourceDirectory(tempPath);		
		}
		tmpTqData->sourceDirectory_=tempPath;
		// Get new control sign
		buf.getline(outbuf, 512, '\n');
		cntrlsign = atoi(outbuf);

		while (cntrlsign == SENDITEMSTART)
		{
			// Skip senditem data
			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');

			// Get new cntrl sign
			buf.getline(outbuf, 512, '\n');
			cntrlsign = atoi(outbuf);
		}

		tqPair.first =	tmpTqData->transferQueue_;
		tqPair.second = tmpTqData;
		tqMap.insert(tqPair);
	}

	// PRINTOUT PART --------------------------------------
	if (header_ == true)
	{
		std::cout << AFPTABLETEXT_ << std::endl;
	}

	for (tqItr=tqMap.begin();tqItr!=tqMap.end();++tqItr)
	{
		if (savedTqName != (*tqItr).first)
		{
			if (header_ == true)
			{
				std::cout << std::endl;
				std::cout << std::setw(34) << std::left << TQTEXT_ << std::endl;//USERGROUPTEXT_ <<
			}
			else
			{
				std::cout << std::endl;
			}

			// Print tq value
			cout.setf(ios::left, ios::adjustfield);
			std::cout << std::setw(34) << std::left << (*tqItr).first << std::endl; //(*tqItr).second->userGroup_ <<
			std::cout << std::endl;
			savedTqName = (*tqItr).first;

			// Print source dir header
			if (header_ == true)
			{
				std::cout << SOURCEDIRTEXT_ << std::endl;
			}
			std::cout << (*tqItr).second->sourceDirectory_ << std::endl;
			std::cout << std::endl;

			// Print definition headers for each column
			if (header_ == true)
			{
				std::cout << PADDING7_ << REMDELAYTEXT_ << PADDING2_ << REMTIMERTEXT_ << PADDING2_ << DEFSTATUSTEXT_ << PADDING2_ << DESTTEXT_ << std::endl;
			}
		}
		// Print value remove delay
		std::cout << PADDING7_;
		cout.setf(ios::right, ios::adjustfield);
		cout << std::setw(12) << (*tqItr).second->removeDelay_;

		// Print remove timer
		std::cout << PADDING2_;
		cout.setf(ios::right, ios::adjustfield);
		cout << std::setw(12) << "0";

		// Print status
		std::cout << PADDING2_;
		cout.setf(ios::left, ios::adjustfield);
		cout << std::setw(14) << (*tqItr).second->status_;

		// Print destination
		std::cout << PADDING2_;
		std::cout.setf(ios::left, ios::adjustfield);
		std::cout << std::setw(33) << (*tqItr).second->destinationSet_ << std::endl;
	}
	for (tqItr=tqMap.begin();tqItr!=tqMap.end(); )
	{
		Tqdata *tempTqData = (*tqItr).second;
		tqMap.erase(tqItr++);
		delete tempTqData;
		tempTqData = 0;
	}
	tqMap.clear();
}


/*===================================================================
   ROUTINE:listTqAndFiles
=================================================================== */
unsigned int aes_afp_ls::listTqAndFiles ()
{
	unsigned int result(0);
	std::stringstream outStream;

	if (transferQueue_.empty() == true)
		{
			AES_AFP_TRACE_MESSAGE("listTqAndFiles()\n List all\n");
			result = api_.listAllFiles(transferQueue_, outStream);
			if (result != AES_NOERRORCODE && result != AES_NOPROCORDER)
				{
					return result;
				}
		}
	else
		{
		AES_AFP_TRACE_MESSAGE("listTqAndFiles()\n List TQ %s\n", transferQueue_.c_str());
			result = api_.listFile(transferQueue_, destination_, outStream);
			if (result != AES_NOERRORCODE)
				{
					return result;
				}
		}

	//outStream.freeze();
	printTqAndFiles(outStream);
	std::cout<<std::endl;

	return AES_NOERRORCODE;
  //## end aes_afp_ls::listTqAndFiles%3B00B3EEFEED.body
}

//## Operation: printTqAndFiles%0F3AA14DFEED; C++
//## Semantics:

/*===================================================================
   ROUTINE:printTqAndFiles
=================================================================== */
void aes_afp_ls::printTqAndFiles (std::stringstream& buf)
{
	char outbuf[512];
	unsigned int cntrlsign(0);
	std::string tq("");
	std::string savedTqName("");
	std::string tmpUserGroup("");

	Tqdata* tmpTqData = 0;
	Filedata* tmpFileData = 0;

	std::multimap<std::string, Tqdata*, std::less<std::string> >tqMap;

	std::multimap<std::string, Tqdata*, std::less<std::string> >::iterator tqItr;
	std::map<std::string, Filedata*, std::less<std::string> >::iterator fileItr;

	std::pair<std::string, Tqdata*>tqPair;
	std::pair<std::string, Filedata*>filePair;

	buf.getline(outbuf, 512, '\n');
	cntrlsign = atoi(outbuf);


	if (cntrlsign != PROCORDERSTART)
	{
		if (header_ == true)
		{
			std::cout << AFPTABLETEXT_ << std::endl;
		}
		return;
	}

	// COLLECT DATA PART ------------------------------------
	while (cntrlsign == PROCORDERSTART)
	{
		tmpTqData = new Tqdata;

		// TRANSFER QUEUE NAME ------------------------------
		buf.getline(outbuf, 512, '\n');
		tmpTqData->transferQueue_ = outbuf;

		// USER GROUP ---------------------------------------
		buf.getline(outbuf, 512, '\n');
		tmpTqData->userGroup_ = outbuf;

		buf.getline(outbuf, 512, '\n'); // Destination
		buf.getline(outbuf, 512, '\n'); // Default status, skip
		buf.getline(outbuf, 512, '\n'); // Remove delay
		buf.getline(outbuf, 512, '\n');
		buf.getline(outbuf, 512, '\n');
		buf.getline(outbuf, 512, '\n');

		// Get new control sign
		buf.getline(outbuf, 512, '\n');
		cntrlsign = atoi(outbuf);

		while (cntrlsign == SENDITEMSTART)
		{
			tmpFileData = new Filedata;
			// FILE OR DIRECTORY NAME -----------------------
			buf.getline(outbuf, 512, '\n');
			filePair.first = outbuf;
			tmpFileData->filename_ = outbuf;

			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n'); // Dir flag
			buf.getline(outbuf, 512, '\n'); // Status
			buf.getline(outbuf, 512, '\n'); // Remove delay

			// Get new control sign
			buf.getline(outbuf, 512, '\n');
			cntrlsign = atoi(outbuf);
			filePair.second = tmpFileData;

			tmpTqData->filedataMap_.insert(filePair);
		}
		tqPair.first = tmpTqData->transferQueue_;
		tqPair.second = tmpTqData;
		tqMap.insert(tqPair);
	}

	// PRINTOUT PART ------------------------------------

	// Print header AFP TABLE
	if (header_ == true)
	{
		std::cout << AFPTABLETEXT_ << std::endl;
	}

	for (tqItr=tqMap.begin();tqItr!=tqMap.end();++tqItr)
	{
		std::cout << std::endl;

		if (savedTqName != (*tqItr).first)
		{
			if (header_ == true)
			{
				// Print TQ header
				cout << std::setw(34) << std::left  << TQTEXT_ << std::setw(14) << std::endl; //USERGROUPTEXT_ <<
			}
			// Print tq value

			std::cout << std::setw(34) << std::left << (*tqItr).first;

			// Print user group
#if 0
			cout << (*tqItr).second->userGroup_ << std::endl;
#endif
			std::cout << std::endl<<std::endl;

			if (header_ == true)
			{
				// Print headers for files
				std::cout << PADDING7_ << FILEDIRTEXT_ << std::endl;
			}
		}

		for (fileItr=(*tqItr).second->filedataMap_.begin();fileItr!=(*tqItr).second->filedataMap_.end();++fileItr)
		{
			std::cout << PADDING7_ << (*fileItr).second->filename_<<endl;
		}
		savedTqName = (*tqItr).first;
	}
	for (tqItr=tqMap.begin();tqItr!=tqMap.end();)
	{
		for (fileItr=(*tqItr).second->filedataMap_.begin();fileItr!=(*tqItr).second->filedataMap_.end();)
		{
			Filedata *tempFileData = (*fileItr).second;
			(*tqItr).second->filedataMap_.erase( fileItr++);
			delete tempFileData;
			tempFileData = 0;
		}
		Tqdata *tempTqData = (*tqItr).second;
		tqMap.erase( tqItr++);
		delete tempTqData;
		tempTqData = 0;
	}
	tqMap.clear();
}

//## Operation: listTqAndOldFile%E0400B77FEED; C++
//## Semantics:

/*===================================================================
   ROUTINE:listTqAndOldFile
=================================================================== */
unsigned int aes_afp_ls::listTqAndOldFile ()
{
  //## begin aes_afp_ls::listTqAndOldFile%E0400B77FEED.body preserve=yes
	AES_AFP_TRACE_MESSAGE("listTqAndOldFile()\n");
	unsigned int result(0);
	std::stringstream outStream;

	result = api_.listOldestFile(transferQueue_, destination_, outStream);
	if (result != AES_NOERRORCODE)
		{
			return result;
		}

	//outStream.freeze();
	printTqAndOldYoungFile(outStream);
	std::cout<<std::endl;
	return AES_NOERRORCODE;
  //## end aes_afp_ls::listTqAndOldFile%E0400B77FEED.body
}

//## Operation: printTqAndOldYoungFile%7967F92EFEED; C++
//## Semantics:

/*===================================================================
   ROUTINE:printTqAndOldYoungFile
=================================================================== */
void aes_afp_ls::printTqAndOldYoungFile (std::stringstream& buf)
{
  //## begin aes_afp_ls::printTqAndOldYoungFile%7967F92EFEED.body preserve=yes
	char outbuf[512];
	unsigned int cntrlsign(0);
	std::string tq("");
	std::string ug("");
	std::string itemName("");
	
	buf.getline(outbuf, 512, '\n');
	cntrlsign = atoi(outbuf);

	// DATA COLLECT PART ------------------------------------
	if (cntrlsign == PROCORDERSTART)
		{
			// TRANSFER QUEUE NAME ------------------------------
			buf.getline(outbuf, 512, '\n');
			tq = outbuf;

			// USER GROUP ---------------------------------------
			buf.getline(outbuf, 512, '\n');
			ug = outbuf;

			// Skip rest of tq data

			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');
			buf.getline(outbuf, 512, '\n');

			// Get new control sign
			buf.getline(outbuf, 512, '\n');
			cntrlsign = atoi(outbuf);

			if (cntrlsign == SENDITEMSTART)
				{
					// FILE OR DIRECTORY NAME -----------------------
					buf.getline(outbuf, 512, '\n');
					itemName = outbuf;

					// Skip rest of item data
					buf.getline(outbuf, 512, '\n');
					buf.getline(outbuf, 512, '\n');
					buf.getline(outbuf, 512, '\n');
					buf.getline(outbuf, 512, '\n');

					// Get new control sign
					buf.getline(outbuf, 512, '\n');
					cntrlsign = atoi(outbuf);
				}
		}
	// PRINTOUT PART ------------------------------------

	// Print header AFP TABLE
	if (header_ == true)
		{
			std::cout << AFPTABLETEXT_ << std::endl << std::endl;
		}

	if (header_ == true)
		{
			cout << std::setw(34) << std::left  << TQTEXT_ << std::setw(14) << std::endl; //USERGROUPTEXT_ <<  //std::cout << TQTEXT_ << std::endl;
		}

	std::cout << std::setw(34) << std::left << tq << std::setw(14) << ug << std::endl << std::endl;
	if (header_ == true)
		{
			std::cout << PADDING7_ << FILEDIRTEXT_ << std::endl;
		}
	std::cout << PADDING7_ << itemName << std::endl;

  //## end aes_afp_ls::printTqAndOldYoungFile%7967F92EFEED.body
}

//## Operation: listTqAndYoungFile%A2F42D4BFEED; C++
//## Semantics:

/*===================================================================
   ROUTINE:listTqAndYoungFile
=================================================================== */
unsigned int aes_afp_ls::listTqAndYoungFile ()
{
  //## begin aes_afp_ls::listTqAndYoungFile%A2F42D4BFEED.body preserve=yes
	AES_AFP_TRACE_MESSAGE("listTqAndYoungFile()\n");
	unsigned int result(0);
	std::stringstream outStream;

	result = api_.listNewestFile(transferQueue_, destination_, outStream);
	if (result != AES_NOERRORCODE)
		{
			return result;
		}

	//outStream.freeze();
	printTqAndOldYoungFile(outStream);
	std::cout<<std::endl;
	return AES_NOERRORCODE;
  //## end aes_afp_ls::listTqAndYoungFile%A2F42D4BFEED.body
}

//## Operation: listTqAndFilesLong%71CB0AB1FEED; C++
//## Semantics:

/*===================================================================
   ROUTINE:listTqAndFilesLong
=================================================================== */
unsigned int aes_afp_ls::listTqAndFilesLong ()
{
  //## begin aes_afp_ls::listTqAndFilesLong%71CB0AB1FEED.body preserve=yes
	unsigned int result(0);
	std::stringstream outStream;

	if (transferQueue_.empty() == true)
		{
			AES_AFP_TRACE_MESSAGE("listTqAndFilesLong()\n List all\n");
			result = api_.listAllFiles(transferQueue_, outStream);
			if (result != AES_NOERRORCODE && result != AES_NOPROCORDER)
				{
					return result;
				}
		}
	else
		{
			AES_AFP_TRACE_MESSAGE("listTqAndFilesLong()\n List TQ %s\n", transferQueue_.c_str());
			result = api_.listFile(transferQueue_, destination_, outStream);
			if (result != AES_NOERRORCODE)
				{
					return result;
				}
		}

	//outStream.freeze();
	printTqAndFilesLong(outStream);
	cout<<std::endl;

	return AES_NOERRORCODE;
  //## end aes_afp_ls::listTqAndFilesLong%71CB0AB1FEED.body
}

//## Operation: printTqAndFilesLong%BC46F719FEED; C++
//## Semantics:

/*===================================================================
   ROUTINE:printTqAndFilesLong
=================================================================== */
void aes_afp_ls::printTqAndFilesLong (std::stringstream& buf)
{
  //## begin aes_afp_ls::printTqAndFilesLong%BC46F719FEED.body preserve=yes
	char outbuf[512];
	unsigned int cntrlsign(0);
	int tmpInt(0);
	AES_GCC_Filestates states(AES_FSNONE);
//	bool firstTime(true);
	std::string tq("");
	std::string savedTqName("");

	Tqdata* tmpTqData;
	Filedata* tmpFileData;

	std::multimap<std::string, Tqdata*, std::less<std::string> >tqMap;

	std::multimap<std::string, Tqdata*, std::less<std::string> >::iterator tqItr;
	std::map<std::string, Filedata*, std::less<std::string> >::iterator fileItr;

	std::pair<std::string, Tqdata*>tqPair;
	std::pair<std::string, Filedata*>filePair;

	buf.getline(outbuf, 512, '\n');
	cntrlsign = atoi(outbuf);

	if (cntrlsign != PROCORDERSTART)
	{
		if (header_ == true)
		{
			std::cout << AFPTABLETEXT_ << std::endl;
		}
		return;
	}
	// COLLECT DATA PART ------------------------------------
	while (cntrlsign == PROCORDERSTART)
	{
		tmpTqData = new Tqdata;

		// TRANSFER QUEUE NAME ------------------------------
		buf.getline(outbuf, 512, '\n');
		tmpTqData->transferQueue_ = outbuf;

		// USER GROUP ---------------------------------------
		buf.getline(outbuf, 512, '\n');
		tmpTqData->userGroup_ = outbuf;

		buf.getline(outbuf, 512, '\n'); // Destination
		tmpTqData->destinationSet_ = outbuf;

		buf.getline(outbuf, 512, '\n'); // Default status, skip

		buf.getline(outbuf, 512, '\n'); // Remove delay
		tmpTqData->removeDelay_ = atoi(outbuf);

		// Skip rest of data
		buf.getline(outbuf, 512, '\n');
		buf.getline(outbuf, 512, '\n');
		buf.getline(outbuf, 512, '\n');

		// Get new control sign
		buf.getline(outbuf, 512, '\n');
		cntrlsign = atoi(outbuf);

		while (cntrlsign == SENDITEMSTART)
		{
			tmpFileData = new Filedata;
			// FILE OR DIRECTORY NAME -----------------------
			buf.getline(outbuf, 512, '\n');
			filePair.first = outbuf;
			tmpFileData->filename_ = outbuf;

			buf.getline(outbuf, 512, '\n');
			tmpFileData->destinationSet_ = outbuf;

			buf.getline(outbuf, 512, '\n'); // Dir flag
			tmpInt = atoi(outbuf);
			if (tmpInt == 0)
			{
				tmpFileData->isDirectory_ = NOTEXT_;
			}
			else
			{
				tmpFileData->isDirectory_ = YESTEXT_;
			}

			buf.getline(outbuf, 512, '\n'); // Status
			states = (AES_GCC_Filestates)atoi(outbuf);

			tmpFileData->status_ = stateString(states);

			buf.getline(outbuf, 512, '\n'); // Remove delay
			tmpFileData->removeTimer_ = atoi(outbuf);

			// Get new control sign
			buf.getline(outbuf, 512, '\n');
			cntrlsign = atoi(outbuf);
			filePair.second = tmpFileData;

			tmpTqData->filedataMap_.insert(filePair);
		}
		tqPair.first = tmpTqData->transferQueue_;
		tqPair.second = tmpTqData;
		tqMap.insert(tqPair);
	}

	// PRINTOUT PART ------------------------------------

	// Print header AFP TABLE
	if (header_ == true)
		{
			std::cout << AFPTABLETEXT_ << std::endl;
		}

	for (tqItr=tqMap.begin();tqItr!=tqMap.end();++tqItr)
		{
			std::cout << std::endl;

			if (savedTqName != (*tqItr).first)
				{
					if (header_ == true)
						{
							// Print TQ header
							cout << std::setw(34) << std::left  << TQTEXT_ << std::setw(14) << REMDELAYTEXT_ << std::endl; //USERGROUPTEXT_ <<
						}
					// Print tq value

					std::cout << std::setw(34) << std::left << (*tqItr).first;

					// Print remove delay value
					cout << std::setw(12) << std::right << (*tqItr).second->removeDelay_;
					cout << PADDING2_ << std::left << std::endl; //(*tqItr).second->userGroup_ <<

					std::cout << std::endl;

					if (header_ == true)
						{
							// Print headers for destinations and files
							cout.setf(ios::left, ios::adjustfield);
							std::cout << std::setw(34) << DESTTEXT_;

							cout.setf(ios::left, ios::adjustfield);
							std::cout << std::setw(5) << DIRTEXT_;

							cout.setf(ios::left, ios::adjustfield);
							std::cout << std::setw(14) << REMTIMERTEXT_;

							cout.setf(ios::left, ios::adjustfield);
							std::cout << std::setw(9) << STATUSTEXT_;

							cout.setf(ios::left, ios::adjustfield);
							std::cout << FILEDIRTEXT_ << std::endl;
						}
				}

			cout.setf(ios::left, ios::adjustfield);
			std::cout << (*tqItr).second->destinationSet_ << std::endl;

			for (fileItr=(*tqItr).second->filedataMap_.begin();fileItr!=(*tqItr).second->filedataMap_.end();++fileItr)
				{
					std::cout << PADDING34_;

					cout.setf(ios::left, ios::adjustfield);
					std::cout << std::setw(5) << (*fileItr).second->isDirectory_;

					cout.setf(ios::right, ios::adjustfield);
					std::cout << std::setw(12) << (*fileItr).second->removeTimer_;

					cout.setf(ios::left, ios::adjustfield);
					std::cout << PADDING2_ << std::setw(9) << (*fileItr).second->status_;

					cout.setf(ios::left, ios::adjustfield);
					std::cout << (*fileItr).second->filename_ << std::endl;
				}
			savedTqName = (*tqItr).first;
		}
	for (tqItr=tqMap.begin();tqItr!=tqMap.end();)
	{
		for (fileItr=(*tqItr).second->filedataMap_.begin();fileItr!=(*tqItr).second->filedataMap_.end();)
		{
			Filedata *tempFileData = (*fileItr).second;
			(*tqItr).second->filedataMap_.erase(fileItr++);
			delete tempFileData;
			tempFileData = 0;
		}
		Tqdata *tempTqData = (*tqItr).second;
		tqMap.erase( tqItr++);
		delete tempTqData;
		tempTqData = 0;
	}
	tqMap.clear();

}

/*===================================================================
	   ROUTINE:listTqAndOldFileLong
=================================================================== */
unsigned int aes_afp_ls::listTqAndOldFileLong ()
{
	AES_AFP_TRACE_MESSAGE("listTqAndOldFileLong()\n");
	unsigned int result(0);
	std::stringstream outStream;

	result = api_.listOldestFile(transferQueue_, destination_, outStream);
	if (result != AES_NOERRORCODE)
		{
			return result;
		}

	printTqAndFilesLong(outStream);
	std::cout<<std::endl;
	return AES_NOERRORCODE;	
}

/*===================================================================
   ROUTINE:listTqAndYoungFileLong
=================================================================== */
unsigned int aes_afp_ls::listTqAndYoungFileLong ()
{
	AES_AFP_TRACE_MESSAGE("listTqAndYoungFileLong()\n");
	unsigned int result(0);
	std::stringstream outStream;

	result = api_.listNewestFile(transferQueue_, destination_, outStream);
	if (result != AES_NOERRORCODE)
		{
			return result;
		}

	printTqAndFilesLong(outStream);
	std::cout<<std::endl;
	return AES_NOERRORCODE;
}

/*===================================================================
   ROUTINE:listAttr
=================================================================== */
unsigned int aes_afp_ls::listAttr ()
{
	AES_AFP_TRACE_MESSAGE("listAttr()\n");
	unsigned int result(0);
	std::stringstream outStream;

  if (destination_.empty() )
    {
      usage();
			return false;
    }

	result = api_.getAttributes(transferQueue_, destination_, outStream);
  if (result != AES_NOERRORCODE)
		{
			return result;
		}

	printAttributes(outStream);

	return AES_NOERRORCODE;
}

/*===================================================================
   ROUTINE:printAttributes
=================================================================== */
void aes_afp_ls::printAttributes (std::stringstream& buf)
{
	char outbuf[512];
	std::string tq("");
	std::string tmpUserGroup("");
	std::string remoteDestination("");
	std::string remBefore("");
	std::string manual("");
	std::string rename("");
	std::string sourceDirectory("");
	std::string renameTemplate("");
	std::string tmpNameTag("");
	std::string tmpStartupSequenceNumber(""); // uabmha: CNI 1135, from afpdef -k option
    int startupSequenceNumber;				  // Use string for check, use int for printout
	int tmpInt(0);
	int status(0);
	int remDelay(0);
	int sendRetries(0);
	int retriesDelay(0);

	// COLLECT DATA PART

	buf.getline(outbuf,512,'\n'); // Get tq
	tq = outbuf;

	buf.getline(outbuf, 512, '\n'); // Get user group
	tmpUserGroup = outbuf;


	buf.getline(outbuf,512,'\n'); // Get remote destination
	remoteDestination = outbuf;

	buf.getline(outbuf,512,'\n'); // Get status
	status = atoi(outbuf);

	buf.getline(outbuf,512,'\n'); // Get remove before
	tmpInt = atoi(outbuf);
	if (tmpInt == 0)
		{
			remBefore = NOTEXT_;
		}
	else
		{
			remBefore = YESTEXT_;
		}

	buf.getline(outbuf,512,'\n'); // Get remove delay
	remDelay = atoi(outbuf);

	buf.getline(outbuf,512,'\n'); // Get send retries
	sendRetries = atoi(outbuf);

	buf.getline(outbuf,512,'\n'); // Get retries delay
	retriesDelay = atoi(outbuf);

	buf.getline(outbuf,512,'\n'); // Get manual

	tmpInt = atoi(outbuf);
	if (tmpInt == 0)
		{
			manual = NOTEXT_;
		}
	else
		{
			manual = YESTEXT_;
		}

	buf.getline(outbuf,512,'\n'); // Get rename
	tmpInt = atoi(outbuf);
	if (tmpInt == 0)
		{
			rename = "LOCAL";
		}
	if (tmpInt == 1)
		{
			rename = "REMOTE";
		}
	if (tmpInt == 2)
		{
			rename = "NONE";
		}

	buf.getline(outbuf,512,'\n'); // Get directory
	sourceDirectory = outbuf;

	buf.getline(outbuf,512,'\n'); // Get template
	renameTemplate = outbuf;

	buf.getline(outbuf,512,'\n'); // Get name tag
	tmpNameTag = outbuf;

	// uabmha: CNI 1135, added optional printout of startup sequence number
	buf.getline(outbuf,512,'\n'); // Get startup sequence number
    tmpStartupSequenceNumber = outbuf;


	// PRINTOUT PART

	if (header_ == true)
		{
			std::cout << AFPTABLETEXT_ << std::endl << std::endl ;
			std::cout << std::setw(34) << std::left << TQTEXT_ << std::endl; //USERGROUPTEXT_ <<
		}

	std::cout << std::setw(34) << std::left << tq << std::left << std::endl; //tmpUserGroup <<
	std::cout<< std::endl;

	if (header_ == true)
		{
			std::cout << DESTTEXT_ << std::endl;
		}
	
	std::cout << remoteDestination << std::endl;
	std::cout<< std::endl;

	if (header_ == true)
		{
			cout.setf(ios::left, ios::adjustfield);
			cout << std::setw(16) << DEFSTATUSTEXT_;
			std::cout << std::setw(15) << REMBEFORETEXT_;
			std::cout << NAMETAGTEXT_ << endl;
		}

	// STATE ----------------------------------------------
	std::cout.setf(ios::left, ios::adjustfield);
	std::cout << std::setw(16) << stateString((AES_GCC_Filestates)status);

	// REMOVE BEFORE --------------------------------------
	std::cout.setf(ios::left, ios::adjustfield);
	std::cout << std::setw(15) << remBefore;

	// NAME TAG ---------------------------------------------
	std::cout.setf(ios::left, ios::adjustfield);
	std::cout << tmpNameTag << std::endl;
	std::cout << std::endl;

	if (header_ == true)
		{
			std::cout.setf(ios::left, ios::adjustfield);
			std::cout << std::setw(14) << REMDELAYTEXT_;

			std::cout.setf(ios::left, ios::adjustfield);
			std::cout << std::setw(9) << RETRIESTEXT_;

			std::cout.setf(ios::left, ios::adjustfield);
			std::cout << std::setw(16) << RETRYINTERVALTEXT_;

			std::cout.setf(ios::left, ios::adjustfield);
			std::cout << MANUALTEXT_ << std::endl;
		}
	
	// REMOVE DELAY -----------------------------------------
	std::cout.setf(ios::right, ios::adjustfield);  
	std::cout << std::setw(12) << remDelay << PADDING2_;

	// RETRIES ----------------------------------------------
	std::cout.setf(ios::right, ios::adjustfield);  
	std::cout <<std::setw(7) << sendRetries << PADDING2_;

	// RETRY DELAY ------------------------------------------
	std::cout.setf(ios::right, ios::adjustfield);  
	std::cout << std::setw(14) << retriesDelay << PADDING2_;

	// MANUAL -----------------------------------------------
	std::cout.setf(ios::left, ios::adjustfield);  
	std::cout << manual << std::endl;
	std::cout << std::endl;

	if (header_ == true)
		{
			std::cout.setf(ios::left, ios::adjustfield);
			std::cout << std::setw(27) << TEMPLATETEXT_; // For HL79347 : Since the template size is 
			std::cout.setf(ios::left, ios::adjustfield); // increased by four characters
			std::cout << std::setw(8) << RENAMETEXT_;
			std::cout << SOURCEDIRTEXT_ << std::endl;
		}

	// TEMPLATE ---------------------------------------------
	std::cout.setf(ios::left, ios::adjustfield);
	if (renameTemplate == "EMPTY")
		{
			std::cout << std::setw(27) << ""; // For HL79347 : Since the template size is 
		}									  // increased by four characters
	else
		{
			std::cout.setf(ios::left, ios::adjustfield);
			std::cout << std::setw(27) << renameTemplate; // For HL79347 : Since the template size is 
		}												  // increased by four characters

	// RENAME -----------------------------------------------
	std::cout.setf(ios::left, ios::adjustfield);
	std::cout << std::setw(8) << rename;

	// SOURCE DIRECTORY -------------------------------------
	std::cout.setf(ios::left, ios::adjustfield);
	std::cout << sourceDirectory << std::endl;

    // uabmha: CNI 1135, new header and value added (for afpdef -k option)
	// Note: header should only
	// STARTUP SEQUENCE NUMBER -------------------------------------
    
	// Note, atoi requires a char * argument...
	startupSequenceNumber = atoi(tmpStartupSequenceNumber.c_str());

	if (startupSequenceNumber == -1)
	{
       // The default value 0 was used (internally repr. as -1)
	   // This means the -k option has not been used to set the value.
       // Suppress the printout of header and value (for backward compatibility)
	}
	else
	{
		std::cout << std::endl; // Empty line before next row...


		// Line not empty, check now if header shall be suppressed due to -q option...
    	if (header_ == true)
		{
			std::cout.setf(ios::left, ios::adjustfield);
			std::cout << STARTUPSEQUENCENUMBERTEXT_ << std::endl;
		}

	    std::cout.setf(ios::right, ios::adjustfield);
    	std::cout << std::setw(23) << startupSequenceNumber << std::endl;
		//std::cout << startupSequenceNumber << std::endl;
	}

	
}


/*===================================================================
   ROUTINE:stateString
=================================================================== */
std::string aes_afp_ls::stateString (AES_GCC_Filestates aFileState)
{
  switch (aFileState)
		{
			case AES_FSREADY:
				{
					return "READY";
				}
			case AES_FSSEND:
				{
					return "SEND";
				}
			case AES_FSARCHIVE:
				{
					return "ARCHIVE";
				}
			case AES_FSDELETE:
				{
					return "DELETE";
				}
			case AES_FSFAILED:
				{
					return "FAILED";
				}
			case AES_FSREMOVE:
				{
					return "REMOVE";
				}
			case AES_FSSTOPPED:
				{
					return "STOPPED";
				}
			case AES_FSPENDING:
				{
					return "PENDING";
				}
			case AES_FSNONE:
				{
					return "NONE";
				}
			default:
				{
					return "NONE";
				}
		}
}

std::string aes_afp_ls::updateSourceDirectory (std::string& sourcePath)
{
	std::string str1 = sourcePath;
	std::string str2("");
	bool bStatus = getFileMPath(str2);
	if(!bStatus)
	{
		str1 = strnotAvailable;
		return str1;
	} 
	size_t pos1 = str1.find(str2);
	if( pos1 != string::npos)
	{
		str1 = str1.substr(pos1+str2.length() , str1.length()-pos1-str2.length());
	}
	else
	{
		str1 = strnotAvailable;
	}
	return str1;
}

/*===================================================================
   ROUTINE:getFileMPath
=================================================================== */
bool  aes_afp_ls::getFileMPath( string & directoryPath )
{
	ACS_APGCC_DNFPath_ReturnTypeT returnType = ACS_APGCC_DNFPATH_FAILURE;

	string fileMFuncName = INTERNAL_ROOT_PATH; 

	char folderPath[1024] = { 0 };

	int folderPathLen = 1024;

	ACS_APGCC_CommonLib objCommonLib;

	returnType = objCommonLib.GetFileMPath( fileMFuncName.c_str(),
			folderPath,
			folderPathLen );

	if( returnType != ACS_APGCC_DNFPATH_SUCCESS )
	{
		return false;
	}

	directoryPath = folderPath;

	return true;
}
