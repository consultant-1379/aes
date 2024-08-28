/*=================================================================== */
/**
   @file   aes_cdh_resultimplementation.cpp

   Class method implementation for AES_CDH_ResultImplementation type module.

   This module contains the implementation of class declared in
   the aes_cdh_resultimplementation.h module

   Provides CDH results

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011   XCHEMAD     Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <sstream>
#include <aes_cdh_resultimplementation.h>

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: AES_CDH_ResultImplementation
=================================================================== */
AES_CDH_ResultImplementation::AES_CDH_ResultImplementation() 
{
    resultCode = AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE: AES_CDH_ResultImplementation
=================================================================== */
AES_CDH_ResultImplementation::AES_CDH_ResultImplementation(const int code,
                                                           const string info)
    :detailInfoText(info)
{
    resultCode = getResultCode(code);
}

/*===================================================================
   ROUTINE: ~AES_CDH_ResultImplementation
=================================================================== */
AES_CDH_ResultImplementation::~AES_CDH_ResultImplementation() 
{}

/*===================================================================
   ROUTINE: AES_CDH_ResultImplementation
=================================================================== */
AES_CDH_ResultImplementation::AES_CDH_ResultImplementation(const AES_CDH_ResultImplementation& res)
    :resultCode(res.resultCode), detailInfoText(res.detailInfoText)
{}

AES_CDH_ResultImplementation& AES_CDH_ResultImplementation::operator=(const AES_CDH_ResultImplementation& res)
{
	resultCode = res.resultCode;
	detailInfoText = res.detailInfoText;
	return *this;

}

/*===================================================================
   ROUTINE: code
=================================================================== */
AES_CDH_ResultCode AES_CDH_ResultImplementation::code(void) const
{
    return resultCode;
}

/*===================================================================
   ROUTINE: errorText
=================================================================== */
string AES_CDH_ResultImplementation::errorText(void) const
{
   string attr;
   
   switch(resultCode)
   {
   case AES_CDH_RC_CMDAUTHERR:
      attr = "User not authorized to perform specified action";
      break;
      
   case AES_CDH_RC_CONNECTERR:
      attr = "Error connecting to remote system";
      break;
      
   case AES_CDH_RC_CONNECTOK:
      attr = "Ok connecting to remote system";
      break;
      
   case AES_CDH_RC_CONNECTWARN:
      attr = "Error connecting to remote system, no data lost";
      break;
     
   case AES_CDH_RC_ILLEGALREMOTEFOLDERPATH:
       attr = "Unreasonable value for remoteFolderPath";
       break;
       
   case AES_CDH_RC_DATAAREAERR:
      attr = "Data area error";
      break;
      
   case AES_CDH_RC_DESTDEF:
      attr = "Destination already defined";
      break;
      
   case AES_CDH_RC_DESTISACTORPRIM:
      attr = "Destination is primary or active";
      break;
      
   case AES_CDH_RC_DESTNOTINDSET:
      attr = "Destination is not in the destination set";
      break;
      
   case AES_CDH_RC_DESTSETDEF:
      attr = "Destination set already defined";
      break;
      
   case AES_CDH_RC_DESTUSEDINSET:
      attr = "Destination already used in a destination set";
      break;
      
   case AES_CDH_RC_ENDOFSTREAM:
      attr = "End of stream";
      break;
      
   case AES_CDH_RC_ERROR:
      attr = "Error when executing";
      break;
      
   case AES_CDH_RC_EVENTHANDLERREM:
      attr = "Event Handler is removed";
      break;
      
   case AES_CDH_RC_EVENTHANDLERREMERR:
      attr = "Error when removing Event Handler";
      break;
      
   case AES_CDH_RC_EVENTHANDLERSET    :
      attr = "Event Handler is set";
      break;
      
   case AES_CDH_RC_EVENTHANDLERSETERR:
      attr = "Error when setting Event Handler";
      break;
      
   case AES_CDH_RC_FILEALREADYSENT:
      attr = "File is already sent";
      break;
      
   case AES_CDH_RC_FILEERR:
      attr = "Error reading file";
      break;
      
   case AES_CDH_RC_FILENAMETOOLONG:
      attr = "Remote file name too long";
      break;
      
   case AES_CDH_RC_ILLEGALCOMB:
      attr = "Illegal combination of destinations";
      break;
      
   case AES_CDH_RC_INCUSAGE:
      attr = "Incorrect usage";
      break;
      
   case AES_CDH_RC_INTPROGERR:
      attr = "Internal Program Error";
      break;
      
   case AES_CDH_RC_LOGONFAILURE:
      attr = "Logon failure at remote system";
      break;
      
   case AES_CDH_RC_NOALIAS:
      attr = "Virtual directory does not exist";
      break;
      
   case AES_CDH_RC_NOVIRTUALPATH:
      attr = "Physical path for virtual directory does not exist";
      break;
      
   case AES_CDH_RC_NOTBLOCKDEST:
      attr = "The destination exists but may not be used for block transfer";
      break;
      
   case AES_CDH_RC_NOTBLOCKDESTSET:
      attr = "The destination set exists but may not be used for block transfer";
      break;
      
   case AES_CDH_RC_NOBLOCKNOAVAILABLE:
      attr = "No block number is available";
      break;
      
   case AES_CDH_RC_NODEST:
      attr = "Destination does not exist";
      break;
      
   case AES_CDH_RC_NODESTSET:
      attr = "Destination set does not exist";
      break;
      
   case AES_CDH_RC_NOTFILEDEST:
      attr = "The destination exists but may not be used for file transfer";
      break;
      
   case AES_CDH_RC_NOTFILEDESTSET:
      attr = "The destination set exists but may not be used for file transfer";
      break;
      
   case AES_CDH_RC_NOTINSYNCH:
      attr = "Not in synch with the rpc server according to the block numbers";
      break;
      
   case AES_CDH_RC_NOTNOTIFDEST:
      attr = "The destination is not defined with file notification";
      break;
      
   case AES_CDH_RC_NOSERVER:
      attr = "Unable to connect to server";
      break;
      
   case AES_CDH_RC_NOTASK:
      attr = "Task does not exist";
      break; 
      
   case AES_CDH_RC_NOTRANS:
      attr = "Transfer type does not exist";
      break;
      
   case AES_CDH_RC_NOTRANSACTIONPERFORMED:
      attr = "No transaction has been performed";
      break;
      
   case AES_CDH_RC_NOTRESPDEST:
      attr = "Destination is not of responding type";
      break;
      
   case AES_CDH_RC_OK:
      attr = "The command was executed successfully";
      break;
      
   case AES_CDH_RC_PHYSFILEERR:
      attr = "Physical file error";
      break;
      
   case AES_CDH_RC_PROTECTEDDEST:
      attr = "Destination is part of a destination set";
      break;
      
   case AES_CDH_RC_PROTECTEDDESTSET:
      attr = "Destination set is protected. ";
      break;
      
   case AES_CDH_RC_SENDERR:
      attr = "Error sending to remote system";
      break;
      
   case AES_CDH_RC_SENDWARN:
      attr = "Error sending to remote system, no data lost";
      break;
      
   case AES_CDH_RC_STREAMERROR:
      attr = "Stream error";
      break;
      
   case AES_CDH_RC_TASKEXISTS:
      attr = "Task already exists";
      break;
      
   case AES_CDH_RC_TASKSTOPPED:
      attr = "Task stopped";
      break;

   case AES_CDH_RC_TIMEOUT:
      attr = "Timeout";
      break;

   case AES_CDH_RC_TRANSACTIONACTIVE:
      attr = "A transaction is already active";
      break;

   case AES_CDH_RC_TRANSACTIONNOTACTIVE:
      attr = "No transaction is active";
      break;

   case AES_CDH_RC_UNREAS:
      attr = "Unreasonable value"; 
      break;

   case AES_CDH_RC_UNREAS_BLOCKIP:
   attr = "Unreasonable value for ipAddress";
   break;
   
   case AES_CDH_RC_UNREAS_BLOCKRETRYDELAY:
   attr = "Unreasonable value for retryDelay";
   break;

   case AES_CDH_RC_UNREAS_BLOCKRETRIES:
   attr = "Unreasonable value for sendRetry";
   break;

   case AES_CDH_RC_UNREAS_PRIBLOCKDESTNAME:
   attr = "Unreasonable value for primaryBlockDestinationId";
   break;

   case AES_CDH_RC_UNREAS_SECBLOCKDESTNAME:
   attr = "Unreasonable value for secondaryBlockDestinationId";
   break;

   case AES_CDH_RC_UNREAS_ADVBLOCKDESTNAME:
   attr = "Unreasonable value for advancedBlockDestInfoId";
   break;

   case AES_CDH_RC_UNREAS_BLOCKDESTSETNAME:
   attr = "Unreasonable value for blockDestinationSetId";
   break;

   case AES_CDH_RC_UNREAS_INITIP:
   attr = "Unreasonable value for ipAddress";
   break;

   case AES_CDH_RC_UNREAS_INITPORTNO:
   attr = "Unreasonable value for portNumber";
   break;

   case AES_CDH_RC_UNREAS_RESPIP:
   attr = "Unreasonable value for ipNotificationAddress";
   break;

   case AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV4MAPPEDIPV6:
	   attr = "IPv4-mapped IPv6 address is not allowed for ipAddress";
	   break;

   case AES_CDH_RC_UNREAS_RESPDEST_IPV4MAPPEDIPV6:
   	   attr = "IPv4-mapped IPv6 address is not allowed for ipNotificationAddress";
   	   break;

   case AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV6_NATIVE:
   	   attr = "IPv6 address is not allowed in NATIVE environment for ipAddress";
   	   break;

   case AES_CDH_RC_UNREAS_RESPDEST_IPV6_NATIVE:
   	   attr = "IPv6 address is not allowed in NATIVE environment for ipNotificationAddress";
   	   break;

   case AES_CDH_RC_UNREAS_RESPPORTNO:
   attr = "Unreasonable value for portNotificationNumber";
   break;

   case AES_CDH_RC_UNREAS_FILETRANSFERTYPE:
   attr = "Unreasonable value for transferProtocol";
   break;

   case AES_CDH_RC_UNREAS_PRIINITDESTNAME:
   attr = "Unreasonable value for primaryInitiatingDestinationId";
   break;

   case AES_CDH_RC_UNREAS_SECINITDESTNAME:
   attr = "Unreasonable value for secondaryInitiatingDestinationId";
   break;

   case AES_CDH_RC_UNREAS_PRIRESPDESTNAME:
   attr = "Unreasonable value for primaryRespondingDestinationId";
   break;

   case AES_CDH_RC_UNREAS_SECRESPDESTNAME:
   attr = "Unreasonable value for secondaryRespondingDestinationId";
   break;
   

   case AES_CDH_RC_UNREAS_FILERETRYDELAY:
   attr = "Unreasonable value for retryDelay";
   break;

   case AES_CDH_RC_UNREAS_FILESENDRETRY:
   attr = "Unreasonable value for sendRetry";
   break;

   case AES_CDH_RC_UNREAS_INITDESTSETNAME:
   attr = "Unreasonable value for initiatingDestinationSetId";
   break;

   case AES_CDH_RC_UNREAS_RESPDESTSETNAME:
   attr = "Unreasonable value for respondingDestinationSetId";
   break;

   case AES_CDH_RC_UNREAS_FILEOVERWRITE:
   attr = "Unreasonable value for fileOverwrite";
   break;
   
   case AES_CDH_RC_UNREAS_SUPPRESSSUBFOLDRCREATION:
   attr = "Unreasonable value for suppressSubFolderCreation";
   break;

   case AES_CDH_RC_UNREAS_INHIBITTQNAMEPRFX:
   attr = "Unreasonable value for inhibitTransferQueueNamePrefx";
   break;

   case AES_CDH_RC_USRGNOTMEMBER:
      attr = "User not member of specified user group "; 
      break;

   case AES_CDH_RC_DESTWITHSAMEOPTEXIST:
      attr = "A destination with the same destination-unique options already exist";
      break;

   case AES_CDH_RC_KEEPOPTNOTDEFINED:
      attr = "Keep option not defined";
      break;

   case AES_CDH_RC_SECDESTEXIST:
         attr = "Secondary destination already defined";
         break;

   case AES_CDH_RC_NOPRIDEST:
         attr = "Primary Destination not defined";
         break;
   case AES_CDH_RC_NOADVPARAM:
         attr = "Adavanced destination parameters object not defined";
         break;

    case AES_CDH_RC_BACKUP_INPROGRESS:
         attr = "Command not executed, AP backup in progress";
         break;

   case AES_CDH_FILEDESTSETLIMIT:
	 attr = "Number of file destination sets crossed the maximum limit";
	 break;

  case AES_CDH_BLOCKDESTSETLIMIT:
         attr = "Number of block destination sets crossed the maximum limit";
         break;
	
   case AES_CDH_RC_SFTP_PASSWD:
	attr = "Both username and password are mandatory for SFTPV2 transferProtocol";
	break;
   case AES_CDH_RC_SFTP_UNAME:
	attr = "Both username and password are mandatory for SFTPV2 transferProtocol";
	break;
   case AES_CDH_RC_FTP_UNAME:
	attr = "username is mandatory for FTPV2 transferProtocol if password is specified";
	break;
   case AES_CDH_RC_MODIFY_TRANSFERTYPE:
	attr = "Modification of transferProtocol is not allowed";
	break;
   case AES_CDH_RC_ROUTEERR:
	attr= "Route Traffic Failed : No Secondary destination is defined in this destination set";
        break;
   case AES_CDH_RC_REMOVE_IPNOTIF:
	attr= "Incorrect Operation. Once set should not remove ipNotificationAddress";
	break;
   case AES_CDH_RC_REMOVE_PORT:
	attr= "Incorrect Operation. Once set should not remove portNotificationNumber";
	break;
   case AES_CDH_RC_COMMITERROR:
	attr="Creation of multiple DestinationSet is not allowed in a single transaction";
	break;

   default:
      {
         ostringstream err_back;
         err_back << "Unknown fault code : " << resultCode;
         attr = err_back.str();
      }
   }

   return attr;
}

/*===================================================================
   ROUTINE: detailInfo
=================================================================== */
string AES_CDH_ResultImplementation::detailInfo(void) const
{
    return detailInfoText;
}

/*===================================================================
   ROUTINE: <<
=================================================================== */
ostream& operator<<(ostream& s, const AES_CDH_ResultImplementation& res)
{
    s << res.errorText();
    if (res.detailInfo().length() > 0)
    {
        s << res.detailInfo();
    }
    return s;
}

/*===================================================================
   ROUTINE: getResultCode
=================================================================== */
AES_CDH_ResultCode AES_CDH_ResultImplementation::getResultCode(int errcode)
{
   switch (errcode)
   {
   case 0:
      return AES_CDH_RC_OK;
      break;

   case 1:
      return AES_CDH_RC_ERROR;
      break;
      
   case 2:
      return AES_CDH_RC_INCUSAGE;
      break;
      
   case 10:
      return AES_CDH_RC_DESTDEF;
      break;
      
   case 11:
      return AES_CDH_RC_DESTSETDEF;
      break;
      
   case 12:
      return AES_CDH_RC_DESTNOTINDSET;
      break;
      
   case 14:
      return AES_CDH_RC_ILLEGALREMOTEFOLDERPATH;
      break;
      
   case 16:
      return AES_CDH_RC_UNREAS;
      break; 
      
   case 19:
      return AES_CDH_RC_DESTISACTORPRIM;
      break;
      
   case 20:
      return AES_CDH_RC_PHYSFILEERR;
      break;
      
   case 21:
      return AES_CDH_RC_NODESTSET;
      break;            
      
   case 22:
      return AES_CDH_RC_NODEST;
      break;
      
   case 23:
      return AES_CDH_RC_PROTECTEDDEST;
      break;
      
   case 24:
      return AES_CDH_RC_PROTECTEDDESTSET;
      break;           
      
   case 25:
      return AES_CDH_RC_DESTUSEDINSET;
      break;  
      
   case 26:
      return AES_CDH_RC_INTPROGERR;
      break;
      
   case 27:
      return AES_CDH_RC_ILLEGALCOMB;
      break; 
      
   case 28:
      return AES_CDH_RC_NOTRANS;
      break;
      
   case 29:
      return AES_CDH_RC_NOSERVER;
      break;           
      
   case 30:
      return AES_CDH_RC_CMDAUTHERR;
      break; 
      
   case 31:
      return AES_CDH_RC_USRGNOTMEMBER;
      break;
      
   case 50:
      return AES_CDH_RC_FILEERR;
      break;
      
   case 51:
      return AES_CDH_RC_FILENAMETOOLONG;
      break;
      
   case 52:
      return AES_CDH_RC_CONNECTERR;
      break;
      
   case 53:
      return AES_CDH_RC_SENDERR;
      break;
      
   case 54:
      return AES_CDH_RC_ENDOFSTREAM;
      break;
      
   case 55:
      return AES_CDH_RC_CONNECTWARN;
      break;
      
   case 56:
      return AES_CDH_RC_SENDWARN;
      break;
      
   case 57:
      return AES_CDH_RC_DATAAREAERR;
      break;
      
   case 58:
      return AES_CDH_RC_TASKSTOPPED;
      break;
      
   case 59:
      return AES_CDH_RC_NOTASK;
      break;
      
   case 60:
      return AES_CDH_RC_TASKEXISTS;
      break;

   case 61:
	   return AES_CDH_RC_FILEALREADYSENT;
	   break;
      
   case 62:
      return AES_CDH_RC_EVENTHANDLERSET;
      break;
      
   case 63:
      return AES_CDH_RC_EVENTHANDLERSETERR;
      break;
      
   case 64:
      return AES_CDH_RC_EVENTHANDLERREM;
      break;
      
   case 65:
      return AES_CDH_RC_EVENTHANDLERREMERR;
      break;  
      
   case 66:
      return AES_CDH_RC_CONNECTOK;
      break;
      
   case 67:
      return AES_CDH_RC_LOGONFAILURE;
      break;
      
   case 68:
      return AES_CDH_RC_NOTFILEDEST;
      break;
      
   case 69:
      return AES_CDH_RC_NOTBLOCKDEST;
      break;
      
   case 70:
      return AES_CDH_RC_TRANSACTIONNOTACTIVE;
      break;
      
   case 71:
      return AES_CDH_RC_TRANSACTIONACTIVE;
      break;
      
   case 72:
      return AES_CDH_RC_NOTRANSACTIONPERFORMED;
      break;
      
   case 73:
      return AES_CDH_RC_NOTINSYNCH;
      break;
      
   case 74:
      return AES_CDH_RC_INVALIDTRANSACTION;
      break;
      
   case 75:
      return AES_CDH_RC_STREAMERROR;
      break;
      
   case 76:
      return AES_CDH_RC_NOBLOCKNOAVAILABLE;
      break;
      
   case 77:
      return AES_CDH_RC_NOTNOTIFDEST;
      break;
      
   case 78:
      return AES_CDH_RC_NOTFILEDESTSET;
      break;
      
   case 79:
      return AES_CDH_RC_NOTBLOCKDESTSET;
      break;

   case 80:
      return AES_CDH_RC_SWITCH;
	  break;
      
   case 81:
      return AES_CDH_RC_NOTRESPDEST;
      break;
      
   case 82:
      return AES_CDH_RC_NOALIAS;
      break;
      
   case 83:
      return AES_CDH_RC_NOVIRTUALPATH;
      break;
      
   case 84:
      return AES_CDH_RC_TIMEOUT;
      break;
      
   case 85:
      return AES_CDH_RC_SKIPPED;
      break;

   case 86:
      return AES_CDH_RC_DESTWITHSAMEOPTEXIST;
      break;

   case 87:
      return AES_CDH_RC_KEEPOPTNOTDEFINED;
      break;

   case 90:
         return AES_CDH_RC_SECDESTEXIST;
         break;

   case 91:
         return AES_CDH_RC_NOPRIDEST;
         break;
   case 114:
         return AES_CDH_RC_BACKUP_INPROGRESS;
         break;

   case 92 :
		  return AES_CDH_FILEDESTSETLIMIT;
		  break;

   case 93 : 
		  return AES_CDH_BLOCKDESTSETLIMIT;
                  break;		
   case 95 :
                  return AES_CDH_RC_UNREAS_BLOCKIP;
                  break;
   case 96 :
                  return AES_CDH_RC_UNREAS_BLOCKRETRYDELAY;
                  break;
   case 97 :
                  return  AES_CDH_RC_UNREAS_BLOCKRETRIES;
                  break;
   case 98 :
                  return   AES_CDH_RC_UNREAS_PRIBLOCKDESTNAME;
                  break;
   case 99 :
                  return   AES_CDH_RC_UNREAS_SECBLOCKDESTNAME;
                  break;
   case 100 :
                  return   AES_CDH_RC_UNREAS_ADVBLOCKDESTNAME;
                  break;
   case 101 :
                  return   AES_CDH_RC_UNREAS_BLOCKDESTSETNAME;
                  break;
				  
   case 102 :
                  return   AES_CDH_RC_UNREAS_INITIP;
                  break;
   
   case 103 :
                  return   AES_CDH_RC_UNREAS_INITPORTNO;
                  break;
   case 104 :
                  return   AES_CDH_RC_UNREAS_FILETRANSFERTYPE;
                  break;
   case 105 :
                  return    AES_CDH_RC_UNREAS_PRIINITDESTNAME;
                  break;

   case 106 :
                  return   AES_CDH_RC_UNREAS_SECINITDESTNAME;
                  break;

   case 107 :
                  return   AES_CDH_RC_UNREAS_PRIRESPDESTNAME;
                  break;
   case 108 :
                  return  AES_CDH_RC_UNREAS_SECRESPDESTNAME ;
                  break;
   case 109 :
                  return  AES_CDH_RC_UNREAS_FILERETRYDELAY ;
                  break;
   case 110 :
                  return  AES_CDH_RC_UNREAS_FILESENDRETRY ;
                  break;
   case 111 :
                  return  AES_CDH_RC_UNREAS_INITDESTSETNAME ;
                  break;
   case 112 :
                  return AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV4MAPPEDIPV6;
                  break;
   case 113 :
                  return AES_CDH_RC_UNREAS_RESPDEST_IPV4MAPPEDIPV6;
                  break;
   case 115:
                  return AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV6_NATIVE;
                  break;
   case 116:
                  return AES_CDH_RC_UNREAS_RESPDEST_IPV6_NATIVE;
                  break;

   case 32 :
                  return  AES_CDH_RC_UNREAS_RESPDESTSETNAME ;
                  break;
   
   case 33 :
                  return   AES_CDH_RC_UNREAS_RESPIP;
                  break;

   case 34:
                  return   AES_CDH_RC_UNREAS_RESPPORTNO;
                  break;

   case 35:
                  return   AES_CDH_RC_UNREAS_FILEOVERWRITE ;
                  break;
   
   case 36:
                return   AES_CDH_RC_UNREAS_SUPPRESSSUBFOLDRCREATION ;
                break;

   case 37:
                return   AES_CDH_RC_UNREAS_INHIBITTQNAMEPRFX ;
                break;
   case 38 :
		return AES_CDH_RC_SFTP_PASSWD;
		break;
   case 39 :	return AES_CDH_RC_SFTP_UNAME;
		break;
   case 40:	return AES_CDH_RC_FTP_UNAME;
		break;
   case 41:	return AES_CDH_RC_MODIFY_TRANSFERTYPE;	
		break;
   case 42:	return AES_CDH_RC_NOADVPARAM;
		break;
   case 43:     return AES_CDH_RC_ROUTEERR;
		break;
   case 44:     return AES_CDH_RC_REMOVE_IPNOTIF;
		break;
   case 45:     return AES_CDH_RC_REMOVE_PORT;
		break;

   case 46:     return AES_CDH_RC_COMMITERROR;
		break; 

   default:
      return AES_CDH_RC_UNKNOWNRC;
      break;
   }
}

