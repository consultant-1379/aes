/*=================================================================== */
/**
	@file   aes_gcc_util.h

	@brief
	The common utilities used by services in AES are provided by
	AES_GCC_Util

	@version 1.1.1
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011     XCHEMAD       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_GCC_UTIL_H
#define AES_GCC_UTIL_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include <ace/Thread_Manager.h>
#include <string>
#include <acs_apgcc_omhandler.h>

using namespace std;
/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief Buffer size

          This variable holds the buffer size for file copy.
 */
/*=================================================================== */
#define BUFFER_SIZE (16 * 1024)
#define AES_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#define AES_GCC_FILETRANSFERM_CLASSNAME "AxeDataTransferFileTransferManager"
#define AES_GCC_BLOCKTRANSFERM_CLASSNAME "AxeDataTransferBlockTransferManager"

#if !defined (AES_GCC_STRINGIZER)
#	define AES_GCC_STRINGIZER(s) #s
#	define AES_GCC_STRINGIZE(s) AES_GCC_STRINGIZER(s)
#endif


/*Enum for dest set type, Added for AP Transmission Fault Alarm chnage*/
typedef enum
{
    DUMMY = 0,
    INIT = 1,
    RESP = 2
} Dest_Set_Type;

/*Struct for getting destset info of a Tq; used for IMM CCB rollback*/
struct TqInfo
{
    std::string destsetName;
    Dest_Set_Type dsetType;
    TqInfo(): destsetName(""), dsetType(DUMMY){}
    ~TqInfo(){}
};

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
		@brief AES_GCC_Util
	The common utilities used by services in AES are provided by
        AES_GCC_Util
 */
/*=================================================================== */

class AES_GCC_Util
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

	      @brief          default constructor
						  Creates an empty AES_GCC_Util_R1 object.
	      @return          none

	      @exception       none
	 */
	/*=================================================================== */

	AES_GCC_Util();
	/*=================================================================== */
	/**

	      @brief          destructor
						  Deletes the AES_GCC_Util_R1 object
	      @return          none

	      @exception       none
	 */
	/*=================================================================== */
	virtual ~AES_GCC_Util();

/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:

/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       Copies one file to another

      @param	   sourceFile
			Absolute path to the source file

      @param	   destinationFile
			Absolute path to the destination file

      @return      ACE_INT32
			Return value

      @exception   none
    */
/*=================================================================== */
	static ACE_INT32 copyFile(string sourceFile , string destinationFile);
	static ACE_INT32 fetchDnOfFileTransferObjFromIMM(OmHandler& omHandler);
	static ACE_INT32 fetchDnOfBlockTransferObjFromIMM(OmHandler& omHandler);
	static string dnOfFileTransferM;
	static string dnOfBlockTransferM;
        static bool getPersistentDir(string &path);
	static bool datapath_trn(std::string inVar1,std::string inVar2, std::string& outVar);
	static void extractComDnfromDn(const char* inDN, std::string& outRDN);

};

#endif
