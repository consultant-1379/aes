//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FD603C7.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FD603C7.cm

//## begin module%3DEF5FD603C7.cp preserve=no
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
//## end module%3DEF5FD603C7.cp

//## Module: aes_afp_rm%3DEF5FD603C7; Package specification
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Source file: Z:\ntaes\afp\afpcmd_caa\inc\aes_afp_rm.h

#ifndef aes_afp_rm_h
#define aes_afp_rm_h 1

//## begin module%3DEF5FD603C7.additionalIncludes preserve=no
//## end module%3DEF5FD603C7.additionalIncludes

//## begin module%3DEF5FD603C7.includes preserve=yes
//## end module%3DEF5FD603C7.includes

#include <string>
#include "aes_afp_command.h"
#include "aes_afp_divide.h"
#include <aes_gcc_util.h>
#include <saAis.h>
#include <ACS_APGCC_Util.H>
//## begin module%3DEF5FD603C7.declarations preserve=no
//## end module%3DEF5FD603C7.declarations

//## begin module%3DEF5FD603C7.additionalDeclarations preserve=yes
//## end module%3DEF5FD603C7.additionalDeclarations


//## begin aes_afp_rm%3DEF5FD7006B.preface preserve=yes
//## end aes_afp_rm%3DEF5FD7006B.preface

//## Class: aes_afp_rm%3DEF5FD7006B
//## Category: afpcmd_caa(CAA 109 0417%3B023EDD00FC
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3DEF5FD700D1; { -> }
//## Uses: <unnamed>%3DEF5FD700D2; { -> }
//## Uses: <unnamed>%3DEF5FD700D6; { -> }
//## Uses: <unnamed>%3DF83CCC032E;string { -> }
//## Uses: <unnamed>%3DF83FB1004F;AES_AFP_Divide { -> }

class aes_afp_rm : public aes_afp_command  //## Inherits: <unnamed>%3DEF5FD700FB
{
  //## begin aes_afp_rm%3DEF5FD7006B.initialDeclarations preserve=yes
  //## end aes_afp_rm%3DEF5FD7006B.initialDeclarations

  private:
    //## begin aes_afp_rm::option_t%3DEF5FD700DA.preface preserve=yes
    //## end aes_afp_rm::option_t%3DEF5FD700DA.preface

    //## Class: option_t%3DEF5FD700DA; private
    //## Category: afpcmd_caa(CAA 109 0417%3B023EDD00FC
    //## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
    //## Persistence: Transient
    //## Cardinality/Multiplicity: n



    typedef enum option_t
       {
          FILENAME,
          DIRECTORY,
          DESTINATION
       } option_t;

    //## begin aes_afp_rm::option_t%3DEF5FD700DA.postscript preserve=yes
    //## end aes_afp_rm::option_t%3DEF5FD700DA.postscript

  public:

      OmHandler omHandler;
    //## Constructors (specified)
      //## Operation: aes_afp_rm%84AE61ECFEED; C++
      aes_afp_rm (int argc, char** argv);

    //## Destructor (generated)
      virtual ~aes_afp_rm();


    //## Other Operations (specified)
      //## Operation: parse%8A3F5707FEED; C++
      bool parse ();

      //## Operation: execute%5464F0FBFEED; C++
      unsigned int execute ();

      //## Operation: usage%729A15FBFEED; C++
      void usage ();

     /* SaAisErrorT  invokeAdminOperation(const std::string & i_dn,
      		unsigned int i_operationId ,
      		const SaImmAdminOperationParamsT_2 ** i_params,
      		SaTimeT i_timeout,SaAisErrorT &aOIValidationReturnValue);*/

      //void* immutil_new_attrValue(SaImmValueTypeT attrValueType, const char *str);

      unsigned int executeAdminOperation(string &errorMessage);
      void createIMMParameter(SaImmAdminOperationParamsT_2* & aParam , std::string aAttributeName,std::string aAttributeValue);
      SaAisErrorT  invokeAdminOperation(const std::string & i_dn,
										  unsigned int i_operationId ,
										  const SaImmAdminOperationParamsT_2 ** i_params,
										  SaTimeT i_timeout,SaAisErrorT &aOIValidationReturnValue);
      void* immutil_new_attrValue(SaImmValueTypeT attrValueType, const char *str);

    // Additional Public Declarations
      //## begin aes_afp_rm%3DEF5FD7006B.public preserve=yes
      //## end aes_afp_rm%3DEF5FD7006B.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_rm%3DEF5FD7006B.protected preserve=yes
      //## end aes_afp_rm%3DEF5FD7006B.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_rm%3DEF5FD7006B.private preserve=yes
      //## end aes_afp_rm%3DEF5FD7006B.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: fileDestination%3DEF5FD700E3
      //## begin aes_afp_rm::fileDestination%3DEF5FD700E3.attr preserve=no  private: std::string {V} ""
      std::string fileDestination_;
      //## end aes_afp_rm::fileDestination%3DEF5FD700E3.attr

      //## Attribute: destination%3DEF5FD700E7
      //## begin aes_afp_rm::destination%3DEF5FD700E7.attr preserve=no  private: std::string {V} ""
      std::string destination_;
      //## end aes_afp_rm::destination%3DEF5FD700E7.attr

      //## Attribute: sendItem%3DEF5FD700EB
      //## begin aes_afp_rm::sendItem%3DEF5FD700EB.attr preserve=no  private: std::string {V} ""
      std::string sendItem_;
      //## end aes_afp_rm::sendItem%3DEF5FD700EB.attr

      //## Attribute: destinationSet%3DEF5FD700EF
      //## begin aes_afp_rm::destinationSet%3DEF5FD700EF.attr preserve=no  private: bool {V} false
      bool destinationSet_;
      //## end aes_afp_rm::destinationSet%3DEF5FD700EF.attr

      //## Attribute: fileSet%3DEF5FD700F3
      //## begin aes_afp_rm::fileSet%3DEF5FD700F3.attr preserve=no  private: bool {V} false
      bool fileSet_;
      //## end aes_afp_rm::fileSet%3DEF5FD700F3.attr

      //## Attribute: dirSet%3DEF5FD700F7
      //## begin aes_afp_rm::dirSet%3DEF5FD700F7.attr preserve=no  private: bool {V} false
      bool dirSet_;
      //## end aes_afp_rm::dirSet%3DEF5FD700F7.attr
      bool srcDir_; //for APZ21230/5-758
    // Additional Implementation Declarations
      //## begin aes_afp_rm%3DEF5FD7006B.implementation preserve=yes
      //## end aes_afp_rm%3DEF5FD7006B.implementation

};

//## begin aes_afp_rm%3DEF5FD7006B.postscript preserve=yes
//## end aes_afp_rm%3DEF5FD7006B.postscript

// Class aes_afp_rm 

//## begin module%3DEF5FD603C7.epilog preserve=yes
//## end module%3DEF5FD603C7.epilog


#endif
