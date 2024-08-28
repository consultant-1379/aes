//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3E2E85BF0338.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3E2E85BF0338.cm

//## begin module%3E2E85BF0338.cp preserve=no
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
//	   19089-CAA 109 0505
//
//	AUTHOR
//	   2003-01-21  UAB/UKB/AU DAPA
//
//	REVISION
//	   A 2003-01-21 DAPA
//	   B 2003-05-05 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3E2E85BF0338.cp

//## Module: aes_afp_protocol%3E2E85BF0338; Package specification
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Source file: Z:\ntaes\afp\afpapi_caa\inc\aes_afp_protocol.h

#ifndef aes_afp_protocol_h
#define aes_afp_protocol_h 1

//## begin module%3E2E85BF0338.additionalIncludes preserve=no
//## end module%3E2E85BF0338.additionalIncludes

//## begin module%3E2E85BF0338.includes preserve=yes
//## end module%3E2E85BF0338.includes

#include <fstream>
#include <utility>
#include <map>
#include <string>
#include <ace/OS.h>
#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/OS_NS_fcntl.h>
#include <ace/OS_NS_sys_sendfile.h>
#include "aes_afp_events.h"
#include <aes_gcc_errorcodes.h>
#include <aes_gcc_log.h>
#include <aes_gcc_util.h>
//## begin module%3E2E85BF0338.declarations preserve=no
//## end module%3E2E85BF0338.declarations

//## begin module%3E2E85BF0338.additionalDeclarations preserve=yes
//## end module%3E2E85BF0338.additionalDeclarations


//## begin ProtocolMap%3E2E86E20156.preface preserve=yes
//## end ProtocolMap%3E2E86E20156.preface

//## Class: ProtocolMap%3E2E86E20156; Instantiated Class
//	This class is responsible for parameters and values in
//	class protocol.
//## Category: afpapi_caa (CAA 109 0505)%3B023DA70326
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Persistence: Transient
//## Cardinality/Multiplicity: n



typedef std::map< std::string , std::string , std::less<std::string>  > ProtocolMap;
typedef std::map< std::string , bool , std::less<std::string>  > ProtocolOptionMap;	// For TR HN35007


//## begin ProtocolMap%3E2E86E20156.postscript preserve=yes
//## end ProtocolMap%3E2E86E20156.postscript

//## begin ProtocolPair%3E50AB0F01DB.preface preserve=yes
//## end ProtocolPair%3E50AB0F01DB.preface

//## Class: ProtocolPair%3E50AB0F01DB; Instantiated Class
//	This class is used for inserting a parameter / value in
//	ProtocolMap.
//## Category: afpapi_caa (CAA 109 0505)%3B023DA70326
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Persistence: Transient
//## Cardinality/Multiplicity: n



typedef std::pair< std::string , std::string  > ProtocolPair;
typedef std::pair< std::string , bool  > ProtocolOptionPair;	// For TR HN35007


//## begin ProtocolPair%3E50AB0F01DB.postscript preserve=yes
//## end ProtocolPair%3E50AB0F01DB.postscript

//## begin aes_afp_protocolbase%3E2E854402A5.preface preserve=yes
//## end aes_afp_protocolbase%3E2E854402A5.preface

//## Class: aes_afp_protocolbase%3E2E854402A5
//	Base class responsible for protocol handling in AFP.
//## Category: afpapi_caa (CAA 109 0505)%3B023DA70326
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3E2E8D5D03DC;string { -> }
//## Uses: <unnamed>%3E2E92660231;basic_fstream { -> }
//## Uses: <unnamed>%3E6C7D160199;AES_GCC_Errorcodes { -> }
//## Uses: <unnamed>%3EB758C1007A;AES_GCC_Log { -> }

class aes_afp_protocolbase 
{
  //## begin aes_afp_protocolbase%3E2E854402A5.initialDeclarations preserve=yes
  //## end aes_afp_protocolbase%3E2E854402A5.initialDeclarations

  public:
    //## Constructors (generated)
      aes_afp_protocolbase();

    //## Destructor (generated)
      virtual ~aes_afp_protocolbase();


    //## Other Operations (specified)
      //## Operation: readFromFile%3E2E8E220321; C++
      //	Reads protocol fields from a file.
      bool readFromFile (std::string& path, AES_GCC_Errorcodes& error);

      //## Operation: addStringValue%3E2E93A60028; C++
      //	Adds a value to a parameter.
      bool addStringValue (std::string parameter, std::string value);

      //## Operation: addIntValue%3E37801902D3; C++
      //	Adds a int value to protocol, int value will be
      //	converted to string.
      bool addIntValue (std::string parameter, int value);

      //## Operation: addLongValue%3E65B9960046; C++
      //	Adds a long value to a parameter. Value will be
      //	converted to a string.
      bool addLongValue (std::string parameter, unsigned long value);

      //## Operation: addBoolValue%3E37E22703E6; C++
      bool addBoolValue (std::string parameter, bool value);

      //## Operation: clearValues%3E2E93AE023D; C++
      //	Removes all values from protocol.
      bool clearValues ();

      //## Operation: getString%3E2FB3E3027B; C++
      //	Retrieves parameters and values as a string object
      bool getString (std::string& str);

      //## Operation: setString%3E2FB4E70094; C++
      //	Sets parameter and values from a string object.
      bool setString (std::string& str);

      //## Operation: getStringValue%3E2FD0E70145; C++
      //	Retrieves a parameter value.
      std::string getStringValue (std::string parameter);

      //## Operation: getIntValue%3E37943B032E; C++
      //	Retrieves value as an int.
      int getIntValue (std::string parameter);

      //## Operation: getLongValue%3E3794790157; C++
      //	Retrieves value as a long.
      long getLongValue (std::string parameter);

      //## Operation: getBoolValue%3E37BBC00033; C++
      bool getBoolValue (std::string parameter);

      //## Operation: getCharStringValue%3E37C23402A5; C++
      const char* getCharStringValue (std::string parameter);

    // Data Members for Class Attributes

      //## Attribute: PROTOCOLVERSION%3E3905740329
      //	Version of protocol.
      //## begin aes_afp_protocolbase::PROTOCOLVERSION%3E3905740329.attr preserve=no  public: std::string {U} "PROTOCOLVERSION=VERSION_2.0"
      std::string PROTOCOLVERSION_;
      //## end aes_afp_protocolbase::PROTOCOLVERSION%3E3905740329.attr

    // Additional Public Declarations
      //## begin aes_afp_protocolbase%3E2E854402A5.public preserve=yes
      //## end aes_afp_protocolbase%3E2E854402A5.public

  protected:
    // Data Members for Class Attributes

      //## Attribute: longValue%3E379824002A
      //	Long value for get operations.
      //## begin aes_afp_protocolbase::longValue%3E379824002A.attr preserve=no  protected: long {U} 0
      long longValue_;
      //## end aes_afp_protocolbase::longValue%3E379824002A.attr

      //## Attribute: intValue%3E37983000A0
      //	Integer value for get operations.
      //## begin aes_afp_protocolbase::intValue%3E37983000A0.attr preserve=no  private: int {U} 0
      int intValue_;
      //## end aes_afp_protocolbase::intValue%3E37983000A0.attr

      //## Attribute: tmpString%3E3798D701C2
      //	Empty string.
      //## begin aes_afp_protocolbase::tmpString%3E3798D701C2.attr preserve=no  private: std::string {U} ""
      std::string tmpString_;
      //## end aes_afp_protocolbase::tmpString%3E3798D701C2.attr

    // Data Members for Associations

      //## Association: afpapi_caa (CAA 109 0505)::<unnamed>%3E2E87030117
      //## Role: aes_afp_protocolbase::protocolMap%3E2E8703030C
      //## begin aes_afp_protocolbase::protocolMap%3E2E8703030C.role preserve=no  protected: ProtocolMap { -> 1UHgN}
      ProtocolMap protocolMap_;
      ProtocolOptionMap protocolOptionMap_;		// For TR HN35007
      
      //## end aes_afp_protocolbase::protocolMap%3E2E8703030C.role

      //## Association: afpapi_caa (CAA 109 0505)::<unnamed>%3E50AB4F02AF
      //## Role: aes_afp_protocolbase::protocolPair%3E50AB5002E3
      //## begin aes_afp_protocolbase::protocolPair%3E50AB5002E3.role preserve=no  protected: ProtocolPair { -> UHgN}
      ProtocolPair protocolPair_;
      ProtocolOptionPair protocolOptionPair_;	// For TR HN35007

      //## end aes_afp_protocolbase::protocolPair%3E50AB5002E3.role

    // Additional Protected Declarations
      //## begin aes_afp_protocolbase%3E2E854402A5.protected preserve=yes
      //## end aes_afp_protocolbase%3E2E854402A5.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_protocolbase%3E2E854402A5.private preserve=yes
      //## end aes_afp_protocolbase%3E2E854402A5.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin aes_afp_protocolbase%3E2E854402A5.implementation preserve=yes
      //## end aes_afp_protocolbase%3E2E854402A5.implementation

};

//## begin aes_afp_protocolbase%3E2E854402A5.postscript preserve=yes
//## end aes_afp_protocolbase%3E2E854402A5.postscript

//## begin aes_afp_protocolapi%3E50A6C603C8.preface preserve=yes
//## end aes_afp_protocolapi%3E50A6C603C8.preface

//## Class: aes_afp_protocolapi%3E50A6C603C8
//	Specialized class for api.
//## Category: afpapi_caa (CAA 109 0505)%3B023DA70326
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class aes_afp_protocolapi : public aes_afp_protocolbase  //## Inherits: <unnamed>%3E50A72103DD
{
  //## begin aes_afp_protocolapi%3E50A6C603C8.initialDeclarations preserve=yes
  //## end aes_afp_protocolapi%3E50A6C603C8.initialDeclarations

  public:
    //## Constructors (generated)
      aes_afp_protocolapi();

    //## Destructor (generated)
      virtual ~aes_afp_protocolapi();

    // Additional Public Declarations
      //## begin aes_afp_protocolapi%3E50A6C603C8.public preserve=yes
      //## end aes_afp_protocolapi%3E50A6C603C8.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_protocolapi%3E50A6C603C8.protected preserve=yes
      //## end aes_afp_protocolapi%3E50A6C603C8.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_protocolapi%3E50A6C603C8.private preserve=yes
      //## end aes_afp_protocolapi%3E50A6C603C8.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin aes_afp_protocolapi%3E50A6C603C8.implementation preserve=yes
      //## end aes_afp_protocolapi%3E50A6C603C8.implementation

};

//## begin aes_afp_protocolapi%3E50A6C603C8.postscript preserve=yes
//## end aes_afp_protocolapi%3E50A6C603C8.postscript

//## begin aes_afp_protocoltq%3E50A73F0141.preface preserve=yes
//## end aes_afp_protocoltq%3E50A73F0141.preface

//## Class: aes_afp_protocoltq%3E50A73F0141
//	Specialized class for transfer queue.
//## Category: afpapi_caa (CAA 109 0505)%3B023DA70326
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class aes_afp_protocoltq : public aes_afp_protocolbase  //## Inherits: <unnamed>%3E50A7D003D4
{
  //## begin aes_afp_protocoltq%3E50A73F0141.initialDeclarations preserve=yes
  //## end aes_afp_protocoltq%3E50A73F0141.initialDeclarations

  public:
    //## Constructors (generated)
      aes_afp_protocoltq();

    //## Destructor (generated)
      virtual ~aes_afp_protocoltq();


    //## Other Operations (specified)
      //## Operation: writeToFile%3E6C832203E4; C++
      //	Write protocol fields to a file.
      bool writeToFile (AES_GCC_Errorcodes& error);

    // Additional Public Declarations
      //## begin aes_afp_protocoltq%3E50A73F0141.public preserve=yes
      //## end aes_afp_protocoltq%3E50A73F0141.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_protocoltq%3E50A73F0141.protected preserve=yes
      //## end aes_afp_protocoltq%3E50A73F0141.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_protocoltq%3E50A73F0141.private preserve=yes
      //## end aes_afp_protocoltq%3E50A73F0141.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin aes_afp_protocoltq%3E50A73F0141.implementation preserve=yes
      //## end aes_afp_protocoltq%3E50A73F0141.implementation

};

//## begin aes_afp_protocoltq%3E50A73F0141.postscript preserve=yes
//## end aes_afp_protocoltq%3E50A73F0141.postscript

//## begin aes_afp_protocolfile%3E50A75203AB.preface preserve=yes
//## end aes_afp_protocolfile%3E50A75203AB.preface

//## Class: aes_afp_protocolfile%3E50A75203AB
//	Specialized class for file.
//## Category: afpapi_caa (CAA 109 0505)%3B023DA70326
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class aes_afp_protocolfile : public aes_afp_protocolbase  //## Inherits: <unnamed>%3E50A7D501DD
{
  //## begin aes_afp_protocolfile%3E50A75203AB.initialDeclarations preserve=yes
  //## end aes_afp_protocolfile%3E50A75203AB.initialDeclarations

  public:
    //## Constructors (generated)
      aes_afp_protocolfile();

    //## Destructor (generated)
      virtual ~aes_afp_protocolfile();


    //## Other Operations (specified)
      //## Operation: writeToFile%3E6C83370326; C++
      //	Write protocol fields to a file.
      bool writeToFile (std::string& tempPath, std::string& path, AES_GCC_Errorcodes& error);
	  bool writeToFile_new (std::string& tempPath, AES_GCC_Errorcodes& error);

    // Additional Public Declarations
      //## begin aes_afp_protocolfile%3E50A75203AB.public preserve=yes
      //## end aes_afp_protocolfile%3E50A75203AB.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_protocolfile%3E50A75203AB.protected preserve=yes
      //## end aes_afp_protocolfile%3E50A75203AB.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_protocolfile%3E50A75203AB.private preserve=yes
      //## end aes_afp_protocolfile%3E50A75203AB.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin aes_afp_protocolfile%3E50A75203AB.implementation preserve=yes
      //## end aes_afp_protocolfile%3E50A75203AB.implementation

};

//## begin aes_afp_protocolfile%3E50A75203AB.postscript preserve=yes
//## end aes_afp_protocolfile%3E50A75203AB.postscript

// Class aes_afp_protocolbase 

// Class aes_afp_protocolapi 

// Class aes_afp_protocoltq 

// Class aes_afp_protocolfile 

//## begin module%3E2E85BF0338.epilog preserve=yes
//## end module%3E2E85BF0338.epilog


#endif
