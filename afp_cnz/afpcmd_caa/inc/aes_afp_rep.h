
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

#ifndef AES_AFP_REP_H
#define AES_AFP_REP_H

#include "aes_afp_command.h"
#include <AES_AFP_Api.h>
#include <string>

class AES_AFP_rep : public AES_AFP_command
{
public:
  AES_AFP_rep (int argc, char* argv []);

  bool parse ();

  unsigned int execute ();

  void usage ();

private:

  enum option_t 
  {
    VERBOSE
  };
 
	std::string fileName;
  std::string destName;
//	AES_AFP_Api	api;
};

#endif
