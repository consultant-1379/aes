/*================================================================== */
/**
   @file   aes_cdh_destinationimplementation.h

   @brief Header file for AES_CDH_DestinationImplementation type module.

	  Implementation of AES_CDH_Destination

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011   XCHEMAD    Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_CDH_DESTINATIONIMPLEMENTATION_H 
#define AES_CDH_DESTINATIONIMPLEMENTATION_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/Synch.h>
#include <ace/Thread_Manager.h>
#include <aes_cdh_destination.h>
#include <string>
#include <vector>
#include <aes_gcc_stream.h>
#include <aes_cdh_result.h>
#include <aes_cdh_asynchreceiver.h> 
using namespace std;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     AES_CDH_DestinationImplementation

                 This class involved implementation of AES_CDH_Destination 
*/
/*=================================================================== */
class AES_CDH_DestinationImplementation
{
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                        CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

      @brief           AES_CDH_DestinationImplementation

                       Class constructor

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_DestinationImplementation();
    
/*=================================================================== */
   /**

      @brief           AES_CDH_DestinationImplementation

                       Class constructor with destination name

      @param	       destName
			Destination name

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_DestinationImplementation(const string destName);
    
/*===================================================================
                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

      @brief           Class destructor

      @return          none

      @exception       none
   */
/*=================================================================== */
    virtual ~AES_CDH_DestinationImplementation();

/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       This method checks if the destination exists

      @return      Result code

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode exists(void);

/*=================================================================== */
   /**
      @brief       This method gets the error code

      @return      Result code

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode getError(void);

/*=================================================================== */
   /**
      @brief       This method checks if the destination exists

      @param	   errText
			Gets the error text

      @return      none

      @exception   none
    */
/*=================================================================== */
    void getErrorText(string& errText);
    
/*=================================================================== */
   /**
      @brief       This method gets all the attributes of the destination

      @param	   argc
			Number of attributes
	
      @param	   argv
			List of attributes

      @param	   attr
			Vector containing the list of attributes

      @return      Result code

      @exception   none
    */
/*=================================================================== */
    static AES_CDH_Result getAttr(int argc, char* argv[],
                vector<AES_CDH_Destination::destAttributes>& attr);    
	
/*===================================================================
                        PRIVATE DECLARATION SECTION
=================================================================== */
private:
/*===================================================================
                        PRIVATE ATTRIBUTE
=================================================================== */

/*=================================================================== */
   /**
      @brief   destinationName

               Name of the destination
   */
/*=================================================================== */
    string destinationName;

/*=================================================================== */
   /**
      @brief   lastErrorCode

               last error code object
   */
/*=================================================================== */
    ACE_Atomic_Op<ACE_Thread_Mutex,int> lastErrorCode;

/*=================================================================== */
   /**
      @brief   threadManager

               Thead manager object
   */
/*=================================================================== */
    ACE_Thread_Manager* threadManager;

/*=================================================================== */
   /**
      @brief   asynchReceiver

               AES_CDH_AsynchReceiver object
   */
/*=================================================================== */
    AES_CDH_AsynchReceiver* asynchReceiver;     

/*=================================================================== */
   /**
      @brief   closeDone

               Flag to check if close is done
   */
/*=================================================================== */
    bool closeDone;                            

/*=================================================================== */
   /**
      @brief   sessionIsOpen

               Flag to check if session is open
   */
/*=================================================================== */
    bool sessionIsOpen;                      

/*=================================================================== */
   /**
      @brief   myDest

               AES_CDH_Destination object
   */
/*=================================================================== */
//    AES_CDH_Destination* myDest;             

/*===================================================================
                        PRIVATE METHOD
=================================================================== */
    
/*=================================================================== */
   /**
      @brief       This method gets the attributes of destination(s) configured in GOH

      @param       destName
			Name of the destination for which listing is required.

      @param	   attr
			Output vector which fetched the attributes of the destination(s).
                        
      @return      Return value

      @exception   none
    */
/*=================================================================== */
    static int list(string destname,
                vector<AES_CDH_Destination::destAttributes>& attr);
    
/*=================================================================== */
   /**
      @brief       This method parses and gets the attributes of destination(s)
		   configured in GOH

      @param	   argc
			Number of attributes

      @param	   argv
			Attribute list

      @param       attr
                        Output vector which fetched the attributes of the destination(s).

      @return      Return value

      @exception   none
    */
/*=================================================================== */
    static int parse_cdhls(int argc, char* argv[],
                vector<AES_CDH_Destination::destAttributes>& attr);

    friend class AES_CDH_AsynchReceiver;

};

#endif // AES_CDH_DESTINATIONIMPLEMENTATION_H
