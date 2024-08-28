//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FDA001F.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FDA001F.cm

//## begin module%3DEF5FDA001F.cp preserve=no
//	INCLUDE aes_afp_fti.h
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
//## end module%3DEF5FDA001F.cp

//## Module: aes_afp_fti%3DEF5FDA001F; Package body
//## Subsystem: AFP::afpcmd_caa::src%3DEF666D0066
//## Source file: Z:\ntaes\afp\afpcmd_caa\src\aes_afp_fti.cpp

//## begin module%3DEF5FDA001F.additionalIncludes preserve=no
//## end module%3DEF5FDA001F.additionalIncludes

//## begin module%3DEF5FDA001F.includes preserve=yes
#include <ACS_ExceptionHandler.h>
#include <algorithm>
//## end module%3DEF5FDA001F.includes

#include "aes_afp_fti.h"
//## begin module%3DEF5FDA001F.declarations preserve=no
ACS_TRA_trace traceoutput = ACS_TRA_DEF("aes_afp_afpfti","C400");

//------------------------------------------------------------------------------
//	main ()
//------------------------------------------------------------------------------

int
main (int argc, char *argv [])
{
   AP_InitProcess ("AES_AFP_afpfti", AP_COMMAND);
   AP_SetCleanupAndCrashRoutine("AES_AFP_afpfti", NULL);
   aes_afp_fti fti (argc, argv);
   return fti.launch ();
}
//## end module%3DEF5FDA001F.declarations

//## begin module%3DEF5FDA001F.additionalDeclarations preserve=yes
//## end module%3DEF5FDA001F.additionalDeclarations


// Class aes_afp_fti 

//## Operation: aes_afp_fti%C5B472EEFEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       Constructor()
//	---------------------------------------------------------
aes_afp_fti::aes_afp_fti (int argc, char** argv)
  //## begin aes_afp_fti::aes_afp_fti%C5B472EEFEED.hasinit preserve=no
      : filename_(""),
        destinationSet_(""),
        newDestinationSet_(""),
        transferQueue_(""),
        sourceDirectory_(""),
        SENDRETRIES_(0),
        transferFailed_(false),
        isDirectory_(false),
        SENDRETRIESDELAY_(10),
        userGroup_("")
  //## end aes_afp_fti::aes_afp_fti%C5B472EEFEED.hasinit
  //## begin aes_afp_fti::aes_afp_fti%C5B472EEFEED.initialization preserve=yes
  //## end aes_afp_fti::aes_afp_fti%C5B472EEFEED.initialization
{
  //## begin aes_afp_fti::aes_afp_fti%C5B472EEFEED.body preserve=yes
  argC_ = argc;
  argV_ = argv;
  //## end aes_afp_fti::aes_afp_fti%C5B472EEFEED.body
}


aes_afp_fti::~aes_afp_fti()
{
  //## begin aes_afp_fti::~aes_afp_fti%3DEF5FD40374_dest.body preserve=yes
  //## end aes_afp_fti::~aes_afp_fti%3DEF5FD40374_dest.body
}



//## Other Operations (implementation)
//## Operation: parse%D51CCB9AFEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       parse()
//	---------------------------------------------------------
bool aes_afp_fti::parse ()
{
  //## begin aes_afp_fti::parse%D51CCB9AFEED.body preserve=yes
  int c(0);
	int index(0);
	int volIndex(0);
	std::string tmpStr("");
	char buffer[_MAX_PATH];
  option_t option;
  optpair_t optlist [] = {{0,0},{0,0},{0,0},{0,0}, {0,0}};

  aes_afp_getopt getopt(argC_, argV_, "c:fv:z:");

  if (argC_ == 1)
		{
			usage();
			return false;
		}
  
  // Parse the options
  while ((c = getopt()) != EOF)
    {
      switch (c) 
        {
					case 'c':
						{
							option = FILENAME;
							break;
						}
					case 'f':
						{
							option = FAILEDFILES;
							transferFailed_ = true;
							break;
						}
					case 'v':
						{
							option = USERGROUP;
							break;
						}
					case 'z':
            {
              option = ISDIRECTORY;
							isDirectory_ = true;
              break;
            }
          case '?':
						{
							usage();
							return false;
						}
          default:
            {
              usage ();
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
    }

	// Syntax check---------------------------------------------------------------

	if (optlist [FILENAME].opt_ && optlist [ISDIRECTORY].opt_)
		{
			usage();
			return false;
		}

	if (transferFailed_ == true && optlist [FILENAME].opt_)
		{
			usage();
			return false;
		}

	if (transferFailed_ == true && optlist [ISDIRECTORY].opt_)
		{
			usage();
			return false;
		}

	if (transferFailed_ == false && optlist [FILENAME].opt_ && optlist [ISDIRECTORY].opt_)
		{
			usage();
			return false;
		}
	if (transferFailed_ == true && optlist [USERGROUP].opt_)
		{
			usage();
			return false;
		}

  if (argC_ > getopt.optind_)
		{
			transferQueue_ = toUpper(argV_[getopt.optind_]);
			getopt.optind_++;
		}

  if (argC_ > getopt.optind_)
		{
			destinationSet_ = toUpper(argV_[getopt.optind_]);
			getopt.optind_++;
		}

  if (argC_ > getopt.optind_ && transferFailed_ == true)
		{
			newDestinationSet_ = toUpper(argV_[getopt.optind_]);
			getopt.optind_++;
		}
	if (argC_ > getopt.optind_ && transferFailed_ == false)
		{
      usage (); 
      return false;
		}

	if (transferQueue_.empty() )
		{
			usage (); 
			return false;
		}

	if (transferFailed_ == false)
		{
			if (destinationSet_.empty() )
				{
					usage (); 
					return false;
				}
		}

  // No more names -------------------------------------------------------------

  if (argC_ != getopt.optind_)
    {
      usage (); 
      return false;
    }

	// Check filename -------------------------------------------------------
	if (optlist[FILENAME].opt_)
		{
			filename_ = optlist[FILENAME].arg_;
		}

	// Check directory name 
	if (optlist[ISDIRECTORY].opt_)
		{
			filename_ = optlist[ISDIRECTORY].arg_;
		}

	// Check user group
	if (optlist[USERGROUP].opt_)
		{
			userGroup_ = optlist[USERGROUP].arg_;
		}
 
  // Sort out Directory  ---------------------------------------------------------

	if (transferFailed_ == false)
		{
			index = filename_.find_last_of('\\');
		
			if (index != -1)
				{
					tmpStr = toUpper((char*)filename_.c_str() );	
					sourceDirectory_ = tmpStr.substr(0, index);
					volIndex = sourceDirectory_.find_first_of(':');

					if (volIndex != 1)
						{
							// No volume supplied, get cwd
							_getcwd(buffer, _MAX_PATH);
							std::string tmpDir(buffer);
							volIndex = sourceDirectory_.find_first_of('\\');
							if (volIndex != 0)
								{
									// If length of tmp dir <= 3 we are in the "root" (eg C:\)
									// and since root has a \ we don't want to add another one.
									if (tmpDir.length() > 3)
										{
											tmpDir += "\\";
										}
								}
							else if (tmpDir.length() <= 3 && volIndex == 0)
								{
									// Remove one backslash at the beginning
									sourceDirectory_.erase(0,1);
								}
									
							tmpDir += sourceDirectory_;

							sourceDirectory_ = tmpDir;
						}
					else
						{
							// We are in the root of the volume (eg C:)
							// Add a backslash only if source directory is two characters.
							if (sourceDirectory_.length() == 2)
								{
									sourceDirectory_ += '\\';
								}
						}

					filename_ = tmpStr.substr(index+1);
				}
			else
				{
					_getcwd(buffer, _MAX_PATH);
					sourceDirectory_ = toUpper(buffer);
				}
		}
  return true;
  //## end aes_afp_fti::parse%D51CCB9AFEED.body
}

//## Operation: execute%4F2D1D62FEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       execute()
//	---------------------------------------------------------
unsigned int aes_afp_fti::execute ()
{
  //## begin aes_afp_fti::execute%4F2D1D62FEED.body preserve=yes
	unsigned int result(0);

	if (transferFailed_ == true)
		{
			if (destinationSet_ == "")
				{
					result = api_.resendAllFailed(transferQueue_);
				}
			else
				{
					result = api_.resendOneFailed(transferQueue_, destinationSet_, newDestinationSet_);
				}
		}
	else
		{
			result = api_.sendFileManually(transferQueue_,
																			userGroup_,
																			filename_,
																			sourceDirectory_,
																			destinationSet_,
																			SENDRETRIES_,
																			SENDRETRIESDELAY_,
																			isDirectory_);
		}

	if (result != 0)
    {
			printErrorMessage(result);
    }

	return result;
  //## end aes_afp_fti::execute%4F2D1D62FEED.body
}

//## Operation: usage%7E1C6DBCFEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       usage()
//	---------------------------------------------------------
void aes_afp_fti::usage ()
{
  //## begin aes_afp_fti::usage%7E1C6DBCFEED.body preserve=yes
  cout<<"Incorrect usage"<<endl;
	cout<<"Usage: "<<argV_[0]<<" -c file | -z directory transferqueue destinationset"<<endl;
	cout<<"       "<<argV_[0]<<" -c file | -z directory [-v usergroup] transferqueue destinationset"<<endl;
	cout<<"       "<<argV_[0]<<" -f transferqueue destinationset [newdestinationset]" << endl;
  cout<<"       "<<argV_[0]<<" -f transferqueue" << endl;
  cout<<endl;

	exit (AES_INCORRECTCOMMAND);
  //## end aes_afp_fti::usage%7E1C6DBCFEED.body
}

// Additional Declarations
  //## begin aes_afp_fti%3DEF5FD40374.declarations preserve=yes
  //## end aes_afp_fti%3DEF5FD40374.declarations

//## begin module%3DEF5FDA001F.epilog preserve=yes
//## end module%3DEF5FDA001F.epilog
