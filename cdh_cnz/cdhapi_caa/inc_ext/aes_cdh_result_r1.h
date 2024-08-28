/*=================================================================== */
/**
   @file  aes_cdh_result_r1.h

   @brief Header file for CDH Result type module.

          The services provided by AES_CDH_Result_R1 facilitates the
          transfer of data to remote systems.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011    XCHEMAD    Initial Release
==================================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_CDH_RESULT_R1_H 
#define AES_CDH_RESULT_R1_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <iostream>
#include <aes_cdh_resultcode.h>

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class AES_CDH_ResultImplementation;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     AES_CDH_Result_R1

                 The services provided by AES_CDH_Result_R1 facilitates the
                 transfer of data to remote systems.
*/
/*=================================================================== */
class AES_CDH_Result_R1
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

      @brief           Result constructor

                       Creates a dummy result object containing an uninitialized
		       result code.

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_Result_R1();
    
/*=================================================================== */
   /**

      @brief           Result constructor

                       Creates a  result object.

      @param	       code
			Result code
	
      @param 	       detail
			Detailed information text

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_Result_R1(const int code, const std::string detail = "");
    
/*===================================================================
                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

      @brief           Result destructor

      @return          none

      @exception       none
   */
/*=================================================================== */
    virtual ~AES_CDH_Result_R1();
    
/*=================================================================== */
   /**

      @brief           Constructor
                       Constructor to initialize the AES_CDH_Destination_R1 object

      @param           res
			AES_CDH_Result_R1 reference

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_Result_R1(const AES_CDH_Result_R1& res);
    
/*=================================================================== */
   /**

      @brief           Assignment operator overloading

                       This method overloads the assignment operator.
                       Used for assigning the AES_CDH_Result_R1 object to another

      @param           res
                       AES_CDH_Result_R1 object reference

      @return          AES_CDH_Result_R1 object reference
   */
/*=================================================================== */
    AES_CDH_Result_R1& operator=(const AES_CDH_Result_R1& res);

    
/*=================================================================== */
   /**
      @brief           Returns the result code.

      @return          Return value
   */
/*=================================================================== */
    AES_CDH_ResultCode code(void);
    
/*=================================================================== */
   /**
      @brief           Returns a text that matches the result code.

      @return          Error text.
   */
/*=================================================================== */
    std::string errorText(void);
    
/*=================================================================== */
   /**
      @brief           Returns the detailed information text.

      @return          Detailed information text
   */
/*=================================================================== */
    std::string detailInfo(void);

/*===================================================================
                        PRIVATE DECLARATION SECTION
=================================================================== */
private:

/*===================================================================
                        PRIVATE METHOD
=================================================================== */

/*=================================================================== */
   /**

      @brief           (<<) operator overloading

                       This method overloads the (<<)operator.

      @param           s
			std::ostream&

      @param	       res
			AES_CDH_Result_R1 object reference

      @return          std::ostream&
   */
/*=================================================================== */
    friend std::ostream& operator<<(std::ostream& s, const AES_CDH_Result_R1& res);

/*===================================================================
                        PRIVATE ATTRIBUTE
=================================================================== */

/*=================================================================== */
   /**
      @brief   AES_CDH_ResultImplementation*

               Pointer to AES_CDH_ResultImplementation object.
   */
/*=================================================================== */
    AES_CDH_ResultImplementation* implementation;

};

std::ostream& operator<<(std::ostream& s, const AES_CDH_Result_R1& res);

#endif // AES_CDH_RESULT_R1_H
