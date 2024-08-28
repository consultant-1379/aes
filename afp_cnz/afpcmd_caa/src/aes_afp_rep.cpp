//  INCLUDE aes_afp_rep.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2000.
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
//	      190 89-CAA 109 0293
//
//	AUTHOR
//	      1999-04-01	UAB/I/LD	DAWN
//
//	 REVISION
//	       PA1  1999-04-01
//				A	2000-08-25	QABDAPA	Firm revision
//
//	LINKAGE
//
//	SEE ALSO

#include "aes_afp_rep.h"
#include <ACS_ExceptionHandler.h>
#include <string>
#include <iostream>

ACS_TRA_trace traceoutput = ACS_TRA_DEF("aes_afp_afprep","C400");


//------------------------------------------------------------------------------
//	AES_AFP_rep::AES_AFP_rep ()
//------------------------------------------------------------------------------

AES_AFP_rep::AES_AFP_rep (int argc, char* argv [])
	:fileName(""),
  destName("")
 
{
  argC = argc;
  argV = argv; 
}


//------------------------------------------------------------------------------
//	AES_AFP_rep::parse ()
//------------------------------------------------------------------------------

bool
AES_AFP_rep::parse ()
{
	int       c;
  option_t  option;
  optpair_t optlist [] = {0,0};
  GetOpt getopt (argC, argV, "");

  // Parse the options

	while ((c = getopt ()) != EOF)
		{
			switch (c) 
				{
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
			if (!optlist [option].opt)
				{
					optlist [option].opt = c;
					optlist [option].arg = getopt.optarg;
				}                             
			else
				{
					usage(); 
					return false;
				}	
		}

  // Parse file name -----------------------------------------------------------

  if (argC > getopt.optind)
		{
			int result(0);

			fileName = toUpper (argV [getopt.optind]);    
			getopt.optind++;
			
			result = fileName.find_first_of("-");
			if (result == -1)
				{
					usage();
					return false;
				}
		}
  else
		{
			usage (); 
			return false;
		}

  if (argC != getopt.optind)
		{
			usage (); 
			return false;
		}

  return true;
} 

//------------------------------------------------------------------------------
//	AES_AFP_rep::execute ()
//------------------------------------------------------------------------------

unsigned int
AES_AFP_rep::execute ()
{
	unsigned int result(0);

	result = api.createSubfile(fileName, destName, AES_FSUSEDEFAULT);
  
	if (result != 0)
		{
			if (result == AES_FILENOTFOUND)
				{
					OSF_DEBUG((LM_DEBUG, "[%t] AFPREP: return code %d\n", AES_SUBFILENOTFOUND));
					printErrorMessage(AES_SUBFILENOTFOUND);
				}
			else
				{
					OSF_DEBUG((LM_DEBUG, "[%t] AFPREP: return code %d\n", result));
					printErrorMessage(result);
				}
		}
	return result;
}

//------------------------------------------------------------------------------
//	AES_AFP_rep::usage ()
//------------------------------------------------------------------------------

void 
AES_AFP_rep::usage ()
{
  cout<<"Incorrect usage"<<endl;
  cout<<"Usage: "<<argV[0]<<" cpfile-subfile[-generation]"<<endl;
  cout<<endl;

	OSF_DEBUG((LM_DEBUG, "[%t] AFPREP: return code %d\n", AES_INCORRECTCOMMAND));  
  exit(AES_INCORRECTCOMMAND);
}

//------------------------------------------------------------------------------
//	main ()
//------------------------------------------------------------------------------

int
main (int argc, char *argv [])
{
	AP_InitProcess ("AES_AFP_afprep", AP_COMMAND);
	AP_SetCleanupAndCrashRoutine("AES_AFP_afprep", NULL);
  AES_AFP_rep rep (argc, argv);
  return rep.launch ();
}
