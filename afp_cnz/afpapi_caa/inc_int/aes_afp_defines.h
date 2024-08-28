//## begin module%3DEF5FD202DB.cp preserve=no
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
//	   19089-CAA 109 0504
//
//	AUTHOR
//	   2002-12-12  UAB/UKB/AU DAPA
//
//	REVISION
//	   A 2002-12-12 DAPA
//	   B 2003-04-25 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3DEF5FD202DB.cp

//## Module: aes_afp_command%3DEF5FD202DB; Package specification
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Source file: Z:\ntaes\afp\afpcmd_caa\inc\aes_afp_command.h

#ifndef aes_afp_defines_h
#define aes_afp_defines_h 1

#include <aes_gcc_errorcodes.h>

#define TQ_CL_FILETQPARAMNAME   "AxeDataTransferAdvancedFileTransferQueueInfo"
#define TQ_RDN_CL_FILETQPARAMNAME	"advancedFileTransferQueueInfo"
#define TQ_IMPL_FILETQPARAMNAME "AdvancedFileTransferQueueInfoImplName"
#define TQ_CL_FILETQ            "AxeDataTransferFileTransferQueue"
#define TQ_CL_NAME            "fileTransferQueue"
#define TQ_IMPL_FILETQ          "FileTransferQueueImplName"
#define AES_AFP_FILETRANSFERM_CLASSNAME "AxeDataTransferFileTransferManager"
#define AES_AFP_FILETRANSFERM_RDN_CL_NAME "fileTransferManager"
#define AES_AFP_FILETRANSFERM_IMPLEMENTER "FileTransferMImplName"
#define TQ_NAME			"fileTransferQueueName"
#define TQ_DEFSTAT		"status"
#define TQ_DEFCNTDWN		"deleteCountDown"
#define TQ_FILETQPARAMRDN	"advancedFileTransferQueueInfoId"
#define TQ_FILETQRDN            "fileTransferQueueId"
#define TQ_RDN                  "fileTransferQueueId"
#define TQ_NAMETAG1		"nameTag1"
#define TQ_NAMETAG             "nametag"
#define TQ_REMOVEDELAY		"removeDelay"
#define TQ_RENAMESTATUS		"renameStatus"
#define TQ_RENAMETEMPLATE	"template"
#define TQ_SRCFOLDER		"sourceFolder"
#define TQ_SEQNO		"startupSequenceNumber"
#define TQ_DESTSET              "fileDestinationGroup"
#define TQ_DESTGROUP            "fileDestinationGroupCategoryId"
#define TQ_ENTITYNAME           "entityName"
#define TQ_RMVFSRC              "removeFromSource"
#define TQ_DESTSETNAME          "destinationSetName"
#define MAX_DN_SIZE		50
#define MAX_TQ_LEN		32
#define TQ_RETRYINTERVAL        "retryInterval"
#define TQ_NAMETAG2             "nameTag2"
#define TQ_NUMOFRETRIES         "numberOfRetryOnFailure"
#define TQ_FILETQINITDEST       "initiatingDestinationSet"
#define TQ_FILETQRESPDEST       "respondingDestinationSet"
#define TQ_FILETQINITDESTOBJ    "initDestinationSetDn"
#define TQ_FILETQRESPDESTOBJ    "respDestinationSetDn"
#define TQ_FILETQINITDESTID       "initiatingDestinationSetId"
#define TQ_FILETQRESPDESTID       "respondingDestinationSetId"
#define AES_IMM_FILE_STREAM_ROOT_CLASS_NAME "AxeDataTransferDataTransferM"
#define AES_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#define FILE_PATH		"filePath"
#define DESTSET              "fileDestinationSet"

namespace actionResult
{
	const std::string FUNCTIONBUSYSTR("Function Busy");
	const int FUNCTIONBUSYERR = AES_NOTAUTHORIZED;

	extern char ErrorCodeAttribute[];
	extern char ErrorMessageAttribute[];

	const char NBI_PREFIX[] ="@ComNbi@"; // To show message inside Com-Cli
	const int SUCCESS = 1; // SA_AIS_OK
	const int FAILED = 21; // SA_AIS_ERR_FAILED_OPERATION
	const int NOOPERATION = 28; // SA_AIS_ERR_NO_OP
}

/*
typedef enum
     {
          AFP_FSREADY = 0,
          AFP_FSDELETE = 1,
          AFP_FSNONE = 2,
          AFP_FSPENDING = 3
     } aes_afp_tqstatus;

*/

#endif
