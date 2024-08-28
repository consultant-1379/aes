
/*=================================================================== */
/**
   @file   aes_gcc_variableimplementation.h

   @brief Header file holds the AES_GCC_VariableImplementation class declaration

   @version 1.0.0
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       10/08/2011     NS       xnadnar
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_GCC_VARIABLEIMPLEMENTATION_R1_H
#define AES_GCC_VARIABLEIMPLEMENTATION_R1_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include <string>
using namespace std;

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
const ACE_TCHAR aesDataDirectory[] = "aesapdata";  // AES data directory
const ACE_TCHAR aesLogsDirectory[] = "aesaplogs";  // AES logs directory

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     AES_GCC_VariableImplementation

                 Class having methods to retrieve data disk path
 */
/*=================================================================== */
class AES_GCC_VariableImplementation
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

	      @brief           AES_GCC_VariableImplementation

	                       Default construction

	      @exception       none
	 */
	/*=================================================================== */

	AES_GCC_VariableImplementation();
	/*=================================================================== */
	/**

          @brief           AES_GCC_VariableImplementation
                           Parametrical constructor



          @param           variableName
                           variable name is string type data,
                           having value "aesapdata" or "aesaplogs"
                           aesapdata : if the path is to be obtain for aes data path.
                           aesaplogs : if the path is to be obtain for aes logs path.

          @return

          @exception       none
	 */
	/*=================================================================== */


	AES_GCC_VariableImplementation(std::string variableName);
	/*===================================================================
						CLASS DESTRUCTOR
=================================================================== */
	virtual ~AES_GCC_VariableImplementation();

	/*===================================================================
						   PUBLIC METHODS
=================================================================== */
	/*=================================================================== */
	/**
	 @brief       exists

				   Method used to check whether aes data or aes logs path exist.

	 @return      true if path exists
				   false if path not exists

	 @exception   none
	 */
	/*=================================================================== */

	bool exists(void);
	/*=================================================================== */
	/**
	 @brief       getStr

				   Method used to retrieve the path of aes data or aes logs.

	 @return       path of the aes data or logs

	 @exception   none
	 */
	/*=================================================================== */
	std::string getStr(void);
/*=====================================================================
						PRIVATE DECLARATION SECTION
==================================================================== */
private:
	std::string variable;

	std::string aesDirPath;
	/*=================================================================== */
	/**
	 @brief       updateStrVar

				   Method used to retrieve the path of aes data or aes logs.
	@param          inVar
					string "aesapdata" or "aesaplogs"

	@param          outVar
					string to hold


	 @return       true if data path exists
				   false if data path not exists

	 @exception   none
	 */
	/*=================================================================== */
	bool updateStrVar(std::string inVar, std::string& outVar);
};

#endif

