
/*=================================================================== */
	/**
	   @file   aes_gcc_variable_r1.h

	   @brief Header file holds the AES_GCC_VariableImplementation class declaration

	   @version 1.0.0
	 */
	/*
	   HISTORY
	   This section contains reference to problem report and related
	   software correction performed inside this module


	   PR           DATE      INITIALS    DESCRIPTION
	   -----------------------------------------------------------
	   N/A       10/08/2011   XNADNAR       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef AES_GCC_VARIABLE_R1_H
#define AES_GCC_VARIABLE_R1_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
/*=====================================================================
                          FARWARD DECLARATION SECTION
==================================================================== */
class AES_GCC_VariableImplementation;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     AES_GCC_Variable_R1

                 Class having methods to retrieve data disk path
 */
/*=================================================================== */
class AES_GCC_Variable_R1
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

	      @brief           AES_GCC_Variable_R1

	                       Default construction

	      @exception       none
	 */
	/*=================================================================== */
	AES_GCC_Variable_R1();

	/*=================================================================== */
	/**

          @brief           AES_GCC_VariableImplementation
                           Parametrical constructor



          @param           var
						   Pointer of AES_GCC_Variable_R1

          @return

          @exception       none
	 */
	/*=================================================================== */
	AES_GCC_Variable_R1(const AES_GCC_Variable_R1& var);
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
	AES_GCC_Variable_R1(std::string variableName);
	/*===================================================================
						CLASS DESTRUCTOR
=================================================================== */
	virtual ~AES_GCC_Variable_R1();

	/*===================================================================
						   PUBLIC METHODS
=================================================================== */
/*=================================================================== */
/**
	 @brief       operator=

				   = operator overloading for AES_GCC_Variable_R1 object
	@param         ev
				   reference of AES_GCC_Variable_R1
	 @return      	AES_GCC_Variable_R1 object

	 @exception   none
 */
/*=================================================================== */
	AES_GCC_Variable_R1& operator=(const AES_GCC_Variable_R1& ev);


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
	/*=====================================================================
							VARIABLE DECLARATION SECTION
	==================================================================== */
	/*=================================================================== */
	/**
	 * @brief 		implementation
	 * 				Pointer to the AES_GCC_VariableImplementation class
	 */
	/*=================================================================== */

	AES_GCC_VariableImplementation* implementation;
};

#endif

