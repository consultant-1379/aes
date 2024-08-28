/**
	@file   aes_afp_ls.h

	@brief
	The service provided by this facilitates the listing of transfer ques and their details

	General rule:
	The error handling is specified for each method.
	No methods initiate or send error reports unless specified.

	@version 1.1.1
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
   N/A       08/11/2011     xbhadur       Initial Release
============================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef aes_afp_ls_h
#define aes_afp_ls_h 1

/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include "aes_afp_printoutdata.h"
#include "aes_afp_command.h"
#include "aes_afp_divide.h"
#include <utility>
#include <list>
#include <map>
#include <set>
#include <string>
//#include <strstream>
#include <iostream>
#include <sstream>
#include<iomanip>


#include <ACS_APGCC_Util.H>
#define INTERNAL_ROOT_PATH "internalRoot"
//## Uses: <unnamed>%3DEF5FD50313; { -> }
//## Uses: <unnamed>%3DEF5FD50314; { -> }
//## Uses: <unnamed>%3DEF5FD50318; { -> }
//## Uses: <unnamed>%3DEF5FD5031C;private:  { -> }
//## Uses: <unnamed>%3DEF5FD5033A;private:  { -> }
//## Uses: <unnamed>%3DF8510103B9;string { -> }
//## Uses: <unnamed>%3DF85104031D;strstream { -> }
//## Uses: <unnamed>%3DF8511E020C;std::map { -> }
//## Uses: <unnamed>%3DF8528E008F;Tqdata { -> }
//## Uses: <unnamed>%3DF852CF01E7;AES_AFP_Divide { -> }
//## Uses: <unnamed>%3DF86E840268;std::pair { -> }
//## Uses: <unnamed>%3DF86F49024D;std::multimap { -> }
//## Uses: <unnamed>%3DF8700C02E4;std::set { -> }
//## Uses: <unnamed>%3DF87F8B011C;list { -> }
//

const std::string strnotAvailable = "Not Available";
/*=====================================================================
                        CLASS DECLARATION SECTION
 ==================================================================== */
/*===================================================================*/
/**
		@brief aes_afp_ls
	This class is responsible for command afpls.
 */
/*=================================================================== */
class aes_afp_ls : public aes_afp_command
{
	/*=====================================================================
	  	  	            PRIVATE DECLARATION SECTION
	==================================================================== */

  private:

	/*=====================================================================
	                        ENUMERATED DECLARATION SECTION
	==================================================================== */
	/*=================================================================== */
	/**
	   @brief  option_t

	           Detailed description
	*/
	/*=================================================================== */

    typedef enum option_t
       {
          ATTRIBUTES,//a
          LONGLIST,//l
          ALLFILES,//s
          FIRSTFILE,//o
          LASTFILE,//y
          NOHEADER//q
       } option_t;

       /*=====================================================================
         	  	            PUBLIC DECLARATION SECTION
       ==================================================================== */
  public:
       /*=====================================================================
                              CLASS CONSTRUCTOR
       ==================================================================== */
       /*=================================================================== */
       /**

                 @brief           aes_afp_ls
                                  constructor of aes_afp_ls

				 @param           argc
								  -
				 @param           argv
								  -
                 @return          void

                 @exception       none
       */
       /*=================================================================== */
      aes_afp_ls (int argc, char** argv);

      /*===================================================================
                               CLASS DESTRUCTOR
      =================================================================== */
      /*=================================================================== */
      /**

                @brief           ~aes_afp_ls
      							Destructor of aes_afp_ls

                @return          void

                @exception       none
      */
      /*=================================================================== */
      virtual ~aes_afp_ls();

      /*=================================================================== */
       /**

       	      @brief		parse
       						Parses the command line.

       	      @return       true    to indicate that everything is OK.
   							false   to indicate that something went wrong.

       	      @exception    none
        */
       /*=================================================================== */
      bool parse ();

      /*=================================================================== */
       /**

       	      @brief		execute
       						Executes the command.

       	      @return       unsigned int

       	      @exception    none
        */
       /*=================================================================== */
      unsigned int execute ();

      /*=================================================================== */
       /**

       	      @brief		usage
       						Prints usage message.

       	      @return       void

       	      @exception    none
        */
       /*=================================================================== */
      void usage ();
  	/*=====================================================================
  	  	  	            PROTECTED DECLARATION SECTION
  	==================================================================== */
  protected:
    // Additional Protected Declarations
      //## begin aes_afp_ls%3DEF5FD502C1.protected preserve=yes
      //## end aes_afp_ls%3DEF5FD502C1.protected
  	/*=====================================================================
  	  	  	            PRIVATE DECLARATION SECTION
  	==================================================================== */
  private:
      /*=================================================================== */
       /**

              @brief       updateSourceDirectory
                           Updating the source directory path by truncating fileM path

              @return      std::string

              @exception    none
        */
       /*=================================================================== */

      std::string updateSourceDirectory (std::string & sourcePath);
	/*=================================================================== */
       /**

              @brief       getFileMPath
                           Gets the file path for internal root

              @return      bool

              @exception    none
        */
       /*=================================================================== */

	bool  getFileMPath( string & directorypath );
      /*=================================================================== */
       /**

       	      @brief		listTqOnly
       						Retrieves list with transfer queues.

       	      @return       unsigned int

       	      @exception    none
        */
       /*=================================================================== */
      unsigned int listTqOnly ();

      /*=================================================================== */
       /**

       	      @brief		printTqOnly
       						Prints transfer queues.

       	      @param        buf
    						-

       	      @return      void

       	      @exception   none
        */
       /*=================================================================== */
      void printTqOnly (std::stringstream& buf);

      /*=================================================================== */
       /**

       	      @brief		listTqOnlyLong
       						Retrieves a long list with transfer queues.

       	      @return       unsigned int

       	      @exception    none
        */
       /*=================================================================== */
      unsigned int listTqOnlyLong ();

      /*=================================================================== */
       /**

       	      @brief		printTqOnlyLong
       						Prints long list with transfer queues.

       	      @param        buf
    						-

       	      @return       void

       	      @exception    none
        */
       /*=================================================================== */
      void printTqOnlyLong (std::stringstream& buf);

      /*=================================================================== */
       /**

       	      @brief		listTqAndFiles
       						Retrieves list with tq and files.

       	      @return       unsigned int

       	      @exception    none
        */
       /*=================================================================== */
      unsigned int listTqAndFiles ();

      /*=================================================================== */
       /**

       	      @brief		printTqAndFiles
       						Prints tq and files.

       	      @param        buf
    						-

       	      @return      void

       	      @exception   none
        */
       /*=================================================================== */
      void printTqAndFiles (std::stringstream& buf);

      /*=================================================================== */
       /**

       	      @brief		listTqAndOldFile
       						Retrieves list with tq and first reported file.

       	      @return       unsigned int

       	      @exception    none
        */
       /*=================================================================== */
      unsigned int listTqAndOldFile ();

      /*=================================================================== */
       /**

       	      @brief		printTqAndOldYoungFile
       						Prints tq and last/first reported file.

       	      @param        buf
    						 -

       	      @return      void

       	      @exception   none
        */
       /*=================================================================== */
      void printTqAndOldYoungFile (std::stringstream& buf);

      /*=================================================================== */
       /**

       	      @brief		listTqAndYoungFile
       						Retrieves tq and last reported file.

       	      @return       unsigned int

       	      @exception    none
        */
       /*=================================================================== */
      unsigned int listTqAndYoungFile ();

      /*=================================================================== */
       /**

       	      @brief		listTqAndFilesLong
       						Retrieves long list with tq and files.

       	      @return       unsigned int

       	      @exception       none
        */
       /*=================================================================== */
      unsigned int listTqAndFilesLong ();

      /*=================================================================== */
       /**

       	      @brief		printTqAndFilesLong
       						Prints tq and long list of files.

       	      @param        buf
    						-

       	      @return       void

       	      @exception    none
        */
       /*=================================================================== */
      void printTqAndFilesLong (std::stringstream& buf);

      /*=================================================================== */
       /**

       	      @brief		listTqAndOldFileLong
       						Print tq and first reported.file.

       	      @return       unsigned int

       	      @exception    none
        */
       /*=================================================================== */
      unsigned int listTqAndOldFileLong ();

      /*=================================================================== */
       /**

       	      @brief		listTqAndYoungFileLong
       						Print tq and last reported file.

       	      @return       unsigned int

       	      @exception    none
        */
       /*=================================================================== */
		 unsigned int listTqAndYoungFileLong ();

      /*=================================================================== */
       /**

       	      @brief		listAttr
       						Retrieves attributes for a transfer queue.

       	      @return       unsigned int

       	      @exception    none
        */
       /*=================================================================== */
      unsigned int listAttr ();

      /*=================================================================== */
       /**

       	      @brief		printAttributes
       						Prints attributes for a transfer queue.

       	      @param        buf
    						-

       	      @return       void

       	      @exception    none
        */
       /*=================================================================== */
      void printAttributes (std::stringstream& buf);

      /*=================================================================== */
       /**

       	      @brief		stateString
       						Status string.

       	      @param        aFileState
    						-

       	      @return       std::string

       	      @exception       none
        */
       /*=================================================================== */
      std::string stateString (AES_GCC_Filestates aFileState);

  	/*=====================================================================
  	  	  	            PRIVATE DECLARATION SECTION
  	==================================================================== */
  private:

      /*=================================================================== */
         /**
          		      @brief   AFPTABLETEXT_
          		               std::string {VC} "AFP TABLE"
         */
       /*=================================================================== */
      const std::string AFPTABLETEXT_;
      //## end aes_afp_ls::AFPTABLETEXT%3DEF5FD5033D.attr

      /*=================================================================== */
         /**
          		      @brief   REMBEFORETEXT_
          		               std::string {VC} "REMOVE BEFORE"
         */
       /*=================================================================== */
      const std::string REMBEFORETEXT_;

      /*=================================================================== */
         /**
          		      @brief   RETRIESTEXT_
          		               std::string {VC} "RETRIES"
         */
       /*=================================================================== */
      const std::string RETRIESTEXT_;

      /*=================================================================== */
         /**
          		      @brief   RETRYINTERVALTEXT_
          		               std::string {VC} "RETRY INTERVAL"
         */
       /*=================================================================== */
     const std::string RETRYINTERVALTEXT_;

      /*=================================================================== */
         /**
          		      @brief   MANUALTEXT_
          		               std::string {VC} "MANUAL"
         */
       /*=================================================================== */
      const std::string MANUALTEXT_;

      /*=================================================================== */
         /**
          		      @brief   TEMPLATETEXT_
          		               std::string {VC} "TEMPLATE"
         */
       /*=================================================================== */
      const std::string TEMPLATETEXT_;

      /*=================================================================== */
         /**
          		      @brief   RENAMETEXT_
          		               std::string {VC} "RENAME"
         */
       /*=================================================================== */
      const std::string RENAMETEXT_;

      /*=================================================================== */
         /**
          		      @brief   TQTEXT_
          		               std::string {VC} "TRANSFER QUEUE"
         */
       /*=================================================================== */
      const std::string TQTEXT_;

      /*=================================================================== */
         /**
          		      @brief   DESTTEXT_
          		               std::string {VC} "DESTINATION SET"
         */
       /*=================================================================== */
      const std::string DESTTEXT_;

      /*=================================================================== */
         /**
          		      @brief   SOURCEDIRTEXT_
          		               std::string {VC} "SOURCE DIRECTORY"
         */
       /*=================================================================== */
      const std::string SOURCEDIRTEXT_;

      /*=================================================================== */
         /**
          		      @brief   REMDELAYTEXT_
          		               std::string {VC} "REMOVE DELAY"
         */
       /*=================================================================== */
      const std::string REMDELAYTEXT_;

      /*=================================================================== */
         /**
          		      @brief   REMTIMERTEXT_
          		               std::string {VC} "REMOVE TIMER"
         */
       /*=================================================================== */
      const std::string REMTIMERTEXT_;

      /*=================================================================== */
         /**
          		      @brief   DEFSTATUSTEXT_
          		               std::string {VC} "DEFAULT STATUS"
         */
       /*=================================================================== */
       const std::string DEFSTATUSTEXT_;

      /*=================================================================== */
         /**
          		      @brief   STATUSTEXT_
          		               std::string {VC} "STATUS"
         */
       /*=================================================================== */
         const std::string STATUSTEXT_;

      /*=================================================================== */
         /**
          		      @brief   FILEDIRTEXT_
          		               std::string {VC} "FILE OR DIRECTORY"
         */
       /*=================================================================== */
      const std::string FILEDIRTEXT_;

      /*=================================================================== */
         /**
          		      @brief   DIRTEXT_
          		               std::string {VC} "DIR"
         */
       /*=================================================================== */
      const std::string DIRTEXT_;

      /*=================================================================== */
         /**
          		      @brief   NOTEXT_
          		               std::string {VC} "NO"
         */
       /*=================================================================== */
      const std::string NOTEXT_;

      /*=================================================================== */
         /**
          		      @brief   YESTEXT_
          		               std::string {VC} "YES"
         */
       /*=================================================================== */
       const std::string YESTEXT_;

      /*=================================================================== */
         /**
          		      @brief   USERGROUPTEXT_
          		               std::string {U} "USER GROUP"
         */
       /*=================================================================== */
      std::string USERGROUPTEXT_;

      /*=================================================================== */
         /**
          		      @brief   NAMETAGTEXT_
          		               std::string {U} "NAME TAG"
         */
       /*=================================================================== */
     std::string NAMETAGTEXT_;

      /*=================================================================== */
         /**
          		      @brief   STARTUPSEQUENCENUMBERTEXT_
          		               adding text for startup sequence number
         */
       /*=================================================================== */
      std::string STARTUPSEQUENCENUMBERTEXT_;
      /*=================================================================== */
         /**
          		      @brief   PADDING34_
          		               34 blank spaces
         */
       /*=================================================================== */
      const std::string PADDING34_;

      /*=================================================================== */
         /**
          		      @brief   PADDING32_
          		               32 blank spaces
         */
       /*=================================================================== */
      const std::string PADDING32_;

      /*=================================================================== */
         /**
          		      @brief   PADDING7_
          		               7 blank spaces
         */
       /*=================================================================== */
      const std::string PADDING7_;

      /*=================================================================== */
         /**
          		      @brief   PADDING2_
          		               2 blank spaces
         */
       /*=================================================================== */
      const std::string PADDING2_;

      /*=================================================================== */
         /**
          		      @brief   PADDING3_
          		               3 blank spaces
         */
       /*=================================================================== */
      const std::string PADDING3_;

      /*=================================================================== */
         /**
          		      @brief   attributes_
          		               True if attributes.
         */
       /*=================================================================== */
      bool attributes_;

      /*=================================================================== */
         /**
          		      @brief   longList_
          		               True if long list.
         */
       /*=================================================================== */
      bool longList_;
      /*=================================================================== */
         /**
          		      @brief   allFiles_
          		               True if all files.
         */
       /*=================================================================== */
      bool allFiles_;

      /*=================================================================== */
         /**
          		      @brief   firstFile_
          		               True if first reported file.
         */
       /*=================================================================== */
      bool firstFile_;

      /*=================================================================== */
         /**
          		      @brief   lastFile_
          		               True if last reported file.
         */
       /*=================================================================== */
      bool lastFile_;

      /*=================================================================== */
         /**
          		      @brief   header_
          		               True if header is printed.
         */
       /*=================================================================== */
      bool header_;

      /*=================================================================== */
         /**
          		      @brief   transferQueue_
          		               Name of transfer queue.
         */
       /*=================================================================== */
      std::string transferQueue_;

      /*=================================================================== */
         /**
          		      @brief   destination_
          		               Name of destination set.
         */
       /*=================================================================== */
       std::string destination_;
};

#endif
