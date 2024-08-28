//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FD8029E.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FD8029E.cm

//## begin module%3DEF5FD8029E.cp preserve=no
//	INCLUDE aes_afp_chd.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002.
//	   All rights reserved.
//	   The Copyright to the computer program(s) herein
//	   is the property of Ericsson Utvecklings AB, Sweden.
//	   The program(s) may be used and/or copied only with
//	   the written permission from Ericsson Utvecklings AB or
//	   in accordance with the terms and conditions stipulated
//	   in the agreement/contract under which the program(s)
//	   have been supplied.
//
//	DESCRIPTION:
//
//	ERROR HANDLING
//	   General rule:
//	   The error handling is specified for each method.
//	   No methods initiate or send error reports unless
//	   specified.
//
//	DOCUMENT NO
//	   19089-CAA 109 0504
//
//	AUTHOR
//	   2002-12-12  UAB/UKB/AU DAPA
//
//	REVISION
//	   A 2002-12-12 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3DEF5FD8029E.cp

//## Module: aes_afp_chd%3DEF5FD8029E; Package body
//## Subsystem: AFP::afpcmd_caa::src%3DEF666D0066
//## Source file: Z:\ntaes\afp\afpcmd_caa\src\aes_afp_chd.cpp

//## begin module%3DEF5FD8029E.additionalIncludes preserve=no
//## end module%3DEF5FD8029E.additionalIncludes

//## begin module%3DEF5FD8029E.includes preserve=yes
#pragma warning(disable : 4786)
#include <ACS_ExceptionHandler.h>
//## end module%3DEF5FD8029E.includes

#include "aes_afp_chd.h"
//## begin module%3DEF5FD8029E.declarations preserve=no
ACS_TRA_trace traceoutput = ACS_TRA_DEF("aes_afp_afpchd","C400");

//------------------------------------------------------------------------------
//	main ()
//------------------------------------------------------------------------------

int
main (int argc, char *argv [])
{
	AP_InitProcess ("AES_AFP_afpchd", AP_COMMAND);
	AP_SetCleanupAndCrashRoutine("AES_AFP_afpchd", NULL);
  aes_afp_chd chd (argc, argv);
  return chd.launch ();
}
//## end module%3DEF5FD8029E.declarations

//## begin module%3DEF5FD8029E.additionalDeclarations preserve=yes
//## end module%3DEF5FD8029E.additionalDeclarations


// Class aes_afp_chd 

//## Operation: aes_afp_chd%86CC6304FEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       Constructor()
//	---------------------------------------------------------
aes_afp_chd::aes_afp_chd (int argc, char** argv)
  //## begin aes_afp_chd::aes_afp_chd%86CC6304FEED.hasinit preserve=no
      : fileDestination_(""),
        destination_(""),
        remBefore_(""),
        changeDefaultState_(false),
        changeRemoveDelay_(false),
        changeRemoveBefore_(false),
        changeNumberOfRetries_(false),
        changeRetryTime_(false),
        defaultState_(AES_FSREADY),
        removeDelay_(DEFAULT_REMOVEDELAY_),
        removeBefore_(false),
        numberOfRetries_(DEFAULT_NROFRETRIES_),
        retryTime_(DEFAULT_RETRYTIME_),trace_("")
  //## end aes_afp_chd::aes_afp_chd%86CC6304FEED.hasinit
  //## begin aes_afp_chd::aes_afp_chd%86CC6304FEED.initialization preserve=yes
  //## end aes_afp_chd::aes_afp_chd%86CC6304FEED.initialization
{
  //## begin aes_afp_chd::aes_afp_chd%86CC6304FEED.body preserve=yes
	argC_ = argc;
  argV_ = argv; 
  //## end aes_afp_chd::aes_afp_chd%86CC6304FEED.body
}


aes_afp_chd::~aes_afp_chd()
{
  //## begin aes_afp_chd::~aes_afp_chd%3DEF5FD20390_dest.body preserve=yes
  //## end aes_afp_chd::~aes_afp_chd%3DEF5FD20390_dest.body
}



//## Other Operations (implementation)
//## Operation: parse%F06555D2FEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       parse()
//	---------------------------------------------------------
bool aes_afp_chd::parse ()
{
  //## begin aes_afp_chd::parse%F06555D2FEED.body preserve=yes
  int c;
	//unsigned int result(0);
	//AES_AFP_Proctype fileType(FT_NONE);//FT_SIMPLE);
  option_t option;
  optpair_t optlist [] = {{0,0},{0,0},{0,0},{0,0},{0,0}}; 

  aes_afp_getopt getopt(argC_, argV_, "s:d:b:r:t:");

	if (ACS_TRA_ON(traceoutput) )
    {
      sprintf(trace_,
							"\n%s\n",
							"afpchd::parse()");
     ACS_TRA_event(&traceoutput, trace_);
    }

  // Parse the options

  while ((c = getopt ()) != EOF)
		{
			switch (c) 
				{
					case 's':
						{
							option = DEFAULTSTATE;
							break;
						}
					case 'd':
						{
							option = REMOVEDELAY;
							break;
						}
					case 'b':
						{
							option = REMOVEBEFORE;
							break;
						}
					case 'r':
						{
							option = NROFRETRIES;
							break;
						}
					case 't':
						{
							option = RETRYTIME;
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

			if (!optlist[option].opt_)
				{
					optlist[option].opt_ = c;
					optlist[option].arg_ = getopt.optarg_;
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
			//bool manual(false);
			fileDestination_ = toUpper(argV_[getopt.optind_]);
			getopt.optind_++;
		}          

  if (argC_ > getopt.optind_)
		{	
			destination_ = toUpper((char*)argV_[getopt.optind_]);
			getopt.optind_++;
		}
  else
		{		
			usage (); 
			return false;
		}
	
  // No more names -------------------------------------------------------------

  if (argC_ != getopt.optind_)
		{
			usage (); 
			return false;
		}

  // Change Default State option -----------------------------------------------
 
  changeDefaultState_ = optlist[DEFAULTSTATE].opt_ ? true: false;

  // Change Remove Delay option ------------------------------------------------
 
  changeRemoveDelay_ = optlist[REMOVEDELAY].opt_ ? true: false;

  // Change Remove Before option -----------------------------------------------
 
  changeRemoveBefore_ = optlist[REMOVEBEFORE].opt_ ? true: false;

  // Change Number Of Retries option -------------------------------------------
 
  changeNumberOfRetries_ = optlist[NROFRETRIES].opt_ ? true: false;

  // Change Retry Time option --------------------------------------------------
 
  changeRetryTime_ = optlist[RETRYTIME].opt_ ? true: false;

  // Change Default state -------------------------------------------------------

  if (changeDefaultState_ == true)
		{
      std::string defstate("");
			defstate = toUpper(optlist[DEFAULTSTATE].arg_);

			if (defstate == "READY")
				{
					defaultState_ = AES_FSREADY;
				}
			else if (defstate == "DELETE")
				{
					defaultState_ = AES_FSDELETE;
				}
			else if (defstate == "NONE")
				{
					defaultState_ = AES_FSNONE;
				}
			else if (defstate == "PENDING")
				{
					defaultState_ = AES_FSPENDING;
				}
			else
				{
					printErrorMessage(AES_ILLEGALSTATUSVALUE);
					exit (AES_ILLEGALSTATUSVALUE);
				}
		}

  // Change Remove delay -------------------------------------------------------

  if (changeRemoveDelay_)
		{
			if (checkNumericValue(optlist[REMOVEDELAY].arg_) == false)
				{
					printErrorMessage(AES_ILLEGALDELAYVALUE);
					exit(AES_ILLEGALDELAYVALUE);
				}
			int num = atoi(optlist[REMOVEDELAY].arg_);
			removeDelay_ = num;
		}

	// Change remove before ------------------------------------------------------
	if (changeRemoveBefore_)
		{
			remBefore_ = toUpper(optlist[REMOVEBEFORE].arg_);
		}

  // Change Number of Retries -------------------------------------------------------

  if (changeNumberOfRetries_)
		{
			if (checkNumericValue(optlist[NROFRETRIES].arg_) == false)
				{
					printErrorMessage(AES_ILLEGALRETRYVALUE);
					exit(AES_ILLEGALRETRYVALUE);
				}
			int num = atoi(optlist[NROFRETRIES].arg_);
			numberOfRetries_ = num;
		}
		
  // Change Retry Time -------------------------------------------------------

  if (changeRetryTime_)
		{
			if (checkNumericValue(optlist[RETRYTIME].arg_) == false)
				{
					printErrorMessage(AES_ILLEGALTIMEVALUE);
					exit(AES_ILLEGALTIMEVALUE);
				}
			int num = atoi(optlist[RETRYTIME].arg_);
			retryTime_ = num;
		}
  return true;
  //## end aes_afp_chd::parse%F06555D2FEED.body
}

//## Operation: execute%CE08DA99FEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       execute()
//	---------------------------------------------------------
unsigned int aes_afp_chd::execute ()
{
  //## begin aes_afp_chd::execute%CE08DA99FEED.body preserve=yes
  AES_GCC_Filestates newDefaultState;
	bool newRemoveBefore(false);
	unsigned int result(0);
 // unsigned int tempstate(0);
	int newNumberOfRetries(0);
  int newRemoveDelay(0);
	int newRetryTime(0);
  int startPos(0);
	int search(0);
  int tmpNum(0);
	//int temptype(0);
  std::strstream attrStream;
  std::string tmpString("");
  std::string tmpSubString("");
	bool fileLocked(false);
	std::string userOfFile("");
	std::string trans("");
	trans = fileDestination_;

	//Get old attributes
	result = api_.getAttributes(fileDestination_, destination_, attrStream);

	if (result == AES_NOPROCORDER)
		{
			unsigned int	retroact(0);
      std::list<std::string>	tqueList;
      std::list<std::string>::iterator itr;
      std::string	tqueString("");
      std::strstream destStream;
      std::string destString("");
      bool destFound(false);
	  int transcheck(0);

			retroact = api_.getDestinationSetList(tqueList);
      for (itr=tqueList.begin(); itr!=tqueList.end(); ++itr)
				{
					tqueString=*itr;
					if(tqueString.compare(trans)==0)
					   transcheck = 1;
					retroact = api_.listDestinationSets(tqueString, destStream);
					if (retroact == AES_NOERRORCODE)
						{
							destStream >> destString; // skip procorderstart
							destStream >> destString;
							while (destStream.good())
								{
									if (destString.compare(destination_)==0)
										{
											destFound = true;
											break;
										}
									destStream >> destString;
								}
						}
          if (destFound)
						{
							break; 
						}
				}
			if (!destFound && transcheck)
				{
					result = AES_NODESTINATION;
				}
		}
                                                                       
	if ( result != AES_NOERRORCODE)
		{
			printErrorMessage(result);
      exit(result);
		}

	attrStream.freeze();
  tmpString = attrStream.str();

  //spool filename
  search = tmpString.find_first_of("\n");
	if (search>0)
		{
			tmpString[search] = ';';
		}
	startPos = search+1;

  //spool destinaton
  search = tmpString.find_first_of("\n");
	if (search>0)
		{
			tmpString[search] = ';';
		}
	startPos = search+1;

  //spool filetype
  search = tmpString.find_first_of("\n");
	if (search>0)
		{
			tmpString[search] = ';';
		}
	tmpSubString = tmpString.substr(startPos, search-startPos);
	/*temptype = */atoi(tmpSubString.c_str() );
	startPos = search+1;

  // get defaultstate
  search = tmpString.find_first_of("\n");
	if (search>0)
		{
			tmpString[search] = ';';
		}
  tmpSubString = tmpString.substr(startPos, search-startPos);
	startPos = search+1;

	unsigned int tempstate = atoi(tmpSubString.c_str() );
	switch (tempstate)
		{
			case AES_FSREADY: 
				{
					newDefaultState = AES_FSREADY;
					break;
				}
			case AES_FSSEND : 
				{	
					newDefaultState = AES_FSSEND;
					break;
				}
			case AES_FSARCHIVE : 
				{
					newDefaultState = AES_FSARCHIVE;
					break;
				}
			case AES_FSDELETE : 
				{
					newDefaultState = AES_FSDELETE;
					break;
				}
			case AES_FSFAILED : 
				{
					newDefaultState = AES_FSFAILED;
					break;
				}
			case AES_FSPENDING:
				{
					newDefaultState = AES_FSPENDING;
					if (changeDefaultState_ == true)
						{
							result = api_.isTransferQueueLocked(fileDestination_, destination_, fileLocked, userOfFile);
							if (fileLocked == true)
								{
									std::cout << api_.getErrorCodeText(AES_FILEISPROTECTED) << " " << userOfFile << " " << destination_ << std::endl;
									exit(AES_FILEISPROTECTED);
								}
						}
					break;
				}
			case AES_FSNONE : 
				{
					newDefaultState = AES_FSNONE;
					break;
				}
			default : 
				{
					newDefaultState = AES_FSNONE;
				}
		}

	//Get removebefore
  search = tmpString.find_first_of("\n");
	if (search>0)
		{
			tmpString[search] = ';';
		}
  tmpSubString = tmpString.substr(startPos, search-startPos);
	startPos = search+1;

	tmpNum = atoi(tmpSubString.c_str());
	if (tmpNum !=0)
		{
			newRemoveBefore = true;
		}
	else
		{
			newRemoveBefore = false;
		}

	//Get removedelay
  search = tmpString.find_first_of("\n");
	if (search>0)
		{
			tmpString[search] = ';';
		}
  tmpSubString = tmpString.substr(startPos, search-startPos);
	startPos = search+1;
	newRemoveDelay = atoi(tmpSubString.c_str());

	//Get numberOfRetries
  search = tmpString.find_first_of("\n");
	if (search>0)
		{
			tmpString[search] = ';';
		}
  tmpSubString = tmpString.substr(startPos, search-startPos);
	startPos = search+1;
	newNumberOfRetries = atoi(tmpSubString.c_str());

	//Get retryTime
  search = tmpString.find_first_of("\n");
	if (search>0)
		{
			tmpString[search] = ';';
		}
  tmpSubString = tmpString.substr(startPos, search-startPos);
	startPos = search+1;
	newRetryTime = atoi(tmpSubString.c_str());

	//spool manual
  search = tmpString.find_first_of("\n");
	if (search>0)
		{
			tmpString[search] = ';';
		}
	startPos = search+1;

	if (changeDefaultState_)
		{
			switch (newDefaultState)
				{
					case AES_FSREADY:
						{
							printErrorMessage(AES_ERRORCHANGESTATUS);
							exit (AES_ERRORCHANGESTATUS);
						}
					case AES_FSSEND:
						{
							if (defaultState_ == AES_FSFAILED || defaultState_ == AES_FSDELETE)
								{
									printErrorMessage(AES_ERRORCHANGESTATUS);
									exit (AES_ERRORCHANGESTATUS);
								}
							else
								{
									newDefaultState = defaultState_;
								}
						}
					default:
						{
							newDefaultState = defaultState_;
						}
				}
		}

	if (changeRemoveDelay_)
		{
			newRemoveDelay = removeDelay_;
		}

	if (changeRemoveBefore_)
		{
			if (remBefore_ == "YES")
				{
					newRemoveBefore = true;
				}
			else if (remBefore_ == "NO")
				{
					newRemoveBefore = false;
				}
			else
				{
					printErrorMessage(AES_ILLEGALREMBEFORE);
					exit(AES_ILLEGALREMBEFORE);
				}
		}

	if (changeNumberOfRetries_)
		{
			newNumberOfRetries = numberOfRetries_;
		}

	if (changeRetryTime_)
		{
			newRetryTime = retryTime_;
		}

	result = api_.setAttributes(fileDestination_,
												destination_,
												newDefaultState,
												newRemoveDelay, 
												newRemoveBefore, 
												newNumberOfRetries, 
												newRetryTime );

	if (result != 0)
		{
			printErrorMessage(result);
			exit(result);
		}

	return(0);
  //## end aes_afp_chd::execute%CE08DA99FEED.body
}

//## Operation: usage%A6DB3689FEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       usage()
//	---------------------------------------------------------
void aes_afp_chd::usage ()
{
  //## begin aes_afp_chd::usage%A6DB3689FEED.body preserve=yes
  cout << "Incorrect usage" << endl;
  cout << "Usage: " << argV_[0] << " [-b yes|no][-d removedelay][-r retries][-s defaultstatus]" << endl;
  cout << "              [-t retryinterval] transferqueue destinationset" << endl;
  cout << endl;

  exit(AES_INCORRECTCOMMAND);
  //## end aes_afp_chd::usage%A6DB3689FEED.body
}

// Additional Declarations
  //## begin aes_afp_chd%3DEF5FD20390.declarations preserve=yes
  //## end aes_afp_chd%3DEF5FD20390.declarations

//## begin module%3DEF5FD8029E.epilog preserve=yes
//## end module%3DEF5FD8029E.epilog
