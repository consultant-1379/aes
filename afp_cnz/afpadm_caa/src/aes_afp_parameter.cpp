//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%37D51F4501F6.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%37D51F4501F6.cm

//## begin module%37D51F4501F6.cp preserve=no
//	INCLUDE aes_afp_parameter.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
//	  All rights reserved.
//
//	  The Copyright to the computer program(s) herein
//	  is the property of Ericsson Utvecklings AB, Sweden.
//	  The program(s) may be used and/or copied only with
//	  the written permission from Ericsson Utvecklings AB or
//	in
//	  accordance with the terms and conditions stipulated in
//	the
//	  agreement/contract under which the program(s) have
//	been
//	  supplied.
//
//	DESCRIPTION
//
//	ERROR HANDLING
//	 General rule:
//	 The error handling is specified for each method.
//	 No methods initiate or send error reports unless
//	specified.
//
//	DOCUMENT NO
//	190 89-CAA 109 0503
//
//	AUTHOR
//	 2003-05-01 DAPA
//
//	REVISION
//	 A 2003-05-01 DAPA
//	 B 2003-05-13 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%37D51F4501F6.cp

//## Module: aes_afp_parameter%37D51F4501F6; Package body
//## Subsystem: AFP::afpadm_caa::src%3AFFB41E0241
//## Source file: Z:\ntaes\afp\afpadm_caa\src\aes_afp_parameter.cpp

//## begin module%37D51F4501F6.additionalIncludes preserve=no
//## end module%37D51F4501F6.additionalIncludes

//## begin module%37D51F4501F6.includes preserve=yes
//#pragma warning(disable : 4786)
//## end module%37D51F4501F6.includes

#include <aes_afp_parameter.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"
AES_AFP_TRACE_DEFINE(AES_AFP_parameter);
//## begin module%37D51F4501F6.declarations preserve=no
#define AES_AFP_TableName "GOS/CXC13740" // AFPADMBIN
#define AES_CPF_TableName "GOS/CXC137401" // CPFAFMBIN
#define AES_AFP_RootDirectory "GOS_AFP_RootDirectory" // parameter name / env name
#define FMS_CPF_RootDirectory "FMS_CPF_RootDirectory" // ""

#ifdef WIN32
# define AES_AFP_defaultroot "K:\\GOS\\data\\AFP" // Spare if not found in PHA
# define FMS_CPF_defaultroot "K:\\FMS\\data\\CPF" // Spare if not found in PHA
# define AES_AFP_delimiter   "\\"
#else
# define AES_AFP_defaultroot "/AP/GOS/data/AFP" // Spare if not found in PHA
# define FMS_CPF_defaultroot "/AP/FMS/data/CPF" // Spare if not found in PHA
# define AES_AFP_delimiter   "/"
#endif

//## end module%37D51F4501F6.declarations

//## begin module%37D51F4501F6.additionalDeclarations preserve=yes
//## end module%37D51F4501F6.additionalDeclarations


// Class aes_afp_parameter 

//## begin aes_afp_parameter::apio%3B417FD001A4.attr preserve=no  private: static bool {U} false
bool aes_afp_parameter::apio_ = false;
//## end aes_afp_parameter::apio%3B417FD001A4.attr

//## begin aes_afp_parameter::afpDataDir%3B0919BA03BB.role preserve=no  public: static string { -> 1UHgN}
std::string aes_afp_parameter::afpDataDir_;
//## end aes_afp_parameter::afpDataDir%3B0919BA03BB.role


//## Other Operations (implementation)
//## Operation: load%37649EE403B3
//## Semantics:
//	----------------------------------------
//	          load
//	----------------------------------------
int aes_afp_parameter::load ()
{

	AES_GCC_Variable aesDataDirectory("aesapdata");

	if (aesDataDirectory.exists())
	{
		afpDataDir_ = aesDataDirectory.getStr();
		AES_AFP_TRACE_MESSAGE("AES data dir %s", afpDataDir_.c_str());
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "AFP root dir not found");
		AES_AFP_TRACE_MESSAGE("AFP root dir not found");
	}

#ifdef DIR_ON_FLOPPY
	// DEBUG
	afpDataDir = "A:\\AES\\data";
	// END DEBUG
#endif

	afpDataDir_.append(AES_AFP_delimiter);
	afpDataDir_ += "afp";

	AES_AFP_TRACE_MESSAGE("AFP data dir %s",afpDataDir_.c_str());
	// Create dirs according to 'afpDataDir'
	DIR *opndir = 0;
	opndir = opendir(afpDataDir_.c_str());
	if((opndir == NULL) && (errno ==ENOENT))
	{
		AES_AFP_TRACE_MESSAGE("No rootdir, creating");
		int resMkdir = ACE_OS::mkdir(afpDataDir_.c_str()); // See if it's the last dir thats missing.
		if (resMkdir == -1)
		{
			std::string dir = afpDataDir_;
			dir.append(AES_AFP_delimiter);
			int pos = 0;
			int lastpos = 1;
			int error = 0;
			do
			{
				pos = dir.find(AES_AFP_delimiter, lastpos);	// pos of first/next dir-delimiter
				lastpos = pos+1;
				if (pos != (int)dir.npos)
				{
					opndir = opendir(dir.substr(0, pos).c_str());
					if((opndir == NULL) && (errno==ENOENT))
					{
						resMkdir = ACE_OS::mkdir(dir.substr(0, pos).c_str(), NULL);
						if (resMkdir == -1)
						{	
							error = 1;
						}
					}
					else	// dir exists
					{
						if( opndir != 0 )
						{
							closedir(opndir);
						}
						// ok
					}
				}
			} while ((pos != (int)dir.npos) && (error == 0));					// and no errors 
			if (error != 0)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Still no rootdir, make event");
				AES_AFP_TRACE_MESSAGE("Still no rootdir, make event");
				Event::report(	EVENT_CREATEROOT,
						"ACCESS ERROR",
						"Parameter afproot",
						"Could not create missing AES_AFP_RootDirectory");

				// Problems some other dir in the path is non existent
			}
		}
	}
	else
	{
		if( opndir != 0 )
		{
			closedir(opndir);
		}
	}
	return 0;

}

//## Operation: dataDir%37649F810119
//## Semantics:
//	----------------------------------------
//	          rootDir
//	----------------------------------------
std::string aes_afp_parameter::dataDir ()
{
	//## begin aes_afp_parameter::dataDir%37649F810119.body preserve=yes
	return afpDataDir_;
	//## end aes_afp_parameter::dataDir%37649F810119.body
}

//## Operation: delimiter%3770FD3A0040
//## Semantics:
//	----------------------------------------
//	          delimiter
//	----------------------------------------
std::string aes_afp_parameter::delimiter ()
{
	//## begin aes_afp_parameter::delimiter%3770FD3A0040.body preserve=yes
	return AES_AFP_delimiter;
	//## end aes_afp_parameter::delimiter%3770FD3A0040.body
}

//## Operation: setRootDir%37649FBF017D
//## Semantics:
//	----------------------------------------
//	          setRootDir
//	----------------------------------------
int aes_afp_parameter::setRootDir (const char *str)
{
	//## begin aes_afp_parameter::setRootDir%37649FBF017D.body preserve=yes
	//if (str == "")
	if(strcmp(str,"")==0)
	{
		load();					// reload path
	}
	else
	{
		afpDataDir_ = str;	// force-set path
	}
	return 0;
	//## end aes_afp_parameter::setRootDir%37649FBF017D.body
}

//## Operation: installedApio%3B417EBA0399; C++
//## Semantics:
//	----------------------------------------
//	          installedApio()
//	----------------------------------------
bool aes_afp_parameter::installedApio ()
{
	//## begin aes_afp_parameter::installedApio%3B417EBA0399.body preserve=yes
	return false;
	//## end aes_afp_parameter::installedApio%3B417EBA0399.body
}

// Additional Declarations
//## begin aes_afp_parameter%37649D5F01FB.declarations preserve=yes
//## end aes_afp_parameter%37649D5F01FB.declarations

//## begin module%37D51F4501F6.epilog preserve=yes
//## end module%37D51F4501F6.epilog
