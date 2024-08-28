/*=================================================================== */
/**
   @file   aes_cdh_destination_r1.h

   @brief Header file for CDH Destination type module.

	  The services provided by AES_CDH_Destination_R1 facilitates the
	  transfer of data to remote systems.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011    XCHEMAD     Initial Release
==================================================================== */


/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_CDH_DESTINATION_R1_H 
#define AES_CDH_DESTINATION_R1_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

using namespace std;

#include <string>
#include <vector>
#include <aes_cdh_resultcode.h>
#include <aes_cdh_result.h>

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */

class AES_GCC_Stream_R1;
class AES_CDH_DestinationImplementation;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     AES_CDH_Destination_R1

                 The services provided by AES_CDH_Destination_R1 facilitates the
          	 transfer of data to remote systems.
*/
/*=================================================================== */

class AES_CDH_Destination_R1
{
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
/**
    @struct  destAttributes

    @brief   Holds the destination attributes.

    @par destName
         Holds the destination name.
    @par destSetName
         Holds the destination set name.
    @par destPath
         Holds the destination path.
    @par transferType
         Holds the type of transfer.
    @par otherAttrs
         Vector containing other attributes.
*/
/*=================================================================== */
    struct destAttributes
    {
        std::string destName;
	std::string destSetName;
	std::string destPath;
        std::string transferType;
        vector<std::string> otherAttrs;
    };

/*=================================================================== */
/**
    @struct  connAttributes

    @brief   Connection attributes returned by checkConnection()

    @par destName
         Holds the destination name.
    @par rCode
         Holds the return code.
*/
/*=================================================================== */
    struct connAttributes
    {
        std::string destName;
        AES_CDH_ResultCode rCode;
    };

/*=====================================================================
                        CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

      @brief           Destination constructor

                       Creates a dummy destination object with an
		       empty string as destination name.

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_Destination_R1();

/*=================================================================== */
   /**

      @brief           Destination constructor

                       Creates a destination object with a
                       destination name.

      @param	       destName
		       Name of the destination.

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_Destination_R1(const std::string& destName);
    
/*===================================================================
                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

      @brief           Destination destructor
		
		       Stops all tasks started by the destination object.

      @return          none

      @exception       none
   */
/*=================================================================== */
    virtual ~AES_CDH_Destination_R1();

/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       Checks whether destName has been defined using define().

      @return      Return value.
			AES_CDH_RC_OK		Destination exists
			AES_CDH_RC_NODEST	Destination does not exist
			AES_CDH_RC_NOSERVER	No contact with AES_CDH_Server

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode exists(void);
    
/*=================================================================== */
   /**
      @brief       Returns the result code from last called method.

      @return      Return value.

      @exception   none
    */
/*=================================================================== */

    AES_CDH_ResultCode getError(void);
    
/*=================================================================== */
   /**
      @brief       Returns a text that matches the result code from last called method.

      @return      errText
			Error Text.

      @exception   none
    */
/*=================================================================== */
    void getErrorText(std::string& errText);
    
/*=================================================================== */
   /**
      @brief       Returns attributes in 'attr' from one destination or all
		   destinations.

      @param	   argc
			Number of arguments
		
      @param	   argv	
			Array of arguments

      @param	   attr
			Vector of destination attributes

      @return      Return value.
			A result object whose result code can be one of the following:
			AES_CDH_RC_OK		Destination(s) listed OK
			AES_CDH_RC_INCUSAGE	An argument is not correct.
			AES_CDH_RC_NODEST	The destination does not exist.
			AES_CDH_RC_INTPROGERR	See event log for more information
			AES_CDH_RC_NOSERVER	No contact with AES_CDH_Server
			
      @exception   none
    */
/*=================================================================== */

    static AES_CDH_Result getAttr(int argc, char* argv[],
                                  std::vector<destAttributes>& attr);
    
/*===================================================================
                        PRIVATE DECLARATION SECTION
=================================================================== */
  private:

/*=====================================================================
                        CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

      @brief           Constructor
		       Private constructor to initialize the AES_CDH_Destination_R1 object.OB

      @param           dest
			AES_CDH_Destination_R1 reference

      @return          none

      @exception       none
   */
/*=================================================================== */

    AES_CDH_Destination_R1(const AES_CDH_Destination_R1& dest);

/*===================================================================
                        PRIVATE METHOD
=================================================================== */

/*=================================================================== */
   /**

      @brief           Assignment operator overloading

                       This method overloads the assignment operator.
		       Used for assigning the AES_CDH_Destination_R1 object to another

      @param           AES_CDH_Destination_R1&
                       Object reference

      @return          AES_CDH_Destination_R1&
		       Object reference
   */
/*=================================================================== */
    AES_CDH_Destination_R1& operator=(const AES_CDH_Destination_R1&);

/*===================================================================
                        PRIVATE ATTRIBUTE
=================================================================== */

/*=================================================================== */
   /**
      @brief   AES_CDH_DestinationImplementation*

               Pointer to AES_CDH_DestinationImplementation object.
   */
/*=================================================================== */
    AES_CDH_DestinationImplementation* implementation;
    
};

#endif // AES_CDH_DESTINATION_R1_H





 
