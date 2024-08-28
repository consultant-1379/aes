//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FD90295.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FD90295.cm

//## begin module%3DEF5FD90295.cp preserve=no
//	INCLUDE aes_afp_fte.h
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
//	   2002-12-06  DAPA
//
//	REVISION
//	   A 2002-12-06 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3DEF5FD90295.cp

//## Module: aes_afp_fte%3DEF5FD90295; Package body
//## Subsystem: AFP::afpcmd_caa::src%3DEF666D0066
//## Source file: Z:\ntaes\afp\afpcmd_caa\src\aes_afp_fte.cpp

//## begin module%3DEF5FD90295.additionalIncludes preserve=no
//## end module%3DEF5FD90295.additionalIncludes

//## begin module%3DEF5FD90295.includes preserve=yes
//## end module%3DEF5FD90295.includes

#include "aes_afp_fte.h"
//## begin module%3DEF5FD90295.declarations preserve=no
ACS_TRA_trace traceoutput = ACS_TRA_DEF("aes_afp_afpfte","C400");

//------------------------------------------------------------------------------
//	main ()
//------------------------------------------------------------------------------
int
main (int argc, char *argv [])
{
	AP_InitProcess ("AES_AFP_afpfte", AP_COMMAND);
	AP_SetCleanupAndCrashRoutine("AES_AFP_afpfte", NULL);
  aes_afp_fte fte (argc, argv);
  return fte.launch ();
}
//## end module%3DEF5FD90295.declarations

//## begin module%3DEF5FD90295.additionalDeclarations preserve=yes
//## end module%3DEF5FD90295.additionalDeclarations


// Class aes_afp_fte 

//## Operation: aes_afp_fte%E0FA9D23FEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       Constructor()
//	---------------------------------------------------------
aes_afp_fte::aes_afp_fte (int argc, char** argv)
  //## begin aes_afp_fte::aes_afp_fte%E0FA9D23FEED.hasinit preserve=no
      : fileDestination_(""),
        filename_(""),
        directory_(""),
        itemName_(""),
        destination_("")
  //## end aes_afp_fte::aes_afp_fte%E0FA9D23FEED.hasinit
  //## begin aes_afp_fte::aes_afp_fte%E0FA9D23FEED.initialization preserve=yes
  //## end aes_afp_fte::aes_afp_fte%E0FA9D23FEED.initialization
{
  //## begin aes_afp_fte::aes_afp_fte%E0FA9D23FEED.body preserve=yes
	argC_ = argc;
  argV_ = argv;
  //## end aes_afp_fte::aes_afp_fte%E0FA9D23FEED.body
}


aes_afp_fte::~aes_afp_fte()
{
  //## begin aes_afp_fte::~aes_afp_fte%3DEF5FD401C5_dest.body preserve=yes 
  //## end aes_afp_fte::~aes_afp_fte%3DEF5FD401C5_dest.body
}



//## Other Operations (implementation)
//## Operation: parse%E78C1E30FEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       parse()
//	---------------------------------------------------------
bool aes_afp_fte::parse ()
{
  //## begin aes_afp_fte::parse%E78C1E30FEED.body preserve=yes
  int c(0);
	//int index(0);
	std::string tmpFileName("");
	//bool manual(false);
  option_t option;
  optpair_t optlist [] = {{0,0},{0,0}};

  aes_afp_getopt getopt (argC_, argV_, "c:z:");
	
  // Parse the options
  while ((c = getopt ()) != EOF)
		{
			switch (c) 
				{
					case 'c':
						{
							option = FILENAME;
							break;
						}
					case 'z':
						{
							option = DIRECTORY;
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

			if (!optlist [option].opt_)
				{
					optlist [option].opt_ = c;
					optlist [option].arg_ = getopt.optarg_;
				}
			else
				{
					usage();
					return false;
				}
		} // while

  // Parse file destination name -----------------------------------------------------------

  if (argC_ > getopt.optind_)
    {                                  
			fileDestination_ = toUpper( argV_[getopt.optind_]);
			getopt.optind_++;
    }
  else
    {
      usage (); 
      return false;
    }


  // Parse destination name ----------------------------------------------------

  if (argC_ > getopt.optind_)
    {
      destination_ = toUpper( argV_[getopt.optind_]);
      getopt.optind_++;
    }
  else
    {
      usage (); 
      return false;
    }

	// Check syntax, not at the same time
	if (optlist[FILENAME].opt_ && optlist[DIRECTORY].opt_)
		{
			usage(); 
			return false;
		}

	// Check syntax, at least one of them
	if ( (optlist[FILENAME].opt_ == false) && (optlist[DIRECTORY].opt_ == false) )
		{
			usage(); 
			return false;
		}

  // Parse no more arguments ---------------------------------------------------
  if (argC_ != getopt.optind_)
		{
			usage (); 
			return false;
		}

	// Check filename -------------------------------------------------------
	if (optlist[FILENAME].opt_)
		{
			itemName_ = toUpper(optlist[FILENAME].arg_);
		}

	// Check directory --------------------------------------
	if (optlist[DIRECTORY].opt_)
		{
			itemName_ = toUpper(optlist[DIRECTORY].arg_);
		}

  return true;
  //## end aes_afp_fte::parse%E78C1E30FEED.body
}

//## Operation: execute%7B2EEF47FEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       execute()
//	---------------------------------------------------------
unsigned int aes_afp_fte::execute ()
{
  //## begin aes_afp_fte::execute%7B2EEF47FEED.body preserve=yes
  unsigned int result(0);

	result = api_.stopFile(fileDestination_, destination_, itemName_);
		
  if (result != 0)
    {
      printErrorMessage(result);
    }
  return result;
  //## end aes_afp_fte::execute%7B2EEF47FEED.body
}

//## Operation: usage%5AD518E4FEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       usage()
//	---------------------------------------------------------
void aes_afp_fte::usage ()
{
  //## begin aes_afp_fte::usage%5AD518E4FEED.body preserve=yes
  cout<<"Incorrect usage"<<endl;
  cout<<"Usage: "<<argV_[0]<<" -c file | -z directory transferqueue destinationset"<<endl; 

  exit(AES_INCORRECTCOMMAND);
  //## end aes_afp_fte::usage%5AD518E4FEED.body
}

// Additional Declarations
  //## begin aes_afp_fte%3DEF5FD401C5.declarations preserve=yes
  //## end aes_afp_fte%3DEF5FD401C5.declarations

//## begin module%3DEF5FD90295.epilog preserve=yes
//## end module%3DEF5FD90295.epilog
