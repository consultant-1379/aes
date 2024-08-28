/*=================================================================== */
/**
   @file   aes_cdh_resultimplementation.h

   @brief Header file for AES_CDH_ResultImplementation type module.

          The services provided by AES_CDH_ResultImplementation facilitates the
          transfer of data to remote systems.

	  Provides CDH Results.

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
#ifndef AES_CDH_RESULTIMPLEMENTATION_H 
#define AES_CDH_RESULTIMPLEMENTATION_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <iostream>
#include <aes_cdh_resultcode.h>

using namespace std;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     AES_CDH_ResultImplementation

                 Provides CDH Results
*/
/*=================================================================== */
class AES_CDH_ResultImplementation
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

      @brief           Result implementation constructor

                       Creates a dummy result object

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_ResultImplementation();

/*=================================================================== */
   /**

      @brief           Result implementation constructor

                       Creates a result object with a result code

      @param	       code
			Result code

      @param	       detail
			Detailed information

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_ResultImplementation(const int code, const string detail);
    
/*=================================================================== */
   /**

      @brief           Result implementation constructor

                       Creates a result object with Result implementation object

      @param           res
                        AES_CDH_ResultImplementation reference

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_ResultImplementation(const AES_CDH_ResultImplementation& res);
    AES_CDH_ResultImplementation& operator=(const AES_CDH_ResultImplementation& res);
/*===================================================================
 
                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

      @brief           Result implementation destructor

                       Destroy the result implementation object

      @return          none

      @exception       none
   */
/*=================================================================== */
    virtual ~AES_CDH_ResultImplementation();
    
/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       Returns the code result.

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode code(void) const;

/*=================================================================== */
   /**
      @brief       Returns the error text.

      @return      Error text

      @exception   none
    */
/*=================================================================== */
    string errorText(void) const;

/*=================================================================== */
   /**
      @brief       Returns the detailed information

      @return      Detailed information

      @exception   none
    */
/*=================================================================== */
    string detailInfo(void) const;

/*===================================================================
                        PRIVATE DECLARATION SECTION
=================================================================== */
private:
/*===================================================================
                        PRIVATE ATTRIBUTE
=================================================================== */

/*=================================================================== */
   /**
      @brief   resultCode

               AES_CDH_ResultCode object
   */
/*=================================================================== */
    AES_CDH_ResultCode resultCode;

/*=================================================================== */
   /**
      @brief   detailInfoText

               Detailed information text.
   */
/*=================================================================== */
    string detailInfoText;
    
/*===================================================================
                        PRIVATE METHOD
=================================================================== */

/*=================================================================== */
   /**

      @brief           Returns the result code for the error code

      @param           errcode
                       Error code

      @return          Result code
   */
/*=================================================================== */
    AES_CDH_ResultCode getResultCode(int errcode);
};

ostream& operator<<(ostream& s, const AES_CDH_ResultImplementation& res);

#endif // AES_CDH_RESULTIMPLEMENTATION_H
