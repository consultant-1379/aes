//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FD90000.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FD90000.cm

//## begin module%3DEF5FD90000.cp preserve=no
//	INCLUDE aes_afp_command.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
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
//	   B 2003-04-25 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3DEF5FD90000.cp

//## Module: aes_afp_command%3DEF5FD90000; Package body
//## Subsystem: AFP::afpcmd_caa::src%3DEF666D0066
//## Source file: Z:\ntaes\afp\afpcmd_caa\src\aes_afp_command.cpp

//## begin module%3DEF5FD90000.additionalIncludes preserve=no
//## end module%3DEF5FD90000.additionalIncludes

//## begin module%3DEF5FD90000.includes preserve=yes
//## end module%3DEF5FD90000.includes

#include "aes_afp_command.h"
//## begin module%3DEF5FD90000.declarations preserve=no
//## end module%3DEF5FD90000.declarations

//## begin module%3DEF5FD90000.additionalDeclarations preserve=yes
//## end module%3DEF5FD90000.additionalDeclarations


// Class aes_afp_command 

//## Operation: aes_afp_command%7822341DFEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       Constructor()
//	---------------------------------------------------------
aes_afp_command::aes_afp_command ()
  //## begin aes_afp_command::aes_afp_command%7822341DFEED.hasinit preserve=no
      : argC_(0),
        DEFAULT_REMOVEDELAY_(10),
        DEFAULT_NROFRETRIES_(0),
        DEFAULT_RETRYTIME_(10),
        DEFAULT_STARTUPSEQUENCENUMBER_(0),
        numericCharsWithMinusSign_("-0123456789")
// uabmha CNI 1135, changed default startup seq. nr. to -1 instead of 0
// to allow afpls header printout only when afdef -k was used.
  //## end aes_afp_command::aes_afp_command%7822341DFEED.hasinit
  //## begin aes_afp_command::aes_afp_command%7822341DFEED.initialization preserve=yes
  //## end aes_afp_command::aes_afp_command%7822341DFEED.initialization
{
  //## begin aes_afp_command::aes_afp_command%7822341DFEED.body preserve=yes
  //## end aes_afp_command::aes_afp_command%7822341DFEED.body
}


aes_afp_command::~aes_afp_command()
{
  //## begin aes_afp_command::~aes_afp_command%3DEF5FD3005B_dest.body preserve=yes
  //## end aes_afp_command::~aes_afp_command%3DEF5FD3005B_dest.body
}



//## Other Operations (implementation)
//## Operation: launch%8410772FFEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       launch()
//	---------------------------------------------------------
int aes_afp_command::launch ()
{
  unsigned retcode = api_.open("AES", "AFP");

  if (retcode != 0)
  {
	  cerr << api_.getErrorCodeText(retcode) << endl<<endl;
	  api_.close();
	  if (retcode == AES_NOSERVERACCESS)
		  return 117;
	  return retcode;
  }
  if (parse() == true)
  {
	  retcode = execute();
  }
  api_.close();
  return retcode;
}
//	---------------------------------------------------------
//	       printErrorMessage()
//	---------------------------------------------------------
void aes_afp_command::printErrorMessage (unsigned int err)
{
	AES_AFP_Api api;
	cout << api.getErrorCodeText(err) << endl<<endl;
	cout.flush();

	return;
}

//	---------------------------------------------------------
//	       printErrorMessage()
//	---------------------------------------------------------
void aes_afp_command::printErrorMessage (const string& errorMessage)
{
	AES_AFP_Api api;
	string strerr1 = api.getErrorCodeText(AES_IDESTSETALREADYATT);
	string strerr2 = api.getErrorCodeText(AES_RDESTSETALREADYATT);
	if(strerr1 == errorMessage || strerr2 == errorMessage)
	{
		cout<<"Destination set is already attached to the transfer queue "<<endl<<endl;
		cout.flush();
		return;
	}
	cout << errorMessage << endl<<endl;
	cout.flush();

	return;
}
//	---------------------------------------------------------
//	       checkNumericValue()
//	---------------------------------------------------------
bool aes_afp_command::checkNumericValue (std::string str)
{
	int index(0);
	index = str.find_first_not_of(numericCharsWithMinusSign_);
	if (index == -1)
		{
			return true;
		}
	return false;
}

//	---------------------------------------------------------
//	       toUpper()
//	---------------------------------------------------------
char* aes_afp_command::toUpper (char* str)
{
  int i = 0;

  while (str[i])
    {
      str[i] = toupper(str[i]);
      i++;
    }

  return str;
}

//------------------------------------------------------------
//	<< optpair_t
//------------------------------------------------------------
ostream& 
operator << (ostream& s, optpair_t option) 
{
  s.clear((std::_Ios_Iostate)(0));
  s << "/ -" << option.opt_ << " " << option.arg_ << " /";
  return s;
}
