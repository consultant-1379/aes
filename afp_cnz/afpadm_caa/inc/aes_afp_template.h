//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3BE131DB020C.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3BE131DB020C.cm

//## begin module%3BE131DB020C.cp preserve=no
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
//	 2003-01-22 UAB/KB/AU DAPA
//
//	REVISION
//	 A 2003-01-22 DAPA
//	 B 2003-05-09 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3BE131DB020C.cp

//## Module: aes_afp_template%3BE131DB020C; Package specification
//## Subsystem: AFP::afpadm_caa::inc%3AFFB40F01D1
//## Source file: Z:\ntaes\afp\afpadm_caa\inc\aes_afp_template.h

#ifndef aes_afp_template_h
#define aes_afp_template_h 1

//## begin module%3BE131DB020C.additionalIncludes preserve=no
//## end module%3BE131DB020C.additionalIncludes

//## begin module%3BE131DB020C.includes preserve=yes
//## end module%3BE131DB020C.includes

#include "aes_afp_timestamp.h"
#include <fstream>
#include <vector>
#include <string>
//## begin module%3BE131DB020C.declarations preserve=no
//## end module%3BE131DB020C.declarations

//## begin module%3BE131DB020C.additionalDeclarations preserve=yes
//## end module%3BE131DB020C.additionalDeclarations


//## begin TemplateVector%3E312E5C0035.preface preserve=yes
//## end TemplateVector%3E312E5C0035.preface

//## Class: TemplateVector%3E312E5C0035; Instantiated Class
//	This class is responsible for a vector used in the
//	template class.
//## Category: afpadm_caa(CAA 109 0503%3AFFB387001D
//## Subsystem: AFP::afpadm_caa::inc%3AFFB40F01D1
//## Persistence: Transient
//## Cardinality/Multiplicity: n



typedef std::vector< std::string  > TemplateVector;

//## begin TemplateVector%3E312E5C0035.postscript preserve=yes
//## end TemplateVector%3E312E5C0035.postscript

//## begin aes_afp_template%3BE1316F027F.preface preserve=yes
//## end aes_afp_template%3BE1316F027F.preface

//## Class: aes_afp_template%3BE1316F027F
//	The class is responible for the template that is used to
//	rename files.
//## Category: afpadm_caa(CAA 109 0503%3AFFB387001D
//## Subsystem: AFP::afpadm_caa::inc%3AFFB40F01D1
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3BE15A040314;aes_afp_timestamp { -> }
//## Uses: <unnamed>%3BE630480376;basic_fstream { -> }
//## Uses: <unnamed>%3E31302D00DE;string { -> }

class aes_afp_template 
{
  //## begin aes_afp_template%3BE1316F027F.initialDeclarations preserve=yes
  //## end aes_afp_template%3BE1316F027F.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: aes_afp_template%3BE13489019A; C++
      //	Constructor
      aes_afp_template (std::string pathToPersistentFile, std::string nameTag,
		                long startupSequenceNumber);

    //## Destructor (generated)
      virtual ~aes_afp_template();


    //## Other Operations (specified)
      //## Operation: setTemplate%3BE13529026C; C++
      //	Sets the template to use and checks if its valid.
      int setTemplate (std::string templateString);

      //## Operation: renameFileLocal%3BE1359402C0; C++
      //	Renames a file or directory according to the template.
      //	The operation presumes that old file or directory exist
      //	and can be renamed.
      bool renameFileLocal (std::string oldFileName, std::string& newFileName);

      //## Operation: createFileName%3BE135E502D1; C++
      //	Creates a new file name according to the template.
      std::string createFileName (std::string oldFileName);

      //## Operation: commitPersNr%3BEBA86F002D; C++
      //	Stores the used persistent number on disc.
      void commitPersNr ();

    // Additional Public Declarations
      //## begin aes_afp_template%3BE1316F027F.public preserve=yes
      //## end aes_afp_template%3BE1316F027F.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_template%3BE1316F027F.protected preserve=yes
      //## end aes_afp_template%3BE1316F027F.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_template%3BE1316F027F.private preserve=yes
      //## end aes_afp_template%3BE1316F027F.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: transientNr%3BE2A7030345
      //	Place holder for transisten number.
      //## begin aes_afp_template::transientNr%3BE2A7030345.attr preserve=no  private: unsigned int {U} 0
	  // For HL79347 : Changed return type to unsigned long as we are increased the transient number range
      unsigned long transientNr_;
      //## end aes_afp_template::transientNr%3BE2A7030345.attr

      //## Attribute: persistentNr%3BEBAB030194
      //	Place holder for the persistent number.
      //## begin aes_afp_template::persistentNr%3BEBAB030194.attr preserve=no  private: unsigned int {U} 0
	  // For HL79347 : Changed return type to unsigned long as we are increased the persistent sequence number range
      unsigned long persistentNr_;
      //## end aes_afp_template::persistentNr%3BEBAB030194.attr

      //## Attribute: usePersNr%3BEBBE810185
      //	True if persistent number is to be used.
      //## begin aes_afp_template::usePersNr%3BEBBE810185.attr preserve=no  private: bool {U} false
      bool usePersNr_;
      //## end aes_afp_template::usePersNr%3BEBBE810185.attr

      //## Attribute: persistentFilePath%3E312FBA0219
      //## begin aes_afp_template::persistentFilePath%3E312FBA0219.attr preserve=no  private: std::string {U} pathToPersistentFile
      std::string persistentFilePath_;
      //## end aes_afp_template::persistentFilePath%3E312FBA0219.attr

      //## Attribute: newFilename%3E312FFA010D
      //## begin aes_afp_template::newFilename%3E312FFA010D.attr preserve=no  private: std::string {U} ""
      std::string newFilename_;
      //## end aes_afp_template::newFilename%3E312FFA010D.attr

      //## Attribute: nameTag%3EBB80A30170
      //	Placeholder for name tag.
      //## begin aes_afp_template::nameTag%3EBB80A30170.attr preserve=no  private: std::string& {U} nameTag
      //std::string& nameTag_;
      std::string nameTag_;
      //## end aes_afp_template::nameTag%3EBB80A30170.attr
      
	  //## Attribute: nameTagPart1
      //	Placeholder for name tag 1.
      //## begin
      std::string nameTagPart1_;
      //## end
	  
	  //## Attribute: nameTagPart2
      //	Placeholder for name tag 2.
      //## begin
      std::string nameTagPart2_;
      //## end
      //
      bool rtrFileFlag;   

	  //## Attribute: multiNameTag
      //   True if name tag contained an asterisk,
	  //   i.e. it holds two name tags separated by *.
      //## begin
      bool multiNameTag_;
      //## end


      //##  uabmha: CNI 1135, needed for afpdef -k option
      //	Placeholder for startup sequence number / rollover value.
      //##
      long startupSequenceNumber_;
      //## 

    // Data Members for Associations

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3BE280C2032F
      //## Role: aes_afp_template::templateVector%3BE280C40007
      //## begin aes_afp_template::templateVector%3BE280C40007.role preserve=no  private: TemplateVector { -> 1UHgN}
      TemplateVector templateVector_;
      //## end aes_afp_template::templateVector%3BE280C40007.role

    // Additional Implementation Declarations
      //## begin aes_afp_template%3BE1316F027F.implementation preserve=yes
      //## end aes_afp_template%3BE1316F027F.implementation
};

//## begin aes_afp_template%3BE1316F027F.postscript preserve=yes
//## end aes_afp_template%3BE1316F027F.postscript

// Class aes_afp_template 

//## begin module%3BE131DB020C.epilog preserve=yes
//## end module%3BE131DB020C.epilog


#endif
