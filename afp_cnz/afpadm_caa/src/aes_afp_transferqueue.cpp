//	INCLUDE aes_afp_transferqueue.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
//	  All rights reserved.
//	  The Copyright to the computer program(s) herein
//	  is the property of Ericsson Utvecklings AB, Sweden.
//	  The program(s) may be used and/or copied only with
//	  the written permission from Ericsson Utvecklings AB or
//	in
//	  accordance with the terms and conditions stipulated in
//	the
//	  agreement/contract under which the program(s) have
//	been
//	  supplied.
//
//	DESCRIPTION
//
//	ERROR HANDLING
//	 General rule:
//	 The error handling is specified for each method.
//	 No methods initiate or send error reports unless
//	specified.
//
//	DOCUMENT NO
//	 190 89-CAA 109 0503
//
//	AUTHOR
//	 2003-01-17 UAB/KB/AU DAPA
//
//	REVISION
//	 A 2003-01-23 DAPA
//	 B 2003-04-14 DAPA
//	 C 2003-05-09 DAPA
//
//	LINKAGE
//
//	SEE ALSO
#include <aes_afp_template.h>
#include <aes_cdh_destinationset.h>
#include <aes_afp_objectfactory.h>
#include <aes_afp_transferqueue.h>
#include <ace/OS_NS_sys_stat.h>
#include <boost/filesystem.hpp>

#include <aes_afp_services.h>
#include <acs_apgcc_omhandler.h>
#include <ACS_CS_API_Common_R1.h>
#include <ACS_CS_API_NetworkElement_R1.h>
#include <ACS_APGCC_CommonLib.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"

#include "aes_afp_renametypes.h"


#include "aes_afp_renametypes.h"

AES_AFP_TRACE_DEFINE(AES_AFP_transferqueue);

std::string aes_afp_transferqueue::auditLogDirPath = "";
aes_afp_transferqueue::aes_afp_transferqueue()
: firstDs_(false),   //HK91962
  dsetType_(DUMMY),
  manualRunOnce_(false),
  transferInProgress_(false), //HF74701
  noDestination_(false),
  afpRestarted_(false),
  AFPFILEDIRECTORY_("SENDITEMS"),
  dummyDest_("-"),
  TEXTCAUSE_("CAUSE"),
  TEXTALARM_("DATA TRANSFER, FILE TRANSFER"),
  TEXTAFPFAULT_("DATA OUTPUT, AP TRANSMISSION FAULT"),
  TEXTFILENAME_("FILE NAME"),
  TEXTTEInfoID_(""),
  TEXTTQID_("fileTransferQueueId="),
  TEXTINITDESTSETID_("initiatingDestinationSetId="),
  TEXTRESPDESTSETID_("respondingDestinationSetId="),
  TEXTDESTINATION_("DESTINATION"),
  TEXTAP_("AP"),
  TEXTAPNAME_("APNAME"),
  TEXTNODE_("NODE"),
  TEXTNODENAME_("NODENAME"),
  TEXTDESTINATIONSET_("DESTINATION SET"),
  TEXTTRANSFERQUEUE_("TRANSFER QUEUE"),
  DATAAP_("1"),
  DATAAPNAME_("...."),
  DATANODE_("...."),
  DATANODENAME_("...."),
  PERCISTANTFILENAME_("persist.nr"),
  AFPSERVERNAME_("AES_AFP_server"),
  applicationEventText_(""),
  TEXTPROBLEMDATA_("Transfer error"),
  transferFile_(""),cdhManageQueue_(NULL),
  renameFile_(AES_RTUNKNOWN),
  applChannel_(NULL),
  m_reloadStatus(false),
  isShutdownSignaled_(false) // TR HW88888
{
	threadManager_ = new ACE_Thread_Manager;
	templateObj_ = 0;
	templateObj_temp = 0;
	renameTemplateToChange = false;
	lastFileToBeTransfered_Itr = fileMap_.end(); 
	nextFileToBeTransfered_Itr = fileMap_.end();
}

aes_afp_transferqueue::~aes_afp_transferqueue()
{
	delete templateObj_;
	templateObj_ = 0;
	delete threadManager_;
	threadManager_ = 0;
	lastFileToBeTransfered_Itr = fileMap_.end();
	nextFileToBeTransfered_Itr = fileMap_.end();
	dsetType_ = DUMMY;
}

int aes_afp_transferqueue::open(void* /*args*/)
{
	AES_AFP_TRACE_FUNCTION;

	int result = 0;

	if(activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED))
	{
		AES_AFP_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "cannot start svc thread");
		AES_AFP_TRACE_MESSAGE("ERROR: cannot start svc thread. errno: %d", errno);
		result = -1;
	}

	return result;
}

int aes_afp_transferqueue::svc(void)
{
	AES_AFP_TRACE_FUNCTION;
	int result = 0;
	AES_GCC_Errorcodes error;
	std::string tqDiskPath( aes_afp_parameter::dataDir() );

	tqDiskPath += aes_afp_parameter::delimiter();
	tqDiskPath += m_tqName;
	tqDiskPath.push_back('_');
	tqDiskPath += m_destinationName;
	tqDiskPath += aes_afp_parameter::delimiter();
	tqDiskPath += m_tqName;
	tqDiskPath.push_back('_');
	tqDiskPath += m_destinationName;

	if( readTransferQueueFromDisc(tqDiskPath, error) )
	{
		m_reloadStatus = true;
		AES_AFP_LOG(LOG_LEVEL_INFO,"### Data for <%s, %s> loaded from disk", m_tqName.c_str(), m_destinationName.c_str());
		AES_AFP_TRACE_MESSAGE("### Data for <%s, %s> loaded from disk", m_tqName.c_str(), m_destinationName.c_str());
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR,"Failed to load data from disk for <%s, %s>", m_tqName.c_str(), m_destinationName.c_str());
		AES_AFP_TRACE_MESSAGE("ERROR: Failed to load data from disk for <%s, %s>", m_tqName.c_str(), m_destinationName.c_str());
	}

	return result;
}

void aes_afp_transferqueue::getAP(string &ap)
{
	ACS_DSD_Session session;
	ACS_DSD_Node node;
	session.get_local_node(node);
	ap = node.system_name ;
	return ;
}

void aes_afp_transferqueue::getHostname(std::string &outputHostname)
{
	char hostName[1024];
	size_t len = 1024;
	if(ACE_OS::hostname(hostName, len) < 0)
	{
		hostName[0] = '\0';
	}

	outputHostname = "";
	for(int i=0; i<1024 && hostName[i] != '\0'; i++)
	{
		outputHostname  += hostName[i];
	}
}


AES_GCC_Filestates aes_afp_transferqueue::getLastFileInTransfer()
{
	if (lastFileToBeTransfered_Itr != fileMap_.end())
	{
		AES_AFP_TRACE_MESSAGE("getLastFileInTransfer = %s, status = %d",(*lastFileToBeTransfered_Itr).first.c_str(), (*lastFileToBeTransfered_Itr).second->getStatus());
		return (*lastFileToBeTransfered_Itr).second->getStatus();
	}
	else
	{
		return  AES_FSNONE;
	}
}

void aes_afp_transferqueue::getNodeLetter(std::string &outputNodeLetter)
{
	FILE *fp = 0;
	char buff[512];
	int nodeId  = 0;
	outputNodeLetter =  "";

	/* popen creates a pipe so we can read the output
        of the program we are invoking */
	if (!(fp = popen("cat /etc/opensaf/slot_id" ,"r")))
	{
		return ;
	}

	/* read the output of route, one line at a time */
	if(fgets(buff, sizeof(buff), fp) != NULL )
	{
		nodeId = atoi(&buff[0]);
	}
	/* close the pipe */
	pclose(fp);

	if (nodeId == 1)
	{
		outputNodeLetter =  "A";
	}
	else if (nodeId == 2)
	{
		outputNodeLetter =  "B";
	}
}

AES_GCC_Filestates aes_afp_transferqueue::getNextFileInTransfer()
{
	if (nextFileToBeTransfered_Itr!= fileMap_.end())
	{
		AES_AFP_TRACE_MESSAGE("getNextFileInTransfer = %s, status = %d",(*nextFileToBeTransfered_Itr).first.c_str(), (*nextFileToBeTransfered_Itr).second->getStatus());
		return (*nextFileToBeTransfered_Itr).second->getStatus();
	}
	else
	{
		return  AES_FSNONE;
	}
}


//	---------------------------------------------------------
//	       init()
//	---------------------------------------------------------
bool aes_afp_transferqueue::init (std::string transferQueue,
		std::string destinationSet,
		std::string userGroup,
		AES_GCC_Errorcodes& error,
		std::string sourceDirectory,
		AES_GCC_Filestates defaultState,
		unsigned int removeDelay,
		bool removeBefore,
		int retriesDelay,
		int sendRetries,
		AES_AFP_Renametypes rename,
		std::string renameTemplate,
		bool updateRootFile,
		bool manual,
		std::string nameTag,
		long startupSequenceNumber,
		Dest_Set_Type dType)
{
	AES_AFP_TRACE_MESSAGE("Initiating TQ %s to destinationset %s", transferQueue.c_str(), destinationSet.c_str());
	AES_AFP_TRACE_MESSAGE("source directory = %s",	sourceDirectory.c_str());
	AES_AFP_TRACE_MESSAGE("def.state = %d remove delay = %d removebefore = %s retries delay = %d retries = %d file rename = %d file template = %s update root file = %s manual = %s nameTag %s startupSequenceNumber = %ld destType = %d", defaultState, removeDelay, removeBefore?"true":"false", retriesDelay, sendRetries, rename, renameTemplate.c_str(), updateRootFile?"true":"false", manual?"true":"false", nameTag.c_str(),  startupSequenceNumber,dType);
	//int result(0);
	std::string tmp("");  
	int templateCheck(false);
	std::fstream fout;
	//int resultRemoveDir = 0;

	dsetType_ = dType;

	// Add supplied data to protocol object
	protocol_.addStringValue("TRANSFERQUEUE", transferQueue);
	protocol_.addStringValue("DESTINATIONSET", destinationSet);
	protocol_.addStringValue("NAMETAG", nameTag);
	protocol_.addLongValue("STARTUPSEQUENCENUMBER", startupSequenceNumber);
	protocol_.addStringValue("USERGROUP", userGroup);
	protocol_.addStringValue("SOURCEDIRECTORY", sourceDirectory);
	protocol_.addBoolValue("REMOVEBEFORE", removeBefore);
	protocol_.addIntValue("STATUS", defaultState);
	protocol_.addIntValue("REMOVEDELAY", removeDelay);
	protocol_.addIntValue("RENAMETYPE", rename);
	protocol_.addIntValue("SENDRETRIES", sendRetries);
	protocol_.addBoolValue("MANUALINITIATED", manual);
	protocol_.addIntValue("SENDRETRIESDELAY", retriesDelay);

	protocol_.addStringValue("RENAMETEMPLATE", renameTemplate);

	tmp = aes_afp_parameter::dataDir() + aes_afp_parameter::delimiter() + protocol_.getStringValue("TRANSFERQUEUE") + "_" + protocol_.getStringValue("DESTINATIONSET");
	protocol_.addStringValue("TQOBJECTDIRECTORY", tmp);
	tempDirectory_  = tmp + "_temp";

	tmp = protocol_.getStringValue("TQOBJECTDIRECTORY") + aes_afp_parameter::delimiter() + protocol_.getStringValue("TRANSFERQUEUE") + "_" + protocol_.getStringValue("DESTINATIONSET") + "_v1";
	protocol_.addStringValue("TQOBJECTFILEPATH", tmp);

	tmp  = protocol_.getStringValue("TQOBJECTDIRECTORY") + aes_afp_parameter::delimiter() + AFPFILEDIRECTORY_;
	filesDirectory_ = tmp;

	tmp  = tempDirectory_ + aes_afp_parameter::delimiter() + AFPFILEDIRECTORY_;
	tempFilesDirectory_ = tmp;

	// Check if source directory is a directory and if it exists
	if (!protocol_.getStringValue("SOURCEDIRECTORY").empty() )
	{

		ACE_stat fstat;
		long fileAttr = ACE_OS::stat(protocol_.getCharStringValue("SOURCEDIRECTORY"), &fstat);
		if (fileAttr == -1)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Source directory %s not found", (protocol_.getStringValue("SOURCEDIRECTORY")).c_str() );
			AES_AFP_TRACE_MESSAGE("Source directory %s not found", (protocol_.getStringValue("SOURCEDIRECTORY")).c_str() );
			error = AES_APPLDIRNOTFOUND;
			return false;
		}
		if (!(fstat.st_mode & S_IFDIR))
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Source directory %s is not a directory", (protocol_.getStringValue("SOURCEDIRECTORY")).c_str() );
			AES_AFP_TRACE_MESSAGE("Source directory %s is not a directory", (protocol_.getStringValue("SOURCEDIRECTORY")).c_str() );
			error = AES_NOTDIRECTORY;
			return false;
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("Source directory %s exists and is a directory", (protocol_.getStringValue("SOURCEDIRECTORY")).c_str() );
		}
	}

	// Check the rename template
	if (!protocol_.getStringValue("RENAMETEMPLATE").empty() )
	{
		templateObj_ = new aes_afp_template(protocol_.getStringValue("TQOBJECTDIRECTORY"), protocol_.getStringValue("NAMETAG"), protocol_.getIntValue("STARTUPSEQUENCENUMBER"));
		templateCheck = templateObj_->setTemplate(protocol_.getStringValue("RENAMETEMPLATE") );
		if (templateCheck == 0)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "File template is incorrect");
			AES_AFP_TRACE_MESSAGE("File template is incorrect");
			delete templateObj_;
			templateObj_ = 0;
			error = AES_TEMPLATEFAULT;
			return false;
		}
		else if (templateCheck == 8)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "An error detected while executing the command");
			AES_AFP_TRACE_MESSAGE("An error detected while executing the command");
			delete templateObj_;
			templateObj_ = 0;
			error = AES_INCUSAGE;
			return false;
		}
		else
		{
			// do nothing
		}

	}

	// Check if dummy destinationset '-' is used
	if (destinationSet == dummyDest_)
	{
		noDestination_ = true;
	}

	// Check if destinationset exists
	if (noDestination_ == false)
	{
		AES_CDH_DestinationSet cdhDest(destinationSet);
		AES_CDH_ResultCode cdhResCode;
		cdhResCode = cdhDest.exists(AES_CDH_DestinationSet::FILE);
		if (cdhResCode == AES_CDH_RC_NODESTSET)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Destinationset %s not found in CDH",	destinationSet.c_str());
			AES_AFP_TRACE_MESSAGE("Destinationset %s not found in CDH",   destinationSet.c_str());
			error = AES_NODESTINATION;
			return false;
		}
		else if (cdhResCode == AES_CDH_RC_NOTFILEDESTSET)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Destinationset %s not configured for file transfer",	destinationSet.c_str());
			AES_AFP_TRACE_MESSAGE("Destinationset %s not configured for file transfer",   destinationSet.c_str());
			error = AES_NOTFILEDEST;
			return false;
		}
		else if (cdhResCode != AES_CDH_RC_OK)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Destinationset %s not found in CDH, result code = %d",	destinationSet.c_str(), cdhResCode );
			AES_AFP_TRACE_MESSAGE("Destinationset %s not found in CDH, result code = %d", destinationSet.c_str(), cdhResCode );
			error = AES_NODESTINATION;
			return false;
		}
	}
	// Create transfer queue_destinationset directory in data path for storing config file
	if(createDirectoryStructure(error))
	{
		AES_AFP_TRACE_MESSAGE("directroy structure creation successful");
	}
	else 
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "directroy structure creation failed");
		AES_AFP_TRACE_MESSAGE("directroy structure creation failed");
		return false;
	}

	protocol_.writeToFile(error);

	AES_AFP_TRACE_MESSAGE("Transfer queue %s initiated", transferQueue.c_str());

	// Initiate the destination set
	if (noDestination_ == false)
	{
		cdhManageQueue_ = destTask_.askForManageQueue();
		error = static_cast<AES_GCC_Errorcodes>(destTask_.open((void*)this));
		if(error != AES_NOERRORCODE)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "cannot open the connection towards CDH. GCC error: %d", error);
			AES_AFP_TRACE_MESSAGE("cannot open the connection towards CDH. GCC error: %d", error);
			return false;
		}
	}
	m_reloadStatus = true;
	error = AES_NOERRORCODE;
	AES_AFP_TRACE_MESSAGE("Exiting");
	return true;
}

bool aes_afp_transferqueue::initFromIMM (std::string objRDN,
		const string &tqName,
		const string &destSetName)
{

	AES_AFP_TRACE_MESSAGE("Transfer Queue DN:<%s>",objRDN.c_str());

	OmHandler myOmHandler;

	if( myOmHandler.Init() == ACS_CC_FAILURE )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR,"OmHandler initialize failed" );
		AES_AFP_TRACE_MESSAGE("OmHandler initialize failed");
		AES_AFP_TRACE_MESSAGE("Leaving aes_afp_transferqueue::initFromIMM " );
		return false;
	}
	string myRDn("");
	std::string nameTag1("");
	std::string nameTag2("");
	std::string nameTag("");
	int retryTimeInterval = 10,retryTimes = 0;
	std::string tmp("");
	int startupSequenceNumber=-1,renamestatus=2;
	std::string srcfolder("");
	char tmpstr[50];
	std::string renameTemplate("");
	int removebefore=0,removeDelay=10,defaultStatus=0;

	//Create the attributes
	std::vector<std::string>  p_rdnList;
	myOmHandler.getChildren(objRDN.c_str() , ACS_APGCC_SUBLEVEL,&p_rdnList);

	if(p_rdnList.size() == 0)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Unable to retrive advanced RDN name for %s",objRDN.c_str());
		AES_AFP_TRACE_MESSAGE("Unable to retrive advanced RDN name for %s",objRDN.c_str());
	}

	AES_AFP_TRACE_MESSAGE("Get TQ childrens:<%zu>", p_rdnList.size());

	for(unsigned int rdnCntr = 0; rdnCntr < p_rdnList.size(); rdnCntr ++)
	{

		AES_AFP_TRACE_MESSAGE("TQ children:<%s>", p_rdnList[rdnCntr].c_str());

		std::string myClassName;
		extractClassNmFromRdn(p_rdnList[rdnCntr], myClassName);
		if( myClassName == TQ_RDN_CL_FILETQPARAMNAME)
		{
			myRDn = p_rdnList[rdnCntr];
		}

	}

	ACS_APGCC_ImmObject myadvObj;
	myadvObj.objName.assign(myRDn) ;

	if( myOmHandler.getObject(&myadvObj) == ACS_CC_FAILURE )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, " Unable to retrieve attributes for %s", myadvObj.objName.c_str());
		AES_AFP_TRACE_MESSAGE(" Unable to retrieve attributes for %s", myadvObj.objName.c_str());
	}


	std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr2;

	for ( attributesListItr2 = myadvObj.attributes.begin();
			attributesListItr2 != myadvObj.attributes.end() ;
			++attributesListItr2 )
	{

		if((*attributesListItr2).attrName == TQ_NAMETAG1)
		{
			if((*attributesListItr2).attrValuesNum != 0)
			{
				ACE_OS::strcpy(tmpstr, reinterpret_cast<char *>((*attributesListItr2).attrValues[0]));

				if(ACE_OS::strcmp(tmpstr,"null")==0)
				{
					strcpy(tmpstr,"");
				}
				nameTag1 += tmpstr;
			}
		}
		else if((*attributesListItr2).attrName== TQ_NAMETAG2)
		{
			if((*attributesListItr2).attrValuesNum != 0)
			{
				ACE_OS::strcpy(tmpstr, reinterpret_cast<char *>((*attributesListItr2).attrValues[0]));

				if(ACE_OS::strcmp(tmpstr,"null")==0)
				{
					strcpy(tmpstr,"");
				}
				nameTag2 += tmpstr;

			}

		}
		else if((*attributesListItr2).attrName== TQ_RENAMETEMPLATE )
		{
			if((*attributesListItr2).attrValuesNum !=0)
			{
				ACE_OS::strcpy(tmpstr, reinterpret_cast<char *>((*attributesListItr2).attrValues[0]));

				if(ACE_OS::strcmp(tmpstr,"null")==0)
				{
					strcpy(tmpstr,"");
				}
				renameTemplate=string(tmpstr);

			}
		}

		if((*attributesListItr2).attrName== TQ_DEFSTAT )
		{
			if((*attributesListItr2).attrValuesNum !=0)
			{
				defaultStatus = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);

			}
		}
		else if((*attributesListItr2).attrName== TQ_REMOVEDELAY )
		{
			if((*attributesListItr2).attrValuesNum !=0)
			{
				removeDelay = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);
			}
		}
		else if((*attributesListItr2).attrName== TQ_NUMOFRETRIES )
		{
			if((*attributesListItr2).attrValuesNum !=0)
			{
				retryTimes = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);
			}
		}
		else if((*attributesListItr2).attrName== TQ_RETRYINTERVAL )
		{
			if((*attributesListItr2).attrValuesNum !=0)
			{
				retryTimeInterval = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);
			}
		}
		//Start - HT50930
		else if((*attributesListItr2).attrName== TQ_SEQNO )
		{
			if((*attributesListItr2).attrValuesNum !=0)
			{
				startupSequenceNumber = *reinterpret_cast<int *>((*attributesListItr2).attrValues[0]);
			}
		}
		//End - HT50930
	}
	if(ACE_OS::strcmp(nameTag1.c_str(),"") != 0)
	{
		nameTag += nameTag1;
		if(ACE_OS::strcmp(nameTag2.c_str(),"") != 0)
		{
			nameTag += "*";
			nameTag += nameTag2;
		}

	}
	protocol_.addBoolValue( "REMOVEBEFORE",removebefore);
	protocol_.addIntValue("STATUS",defaultStatus);
	protocol_.addIntValue("REMOVEDELAY", removeDelay);
	if(nameTag!="")
	{
		protocol_.addStringValue("NAMETAG", nameTag);
	}

	protocol_.addIntValue("SENDRETRIES", retryTimes);
	protocol_.addIntValue("SENDRETRIESDELAY", retryTimeInterval);
	protocol_.addIntValue("RENAMETYPE",(AES_AFP_Renametypes)renamestatus);
	protocol_.addStringValue("SOURCEDIRECTORY", srcfolder);
	protocol_.addLongValue("STARTUPSEQUENCENUMBER",startupSequenceNumber);//need to check type
	protocol_.addStringValue("DESTINATIONSET", destSetName );
	// Check if dummy destinationset '-' is used
	if (dummyDest_ == destSetName)
	{
		noDestination_ = true;
	}
	protocol_.addStringValue("TRANSFERQUEUE", tqName );

	tmp = aes_afp_parameter::dataDir() + aes_afp_parameter::delimiter() + protocol_.getStringValue("TRANSFERQUEUE") + "_" + protocol_.getStringValue("DESTINATIONSET");

	tempDirectory_  = tmp + "_temp";
	protocol_.addStringValue("TQOBJECTDIRECTORY", tmp);
	filesDirectory_=protocol_.getStringValue("TQOBJECTDIRECTORY") + aes_afp_parameter::delimiter() + AFPFILEDIRECTORY_;
	tempFilesDirectory_ = tempDirectory_ + aes_afp_parameter::delimiter() + AFPFILEDIRECTORY_;
	protocol_.addStringValue( "RENAMETEMPLATE",renameTemplate);
	if (!protocol_.getStringValue("RENAMETEMPLATE").empty())
	{
		templateObj_ = new aes_afp_template(protocol_.getStringValue("TQOBJECTDIRECTORY"), protocol_.getStringValue("NAMETAG"), protocol_.getIntValue("STARTUPSEQUENCENUMBER"));
		templateObj_->setTemplate(protocol_.getStringValue("RENAMETEMPLATE") );
		int rename = 0;
		protocol_.addIntValue("RENAMETYPE", rename);
	}
	else
	{
		int rename = 2;
		protocol_.addIntValue("RENAMETYPE", rename);
	}
	myOmHandler.Finalize();
	AES_AFP_TRACE_MESSAGE("Leaving");
	return true;
}
Dest_Set_Type aes_afp_transferqueue::getDestSetType()
{
	return dsetType_;
}

void aes_afp_transferqueue::setDestSetType(Dest_Set_Type dsetType)
{
	dsetType_= dsetType;
}


bool aes_afp_transferqueue::initFromFile (std::string pathToTqFile, AES_GCC_Errorcodes& error)
{
	std::string tmpStr;
	char tmpBuf[512];
	AES_AFP_TRACE_MESSAGE("Path to initiating file %s", pathToTqFile.c_str());
	bool fileRestored = false;
	bool first = true;
	while( first || fileRestored)
	{
		std::fstream fin;
		first = false;
		fin.open(pathToTqFile.c_str(), ios::in);
		if (fin.good() )
		{
			if (fin.good() )
			{
				fin.getline(tmpBuf, 512, '\n');
				if (strcmp(tmpBuf, protocol_.PROTOCOLVERSION_.c_str() ) != 0)
				{
					fin.clear();
					fin.close();
				}
				aes_afp_protocoltq tempProtocol_;
				tempProtocol_.readFromFile( pathToTqFile, error );

				//Now copy only the required fields to protocol_.

				protocol_.addStringValue("SOURCEDIRECTORY", tempProtocol_.getStringValue("SOURCEDIRECTORY"));
				protocol_.addStringValue("TQOBJECTFILEPATH", tempProtocol_.getStringValue("TQOBJECTFILEPATH"));

				protocol_.addStringValue("TQOBJECTDIRECTORY", tempProtocol_.getStringValue("TQOBJECTDIRECTORY"));
				protocol_.addStringValue("MANUALINITIATED", tempProtocol_.getStringValue("MANUALINITIATED"));
				protocol_.addStringValue("TQLOCK", tempProtocol_.getStringValue("TQLOCK"));
				protocol_.addStringValue("ALARM", tempProtocol_.getStringValue("ALARM"));
				protocol_.addStringValue("APPLICATION", tempProtocol_.getStringValue("APPLICATION"));
				protocol_.addStringValue("ORIGINALFILENAME", tempProtocol_.getStringValue("ORIGINALFILENAME"));
				protocol_.addStringValue("GENERATEDFILENAME", tempProtocol_.getStringValue("GENERATEDFILENAME"));

				if( !fileRestored && (strcmp(tmpBuf, "") == 0 ||
						protocol_.getStringValue("TRANSFERQUEUE") == "" ||
						protocol_.getStringValue("DESTINATIONSET") == "") )
				{
					fin.clear();
					fin.close();
					restoreFromTmpFile(pathToTqFile + "_temp", pathToTqFile);
					fileRestored = true;
					continue;
				}
				else
					fileRestored = false;

				if (protocol_.getStringValue("DESTINATIONSET") == dummyDest_ )
				{
					noDestination_ = true;
				}
				tmpStr = aes_afp_parameter::dataDir() + aes_afp_parameter::delimiter() + protocol_.getStringValue("TRANSFERQUEUE") + "_" + protocol_.getStringValue("DESTINATIONSET");
				tempDirectory_ = tmpStr + "_temp";
				protocol_.addStringValue("TQOBJECTDIRECTORY", tmpStr);

			}
		}
		else
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to open initiating file");
			AES_AFP_TRACE_MESSAGE("Not able to open initiating file");
			fin.close();
			return false;
		}

		fin.close();

	}
	AES_AFP_TRACE_MESSAGE("Leaving");
	return true;
}

//	---------------------------------------------------------
//	       getName()
//	---------------------------------------------------------
std::string aes_afp_transferqueue::getName ()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	return protocol_.getStringValue("TRANSFERQUEUE");
}

//	---------------------------------------------------------
//	       getDestinationSet()
//	---------------------------------------------------------
std::string aes_afp_transferqueue::getDestinationSet ()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	return protocol_.getStringValue("DESTINATIONSET");
}

//	---------------------------------------------------------
//	       getNameTag()
//	---------------------------------------------------------
std::string aes_afp_transferqueue::getNameTag ()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	return protocol_.getStringValue("NAMETAG");
}

//	---------------------------------------------------------
//	       getStartupSequenceNumber()
//	---------------------------------------------------------
int aes_afp_transferqueue::getStartupSequenceNumber ()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	return protocol_.getIntValue("STARTUPSEQUENCENUMBER");
}


//	-------------------------------------------------
//	      insertFile()
//	-------------------------------------------------
bool aes_afp_transferqueue::insertFile (std::string filename, AES_GCC_Errorcodes& error, AES_GCC_Filestates status, std::string filePath, std::string caller, std::string originalFilename, bool isDirectory)
{
	StringMap::iterator timeItr;
	FileMap::iterator fileItr;
	AES_AFP_TRACE_MESSAGE("Inserting %s in TQ %s, directory = %s filepath %s caller %s originalFilename %s",
			filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str(), isDirectory?"true":"false",filePath.c_str(),caller.c_str(),originalFilename.c_str());
	switch (protocol_.getIntValue("STATUS") )
	{
	case AES_FSREADY:
	{
		break;
	}
	case AES_FSSEND:
	{
		break;
	}
	case AES_FSARCHIVE:
	{
		break;
	}
	case AES_FSDELETE:
	{
		break;
	}
	case AES_FSFAILED:
	{
		return true;
	}
	case AES_FSUSEDEFAULT:
	{
		break;
	}
	case AES_FSSTOPPED:
	{
		return true;
	}
	case AES_FSPENDING:
	{
		break;
	}
	case AES_FSNONE:
	{
		return true;
	}
	default:
	{
		return true;
	}
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	FileMap::iterator itr;

	std::string tmpNewFullPath;
	std::string tmpStr;
	bool boolResult(false);

	itr = fileMap_.find(filename);

	if (itr != fileMap_.end() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "File %s already exist", filename.c_str());
		AES_AFP_TRACE_MESSAGE("File %s already exist", filename.c_str());
		error = AES_SENDITEMEXIST;
		return false;  // Send item already in map
	}
	if (status == AES_FSUSEDEFAULT)
	{
		status = (AES_GCC_Filestates)protocol_.getIntValue("STATUS");
	}

	tmpNewFullPath = protocol_.getStringValue("SOURCEDIRECTORY");
	tmpNewFullPath += aes_afp_parameter::delimiter() + filename;
	ACE_stat fileData;
	if((ACE_OS::stat(tmpNewFullPath.c_str(),&fileData)))
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Physical file %s not found", tmpNewFullPath.c_str());
		AES_AFP_TRACE_MESSAGE("Physical file %s not found", tmpNewFullPath.c_str());
		Event::report(EVENT_FILENOTFOUND, "FIND FILE FAULT", tmpNewFullPath, "Not able to find file or directory: " + tmpNewFullPath);
		error = AES_FILENOTFOUND;
		return false;
	}


	aes_afp_file* newFileObj = aes_afp_objectfactory::Instance().makeFileObj();
	std::string discName;
	std::string tempDiscName;

	discName = filesDirectory_ + aes_afp_parameter::delimiter() + filename;
	tempDiscName = tempFilesDirectory_ + aes_afp_parameter::delimiter() + filename;

	if (newFileObj)
	{
		fileMapPair_.first = filename;
		fileMapPair_.second = newFileObj;
		//HK47740 - REMOVEDELAY attribute has been moved to destinationset level,
		//and aes_afp_file dont have removedelay attribute. Hence removeDelay has been removed from argument list
		boolResult = newFileObj->init(tempDiscName,
				discName,
				status,
				error,															protocol_.getStringValue("SOURCEDIRECTORY"),
				protocol_.getBoolValue("REMOVEBEFORE"),
				isDirectory,originalFilename);    //HU96961 adding originalFilename to the param

		if (boolResult == false)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to insert file %s", filename.c_str());
			AES_AFP_TRACE_MESSAGE("Not able to insert file %s", filename.c_str());
			error = AES_CATASTROPHIC;
			return false;
		}

		fileMap_.insert(fileMapPair_);

		timeSortedMapPair_.first = (newFileObj->getCreationDate() );

		timeSortedMapPair_.first = (newFileObj->nextSeqNrCreationDate() );

		timeSortedMapPair_.second = filename;


		for(;;)
		{
			std::pair<StringMap::iterator, bool> result = timeSortedMap_.insert(timeSortedMapPair_);

			if(result.second != false)
			{
				break;
			}

			timeSortedMapPair_.first = (newFileObj->nextSeqNrCreationDate() );
		}


		if (status == AES_FSREADY)
		{
			if(lastFileToBeTransfered_Itr != fileMap_.end()) //Also means that fileMap_.size() > 1
			{
				if((*lastFileToBeTransfered_Itr).second->isYounger(*newFileObj)) //True if clock has been set backwards.
				{

					if( nextFileToBeTransfered_Itr == fileMap_.end()) //If there no queued file.
					{
						AES_AFP_TRACE_MESSAGE("lastFileToBeTransfered_Itr is younger than this file, nextFileToBeTransfered_Itr is set to %s", filename.c_str());
						Event::report(	EVENT_TIMECHANGED,
								"AP LOCAL TIME CHANGED BACKWARDS",
								"FILE: " + filename + " is younger than FILE:" + (*lastFileToBeTransfered_Itr).first,
								"Local time has been changed backwards. AFP takes appropriate actions.");

						nextFileToBeTransfered_Itr = fileMap_.find(filename);

					}else if((*nextFileToBeTransfered_Itr).second->isYounger(*newFileObj))
					{
						AES_AFP_TRACE_MESSAGE("nextFileToBeTransfered_Itr is younger than this file, nextFileToBeTransfered_Itr is set to %s", filename.c_str());
						Event::report(	EVENT_TIMECHANGED,
								"AP LOCAL TIME CHANGED BACKWARDS",
								"FILE: " + filename + " is younger than FILE:" + (*nextFileToBeTransfered_Itr).first,
								"Local time has been changed backwards. AFP takes appropriate actions.");

						nextFileToBeTransfered_Itr = fileMap_.find(filename);
					}

				}

				switch ((*lastFileToBeTransfered_Itr).second->getStatus()) //Status of last transfered file.
				{
				case AES_FSSEND:
				{
					AES_AFP_TRACE_MESSAGE("State of previous is FS_SEND, this file is not sent to CDH");
					break;
				}
				case AES_FSREADY:
				{
					AES_AFP_TRACE_MESSAGE("State of previous is FS_READY, this file is not sent to CDH");
					break;
				}
				default:
				{
					AES_AFP_TRACE_MESSAGE("State of previous is other than FS_READY, FS_SEND, this file is sent to CDH");
					sendToCdh(filename, newFileObj, error);
					if ( error != AES_NOERRORCODE )
					{
						return false;
					}
					break;
				}
				}

			}else
			{
				sendToCdh(filename, newFileObj, error);
				if ( error != AES_NOERRORCODE )
				{
					return false;
				}
			}

		}else
		{
			lastFileToBeTransfered_Itr = fileMap_.find(filename);
		}

		protocol_.addStringValue("ORIGINALFILENAME", filename);

		protocol_.writeToFile(error);

		if ( error != AES_NOERRORCODE )
		{
			return false;
		}

		error = AES_NOERRORCODE;

		AES_AFP_TRACE_MESSAGE("File inserted OK");
		return true;
	}
	error = AES_CATASTROPHIC;
	AES_AFP_LOG(LOG_LEVEL_ERROR, "Catastrophic fault occured!");
	AES_AFP_TRACE_MESSAGE("Catastrophic fault occured!");
	return false;
}

//	---------------------------------------------------------
//	       removeAll()
//	---------------------------------------------------------
bool aes_afp_transferqueue::removeAll (AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("Removing TQ %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	if (getLastFileInTransfer() == AES_FSSEND ||  getNextFileInTransfer() == AES_FSSEND)
	{
		error = AES_TQISOPEN;
		return false;
	}

	/*if (transferInProgress_)//HE74701
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s is open", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		AES_AFP_TRACE_MESSAGE("TQ %s is open", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		error = AES_TQISOPEN;
		return false;
	}*/

	if (protocol_.getBoolValue("TQLOCK") == true)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s is protected", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		AES_AFP_TRACE_MESSAGE("TQ %s is protected", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		error = AES_FILEISPROTECTED;
		return false;
	}

	FileMap::iterator itr;
	std::map<std::string, std::string>::iterator timeItr;
	std::string tmpRemovePath("");

	bool result(false);


	if (noDestination_ == false)
	{
		destTask_.set_remove_tq(true);
		sendHangupToCdh();
	}

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap); // TR HI61871

	try
	{
		if (!fileMap_.empty() )
		{
			itr = fileMap_.begin();
			while (itr != fileMap_.end())
			{
				if(isShutdownSignaled_) // TR HW88888
					return false;

				AES_AFP_TRACE_MESSAGE("File Name = %s", ((*itr).first).c_str() );
				tmpRemovePath = filesDirectory_ + aes_afp_parameter::delimiter() + (*itr).first;
				result = (*itr).second->removeFile(tmpRemovePath, error);
				if (result == false)
				{
					if (error == AES_NOACCESS)
					{
						AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove %s, access fault in file system",
								protocol_.getCharStringValue("TRANSFERQUEUE"));
						AES_AFP_TRACE_MESSAGE("Not able to remove %s, access fault in file system",
								protocol_.getCharStringValue("TRANSFERQUEUE"));
						eventNoAccessAfpFile((*itr).first);
						return false;
					}
				}
				aes_afp_objectfactory::Instance().relFileObj((*itr).second);
				fileMap_.erase(itr);
				itr = fileMap_.begin();

			}
		}

		// HF20783 send event if unprotect transferqueue fail
		StringMap::iterator timeItr = timeSortedMap_.begin();
		while( timeItr != timeSortedMap_.end() )
		{
			if(isShutdownSignaled_) return false; // TR HW88888
			timeSortedMap_.erase( timeItr );
			timeItr = timeSortedMap_.begin();
		}
		timeSortedMap_.clear();

		fileMap_.clear();
	}catch (...)
	{}

	result = remTqFromFile(error);
	if (result == false)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove %s, access fault in file system", (protocol_.getStringValue("TRANSFERQUEUE")).c_str());
		AES_AFP_TRACE_MESSAGE("Not able to remove %s, access fault in file system", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		error = AES_NOACCESS;
		return false;
	}
	error = AES_NOERRORCODE;

	if (protocol_.getBoolValue("ALARM") == true)
	{
		ceaseTransferFailed();
	}

	AES_AFP_TRACE_MESSAGE("TQ %s removed OK", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
	return true;
}

//	---------------------------------------------------------
//	       readTransferQueueFromDisc()
//	---------------------------------------------------------
bool aes_afp_transferqueue::readTransferQueueFromDisc (std::string& pathToTqFile, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("Enetered in aes_afp_transferqueue::readTransferQueueFromDisc");
	std::fstream fin;
	std::string filePath("");
	std::string tmpPath("");
	std::string fileN("");
	bool result = true;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	fileMap_.clear();

	//In APG43L, the name of the TQ_DESTSET file is modified to TQ_DESTSET_v1 and TQ_DESTSET_temp is modified to TQ_DESTSET_v1_temp.
	//First, we will look for TQ_DESTSET_v1 file. If this file is not found, then it might be a upgrade from APG43 Windows to APG43L. Read the desired fields from older file and create a new file in linux format. 

	string newPathToTqFile = pathToTqFile + "_v1";
	bool retVal = initFromFile(newPathToTqFile, error);
	if (retVal == false)
	{
		retVal = initFromFile( pathToTqFile, error);
		if( retVal == false )
		{
			AES_AFP_TRACE_MESSAGE("initFromFile failed !!");
		}
		protocol_.addStringValue("TQOBJECTFILEPATH", newPathToTqFile );
		retVal = protocol_.writeToFile( error );
		if( retVal == false )
		{
			AES_AFP_TRACE_MESSAGE("protocol_.writeToFile failed !!" );
		}
	}
	if(retVal == true)
	{	
		if(initCdhTask(error) == false)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "initCdhTask() failed !!");
			AES_AFP_TRACE_MESSAGE("initCdhTask() failed !!");
			return false;
		}
	}
	filePath = filesDirectory_ ;
	DIR *dir = 0;
	dir = opendir(filePath.c_str());
	ACE_stat fstat;
	if (dir == NULL)
	{
		if(createDirectoryStructure(error))
		{
			AES_AFP_TRACE_MESSAGE("directory structure creation successful");
		}
		else
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "directory structure creation failed");
			AES_AFP_TRACE_MESSAGE("directory structure creation failed");
			return false;
		}
		if(ACE_OS::stat(newPathToTqFile.c_str(),&fstat) == -1)
		{
			retVal = protocol_.writeToFile( error );
			if( retVal == false )
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "protocol_.writeToFile failed !!" );
				AES_AFP_TRACE_MESSAGE(" protocol_.writeToFile failed !!");
				return false;
			}
			if( initCdhTask(error) == false )
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "initCdhTask() failed !!" );
				AES_AFP_TRACE_MESSAGE(" initCdhTask() failed !!");
				return false;
			}
		}
	}
	else
	{
		struct dirent * finddata = 0;
		AES_AFP_TRACE_MESSAGE("Inserting info from status files in to fileMap from %s , Started..!!",filesDirectory_.c_str());

		while ((finddata = readdir(dir)) != NULL)
		{

			if(aes_afp_services::afpStopEvt == true)
			{
				break;
			}
			if (strcmp(finddata->d_name, ".") && strcmp(finddata->d_name, ".."))
			{

				fileN = filesDirectory_ + "/" +  finddata->d_name;
				if(!(ACE_OS::stat(fileN.c_str(),&fstat)))
				{
					if(!(fstat.st_mode & S_IFDIR))
					{
						filePath = filesDirectory_ + aes_afp_parameter::delimiter() + finddata->d_name;

						aes_afp_file* tmpFileObj = aes_afp_objectfactory::Instance().makeFileObj();

						if (tmpFileObj->initFromFile(filePath, error))
						{
							result = tmpFileObj->isFileStatusDataValid();
							if( result == false)
							{
								//FILE CORRUPTED
								AES_AFP_TRACE_MESSAGE("SENDITEM FILE CORRUPTION for filePath = %s",filePath.c_str() );
								AES_AFP_LOG(LOG_LEVEL_ERROR, "SENDITEM FILE CORRUPTION for filePath = %s",filePath.c_str() );
								Event::report(EVENT_SENDITEMCORRUPT, "SENDITEM FILE CORRUPTION", filePath, "corrupted senditemfile: " + filePath);
								if(tmpFileObj->getStatus() != AES_FSDELETE)
								{
									ceaseTransferFailed();
									alarmTransferFailed(finddata->d_name);
									tmpFileObj->removeFile(filePath, error);
									tmpFileObj->recover_data();
									tmpFileObj->dumpToFile_new(filePath,error);
								}
							}
							fileMapPair_.first = finddata->d_name;
							fileMapPair_.second = tmpFileObj;
							fileMap_.insert(fileMapPair_);
						}
						else
						{
							AES_AFP_TRACE_MESSAGE("ERROR: File does not exist or it is empty <%s>",filePath.c_str() );
							AES_AFP_LOG(LOG_LEVEL_ERROR, "File does not exist or it is empty <%s>",filePath.c_str() );
							delete tmpFileObj;
						}
					}
				}
			}
		}

		AES_AFP_TRACE_MESSAGE("Inserting info from status files in to fileMap from %s , DONE!!",filesDirectory_.c_str());
		//Raising alarms for failed files during service restart -- Fix for TR HR14912
		FileMap::iterator currentItr;
		std::vector<string> alarmList;
		std::vector<string>::iterator listIt;
		for(currentItr = fileMap_.begin();(currentItr != fileMap_.end()) ;currentItr++)
		{
			if(aes_afp_services::afpStopEvt == true)
			{
				break;
			}
			if((*currentItr).second->getStatus() == AES_FSFAILED)
			{
				alarmList.push_back((*currentItr).first);
			}
		}

		if(dir != NULL)
			closedir(dir);

		if(aes_afp_services::afpStopEvt == true)
		{
			return true;
		}

		if(!alarmList.empty())
		{
			listIt=alarmList.begin();
			if(listIt != alarmList.end())
			{
				string lastFailedFile = listIt[alarmList.size()-1];
				if (protocol_.getBoolValue("ALARM") == true)
				{
					ceaseTransferFailed();
					alarmTransferFailed(lastFailedFile);
				}
			}
			alarmList.clear();
		}

	}
	initTimeSortedMap();
	AES_AFP_TRACE_MESSAGE("calling resendFiles ...");
	resendFiles();
	return true;
}

void aes_afp_transferqueue::fillDestsetInfo(vector<TqInfo> &destsetList)
{
	TqInfo temp;
	temp.destsetName =  protocol_.getStringValue("DESTINATIONSET"); 
	temp.dsetType = dsetType_;
	destsetList.push_back(temp);
}

//	---------------------------------------------------------
//	       fillInfoBuffer()
//	---------------------------------------------------------
void aes_afp_transferqueue::fillInfoBuffer (std::string& listBuffer)
{
	AES_AFP_TRACE_FUNCTION;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	FileMap::iterator itr;

	char tmp[8];
	sprintf(tmp, "%d", PROCORDERSTART);
	listBuffer += tmp;
	listBuffer += "\n";
	addTransferQueueInfo(listBuffer);

	if (!fileMap_.empty() )
	{
		//HK47740 - REMOVEDELAY attribute has been moved to destinationset level,
		//and it will used for further calculations
		int removeDelay = getRemoveDelay(); 

		for (itr = fileMap_.begin();itr != fileMap_.end();++itr)
		{
			sprintf(tmp, "%d", SENDITEMSTART);
			listBuffer += tmp;
			listBuffer += "\n";
			listBuffer += (*itr).first;
			listBuffer += "\n";
			listBuffer += protocol_.getStringValue("DESTINATIONSET");
			listBuffer += "\n";
			(*itr).second->fillFileInfo(listBuffer, removeDelay);	//HK47740

		}
	}
}

void aes_afp_transferqueue::fillInfoBufferShort(std::string& listBuffer)
{
	FileMap::iterator itr;

	char tmp[8];
	sprintf(tmp, "%d", PROCORDERSTART);
	listBuffer += tmp;
	listBuffer += "\n";
	addTransferQueueInfo(listBuffer);
}



//	---------------------------------------------------------
//	       setRemoveDelay()
//	---------------------------------------------------------
void aes_afp_transferqueue::setRemoveDelay (int delay)
{
	protocol_.addIntValue("REMOVEDELAY", delay);
}

//	---------------------------------------------------------
//	       getRemoveDelay()
//	---------------------------------------------------------
int aes_afp_transferqueue::getRemoveDelay ()
{
	return protocol_.getIntValue("REMOVEDELAY");
}

//	---------------------------------------------------------
//	       remTqFromFile()
//	---------------------------------------------------------
bool aes_afp_transferqueue::remTqFromFile (AES_GCC_Errorcodes& error)
{
	AES_AFP_LOG(LOG_LEVEL_INFO, "Removing directories for transfer queue %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str());
	AES_AFP_TRACE_MESSAGE("Removing directories for transfer queue %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str());
	bool result = true;

	if(this->dirDelete(protocol_.getCharStringValue("TQOBJECTDIRECTORY")) && this->dirDelete(tempDirectory_))
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "TQ %s removed from disk", (protocol_.getStringValue("TRANSFERQUEUE")).c_str());
		AES_AFP_TRACE_MESSAGE("TQ %s removed from disk", (protocol_.getStringValue("TRANSFERQUEUE")).c_str());
		return result;
	}
	ACE_OS::mkdir(protocol_.getCharStringValue("TQOBJECTDIRECTORY") );
	protocol_.writeToFile(error);
	ACE_OS::mkdir(filesDirectory_.c_str() );
	ACE_OS::mkdir(tempDirectory_.c_str() );
	ACE_OS::mkdir(tempFilesDirectory_.c_str());
	result = false;
	AES_AFP_LOG(LOG_LEVEL_WARN, "Transfer queue %s is failed to remove from disk", (protocol_.getStringValue("TRANSFERQUEUE")).c_str());
	AES_AFP_TRACE_MESSAGE("Transfer queue %s is failed to remove from disk", (protocol_.getStringValue("TRANSFERQUEUE")).c_str());

	return result;
}

//	---------------------------------------------------------
//	       findNewestFile()
//	---------------------------------------------------------
bool aes_afp_transferqueue::findNewestFile (std::string& listBuffer, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("Finding newest file for TQ %s",
			(protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	FileMap::iterator itr;
	StringMap::iterator timeItr;

	char tmp[128] = {0};
	snprintf(tmp, sizeof(tmp) - 1, "%d\n", PROCORDERSTART);

	listBuffer.assign(tmp);
	addTransferQueueInfo(listBuffer);
	if (fileMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "No file found, TQ is empty");
		AES_AFP_TRACE_MESSAGE("No file found, TQ is empty");
		error = AES_FILENOTFOUND;
		return false;
	}

	timeItr = timeSortedMap_.end();
	--timeItr;

	itr = fileMap_.find( (*timeItr).second );

	snprintf(tmp, sizeof(tmp) - 1, "%d\n", SENDITEMSTART);
	listBuffer.append(tmp);
	listBuffer.append((*itr).first);
	listBuffer.append("\n");
	listBuffer.append(protocol_.getStringValue("DESTINATIONSET"));
	listBuffer.append("\n");

	//HK47740 - REMOVEDELAY attribute has been moved to destinationset level,
	//and it will used for further calculations
	(*itr).second->fillFileInfo(listBuffer, getRemoveDelay());

	error = AES_NOERRORCODE;
	AES_AFP_TRACE_MESSAGE("Newest file is %s", (*itr).first.c_str() );
	return true;
}

//	---------------------------------------------------------
//	       findOldestFile()
//	---------------------------------------------------------
bool aes_afp_transferqueue::findOldestFile (std::string& listBuffer, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_FUNCTION;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	std::map<std::string, aes_afp_file*, std::less <std::string> >::iterator itr;
	StringMap::iterator timeItr;
	char tmp[128] = {0};
	snprintf(tmp, sizeof(tmp) - 1, "%d\n", PROCORDERSTART);
	listBuffer.assign(tmp);
	addTransferQueueInfo(listBuffer);

	AES_AFP_TRACE_MESSAGE("Finding oldest file for TQ %s",
			(protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	if (fileMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "No file found, TQ is empty");
		AES_AFP_TRACE_MESSAGE("No file found, TQ is empty");
		error = AES_FILENOTFOUND;
		return false;
	}

	timeItr = timeSortedMap_.begin();
	itr = fileMap_.find( (*timeItr).second );

	snprintf(tmp, sizeof(tmp) - 1, "%d\n", SENDITEMSTART);
	listBuffer.append(tmp);
	listBuffer.append((*itr).first);
	listBuffer.append("\n");
	listBuffer.append(protocol_.getStringValue("DESTINATIONSET"));
	listBuffer.append("\n");

	//HK47740 - REMOVEDELAY attribute has been moved to destinationset level,
	//and it will used for further calculations
	(*itr).second->fillFileInfo(listBuffer, getRemoveDelay());

	error = AES_NOERRORCODE;

	AES_AFP_TRACE_MESSAGE("Oldest file is %s", (*itr).first.c_str());
	return true;
}

//	---------------------------------------------------------
//	       findExactFile()
//	---------------------------------------------------------
bool aes_afp_transferqueue::findExactFile (std::string& filename, std::string& listBuffer, AES_GCC_Errorcodes& error)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	FileMap::iterator itr;
	char tmp[128] = {0};

	AES_AFP_TRACE_MESSAGE("Searching for file %s in TQ %s",
			filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	snprintf(tmp, sizeof(tmp) - 1, "%d\n", PROCORDERSTART);
	listBuffer.assign(tmp);
	addTransferQueueInfo(listBuffer);

	if (fileMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "No files in TQ %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		AES_AFP_TRACE_MESSAGE("No files in TQ %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		error = AES_FILENOTFOUND;
		return false;
	}

	itr = fileMap_.find(filename);
	if (itr == fileMap_.end() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "File %s not found in TQ %s",
				filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		AES_AFP_TRACE_MESSAGE("File %s not found in TQ %s",
				filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		error = AES_FILENOTFOUND;
		return false;
	}

	snprintf(tmp, sizeof(tmp) - 1, "%d", SENDITEMSTART);
	listBuffer.append(tmp);
	listBuffer.append((*itr).first);
	listBuffer.append("\n");
	listBuffer.append(protocol_.getStringValue("DESTINATIONSET"));
	listBuffer.append("\n");

	//HK47740 - REMOVEDELAY attribute has been moved to destinationset level,
	//and it will used for further calculations
	(*itr).second->fillFileInfo(listBuffer, getRemoveDelay());


	error = AES_NOERRORCODE;

	AES_AFP_TRACE_MESSAGE( "File %s found in TQ %s",
			filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	return true;
}

//	---------------------------------------------------------
//	       findExactFile()
//	---------------------------------------------------------
bool aes_afp_transferqueue::findExactFile (std::string filename)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	return (fileMap_.find(filename) != fileMap_.end());
}

//	---------------------------------------------------------
//	       addTransferQueueInfo()
//	---------------------------------------------------------
void aes_afp_transferqueue::addTransferQueueInfo (std::string& listBuffer)
{
	listBuffer += protocol_.getStringValue("TRANSFERQUEUE");
	listBuffer += "\n";

	listBuffer += protocol_.getStringValue("USERGROUP");
	listBuffer += "\n";

	listBuffer += protocol_.getStringValue("DESTINATIONSET");
	listBuffer += "\n";

	listBuffer += protocol_.getStringValue("STATUS");
	listBuffer += "\n";

	listBuffer += protocol_.getStringValue("REMOVEDELAY");
	listBuffer += "\n";

	listBuffer += protocol_.getStringValue("MANUALINITIATED");
	listBuffer += "\n";

	listBuffer += protocol_.getStringValue("RENAMETYPE");
	listBuffer += "\n";

	listBuffer += protocol_.getStringValue("SOURCEDIRECTORY");
	listBuffer += "\n";
}

//	---------------------------------------------------------
//	       deleteFiles()
//	---------------------------------------------------------
bool aes_afp_transferqueue::deleteFiles (AES_GCC_Errorcodes& error, std::map<std::string, std::string>& deleteMap)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	FileMap::iterator itr;
	StringMap::iterator timeItr;
	StringMap::iterator deleteItr;

	bool result = false;
	bool filesInStateFailed = false;
	std::string tmpFileString;
	std::string tmpRemovePath;
	std::string tmpNameStr;
	int removeDelay = getRemoveDelay();
	AES_GCC_Filestates fileStatus;
	string statusFilePath;

	try
	{
		timeItr = timeSortedMap_.begin();
		while ((timeItr != timeSortedMap_.end())) 
		{
			itr = fileMap_.find((*timeItr).second);

			if(!filesInStateFailed && (AES_FSFAILED == (fileStatus = (*itr).second->getStatus())))
			{
				filesInStateFailed = true;
			}
			result = (*itr).second->readyForDelete(error, removeDelay);				
			//HK47740

			if (result == true)
			{
				if (AES_NOERRORCODE == error)
				{

					tmpRemovePath = filesDirectory_ + "/" + (*itr).first;
					tmpFileString = getSourceDirPath((*itr).second)+ "/" + (*itr).first;

					deleteMapPair_.first = tmpRemovePath;   // Status file
					deleteMapPair_.second = tmpFileString;  // source file
					deleteMap.insert(deleteMapPair_);

					aes_afp_objectfactory::Instance().relFileObj((*itr).second);

					if(itr == lastFileToBeTransfered_Itr)
					{
						lastFileToBeTransfered_Itr = fileMap_.end();
					}

					if(itr == nextFileToBeTransfered_Itr)
					{
						nextFileToBeTransfered_Itr = fileMap_.end();
					}

					fileMap_.erase(itr);
					timeSortedMap_.erase(timeItr++);
					continue;
				}
			}
			else
			{
				tmpNameStr = filesDirectory_ + "/" + (*itr).first;
				deleteItr = deleteMap.find(tmpNameStr);
				if (deleteItr != deleteMap.end() )
				{
					std::string oldStatusFile(deleteItr->first);
					(*itr).second->removeFile(oldStatusFile, error);
					AES_AFP_TRACE_MESSAGE("Removing %s from delete map",(*itr).first.c_str() );
					deleteMap.erase(deleteItr);
				}
/*  Fix for remove dealy TR HV15700
				if ( (AES_FSDELETE == fileStatus) || (AES_FSREMOVE == fileStatus))
				{
					break;
				} */
				++timeItr;
			}
		}// end While
	}catch (...)
	{}

	if (filesInStateFailed == false)
	{
		if (protocol_.getBoolValue("ALARM") == true)
		{
			AES_AFP_TRACE_MESSAGE("Ceasing alarm for TQ %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
			ceaseTransferFailed();

			protocol_.writeToFile(error);
		}
	}
	if (fileMap_.empty() )
	{
		error = AES_NOERRORCODE;
		return true;
	}
	error = AES_TIMERNOTEXPERIED;
	return false;
}

//	---------------------------------------------------------
//	       getFileStatus()
//	---------------------------------------------------------
bool aes_afp_transferqueue::getFileStatus (std::string filename, aes_afp_datablock* dataBlock, AES_GCC_Errorcodes& error)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	FileMap::iterator itr;
	bool result = false;
	bool retVal =  false;
	std::string tmp;
	itr = fileMap_.find(filename);
	if (itr == fileMap_.end() )
	{
		//error = AES_FILENOTFOUND;
		 tmp = protocol_.getStringValue("TQOBJECTDIRECTORY") + aes_afp_parameter::delimiter() + protocol_.getStringValue("TRANSFERQUEUE") + "_" + protocol_.getStringValue("DESTINATIONSET") + "_v1";
                bool retVAl = initFromFile( tmp, error);

                //HU96961
                dataBlock->setFilename(protocol_.getStringValue("GENERATEDFILENAME"));
                dataBlock->setDestinationSet(protocol_.getStringValue("DESTINATIONSET"));
                dataBlock->setOriginalFileName(protocol_.getStringValue("ORIGINALFILENAME"));
		 if (protocol_.getStringValue("ORIGINALFILENAME") == "")
                {
                error = AES_FILENOTFOUND;
                result=false;
                return result;
                }

                dataBlock->setStatus(AES_FSDELETE);
                dataBlock->setReadyDate("");
                dataBlock->setDeleteDate("");
                dataBlock->setSendDate("");
                dataBlock->setStoppedDate("");
                dataBlock->setFailedDate("");
                dataBlock->setArchiveDate("");
                result = true;

	}
	else
	{
		(*itr).second->getFile(dataBlock);
		dataBlock->setFilename(filename);
		dataBlock->setDestinationSet(protocol_.getStringValue("DESTINATIONSET") );
		dataBlock->setTransferQueue(protocol_.getStringValue("TRANSFERQUEUE") );
		dataBlock->setRemoveDelay(protocol_.getIntValue("REMOVEDELAY") );
		dataBlock->setOriginalFileName((*itr).second->getOriginalFileName());  //HU96961
		if (protocol_.getBoolValue("MANUALINITIATED") == true)
		{
			dataBlock->setManual();
		}
		error = AES_NOERRORCODE;
		result = true;
	}
	return result;

}

//	---------------------------------------------------------
//	       getOldestFileStatus()
//	---------------------------------------------------------
bool aes_afp_transferqueue::getOldestFileStatus (aes_afp_datablock* dataBlock, AES_GCC_Errorcodes& error)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	FileMap::iterator itr;
	StringMap::iterator timeItr;
	bool result = false;
	bool retVal =  false;
	std::string tmp;
	if (fileMap_.empty() )
	{
		//error = AES_FILENOTFOUND;
		 tmp = protocol_.getStringValue("TQOBJECTDIRECTORY") + aes_afp_parameter::delimiter() + protocol_.getStringValue("TRANSFERQUEUE") + "_" + protocol_.getStringValue("DESTINATIONSET") + "_v1";
                bool retVAl = initFromFile( tmp, error);

                 //HU96961
                dataBlock->setFilename(protocol_.getStringValue("GENERATEDFILENAME"));
                dataBlock->setDestinationSet(protocol_.getStringValue("DESTINATIONSET"));
                dataBlock->setOriginalFileName(protocol_.getStringValue("ORIGINALFILENAME"));
		if (protocol_.getStringValue("ORIGINALFILENAME") == "")
		{
		error = AES_FILENOTFOUND;
		result=false;
		return result;
		}
                dataBlock->setStatus(AES_FSDELETE);
                dataBlock->setReadyDate("");
                dataBlock->setDeleteDate("");
                dataBlock->setSendDate("");
                dataBlock->setStoppedDate("");
                dataBlock->setFailedDate("");
                dataBlock->setArchiveDate("");
                result = true;

	}
	else
	{
		timeItr = timeSortedMap_.begin();
		itr = fileMap_.find( (*timeItr).second );
		(*itr).second->getFile(dataBlock);
		dataBlock->setFilename( (*itr).first);
		dataBlock->setDestinationSet(protocol_.getStringValue("DESTINATIONSET") );
		dataBlock->setOriginalFileName((*itr).second->getOriginalFileName());              //HU96961
		error = AES_NOERRORCODE;
		result = true;
	}

	return result;
}

//	---------------------------------------------------------
//	       getYoungestFileStatus()
//	---------------------------------------------------------
bool aes_afp_transferqueue::getYoungestFileStatus (aes_afp_datablock* dataBlock, AES_GCC_Errorcodes& error)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	FileMap::iterator itr;
	StringMap::iterator timeItr;
	bool retVal =  false;
	bool result = false;
	std::string tmp;

	if (fileMap_.empty() )
        {
                tmp = protocol_.getStringValue("TQOBJECTDIRECTORY") + aes_afp_parameter::delimiter() + protocol_.getStringValue("TRANSFERQUEUE") + "_" + protocol_.getStringValue("DESTINATIONSET") + "_v1";
                bool retVAl = initFromFile( tmp, error);
		
		//HU96961
                dataBlock->setFilename(protocol_.getStringValue("GENERATEDFILENAME"));
                dataBlock->setDestinationSet(protocol_.getStringValue("DESTINATIONSET"));
                dataBlock->setOriginalFileName(protocol_.getStringValue("ORIGINALFILENAME"));
		if (protocol_.getStringValue("ORIGINALFILENAME") == "")
                {
                error = AES_FILENOTFOUND;
                result = false;
                return result;
                }

                dataBlock->setStatus(AES_FSDELETE);
                dataBlock->setReadyDate("");
                dataBlock->setDeleteDate("");
                dataBlock->setSendDate("");
                dataBlock->setStoppedDate("");
                dataBlock->setFailedDate("");
                dataBlock->setArchiveDate("");
                result = true;
		
/*		else
		{
		result = false;
		}*/
        }
	
	else
	{
		timeItr = timeSortedMap_.end();
		--timeItr;
		itr = fileMap_.find( (*timeItr).second );
		(*itr).second->getFile(dataBlock);
		dataBlock->setFilename( (*itr).first);
		dataBlock->setDestinationSet(protocol_.getStringValue("DESTINATIONSET") );
		dataBlock->setOriginalFileName((*itr).second->getOriginalFileName()); //HU96961
		error = AES_NOERRORCODE;
		result = true;
	}

	return result;
}

//	---------------------------------------------------------
//	       shutDown()
//	---------------------------------------------------------
void aes_afp_transferqueue::shutDown ()
{
	AES_AFP_TRACE_MESSAGE("Closing TQ %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	isShutdownSignaled_ = true;  // TR HW88888

	if (noDestination_ == false)
	{
		AES_AFP_TRACE_MESSAGE("noDestination is false");
		if (protocol_.getBoolValue("ALARM") == true)
		{
			AES_AFP_TRACE_MESSAGE("Ceasing alarm for TQ %s",
					(protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

			ceaseTransferFailed();			// Remove remaining alarm before the destination gets removed
		}

		sendHangupToCdh_stop();
	}

	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
		FileMap::iterator itr;
		AES_AFP_TRACE_MESSAGE("releasing file object. Number of files in Map %d", (int)fileMap_.size());
		for ( itr = fileMap_.begin();itr != fileMap_.end();)
		{
			aes_afp_objectfactory::Instance().relFileObj((*itr).second);
			fileMap_.erase(itr++);
		}
		AES_AFP_TRACE_MESSAGE("releasing file objects DONE");
		fileMap_.clear();
		StringMap::iterator timeItr;
		for( timeItr = timeSortedMap_.begin(); timeItr != timeSortedMap_.end(); )
		{
			timeSortedMap_.erase( timeItr++ );
		}
		timeSortedMap_.clear();
	}

}



//	---------------------------------------------------------
//	       protectTransferQueue()
//	---------------------------------------------------------
bool aes_afp_transferqueue::protectTransferQueue ()
{

	return true;
}

//	---------------------------------------------------------
//	       unprotectTransferQueue()
//	---------------------------------------------------------
int aes_afp_transferqueue::unprotectTransferQueue ()
{
	int result=0;
	if (transferQueueProtected() == true)
	{
		AES_AFP_TRACE_MESSAGE("Unprotecting TQ %s",
				(protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		//Commented for APG43L.
#if 0
		AES_GCC_FileDestProtection fileDestProtect("AES", "AFP");
		result = fileDestProtect.Unprotect(protocol_.getStringValue("TRANSFERQUEUE") );
#endif
	}
	return result;
}

//	---------------------------------------------------------
//	       unprotectDestinationSet()
//	---------------------------------------------------------
void aes_afp_transferqueue::unprotectDestinationSet ()
{
	//  AES_GCC_NameProtection nameProtect("AFP", AES_CDH_NameProtectionType);
	// nameProtect.Unprotect( (char*)protocol_.getCharStringValue("DESTINATIONSET") );

	AES_AFP_TRACE_MESSAGE( "Protection for %s removed",
			(protocol_.getStringValue("DESTINATIONSET")).c_str() );

}

//	---------------------------------------------------------
//	       removeFile()
//	---------------------------------------------------------
bool aes_afp_transferqueue::removeFile (std::string filename, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("Removing file %s", filename.c_str() );

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	if (protocol_.getBoolValue("TQLOCK") == true)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "File %s is protected", filename.c_str() );
		AES_AFP_TRACE_MESSAGE("File %s is protected", filename.c_str() );
		error = AES_FILEISPROTECTED;
		return false;
	}

	FileMap::iterator fileItr;
	StringMap::iterator timeItr;
	std::string tmpRemovePath("");
	bool result(false);
	if (fileMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "File map is empty");
		AES_AFP_TRACE_MESSAGE("File map is empty");
		error = AES_SENDITEMNOTREP;
		return false;
	}
	fileItr = fileMap_.find(filename);

	if (fileItr == fileMap_.end() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "File %s is not reported", filename.c_str() );
		AES_AFP_TRACE_MESSAGE("File %s is not reported", filename.c_str() );
		error = AES_SENDITEMNOTREP;
		return false;
	}
	else
	{
		tmpRemovePath = filesDirectory_ + aes_afp_parameter::delimiter() + (*fileItr).first;

		if ( (*fileItr).second->getStatus() == AES_FSSEND)
		{
			std:: string tmpFullPath("");
			tmpFullPath = filesDirectory_;
			tmpFullPath += '/';
			tmpFullPath += (*fileItr).first;

			sendStopFileToCdh(tmpFullPath);
		}

		result = (*fileItr).second->removeFile(tmpRemovePath, error);
		if (result == false)
		{
			if (error == AES_NOACCESS)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Could not remove %s, no access to file", filename.c_str() );
				AES_AFP_TRACE_MESSAGE("Could not remove %s, no access to file", filename.c_str() );
				eventNoAccessAfpFile(protocol_.getStringValue("TRANSFERQUEUE") );
				return false;
			}
			if (error == AES_FILENOTFOUND)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "File %s not found", filename.c_str() );
				AES_AFP_TRACE_MESSAGE("File %s not found", filename.c_str() );
			}
		}

		timeItr = timeSortedMap_.find((*fileItr).second->getCreationDate() );

		if(timeItr != timeSortedMap_.end())
		{
			timeSortedMap_.erase(timeItr);
		}

		aes_afp_objectfactory::Instance().relFileObj((*fileItr).second);
		//Transfer Entity code commented



		if(fileItr == lastFileToBeTransfered_Itr)
		{
			lastFileToBeTransfered_Itr = fileMap_.end();			  
		}

		if(fileItr == nextFileToBeTransfered_Itr)
		{
			nextFileToBeTransfered_Itr = fileMap_.end();			  
		}

		string fileName = (*fileItr).first;

		fileMap_.erase(fileItr);
		protectTransferQueue();
		error = AES_NOERRORCODE;

		AES_AFP_TRACE_MESSAGE( "File %s removed OK", filename.c_str() );
		return true;
	}

}

//	---------------------------------------------------------
//	       getTransferQueueState()
//	---------------------------------------------------------
AES_GCC_Filestates aes_afp_transferqueue::getTransferQueueState ()
{
	return (AES_GCC_Filestates)protocol_.getIntValue("STATUS");
}

//	---------------------------------------------------------
//	       initTimeSortedMap()
//	---------------------------------------------------------
void aes_afp_transferqueue::initTimeSortedMap ()
{
	FileMap::iterator fileItr;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	if (!(fileMap_.empty()) )
	{
		for (fileItr = fileMap_.begin();(fileItr != fileMap_.end());++fileItr)
		{
			if(aes_afp_services::afpStopEvt == true)
			{
				break;
			}
			timeSortedMapPair_.first = (*fileItr).second->getCreationDate();
			timeSortedMapPair_.second = (*fileItr).first;

			int counter = 0;
			for(;;)
			{
				std::pair<StringMap::iterator, bool> result = timeSortedMap_.insert(timeSortedMapPair_);

				if(result.second == false){
					counter++;
				}
				else
				{
					counter=0;
					break;
				}
				if(counter>50000){ // In case of senditem file corruption to avoid infinite loop
					counter = 0;
					break;
				}

				timeSortedMapPair_.first = ((*fileItr).second->nextSeqNrCreationDate() );
			}

		}
	}
}

//	---------------------------------------------------------
//	       resendFiles()
//	---------------------------------------------------------
void aes_afp_transferqueue::resendFiles ()
{
	AES_AFP_TRACE_FUNCTION;
	FileMap::iterator fileItr;
	FileMap::iterator statusSendItr;
	StringMap::iterator timeItr;
	bool foundOneInSend(false);
	AES_GCC_Errorcodes errorCode;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	AES_AFP_TRACE_MESSAGE("Resending for TQ %s",
			(protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	if (fileMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Could not resend, %s map is empty",
				(protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		AES_AFP_TRACE_MESSAGE("Could not resend, %s map is empty",
				(protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		return;
	}

	statusSendItr = fileMap_.end();

	for (timeItr = timeSortedMap_.begin();timeItr != timeSortedMap_.end();++timeItr)
	{
		fileItr = fileMap_.find( (*timeItr).second );
		if ( (*fileItr).second->getStatus() == AES_FSSEND)
		{
			if (foundOneInSend == true)
			{
				changeFileStatus(AES_FSREADY, (*fileItr).first, errorCode);
				AES_AFP_TRACE_MESSAGE("changeFileStatus result in resendFiles when more than one file is in SEND state errorCode = %d", errorCode);
			}
			if (foundOneInSend == false)
			{
				statusSendItr = fileItr; // After loop finished "statusSendItr" will point to the last
				// send item in status FS_SEND
			}
			foundOneInSend = true;
		}
	}

	if (statusSendItr != fileMap_.end() )
	{
		// Send to CDH last with status send
		fileItr = fileMap_.find( (*statusSendItr).first );
		AES_AFP_TRACE_MESSAGE("Resending %s for TQ %s", (*fileItr).first.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		afpRestarted_ = true;
		sendToCdh( (*fileItr).first, (*fileItr).second, errorCode);
		afpRestarted_ = false;
	}

	if (foundOneInSend)
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "found one file in send state ");
		AES_AFP_TRACE_MESSAGE("found one file in send state ");
		return;
	}

	AES_AFP_TRACE_MESSAGE("No send retries set, setting file in status send to failed");
	for (timeItr = timeSortedMap_.begin();timeItr != timeSortedMap_.end();++timeItr)
	{
		fileItr = fileMap_.find( (*timeItr).second );
		//		if ( (*fileItr).second->getStatus() == AES_FSSEND)
		//			{
		//				changeFileStatus(AES_FSFAILED, (*fileItr).first, errorCode);
		//			}
		if ( (*fileItr).second->getStatus() == AES_FSREADY)
		{
			AES_AFP_TRACE_MESSAGE("Find file %s in status ready, sending to CDH",
					(*fileItr).first.c_str() );
			sendToCdh( (*fileItr).first, (*fileItr).second, errorCode);
			break;
		}
	}
	AES_AFP_TRACE_MESSAGE( "Resending for TQ %s, OK",
			(protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

}

//	---------------------------------------------------------
//	       sendToCdh()
//	---------------------------------------------------------
void aes_afp_transferqueue::sendToCdh (std::string filename, aes_afp_file* fileObject,AES_GCC_Errorcodes& error)
{
	error=AES_NOERRORCODE;
	aes_afp_msgblock* cdhMessage;
	aes_afp_datablock* cdhDb;
	std::string fullFileName("");
	std::string tmpFilename("");
	std::string tmpDumpPath1("");
	std::string tempPath1("");
	FileMap::iterator itr1;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	itr1 = fileMap_.find(filename);

	tmpFilename = filename;
	if (protocol_.getIntValue("RENAMETYPE") == AES_REMOTE && templateObj_ != 0)
	{
		fullFileName = getSourceDirPath(fileObject); 
		fullFileName += aes_afp_parameter::delimiter() + filename;
		tmpFilename = templateObj_->createFileName(fullFileName);
	}

	cdhDb = new(std::nothrow) aes_afp_datablock;
	if ( NULL == cdhDb) 
	{
		error=AES_CATASTROPHIC;
		return;
	}
	cdhDb->setTransferQueue(protocol_.getStringValue("TRANSFERQUEUE") );
	cdhDb->setDestinationSet(protocol_.getStringValue("DESTINATIONSET") );
	cdhDb->setFilename(filename);
	cdhDb->setNewFilename(tmpFilename);
	std::string tmpPath = getSourceDirPath(fileObject);
	tmpPath += "/";
	tmpPath += filename;

	cdhDb->setFullPath(tmpPath);

	cdhDb->setSendRetries(protocol_.getIntValue("SENDRETRIES") );
	cdhDb->setFileTemplate(protocol_.getStringValue("RENAMETEMPLATE") );
	cdhDb->setMask(fileObject->getTransferMask() );
	cdhDb->setFormat(fileObject->getTransferMode() );
	if (fileObject->getDirectoryFlag() == true)
	{
		cdhDb->setDirectoryFlag();
	}
	cdhDb->setAfpRestarted(afpRestarted_);
	if (protocol_.getBoolValue("MANUALINITIATED") == true)
	{
		cdhDb->setManual();
	}
	cdhDb->setRetriesDelay(protocol_.getIntValue("SENDRETRIESDELAY") );
	cdhMessage = new(std::nothrow) aes_afp_msgblock(sizeof(*cdhDb), cdhDb);
	if( NULL == cdhMessage)
	{
		delete cdhDb;
		cdhDb=0;
		error=AES_CATASTROPHIC;
		return;
	}
	cdhMessage->setMsgType(CDHSENDFILE);
	cdhMessage->setCallerName(protocol_.getStringValue("TRANSFERQUEUE") );
	cdhMessage->setCallerId(this);

	AES_AFP_TRACE_MESSAGE("TQ = %s Destination set = %s New filename = %s AFP filename = %s",
			(protocol_.getStringValue("TRANSFERQUEUE")).c_str(), (protocol_.getStringValue("DESTINATIONSET")).c_str(), tmpFilename.c_str(), filename.c_str());
	AES_AFP_TRACE_MESSAGE("Send retries = %d template = %s",
			protocol_.getIntValue("SENDRETRIES"), (protocol_.getStringValue("RENAMETEMPLATE")).c_str() );
	AES_AFP_TRACE_MESSAGE("File mask = %s Format = %d Directory = %s AFP restarted = %s Manual = %s",
			fileObject->getTransferMask().c_str(), fileObject->getTransferMode(), fileObject->getDirectoryFlag()?"true":"false", afpRestarted_?"true":"false", protocol_.getBoolValue("MANUALINITIATED")?"true":"false");
	transferFile_ = filename;

	lastFileToBeTransfered_Itr = fileMap_.find(filename);

	if(lastFileToBeTransfered_Itr == nextFileToBeTransfered_Itr)
	{
		nextFileToBeTransfered_Itr = fileMap_.end();			  
	}

	tmpDumpPath1 = filesDirectory_ + aes_afp_parameter::delimiter() + (*itr1).first;
	tempPath1 = tempFilesDirectory_ + aes_afp_parameter::delimiter() + "temp";
	if( !(*itr1).second->setStatus(tempPath1, tmpDumpPath1, AES_FSSEND, error))
	{
		delete cdhMessage;
		cdhMessage=0;
		return;
	}
	if (destTask_.putq(cdhMessage) < 0)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "putq FAILED");
		AES_AFP_TRACE_MESSAGE("putq FAILED");
		error=AES_CATASTROPHIC;
		delete cdhMessage;
		cdhMessage=0;
		return;
	}
	else
	{
		AES_AFP_TRACE_MESSAGE("putq SUCCESS");
	}

}

//	---------------------------------------------------------
//	       sendHangupToCdh()
//	---------------------------------------------------------
void aes_afp_transferqueue::sendHangupToCdh ()
{
	aes_afp_msgblock* cdhManageMessage;
	//int result(0);

	cdhManageMessage = new aes_afp_msgblock(0, 0);
	cdhManageMessage->setMsgType(HANGUP);
	cdhManageMessage->setCallerName(protocol_.getStringValue("TRANSFERQUEUE") );
	cdhManageMessage->setCallerId(this);

	AES_AFP_TRACE_MESSAGE("Sending hangup for destination set %s from TQ %s",
			(protocol_.getStringValue("DESTINATIONSET")).c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str());

	cdhManageQueue_->enqueue_head(cdhManageMessage);
	/*result = */threadManager_->wait();

	AES_AFP_TRACE_MESSAGE("CDH thread finished for destination set %s, TQ %s", (protocol_.getStringValue("DESTINATIONSET")).c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
}

//	---------------------------------------------------------
//	       getAttributes()
//	---------------------------------------------------------
void aes_afp_transferqueue::getAttributes (aes_afp_datablock* dataBlock)
{
	AES_AFP_TRACE_MESSAGE("TQ %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	dataBlock->setTransferQueue(protocol_.getStringValue("TRANSFERQUEUE") );
	dataBlock->setDestinationSet(protocol_.getStringValue("DESTINATIONSET") );
	dataBlock->setStatus( (AES_GCC_Filestates)protocol_.getIntValue("STATUS") );
	if (protocol_.getBoolValue("REMOVEBEFORE") == true)
	{
		dataBlock->setRemoveBefore();
	}
	else
	{
		dataBlock->unsetRemoveBefore();
	}

	if (protocol_.getBoolValue("MANUALINITIATED") == true)
	{
		dataBlock->setManual();
	}
	dataBlock->setRemoveDelay(protocol_.getIntValue("REMOVEDELAY") );
	dataBlock->setSendRetries(protocol_.getIntValue("SENDRETRIES") );
	dataBlock->setRetriesDelay(protocol_.getIntValue("SENDRETRIESDELAY") );
	dataBlock->setSourceDirectory(protocol_.getStringValue("SOURCEDIRECTORY") );
	dataBlock->setFileRename( (AES_AFP_Renametypes)protocol_.getIntValue("RENAMETYPE") );
	dataBlock->setFileTemplate(protocol_.getStringValue("RENAMETEMPLATE") );
	dataBlock->setUserGroup(protocol_.getStringValue("USERGROUP") );
	dataBlock->setNameTag(protocol_.getStringValue("NAMETAG") );
	dataBlock->setStartupSequenceNumber(protocol_.getIntValue("STARTUPSEQUENCENUMBER") );
}

//	---------------------------------------------------------
//	       setAttributes()
//	---------------------------------------------------------
bool aes_afp_transferqueue::setAttributes (int sendRetries, int removeDelay, AES_GCC_Filestates defaultState, int retryDelayTime, bool removeDelayBefore, std::string renameTemplate, std::string nameTag, long startupSequenceNumber, AES_GCC_Errorcodes& error, bool changeRenameTemplate)
{
	bool templateCheck(false);
	AES_AFP_TRACE_MESSAGE("TQ %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str());
	AES_AFP_TRACE_MESSAGE("Send retries %d remove delay %d status %d retry delay %d startupSequenceNumber %ld remove before %s", sendRetries, removeDelay, defaultState, retryDelayTime, startupSequenceNumber, removeDelayBefore?"true":"false");
	bool result(false);

	// fix to see if data file is  accessable
	result = protocol_.writeToFile(error);
	if (result == false)
	{
		return result;
	}
	if(changeRenameTemplate)
	{
		if(ACE_OS::strcmp(renameTemplate.c_str(),"") != 0 )
		{

			if(transferInProgress_)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Transfer In Progress.Cannot modify rename Template , NameTag and StartupSeqNr"); //HT50930
				AES_AFP_TRACE_MESSAGE("Transfer In Progress.Cannot modify rename Template , NameTag and StartupSeqNr"); //HT50930
				error = AES_TQISOPEN;
				return false;
			}
			else 
			{
				if(templateObj_)
				{
					AES_AFP_TRACE_MESSAGE("Deleting the template obj.");
					delete templateObj_;
					templateObj_ = 0;
				}

				AES_AFP_TRACE_MESSAGE("Creating the template obj rename Template %s NameTag %s.", renameTemplate.c_str(),nameTag.c_str());
				templateObj_ = new aes_afp_template(protocol_.getStringValue("TQOBJECTDIRECTORY"),nameTag, startupSequenceNumber);  //HT50930
				templateCheck = templateObj_->setTemplate(renameTemplate);

				if (templateCheck == false)
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "File template is incorrect");
					AES_AFP_TRACE_MESSAGE("File template is incorrect");
					delete templateObj_;
					templateObj_ = 0;
					error = AES_TEMPLATEFAULT;
					protocol_.addIntValue("RENAMETYPE", AES_NONE);
					return false;
				}
				protocol_.addIntValue("RENAMETYPE", AES_LOCAL);
			}
		}
		else
		{
			if(templateObj_)
			{
				if(transferInProgress_)
				{
					AES_AFP_LOG(LOG_LEVEL_INFO, "Transfer In Progress.Cannot modify rename Template , NameTag.");
					AES_AFP_TRACE_MESSAGE("Transfer In Progress.Cannot modify rename Template , NameTag.");
					error = AES_TQISOPEN;
					return false;
				}
				else
				{
					AES_AFP_TRACE_MESSAGE("Deleting the template obj.");
					delete templateObj_;
					templateObj_ = 0;
					protocol_.addIntValue("RENAMETYPE", AES_NONE);
				}
			}
		}
	}
	protocol_.addIntValue("SENDRETRIES", sendRetries);
	protocol_.addIntValue("REMOVEDELAY", removeDelay);
	protocol_.addIntValue("STATUS", defaultState);
	protocol_.addIntValue("SENDRETRIESDELAY", retryDelayTime);
	protocol_.addBoolValue("REMOVEBEFORE", removeDelayBefore);
	protocol_.addStringValue("NAMETAG", nameTag);
	protocol_.addStringValue("RENAMETEMPLATE", renameTemplate);
	protocol_.addLongValue("STARTUPSEQUENCENUMBER", startupSequenceNumber); //HT50930

	result = protocol_.writeToFile(error);

	return result;
}

//	---------------------------------------------------------
//	       stopSendFile()
//	---------------------------------------------------------
bool aes_afp_transferqueue::stopSendFile (std::string filename, AES_GCC_Errorcodes& error)
{
	FileMap::iterator itr;
	std::string tmpDumpPath("");
	std::string tempPath("");
	AES_GCC_Filestates state;
	bool result(false);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	AES_AFP_TRACE_MESSAGE("Stop file transfer for %s in TQ %s",
			filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	if (protocol_.getBoolValue("MANUALINITIATED") == false)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to stop file transfer, file is not manually initiated");
		AES_AFP_TRACE_MESSAGE("Not able to stop file transfer, file is not manually initiated");
		error = AES_NOTMANUALLY;
		return false;
	}
	if (noDestination_ == true)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to stop file transfer, destination is %s", (protocol_.getStringValue("DESTINATIONSET")).c_str() );
		AES_AFP_TRACE_MESSAGE("Not able to stop file transfer, destination is %s", (protocol_.getStringValue("DESTINATIONSET")).c_str() );
		error = AES_UNABLETOSTOPFILE;
		return false;
	}

	itr = fileMap_.find(filename);
	if (itr != fileMap_.end() )
	{
		state = (*itr).second->getStatus();
		switch (state)
		{
		case AES_FSREADY:
			// Fall through
		case AES_FSSEND:
		{
			AES_AFP_TRACE_MESSAGE("Sending stop file to CDH for file %s, status is send", (*itr).first.c_str() );
			tmpDumpPath = filesDirectory_ + aes_afp_parameter::delimiter() + (*itr).first;
			tempPath = tempFilesDirectory_ + aes_afp_parameter::delimiter() + "temp";//(*itr).first;//hf99814
			result = (*itr).second->setStatus(tempPath, tmpDumpPath, AES_FSSTOPPED, error);
			if (result == false)
			{
				return result;
			}

			std::string tmpPath("");
			tmpPath = getSourceDirPath((*itr).second);
			tmpPath += '/';
			tmpPath += (*itr).first;

			sendStopFileToCdh(tmpPath);
			error = AES_NOERRORCODE;
			return true;
		}
		case AES_FSDELETE:
			// Fall through
		case AES_FSFAILED:
			// Fall through
		case AES_FSREMOVE:
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to stop file transfer, status not send");
			AES_AFP_TRACE_MESSAGE("Not able to stop file transfer, status not send");
			error = AES_UNABLETOSTOPFILE;
			return false;
		}
		case AES_FSNONE:
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Sending stop file to CDH for file %s, status is none", (*itr).first.c_str() );
			AES_AFP_TRACE_MESSAGE("Sending stop file to CDH for file %s, status is none", (*itr).first.c_str() );
			tmpDumpPath = filesDirectory_ + aes_afp_parameter::delimiter() + (*itr).first;
			tempPath = tempFilesDirectory_ + aes_afp_parameter::delimiter() + "temp";//(*itr).first;//hf99814
			result = (*itr).second->setStatus(tempPath, tmpDumpPath, AES_FSSTOPPED, error);
			return result;
		}
		default:
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Default reached, could not find file %s in TQ %s", (*itr).first.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
			AES_AFP_TRACE_MESSAGE("Default reached, could not find file %s in TQ %s", (*itr).first.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
			error = AES_FILENOTFOUND;
			return false;
		}
		}
	}
	AES_AFP_TRACE_MESSAGE("Could not find file in TQ %s",  (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
	error = AES_FILENOTFOUND;
	return false;
}

//	---------------------------------------------------------
//	       stopSendAll()
//	---------------------------------------------------------
bool aes_afp_transferqueue::stopSendAll (AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE( "Send stop for all files to CDH");
	sendStopAllToCdh();
	error = AES_NOERRORCODE;
	return true;
}

//	---------------------------------------------------------
//	       sendStopSendFileToCdh()
//	---------------------------------------------------------
void aes_afp_transferqueue::sendStopFileToCdh (std::string filePathName)
{
	aes_afp_msgblock* cdhManageMessage;
	aes_afp_datablock* cdhManageDatablock;

	cdhManageDatablock = new aes_afp_datablock;
	cdhManageDatablock->setFullPath(filePathName);
	cdhManageMessage = new aes_afp_msgblock(sizeof(*cdhManageDatablock), cdhManageDatablock);

	cdhManageMessage->setMsgType(CDHSTOPSENDFILE);
	cdhManageMessage->setCallerName(protocol_.getStringValue("TRANSFERQUEUE") );
	cdhManageMessage->setCallerId(this);

	AES_AFP_TRACE_MESSAGE("Sending stop transfer for %s in TQ %s to CDH", filePathName.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	cdhManageQueue_->enqueue_head(cdhManageMessage);
}

//	---------------------------------------------------------
//	       sendStopAllToCdh
//	---------------------------------------------------------
void aes_afp_transferqueue::sendStopAllToCdh ()
{
	aes_afp_msgblock* cdhManageMessage;

	cdhManageMessage = new aes_afp_msgblock(0, 0);
	cdhManageMessage->setMsgType(CDHSTOPSENDALL);
	cdhManageMessage->setCallerName(protocol_.getStringValue("TRANSFERQUEUE") );
	cdhManageMessage->setCallerId(this);

	AES_AFP_TRACE_MESSAGE("Sending stop transfer for all files in TQ %s to CDH", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	cdhManageQueue_->enqueue_head(cdhManageMessage);
}

//	---------------------------------------------------------
//	       eventNoAccessAfpFile()
//	---------------------------------------------------------
void aes_afp_transferqueue::eventNoAccessAfpFile (std::string afpFile)
{
	AES_AFP_TRACE_MESSAGE("Afp data file %s", afpFile.c_str() );

	std::string problemData("");
	std::string problemText("");

	problemData = protocol_.getStringValue("TRANSFERQUEUE");
	problemData += "_";
	problemData += protocol_.getStringValue("DESTINATIONSET");

	problemText = "Not able to access file in AFP filesystem. File: ";
	string newdr("");
	if (AES_GCC_Util::datapath_trn(afpFile,AES_DATA_PATH,newdr)==false)
	{
		AES_AFP_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
		newdr = "Not Available";
	}
	problemText += newdr;

	Event::report(EVENT_NOACCESSAFP, "ACCESS ERROR", problemData, problemText);
}

//	--------------------------------------------------------
//	       eventNoAccessAfpDir()
//	---------------------------------------------------------
void aes_afp_transferqueue::eventNoAccessAfpDir (std::string directory)
{
	AES_AFP_TRACE_MESSAGE("Afp dir %s", directory.c_str());

	std::string problemData("");
	std::string problemText("");

	problemData = protocol_.getStringValue("TRANSFERQUEUE");
	problemData += "_";
	problemData += protocol_.getStringValue("DESTINATIONSET");

	problemText = "Not able to access directory in AFP file system: ";
	string newdr("");
	if (AES_GCC_Util::datapath_trn(directory,AES_DATA_PATH,newdr)==false)
	{
		AES_AFP_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
		newdr = "Not Available";
	}
	problemText += newdr;

	Event::report(EVENT_NOACCESSDIR, "ACCESS ERROR", problemData, problemText);
}

//	---------------------------------------------------------
//	       eventTqProtect()
//	---------------------------------------------------------
void aes_afp_transferqueue::eventTqProtect ()
{
	AES_AFP_TRACE_MESSAGE("File %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	std::string problemData("");
	std ::string problemText("");

	problemData = protocol_.getStringValue("TRANSFERQUEUE");
	problemData += "_";
	problemData += protocol_.getStringValue("DESTINATIONSET");

	problemText = "Not able to protect transfer queue: ";
	problemText += protocol_.getStringValue("TRANSFERQUEUE");

	Event::report(EVENT_FILEPROTECT, "PROTECTION FAULT", problemData, problemText);
}

//	---------------------------------------------------------
//	       eventTqUnProtect()
//	---------------------------------------------------------
void aes_afp_transferqueue::eventTqUnProtect ()
{
	AES_AFP_TRACE_MESSAGE("TQ %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	std::string problemData("");
	std ::string problemText("");

	problemData = protocol_.getStringValue("TRANSFERQUEUE");
	problemData += "_";
	problemData += protocol_.getStringValue("DESTINATIONSET");

	problemText = "Not able to unprotect transfer queue: ";
	problemText += protocol_.getStringValue("TRANSFERQUEUE");

	Event::report(EVENT_DESTUNPROTECT, "PROTECTION FAULT", problemData, problemText);
}


//	---------------------------------------------------------
//	       eventDestSetProtect()
//	---------------------------------------------------------
void aes_afp_transferqueue::eventDestSetProtect ()
{
	AES_AFP_TRACE_MESSAGE("Destination set %s", (protocol_.getStringValue("DESTINATIONSET")).c_str() );

	std::string problemData("");
	std ::string problemText("");

	problemData = protocol_.getStringValue("TRANSFERQUEUE");
	problemData += "_";
	problemData += protocol_.getStringValue("DESTINATIONSET");

	problemText = "Not able to protect destination: ";
	problemText += protocol_.getStringValue("DESTINATIONSET");

	Event::report(EVENT_DESTPROTECT, "PROTECTION FAULT", problemData, problemText);
}

//	---------------------------------------------------------
//	       eventFileRename()
//	---------------------------------------------------------
void aes_afp_transferqueue::eventFileRename (std::string filename)
{
	AES_AFP_TRACE_MESSAGE("File %s", filename.c_str());

	std::string problemData("");
	std ::string problemText("");

	problemData = protocol_.getStringValue("TRANSFERQUEUE");
	problemData += "_";
	problemData += protocol_.getStringValue("DESTINATIONSET");

	problemText = "Not able to rename file: ";
	problemText += filename;

	Event::report(EVENT_RENAMEFILE, "RENAME FAULT", problemData, problemText);

}

//	---------------------------------------------------------
//	       alarmTransferFailed()
//	---------------------------------------------------------
void aes_afp_transferqueue::alarmTransferFailed (string filename)
{
	AES_AFP_TRACE_MESSAGE("TQ %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );


	char problemText[1024];
	std::string problemData("");
	char objectRef[128];

	//Added for getting destset rdn in AP Transmission fault alarm
	string tempDset;
	if (dsetType_== INIT)
	{
		tempDset = TEXTINITDESTSETID_;
	}
	else if(dsetType_ == RESP)
	{
		tempDset = TEXTRESPDESTSETID_;
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Destinationset type unknown, type = %d ",dsetType_);
		AES_AFP_TRACE_MESSAGE("Destinationset type unknown, type = %d ",dsetType_);
	}

	sprintf(problemText,
#if 0
			"%s\t%s\t%s\t%s\n%s\t%s\t%s\t%s\n%s\n%s\n\n%s\n%s%s\n\n%s\n\n%s%s\n\n%s\n%s%s\n\n%s\n%s",
			TEXTAP_.c_str(),
			TEXTAPNAME_.c_str(),
			TEXTNODE_.c_str(),
			TEXTNODENAME_.c_str(),
			dataAP.c_str(),
			dataAPNm.c_str(),
			nodeLetter.c_str(),
			hostName.c_str(),	
#endif
			"%s\n%s\n\n%s\n%s%s\n\n%s\n%s%s\n\n%s\n%s%s\n\n%s\n%s\n",
			TEXTCAUSE_.c_str(),
			TEXTALARM_.c_str(),
			TEXTFILENAME_.c_str(),
			TEXTTEInfoID_.c_str(),		
			//  protocol_.getCharStringValue("GENERATEDFILENAME"),
			filename.c_str(),
			TEXTTRANSFERQUEUE_.c_str(),
			TEXTTQID_.c_str(),
			protocol_.getCharStringValue("TRANSFERQUEUE"),
			TEXTDESTINATIONSET_.c_str(),
			tempDset.c_str(),
			protocol_.getCharStringValue("DESTINATIONSET"),
			TEXTDESTINATION_.c_str(),
			"-" );

	AES_AFP_TRACE_MESSAGE("*******************************AlarmTransferFailed() problemText =  %s",problemText);
	sprintf(objectRef,
			"%s%s%s",
			protocol_.getCharStringValue("TRANSFERQUEUE"),
			"_",
			protocol_.getCharStringValue("DESTINATIONSET")
	);
	if (applicationEventText_.empty() )
	{
		problemData = TEXTPROBLEMDATA_;
	}
	else
	{
		problemData = applicationEventText_;
	}

	AES_GCC_Event transFailed;
	transFailed.setProbableCause(TEXTAFPFAULT_.c_str());
	transFailed.setProblemText(problemText);
	transFailed.setProblemData(problemData.c_str() );
	transFailed.setEventCode(ALARM_TRANSFAILED);
	transFailed.setObjectOfReference(objectRef);
	AES_GCC_EventHandler::instance().alarm(transFailed, "A2");
	protocol_.addBoolValue("ALARM", true);

}

//	---------------------------------------------------------
//	       ceaseTransferFailed()
//	---------------------------------------------------------
void aes_afp_transferqueue::ceaseTransferFailed ()
{
	AES_AFP_TRACE_MESSAGE( "TQ %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	char problemText[128];
	std::string problemData("");
	char objectRef[128];

	sprintf(problemText,
			"%s\n%s\n%s\n%s\n%s",
			TEXTALARM_.c_str(),
			TEXTTRANSFERQUEUE_.c_str(),
			protocol_.getCharStringValue("TRANSFERQUEUE"),
			TEXTDESTINATIONSET_.c_str(),
			protocol_.getCharStringValue("DESTINATIONSET") );

	sprintf(objectRef,
			"%s%s%s",
			protocol_.getCharStringValue("TRANSFERQUEUE"),
			"_",
			protocol_.getCharStringValue("DESTINATIONSET")
	);

	if (applicationEventText_.empty() )
	{
		problemData = TEXTPROBLEMDATA_;
	}
	else
	{
		problemData = applicationEventText_;
	}

	AES_GCC_Event ceaseAlarm;
	ceaseAlarm.setProbableCause(TEXTAFPFAULT_.c_str());
	ceaseAlarm.setProblemText(problemText);
	ceaseAlarm.setProblemData(problemData.c_str() );
	ceaseAlarm.setEventCode(ALARM_TRANSFAILED);
	ceaseAlarm.setObjectOfReference(objectRef);
	AES_AFP_TRACE_MESSAGE("EVENTCODE = %d OBJOFREF = %s PROBLEMTEXT = %s PROBLEMDATA = %s",ALARM_TRANSFAILED,objectRef,problemText,problemData.c_str());
	AES_GCC_EventHandler::instance().cease(ceaseAlarm);
	protocol_.addBoolValue("ALARM", false);
}

//	---------------------------------------------------------
//	       lockTransferQueue()
//	---------------------------------------------------------
bool aes_afp_transferqueue::lockTransferQueue (std::string application, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("Locking TQ %s for application %s",
			(protocol_.getStringValue("TRANSFERQUEUE")).c_str(), application.c_str() );

	bool result(false);

	if (protocol_.getBoolValue("TQLOCK") ==  false)
	{
		protocol_.addBoolValue("TQLOCK", true);
		protocol_.addStringValue("APPLICATION", application);

		AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ locked OK");
		AES_AFP_TRACE_MESSAGE("TQ locked OK");
		result = protocol_.writeToFile(error);

		return result;
	}

	AES_AFP_TRACE_MESSAGE("Not able to lock TQ, TQ already locked");
	return false;
}

//	---------------------------------------------------------
//	       unlockTransferQueue()
//	---------------------------------------------------------
bool aes_afp_transferqueue::unlockTransferQueue (std::string application, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("Unlock TQ %s, application = %s",
			(protocol_.getStringValue("TRANSFERQUEUE")).c_str(), application.c_str() );

	bool result(false);

	if (protocol_.getStringValue("APPLICATION") == application)
	{
		if (protocol_.getBoolValue("TQLOCK") == true)
		{
			protocol_.addBoolValue("TQLOCK", false);
			protocol_.addStringValue("APPLICATION", "");
			result = protocol_.writeToFile(error);
			AES_AFP_TRACE_MESSAGE("TQ unlocked OK");
			return result;
		}
		else
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to unlock, TQ not locked");
			AES_AFP_TRACE_MESSAGE("Not able to unlock, TQ not locked");
		}
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_DEBUG, "Not able to unlock, %s is not locking application", application.c_str() );
		AES_AFP_TRACE_MESSAGE("Not able to unlock, %s is not locking application", application.c_str() );
	}
	return false;
}

//	---------------------------------------------------------
//	       isTransferQueueLocked()
//	---------------------------------------------------------
bool aes_afp_transferqueue::isTransferQueueLocked (std::string& application)
{
	if (protocol_.getBoolValue("TQLOCK") == true)
	{
		application = protocol_.getStringValue("APPLICATION");
		AES_AFP_TRACE_MESSAGE("TQ is locked by %s",
				(protocol_.getStringValue("APPLICATION")).c_str() );
		return true;
	}
	else
	{
		application = "";
		AES_AFP_TRACE_MESSAGE("TQ is not locked");
		return false;
	}
}

//	---------------------------------------------------------
//	       transferQueueProtected()
//	---------------------------------------------------------
bool aes_afp_transferqueue::transferQueueProtected ()
{
	bool result(true);

	//  AES_GCC_FileDestProtection fileDestProtect("AES", "AFP");

	//  result = fileDestProtect.isProtected(protocol_.getStringValue("TRANSFERQUEUE") );
	return result;
}

//	---------------------------------------------------------
//	       changeFileStatus()
//	---------------------------------------------------------
bool aes_afp_transferqueue::changeFileStatus (AES_GCC_Filestates newState, std::string filename, AES_GCC_Errorcodes& error)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	AES_AFP_TRACE_MESSAGE("Changing status for %s in TQ %s File in transfer is %s",
			filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str(), transferFile_.c_str() );
	bool result(false);

	if (fileMap_.empty() == true)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "File map is empty" );
		AES_AFP_TRACE_MESSAGE("File map is empty");
		return result;
	}

	FileMap::iterator currentItr;
	StringMap::iterator timeItr;
	FileMap::iterator fileItr;
	std::string tmpDumpPath("");
	std::string tempPath("");

	currentItr = fileMap_.find(filename);

	if (currentItr != fileMap_.end() )
	{
		tmpDumpPath = filesDirectory_ + aes_afp_parameter::delimiter() + (*currentItr).first;
		tempPath = tempFilesDirectory_ + aes_afp_parameter::delimiter() + "temp";//(*currentItr).first;//hf99814

		timeItr = timeSortedMap_.find((*currentItr).second->getCreationDate() );


		switch (newState)
		{
		// New state DELETE ----------------------------------------------------------------------
		case AES_FSDELETE:

		{
			AES_AFP_TRACE_MESSAGE("New status %s", "DELETE");
			result = (*currentItr).second->setStatus(tempPath, tmpDumpPath, newState, error);
			if (result == false)
			{
				return result;
			}

			//#############################################################
			// Here should oldest "Ready" file be sent to cdh...
			//-------------------------------------------------------------
			// If nextFileToBeTransfered_Itr is older than lastFileToBeTransfered_Itr.
			// Then nextFileToBeTransfered_Itr is sent to CDH.
			// Else proceed if no other transfer.
			//#############################################################

			if( (nextFileToBeTransfered_Itr != fileMap_.end()) &&
					((*nextFileToBeTransfered_Itr).second->getStatus() == AES_FSREADY) &&
					((*lastFileToBeTransfered_Itr).second->getStatus() != AES_FSREADY) &&
					((*lastFileToBeTransfered_Itr).second->getStatus() != AES_FSSEND))
			{
				AES_AFP_TRACE_MESSAGE("AP time set back, sending new next file (%s) to cdh.", (*nextFileToBeTransfered_Itr).first.c_str());
				sendToCdh( (*nextFileToBeTransfered_Itr).first, (*nextFileToBeTransfered_Itr).second, error);
				break;

			}else if (timeItr != timeSortedMap_.end() )
			{
				++timeItr;

				while (timeItr != timeSortedMap_.end() )
				{
					fileItr = fileMap_.find( (*timeItr).second );
					if (fileItr != fileMap_.end() )
					{
						if ( (*fileItr).second->getStatus() == AES_FSREADY)
						{
							if (transferFile_ == filename || transferFile_.empty() )
							{
								sendToCdh( (*fileItr).first, (*fileItr).second, error);
								break;
							}
						}
						++timeItr;
					}
				}

				if (timeItr == timeSortedMap_.end() )
				{
					transferFile_ = "";
				}
			}
			break;
		}


		// New state REMOVE ----------------------------------------------------------------------
		case AES_FSREMOVE:
		{
			AES_AFP_TRACE_MESSAGE("New status REMOVE");
			result = (*currentItr).second->setStatus(tempPath, tmpDumpPath, newState, error);
			if (result == false)
			{
				return result;
			}

			if (timeItr != timeSortedMap_.end() )
			{
				++timeItr;

				while (timeItr != timeSortedMap_.end() )
				{
					fileItr = fileMap_.find( (*timeItr).second );
					if (fileItr != fileMap_.end() )
					{
						if ( (*fileItr).second->getStatus() == AES_FSREADY)
						{
							if (transferFile_ == filename || transferFile_.empty() )
							{
								sendToCdh( (*fileItr).first, (*fileItr).second, error);
								break;
							}
						}
						++timeItr;
					}
				}

				if (timeItr == timeSortedMap_.end() )
				{
					transferFile_ = "";
				}
			}
			break;

		}


		// New state SEND ----------------------------------------------------------------------
		case AES_FSSEND:
		{
			AES_AFP_TRACE_MESSAGE("New status SEND");
			result = (*currentItr).second->setStatus(tempPath, tmpDumpPath, newState, error);
			if (result == false)
			{
				return result;
			}

			break;
		}

		// New state FAILED ----------------------------------------------------------------------
		case AES_FSFAILED:
		{
			AES_AFP_TRACE_MESSAGE("New status FAILED");
			result = (*currentItr).second->setStatus(tempPath, tmpDumpPath, newState, error);
			//cease & raise with latest failed file
			if (protocol_.getBoolValue("ALARM") == true)
			{
				ceaseTransferFailed();
			}
			alarmTransferFailed(filename);

			if (result == false)
			{
				return result;
			}

			//#############################################################
			// Here should oldest "Ready" file be sent to cdh...
			//-------------------------------------------------------------
			// If nextFileToBeTransfered_Itr is older than lastFileToBeTransfered_Itr.
			// Then nextFileToBeTransfered_Itr is sent to CDH.
			// Else proceed if no other transfer.
			//#############################################################

			if( (nextFileToBeTransfered_Itr != fileMap_.end()) &&
					((*nextFileToBeTransfered_Itr).second->getStatus() == AES_FSREADY) &&
					((*lastFileToBeTransfered_Itr).second->getStatus() != AES_FSREADY) &&
					((*lastFileToBeTransfered_Itr).second->getStatus() != AES_FSSEND))
			{
				AES_AFP_TRACE_MESSAGE("AP time set back, sending new next file (%s) to cdh.", (*nextFileToBeTransfered_Itr).first.c_str());
				sendToCdh( (*nextFileToBeTransfered_Itr).first, (*nextFileToBeTransfered_Itr).second, error);
				break;

			}else if (timeItr != timeSortedMap_.end() )
			{
				++timeItr;

				while (timeItr != timeSortedMap_.end() )
				{
					fileItr = fileMap_.find( (*timeItr).second );
					if (fileItr != fileMap_.end() )
					{
						if ( (*fileItr).second->getStatus() == AES_FSREADY)
						{
							if (transferFile_ == filename || transferFile_.empty() )
							{

								sendToCdh( (*fileItr).first, (*fileItr).second, error);
								break;
							}
						}
						++timeItr;
					}
				}


				if (timeItr == timeSortedMap_.end() )
				{
					transferFile_ = "";
				}
			}
			break;
		}

		// New state READY ----------------------------------------------------------------------
		case AES_FSREADY:
		{
			AES_AFP_TRACE_MESSAGE("New status READY");
			if (timeItr != timeSortedMap_.end() )
			{
				--timeItr;
				if (timeItr != timeSortedMap_.end() )
				{
					result = (*currentItr).second->setStatus(tempPath, tmpDumpPath, newState, error);
					if (result == false)
					{
						return result;
					}

					if (timeItr != timeSortedMap_.begin() )
					{
						fileItr = fileMap_.find( (*timeItr).second );
						if (fileItr != fileMap_.end() )
						{

							if ( (*fileItr).second->getStatus() != AES_FSREADY)
							{
								AES_AFP_TRACE_MESSAGE("Previous %s has state %d in TQ %s",
										filename.c_str(), (*fileItr).second->getStatus(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
								if (transferFile_ == (*fileItr).first || transferFile_.empty() )
								{
									sendToCdh( (*currentItr).first, (*currentItr).second, error);
								}
							}
						}
					}
					else
					{
						fileItr = fileMap_.find( (*timeItr).second );
						if (fileItr != fileMap_.end() )
						{
							if ( (*fileItr).second->getStatus() != AES_FSREADY && (*fileItr).second->getStatus() != AES_FSSEND)
							{
								AES_AFP_TRACE_MESSAGE("%s changeFileStatus() File %s is not in state FS_READY or FSSEND Transfer file is %s, send file to CDH",
										(*fileItr).first.c_str(), (*currentItr).first.c_str(), transferFile_.c_str());
								if (transferFile_ == filename || transferFile_.empty() )
								{

									sendToCdh( (*fileItr).first, (*fileItr).second, error);
								}
							}
							else
							{
								AES_AFP_TRACE_MESSAGE("File %s is in state FS_READY or FSSEND, file is not sent to CDH",
										(*currentItr).first.c_str() );
							}
						}
					}


				}
				else // This was the first item in the list, check if it is to be sent to CDH
				{
					if ( (*currentItr).second->getStatus() != AES_FSREADY && (*currentItr).second->getStatus() != AES_FSSEND)
					{
						AES_AFP_TRACE_MESSAGE("One in list, file %s is not in state FS_READY or FSSEND, send file to CDH",
								(*currentItr).first.c_str() );
						result = (*currentItr).second->setStatus(tempPath, tmpDumpPath, newState, error);
						if (result == false)
						{
							return result;
						}


						sendToCdh( (*currentItr).first, (*currentItr).second, error);
					}
					else
					{
						AES_AFP_TRACE_MESSAGE("One in list, file %s is in state FS_READY or FSSEND, file is not sent to CDH",
								(*currentItr).first.c_str() );
					}
				}
			}
			break;
		}

		// New state ? ----------------------------------------------------------------------
		default:
		{
			AES_AFP_TRACE_MESSAGE("Default reached");
			result = (*currentItr).second->setStatus(tempPath, tmpDumpPath, newState, error);
			if (result == false)
			{
				return result;
			}
		}
		}
		// END SWITCH ---------------------------------------------------------------------------------


		result = protocol_.writeToFile(error);
		if (result == false)
		{
			return result;
		}

		if (newState == AES_FSSEND)
		{
			// Commit pers nr if remote renaming
			if (protocol_.getIntValue("RENAMETYPE") == AES_REMOTE)
			{
				if (templateObj_ !=0)
				{
					templateObj_->commitPersNr();
				}
			}
		}

		error = AES_NOERRORCODE;
		return true;
	} // end if
	else
	{
		AES_AFP_TRACE_MESSAGE("Senditem %s not found in TQ %s Tried to set new state = %d for a non existent file", filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str(), newState );
		//	if (newState != AES_FSREADY)
		if (newState != AES_FSSEND) 
		{
			AES_AFP_TRACE_MESSAGE("New state not READY, trying to find a file in status READY for TQ %s",
					(protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
			for (timeItr=timeSortedMap_.begin();timeItr!=timeSortedMap_.end();++timeItr)
			{
				fileItr = fileMap_.find( (*timeItr).second );
				if (fileItr != fileMap_.end() )
				{
					if ( (*fileItr).second->getStatus() == AES_FSREADY)
					{
						AES_AFP_TRACE_MESSAGE("Found %s with status READY in TQ %s, sending to CDH",
								(*fileItr).first.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
						sendToCdh( (*fileItr).first, (*fileItr).second, error);

						break;
					}
				}
			}
		}
	}
	error = AES_FILENOTFOUND;
	return false;

}

//	---------------------------------------------------------
//	       getSendItemNames()
//	---------------------------------------------------------
bool aes_afp_transferqueue::getSendItemNames (std::stringstream& streamBuffer, AES_GCC_Filestates state)
{
	FileMap::iterator itr;
	bool result(false);
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	if (!(fileMap_.empty()) )
	{
		result = true;
		for (itr=fileMap_.begin();itr != fileMap_.end();++itr)
		{
			if (state == AES_FSUSEDEFAULT)
			{
				streamBuffer << (*itr).first << "\n";
			}
			else
			{
				if ( (*itr).second->getStatus() == state )
				{
					streamBuffer << (*itr).first << "\n";
				}
			}
		}
	}
	return true;
}

//	---------------------------------------------------------
//	       transferFailedFile()
//	---------------------------------------------------------
bool aes_afp_transferqueue::transferFailedFile (std::string filename, AES_GCC_Errorcodes& error)
{
	FileMap::iterator itr;
	std::string tmpDumpPath("");
	std::string tempPath("");
	bool result(false);
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	itr = fileMap_.find(filename);
	if (itr != fileMap_.end() )
	{
		AES_AFP_TRACE_MESSAGE("Sending %s from TQ %s to CDH", filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		tmpDumpPath = filesDirectory_ + aes_afp_parameter::delimiter() + (*itr).first;
		tempPath = tempFilesDirectory_ + aes_afp_parameter::delimiter() + "temp";//(*itr).first;//hf99814
		result = (*itr).second->setStatus(tempPath, tmpDumpPath, AES_FSPENDING, error);
		if (result == false)
		{
			return result;
		}
		sendToCdh( (*itr).first, (*itr).second, error);
		if ( AES_NOERRORCODE != error )
			result = false;
		else
			result = true;
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Sending %s from TQ %s to CDH, file not found", filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		AES_AFP_TRACE_MESSAGE("Sending %s from TQ %s to CDH, file not found", filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		error = AES_FILENOTFOUND;
	}
	return result;
}

//	---------------------------------------------------------
//	       createTimeString()
//	---------------------------------------------------------
char* aes_afp_transferqueue::createTimeString (aes_afp_date_time& time, char* str)
{
	str[0] = 0;

	char tmpYear[8];
	char tmpMonth[8];
	char tmpDay[8];
	char tmpHour[8];
	char tmpMinute[8];
	char tmpSecond[8];
	char tmpMicrosec[8];

	unsigned short tmpShort(0);

	sprintf(tmpYear, "%d", time.getYear() );

	tmpShort = time.getMonth();
	if (tmpShort < 10)
	{
		sprintf(tmpMonth, "%d%d", 0, tmpShort);
	}
	else
	{
		sprintf(tmpMonth, "%d", tmpShort);
	}

	tmpShort = time.getDay();
	if (tmpShort < 10)
	{
		sprintf(tmpDay, "%d%d", 0, tmpShort);
	}
	else
	{
		sprintf(tmpDay, "%d", tmpShort);
	}

	tmpShort = time.getHour();
	if (tmpShort < 10)
	{
		sprintf(tmpHour, "%d%d", 0, tmpShort);
	}
	else
	{
		sprintf(tmpHour, "%d", tmpShort);
	}

	tmpShort = time.getMin();
	if (tmpShort < 10)
	{
		sprintf(tmpMinute, "%d%d", 0, tmpShort);
	}
	else
	{
		sprintf(tmpMinute, "%d", tmpShort);
	}

	tmpShort = time.getSec();
	if (tmpShort < 10)
	{
		sprintf(tmpSecond, "%d%d", 0, tmpShort);
	}
	else
	{
		sprintf(tmpSecond, "%d", tmpShort);
	}

	tmpShort = time.getSeq();
	if (tmpShort < 10)
	{
		sprintf(tmpMicrosec, "%d%d", 0, tmpShort);
	}
	else
	{
		sprintf(tmpMicrosec, "%d", tmpShort);
	}


	sprintf(str,"%s%s%s%s%s%s%s",
			tmpYear,
			tmpMonth,
			tmpDay,
			tmpHour,
			tmpMinute,
			tmpSecond,
			tmpMicrosec);

	return str;
}

//	---------------------------------------------------------
//	       transferFailed()
//	---------------------------------------------------------
bool aes_afp_transferqueue::transferFailed (AES_GCC_Errorcodes& error)
{
	bool firstTimeOnly(true);
	StringMap::iterator timeItr;
	FileMap::iterator itr;
	std::string tmpDumpPath("");
	std::string tempPath("");
	std::string tmpNewFullPath("");
	bool result(false);

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	AES_AFP_TRACE_MESSAGE("Sending failed files from TQ %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	if (fileMap_.empty())
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Sending failed files from TQ %s to CDH, no files to transfer, map is empty", protocol_.getCharStringValue("TRANSFERQUEUE") );
		AES_AFP_TRACE_MESSAGE("Sending failed files from TQ %s to CDH, no files to transfer, map is empty", protocol_.getCharStringValue("TRANSFERQUEUE") );
		error = AES_NOERRORCODE;
		result = true;
		return result;
	}
	//Cease tansfer failed alarm on TQ before manualTranasferOfFailedFiles
	if (protocol_.getBoolValue("ALARM") == true)
	{
		ceaseTransferFailed();
	}

	for (timeItr = timeSortedMap_.begin();timeItr != timeSortedMap_.end();++timeItr)
	{
		bool isFileFound(true);
		itr = fileMap_.find(timeItr->second);
		if (itr != fileMap_.end())
		{
			if ( (*itr).second->getStatus() == AES_FSFAILED)
			{
				tmpNewFullPath = getSourceDirPath((*itr).second);
				tmpNewFullPath += aes_afp_parameter::delimiter() + (*itr).first;
				ACE_stat fileData;
				if(!(ACE_OS::stat(tmpNewFullPath.c_str(),&fileData)))
				{
					if(!(fileData.st_mode & S_IFDIR) && (*itr).second->getDirectoryFlag() == true)
					{
						AES_AFP_TRACE_MESSAGE("File %s is not a directory, directory flag is set to true",((*itr).first).c_str());
						isFileFound = false;	
					}
				}
				else
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "file stat failed , errorno %d ",errno);
					AES_AFP_TRACE_MESSAGE("file stat failed , errorno %d ",errno);
					AES_AFP_LOG(LOG_LEVEL_ERROR, "File %s was not found at path %s", ((*itr).first).c_str() , tmpNewFullPath.c_str() );
					AES_AFP_TRACE_MESSAGE("File %s was not found at path %s", ((*itr).first).c_str() , tmpNewFullPath.c_str() );
					string newdr("");
					if (AES_GCC_Util::datapath_trn(tmpNewFullPath,AES_DATA_PATH,newdr)==false)
					{
						AES_AFP_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
						newdr = "Not Available";
					}
					Event::report(EVENT_FILENOTFOUND, "FIND FILE FAULT", newdr, "Not able to find file or directory: " + newdr);
					//error = AES_FILENOTFOUND;
					isFileFound = false;
				}
				if(isFileFound)
				{
					tmpDumpPath = filesDirectory_ + aes_afp_parameter::delimiter() + (*itr).first;
					tempPath = tempFilesDirectory_ + aes_afp_parameter::delimiter() + "temp";//(*itr).first;//hf99814
					result = (*itr).second->setStatus(tempPath, tmpDumpPath, AES_FSREADY, error);
					if (result == false)
					{
						return result;
					}

					if (firstTimeOnly == true)
					{
						if( (lastFileToBeTransfered_Itr != fileMap_.end()) && ((*lastFileToBeTransfered_Itr).second->getStatus() == AES_FSSEND))
						{
							AES_AFP_TRACE_MESSAGE("previous file in SEND state, iterator set as nextFileToBeTransfered_Itr");
							nextFileToBeTransfered_Itr = itr;
						}
						else
						{
							AES_AFP_TRACE_MESSAGE("previous file not in SEND state, sendToCdh called");
							sendToCdh( (*itr).first, (*itr).second, error);
							if( error != AES_NOERRORCODE)
							{
								return false;
							}
						}

						AES_AFP_TRACE_MESSAGE("%s Sending file %s to CDH, TQ %s", transferFile_.c_str(),(*itr).first.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
						if (protocol_.getIntValue("RENAMETYPE") == AES_REMOTE && templateObj_!=0)
						{
							templateObj_->commitPersNr();
						}
						firstTimeOnly = false;
					}
				}
			}
		}
	}

	AES_AFP_TRACE_MESSAGE("Transfering failed files OK for TQ %s",
			(protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
	error = AES_NOERRORCODE;

	return true;
}

//	---------------------------------------------------------
//	       getDirectory()
//	---------------------------------------------------------
std::string aes_afp_transferqueue::getDirectory ()
{
	AES_AFP_TRACE_MESSAGE("Source directory %s",
			(protocol_.getStringValue("SOURCEDIRECTORY")).c_str() );
	return protocol_.getStringValue("SOURCEDIRECTORY");
}

//	---------------------------------------------------------
//	       setDirectory()
//	---------------------------------------------------------
void aes_afp_transferqueue::setDirectory (std::string newDirectory)
{
	AES_AFP_TRACE_MESSAGE("Source directory %s", newDirectory.c_str() );
	protocol_.addStringValue("SOURCEDIRECTORY", newDirectory);
}

//	-------------------------------------------------
//	      openTransferQueue()
//	-------------------------------------------------
bool aes_afp_transferqueue::openTransferQueue (ACS_DSD_Session* dsdChannel, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	transferInProgress_ = true;
	applChannel_ = dsdChannel;
	applicationEventText_ = "";
	bool result = true;

	if(!m_reloadStatus)
	{
		//try reload TQ again from disk
		AES_AFP_LOG(LOG_LEVEL_INFO, "TQ %s not loaded from DISK, loading again",protocol_.getStringValue("TRANSFERQUEUE").c_str());
		AES_AFP_TRACE_MESSAGE("TQ %s not loaded from DISK, loading again",protocol_.getStringValue("TRANSFERQUEUE").c_str());
		svc();
	}

	if(m_reloadStatus)
	{
		if (noDestination_ == false)
		{
			AES_AFP_TRACE_MESSAGE("noDestination_ is false, attachApplication...");
			error = destTask_.attachApplication(protocol_.getStringValue("TRANSFERQUEUE"), applicationEventText_, applChannel_);
			AES_AFP_TRACE_MESSAGE("attachApplication return GCC error code: %d", error);
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("noDestination_ is true, already attached");
			error = AES_NOERRORCODE;
		}
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to load the TQ again ");
		AES_AFP_TRACE_MESSAGE("ERROR: Not able to load the TQ again");
		error = AES_NOSERVERACCESS;
	}

	if (error != AES_NOERRORCODE)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Attach failed to CDH for TQ %s. ", (protocol_.getStringValue("TRANSFERQUEUE")).c_str());
		AES_AFP_TRACE_MESSAGE("Attach failed to CDH for TQ %s. ", (protocol_.getStringValue("TRANSFERQUEUE")).c_str());
		sendApplEvent(AES_EVELINKDOWN, protocol_.getStringValue("DESTINATIONSET"), AES_NOSERVERACCESS);
		transferInProgress_ = false;//HE74701
		result = false;
	}

	AES_AFP_TRACE_MESSAGE("TQ %s attached to CDH",	(protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
	return result;
}

//	---------------------------------------------------------
//	       closeTransferQueue()
//	---------------------------------------------------------
bool aes_afp_transferqueue::closeTransferQueue ()
{
	AES_AFP_TRACE_MESSAGE("Entering");

	transferInProgress_ = false; //HE74701

	if (noDestination_ == false)
	{
		AES_AFP_TRACE_MESSAGE("TQ %s deattaching from CDH", protocol_.getCharStringValue("TRANSFERQUEUE") );
		destTask_.detachApplication();
	}
	return true;
}

//	---------------------------------------------------------
//	       sendFile()
//	---------------------------------------------------------
bool aes_afp_transferqueue::sendFile ( std::string filename,
		AES_GCC_Errorcodes& error,
		AES_GCC_Format mode,
		std::string mask,
		bool isDirectory,
		std::string originalFilename,
		AES_GCC_Filestates fileState)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	StringMap::iterator timeItr;
	FileMap::iterator fileItr;
	std::string sendItemGeneration("");
	AES_AFP_TRACE_MESSAGE("TQ = %s Destination = %s File = %s Mode %d Mask = %s Directory = %s Original filename = %s File state = %d", (protocol_.getStringValue("TRANSFERQUEUE")).c_str(), (protocol_.getStringValue("DESTINATIONSET")).c_str(), filename.c_str(), mode, mask.c_str(), isDirectory?"true":"false", originalFilename.c_str(), fileState);
	switch (protocol_.getIntValue("STATUS") )
	{
	case AES_FSREADY:
	{
		break;
	}
	case AES_FSSEND:
	{
		break;
	}
	case AES_FSARCHIVE:
	{
		break;
	}
	case AES_FSDELETE:
	{
		break;
	}
	case AES_FSFAILED:
	{
		return true;
	}
	case AES_FSUSEDEFAULT:
	{
		break;
	}
	case AES_FSSTOPPED:
	{
		return true;
	}
	case AES_FSPENDING:
	{
		break;
	}
	case AES_FSNONE:
	{
		break;
	}
	default:
	{
		return true;
	}
	}

	FileMap::iterator itr;

	std::string tmpNewFullPath("");
	std::string tmpStr("");
	bool boolResult(false);

	itr = fileMap_.find(filename);
	if (itr != fileMap_.end() )
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "File %s already reported to TQ %s and destination %s", filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str(), (protocol_.getStringValue("DESTINATIONSET")).c_str() );
		AES_AFP_TRACE_MESSAGE("File %s already reported to TQ %s and destination %s", filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str(), (protocol_.getStringValue("DESTINATIONSET")).c_str() );
		error = AES_SENDITEMEXIST;
		return boolResult;  // Send item already in map
	}

	tmpNewFullPath = protocol_.getStringValue("SOURCEDIRECTORY");
	tmpNewFullPath += aes_afp_parameter::delimiter() + filename;

	ACE_stat fileData;
	if(!(ACE_OS::stat(tmpNewFullPath.c_str(),&fileData)))
	{
		if(!(fileData.st_mode & S_IFDIR) && (isDirectory == true))
		{
			AES_AFP_LOG(LOG_LEVEL_INFO, "File %s is not a directory, directory flag = %s", filename.c_str(), isDirectory?"true":"false");
			AES_AFP_TRACE_MESSAGE("File %s is not a directory, directory flag = %s", filename.c_str(), isDirectory?"true":"false");
			error = AES_NOTDIRECTORY;
			return boolResult;

		}
	}
	else
	{
		AES_AFP_TRACE_MESSAGE("file stat failed , errorno %d ",errno);
		AES_AFP_LOG(LOG_LEVEL_ERROR, "File %s was not found at path %s", filename.c_str(), tmpNewFullPath.c_str() );
		AES_AFP_TRACE_MESSAGE("File %s was not found at path %s", filename.c_str(), tmpNewFullPath.c_str() );
		string newdr("");
		if (AES_GCC_Util::datapath_trn(tmpNewFullPath,AES_DATA_PATH,newdr)==false)
		{
			AES_AFP_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
			newdr = "Not Available";
		}
		Event::report(EVENT_FILENOTFOUND, "FIND FILE FAULT", newdr, "Not able to find file or directory: " + newdr);
		error = AES_FILENOTFOUND;
		return false;
	}

	if (fileState == AES_FSUSEDEFAULT)
	{
		fileState = (AES_GCC_Filestates)protocol_.getIntValue("STATUS");
	}

	aes_afp_file* newSendItemObj = aes_afp_objectfactory::Instance().makeFileObj();
	std::string discName;
	std::string tempDiscName;

	discName = filesDirectory_ + aes_afp_parameter::delimiter() + filename;
	tempDiscName = tempFilesDirectory_ + aes_afp_parameter::delimiter() + "temp";

	if (newSendItemObj)
	{
		fileMapPair_.first = filename;
		fileMapPair_.second = newSendItemObj;
		AES_AFP_TRACE_MESSAGE("calling init for file %s as it is a newSendItemObj",  filename.c_str());
		boolResult = newSendItemObj->init(tempDiscName,
				discName,
				fileState,
				error,
				protocol_.getStringValue("SOURCEDIRECTORY"),
				protocol_.getBoolValue("REMOVEBEFORE"),
				isDirectory,originalFilename,                                        //HU96961
				mask,
				mode);


		if (boolResult == false)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to insert %s in TQ %s", filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
			AES_AFP_TRACE_MESSAGE("Not able to insert %s in TQ %s", filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
			error = AES_CATASTROPHIC;
			delete newSendItemObj;
			newSendItemObj = 0;
			return boolResult;
		}

		fileMap_.insert(fileMapPair_);
		AES_AFP_TRACE_MESSAGE("Inserted in fileMap for file %s",  filename.c_str());
		timeSortedMapPair_.first = newSendItemObj->getCreationDate();

		timeSortedMapPair_.second = filename;

		for(;;)
		{
			std::pair<StringMap::iterator, bool> result = timeSortedMap_.insert(timeSortedMapPair_);

			if(result.second != false)
			{
				break;
			}

			timeSortedMapPair_.first = (newSendItemObj->nextSeqNrCreationDate() ); 
		}

		if (fileState == AES_FSREADY)
		{
			if(lastFileToBeTransfered_Itr != fileMap_.end()) //Also means that fileMap_.size() > 1
			{
				if((*lastFileToBeTransfered_Itr).second->isYounger(*newSendItemObj)) //True if clock has been set backwards.
				{							

					if( nextFileToBeTransfered_Itr == fileMap_.end()) //If there no queued file.
					{
						AES_AFP_TRACE_MESSAGE("lastFileToBeTransfered_Itr is younger than this file, nextFileToBeTransfered_Itr is set to %s", filename.c_str());
						string newdr("");
						if (AES_GCC_Util::datapath_trn(filename,AES_DATA_PATH,newdr)==false)
						{
							AES_AFP_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
							newdr = "Not Available";
						}
						Event::report(	EVENT_TIMECHANGED,
								"AP LOCAL TIME CHANGED BACKWARDS",
								"FILE: " + newdr + " is younger than FILE:" + (*lastFileToBeTransfered_Itr).first,
								"Local time has been changed backwards. AFP takes appropriate actions.");

						nextFileToBeTransfered_Itr = fileMap_.find(filename);

					}
					else if((*nextFileToBeTransfered_Itr).second->isYounger(*newSendItemObj))
					{
						AES_AFP_TRACE_MESSAGE("nextFileToBeTransfered_Itr is younger than this file, nextFileToBeTransfered_Itr is set to %s", filename.c_str());
						string newdr("");
						if (AES_GCC_Util::datapath_trn(filename,AES_DATA_PATH,newdr)==false)
						{
							AES_AFP_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
							newdr = "Not Available";
						}
						Event::report(	EVENT_TIMECHANGED,
								"AP LOCAL TIME CHANGED BACKWARDS",
								"FILE: " + newdr + " is younger than FILE:" + (*nextFileToBeTransfered_Itr).first,
								"Local time has been changed backwards. AFP takes appropriate actions.");


						nextFileToBeTransfered_Itr = fileMap_.find(filename);
					}

				}

				switch ((*lastFileToBeTransfered_Itr).second->getStatus()) //Status of last transfered file.
				{
				case AES_FSSEND:
				{
					AES_AFP_TRACE_MESSAGE( "State of previous is FS_SEND, this file is not sent to CDH");
					break;
				}
				case AES_FSREADY:
				{
					AES_AFP_TRACE_MESSAGE("State of previous is FS_READY, this file is not sent to CDH");
					break;
				}
				default:
				{
					AES_AFP_TRACE_MESSAGE("State of previous is other than FS_READY, FS_SEND, this file is sent to CDH");
					sendToCdh(filename, newSendItemObj, error);
					if ( error != AES_NOERRORCODE )
					{
						return false;
					}
					break;
				}
				}
			}
			else
			{
				sendToCdh(filename, newSendItemObj, error);
				if ( error != AES_NOERRORCODE ) 
				{
					return false;
				}
			}

		}
		else
		{
			lastFileToBeTransfered_Itr = fileMap_.find(filename);			
		}

		protocol_.addStringValue("ORIGINALFILENAME", originalFilename);
		protocol_.addStringValue("GENERATEDFILENAME", filename);
		int result = protocol_.writeToFile(error);
		if (result == false)
		{
			return result;
		}

		error = AES_NOERRORCODE;
		AES_AFP_TRACE_MESSAGE("File %s inserted in TQ %s", filename.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		return true;
	}
	error = AES_CATASTROPHIC;
	return false;
}

//	---------------------------------------------------------
//	       getApplChannel()
//	---------------------------------------------------------
ACS_DSD_Session* aes_afp_transferqueue::getApplChannel ()
{
	return applChannel_;
}

//	---------------------------------------------------------
//	       sendApplEvent()
//	---------------------------------------------------------
void aes_afp_transferqueue::sendApplEvent (AES_GCC_Eventcodes applEvent,
		std::string dataText,
		AES_GCC_Errorcodes error)
{
	AES_AFP_TRACE_MESSAGE("Sending event %s to application", dataText.c_str());

	char sendBuff[2048] = { 0 };
	int msgLen(0);

	sprintf(sendBuff, "%s%d\n%d\n%d\n%d\n%d\n%s\n", "rpch", 1, EVENT, applEvent, error, 1, dataText.c_str() );
	msgLen = strlen(sendBuff);

	aes_afp_apcom_serv::afpMutex.acquire();
	applChannel_ = aes_afp_dsdsessionmap::Instance().searchSession(protocol_.getStringValue("TRANSFERQUEUE") );
	if (applChannel_ != 0)
	{
		applChannel_->send((void*)sendBuff, msgLen);
	}
	else
	{
		destTask_.detachApplication();
	}
	aes_afp_apcom_serv::afpMutex.release();

}

//	---------------------------------------------------------
//	       getLastReportedFileOrig()
//	---------------------------------------------------------
std::string aes_afp_transferqueue::getLastReportedFileOrig ()
{
	return protocol_.getStringValue("ORIGINALFILENAME");
}

//	---------------------------------------------------------
//	       getLastReportedFileGen()
//	---------------------------------------------------------
std::string aes_afp_transferqueue::getLastReportedFileGen ()
{
	return protocol_.getStringValue("GENERATEDFILENAME");
}

//	---------------------------------------------------------
//	       getStatusFile()
//	---------------------------------------------------------
bool aes_afp_transferqueue::getStatusFile (std::string filename, AES_GCC_Filestates& status)
{
	bool result = false;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	FileMap::iterator itr = fileMap_.find(filename);
	if (itr != fileMap_.end() )
	{
		status = (*itr).second->getStatus();
		result = true;
	}

	return result;
}

//	---------------------------------------------------------
//	       getRenameFile()
//	---------------------------------------------------------
AES_AFP_Renametypes aes_afp_transferqueue::getRenameFile ()
{
	return (AES_AFP_Renametypes)protocol_.getIntValue("RENAMETYPE");
}

//	---------------------------------------------------------
//	       localRenameFile()
//	---------------------------------------------------------
bool aes_afp_transferqueue::localRenameFile (std::string filename, std::string& renamedFile)
{
	std::string tmpNewFullPath("");
	std::string tmpFileName("");
	bool result(false);
	tmpNewFullPath = protocol_.getStringValue("SOURCEDIRECTORY");
	tmpNewFullPath += aes_afp_parameter::delimiter() + filename;
	renamedFile = templateObj_->createFileName(tmpNewFullPath);
	//AES_AFP_LOG(LOG_LEVEL_INFO, "Original File Name = %s, RenamedFile = %s",tmpNewFullPath.c_str(),renamedFile.c_str());
	result = templateObj_->renameFileLocal(tmpNewFullPath, renamedFile);

	AES_AFP_TRACE_MESSAGE("Renaming file %s to %s for TQ %s", filename.c_str(), renamedFile.c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	if (result == false)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to rename file");
		AES_AFP_TRACE_MESSAGE("Not able to rename file");
		eventFileRename(filename);
		renamedFile = filename;
		return false;
	}

	templateObj_->commitPersNr();
	return true;
}

//	---------------------------------------------------------
//	       getTemplate()
//	---------------------------------------------------------
std::string aes_afp_transferqueue::getTemplate ()
{
	AES_AFP_TRACE_MESSAGE("Template %s", (protocol_.getStringValue("RENAMETEMPLATE")).c_str() );
	return protocol_.getStringValue("RENAMETEMPLATE");
}

//	---------------------------------------------------------
//	       getManual()
//	---------------------------------------------------------
bool aes_afp_transferqueue::getManual ()
{
	return protocol_.getBoolValue("MANUALINITIATED");
}

//	---------------------------------------------------------
//	       setManual()
//	---------------------------------------------------------
void aes_afp_transferqueue::setManual ()
{
	protocol_.addBoolValue("MANUALINITIATED", true);
}

//	---------------------------------------------------------
//	       getSendItemDirectoryFlag()
//	---------------------------------------------------------
bool aes_afp_transferqueue::getSendItemDirectoryFlag (std::string sendItemName)
{
	bool result = false;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	FileMap::iterator itr = fileMap_.find(sendItemName);
	if (itr != fileMap_.end() )
	{
		result = (*itr).second->getDirectoryFlag();
	}

	return result;
}

//	---------------------------------------------------------
//	       getTqObjectDirectory()
//	---------------------------------------------------------
// Returns the transferqueue object directory path. For TR_HJ10892
std::string aes_afp_transferqueue::getTqObjectDirectory()
{
	return protocol_.getStringValue("TQOBJECTDIRECTORY");
}

//	---------------------------------------------------------
//	       getNextFilename()
//	---------------------------------------------------------
bool aes_afp_transferqueue::getNextFilename (std::string sendItem, std::string& nextFilename)
{
	FileMap::iterator itr;
	StringMap::iterator timeitr;
	bool result =  false;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	if (!fileMap_.empty())
	{
		
		for (timeitr=timeSortedMap_.begin();timeitr!=timeSortedMap_.end();++timeitr)
		{
			if ( (*timeitr).second == sendItem)
			{
				++timeitr;
				if (timeitr != timeSortedMap_.end() )
				{
					nextFilename = (*timeitr).second;
					result = true;
				}
				break;
			}
		}
	}
	return result;
}

//	---------------------------------------------------------
//	       getUserGroup()
//	---------------------------------------------------------
std::string aes_afp_transferqueue::getUserGroup ()
{
	return protocol_.getStringValue("USERGROUP");
}

void aes_afp_transferqueue::sendHangupToCdh_stop ()
{
	aes_afp_msgblock* cdhManageMessage;
	//int result(0);

	cdhManageMessage = new aes_afp_msgblock(0, 0);
	cdhManageMessage->setMsgType(HANGUP);
	cdhManageMessage->setCallerName(protocol_.getStringValue("TRANSFERQUEUE") );
	cdhManageMessage->setCallerId(this);

	AES_AFP_TRACE_MESSAGE("Sending hangup for destination set %s from TQ %s", (protocol_.getStringValue("DESTINATIONSET")).c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
	cdhManageQueue_->enqueue_head(cdhManageMessage);

	threadManager_->wait();

	AES_AFP_TRACE_MESSAGE("CDH thread finished for destination set %s, TQ %s", (protocol_.getStringValue("DESTINATIONSET")).c_str(), (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
}

//	---------------------------------------------------------
//	       removeSourceDirectoryAll()
//	---------------------------------------------------------
bool aes_afp_transferqueue::removeSourceDirectoryAll (AES_GCC_Errorcodes& error)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);
	AES_AFP_TRACE_MESSAGE("Removing SourceDirectory for TQ %s", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );

	if (getLastFileInTransfer() == AES_FSSEND ||  getNextFileInTransfer() == AES_FSSEND)
	{
		error = AES_TQISOPEN;
		return false;
	}
	/*if (transferInProgress_)//HE74701
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "TQ %s is open", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		AES_AFP_TRACE_MESSAGE("TQ %s is open", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		error = AES_TQISOPEN;
		return false;
	}*/

	if (protocol_.getBoolValue("TQLOCK") == true)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s is protected", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		AES_AFP_TRACE_MESSAGE("TQ %s is protected", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		error = AES_FILEISPROTECTED;

		return false;
	}
	FileMap::iterator itr;
	std::map<std::string, std::string>::iterator timeItr;
	std::string tmpRemoveSendItemsPath;
	std::string tmpRemoveSrcDirPath;

	bool result(false);
	bool tempresult(false);

	try
	{
		if (!fileMap_.empty() )
		{
			for (itr = fileMap_.begin(); itr != fileMap_.end();)
			{
				if(isShutdownSignaled_)    // TR HW88888
					return false;				

				tmpRemoveSendItemsPath = filesDirectory_ + aes_afp_parameter::delimiter() + (*itr).first;
				tempresult = (*itr).second->removeFile(tmpRemoveSendItemsPath , error);
				if (tempresult)// remove if no error with senditem deletion
				{
					if(!isAuditLogSourceDir())
					{
						tmpRemoveSrcDirPath = getSourceDirPath((*itr).second) + aes_afp_parameter::delimiter() + (*itr).first;
						result = (*itr).second->removeFile(tmpRemoveSrcDirPath , error);
					}
					else result = true; 
				}
				if (result == false || tempresult == false)
				{
					if(result == false)
					{
						AES_AFP_LOG(LOG_LEVEL_INFO,"Source file %s not removed", ((*itr).first).c_str());
						AES_AFP_TRACE_MESSAGE("Source file %s not removed", ((*itr).first).c_str());
					}
					if(tempresult == false)
					{
						AES_AFP_LOG(LOG_LEVEL_INFO,"Send Item %s not removed", ((*itr).first).c_str());
						AES_AFP_TRACE_MESSAGE("Send Item %s not removed", ((*itr).first).c_str());
					}
					if (error == AES_NOACCESS)
					{
						AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove %s, access fault in file system",
								protocol_.getCharStringValue("TRANSFERQUEUE") );
						AES_AFP_TRACE_MESSAGE("Not able to remove %s, access fault in file system",
								protocol_.getCharStringValue("TRANSFERQUEUE") );
						eventNoAccessAfpFile((*itr).first);
						return false;
					}
				}
				aes_afp_objectfactory::Instance().relFileObj((*itr).second);
				fileMap_.erase(itr++);

			}
		}
		StringMap::iterator timeItr;
		for( timeItr = timeSortedMap_.begin(); timeItr != timeSortedMap_.end(); )
		{
			if(isShutdownSignaled_) return false; // TR HW88888

			timeSortedMap_.erase( timeItr++ );
		}
		timeSortedMap_.clear();

		fileMap_.clear();
		//Need to handle multiple source directories
		std::string tqSourcedir = protocol_.getStringValue("SOURCEDIRECTORY");

		if(isAuditLogSourceDir())
		{
			AES_AFP_LOG(LOG_LEVEL_INFO, "transfer queue source directory cannot be deleted for ALOG %s", tqSourcedir.c_str());
			AES_AFP_TRACE_MESSAGE("transfer queue source directory cannot be deleted for ALOG %s", tqSourcedir.c_str());
		}
		else if (!isSourceDir())
		{
			AES_AFP_LOG(LOG_LEVEL_INFO, "transfer queue source directory cannot be deleted for manual transfer of entity %s", tqSourcedir.c_str());
			AES_AFP_TRACE_MESSAGE("transfer queue source directory cannot be deleted for manual transfer of entity %s", tqSourcedir.c_str());
		}	
		else
		{	
			if( dirDelete(tqSourcedir ))
			{
				AES_AFP_LOG(LOG_LEVEL_INFO, "Deleted transfer queue source directory %s", tqSourcedir.c_str());
				AES_AFP_TRACE_MESSAGE("Deleted transfer queue source directory %s", tqSourcedir.c_str());
			}
			else
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Error occurred while deleting transfer queue source direcotry %s", tqSourcedir.c_str());
				AES_AFP_TRACE_MESSAGE("Error occurred while deleting transfer queue source direcotry %s", tqSourcedir.c_str());
			}
		}
		tqSourcedir = "";

	}catch (...)
	{}

	if (noDestination_ == false)
	{
		destTask_.set_remove_tq(true);
		sendHangupToCdh();
	}
	//Remove SENDITEMS directory
	result = remTqFromFile(error);
	if (result == false)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove %s, access fault in file system", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		AES_AFP_TRACE_MESSAGE("Not able to remove %s, access fault in file system", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
		error = AES_NOACCESS;
		return false;
	}
	error = AES_NOERRORCODE;

	if (protocol_.getBoolValue("ALARM") == true)
	{
		ceaseTransferFailed();
	}

	AES_AFP_TRACE_MESSAGE("TQ %s removed OK", (protocol_.getStringValue("TRANSFERQUEUE")).c_str() );
	return true;

}

//	---------------------------------------------------------
//	       removeFileFromSourceDirectory()
//	---------------------------------------------------------
bool aes_afp_transferqueue::removeFileFromSourceDirectory (std::string filename,
		AES_GCC_Errorcodes& error)		//for APZ21230/5-758
{
	AES_AFP_TRACE_MESSAGE("Removing file %s", filename.c_str() );
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	if (protocol_.getBoolValue("TQLOCK") == true) // check if TQ locked
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "File %s is protected", filename.c_str() );
		AES_AFP_TRACE_MESSAGE("File %s is protected", filename.c_str() );
		error = AES_FILEISPROTECTED;
		return false;
	}

	FileMap::iterator fileItr;
	StringMap::iterator timeItr;
	std::string tmpRemoveSourceDirPath("");
	std::string tmpRemoveSendItemsPath("");
	bool result(false);
	bool tempresult(false);
	if (fileMap_.empty() ) //file not reported
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "File map is empty");
		AES_AFP_TRACE_MESSAGE("File map is empty");
		error = AES_SENDITEMNOTREP;
		return result;
	}
	fileItr = fileMap_.find(filename);

	try
	{
		if (fileItr == fileMap_.end() )//file not reported
		{
			AES_AFP_TRACE_MESSAGE("File %s is not reported", filename.c_str() );
			error = AES_SENDITEMNOTREP;
			return false;
		}
		else
		{
			tmpRemoveSourceDirPath = getSourceDirPath((*fileItr).second) + aes_afp_parameter::delimiter() + (*fileItr).first;
			tmpRemoveSendItemsPath = filesDirectory_ + aes_afp_parameter::delimiter() + (*fileItr).first;

			if ( (*fileItr).second->getStatus() == AES_FSSEND)
			{
				std:: string tmpFullPath("");
				tmpFullPath = filesDirectory_;
				tmpFullPath += '/';
				tmpFullPath += (*fileItr).first;

				sendStopFileToCdh(tmpFullPath);
			}

			result = (*fileItr).second->removeFile(tmpRemoveSendItemsPath, error);

			if(result) // remove Srcdir if no error with senditem
			{
				tempresult = (*fileItr).second->removeFile(tmpRemoveSourceDirPath, error);
			}
			if (result == false || tempresult == false)
			{
				if (error == AES_NOACCESS)
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "Could not remove %s, no access to file", filename.c_str() );
					AES_AFP_TRACE_MESSAGE("Could not remove %s, no access to file", filename.c_str() );
					eventNoAccessAfpFile(protocol_.getStringValue("TRANSFERQUEUE") );
					return false;
				}
				if (error == AES_FILENOTFOUND)
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "File %s not found", filename.c_str() );
					AES_AFP_TRACE_MESSAGE("File %s not found", filename.c_str() );
				}
			}

			timeItr = timeSortedMap_.find((*fileItr).second->getCreationDate() );

			if(timeItr != timeSortedMap_.end()) //erase entry in timesorted map
			{
				timeSortedMap_.erase(timeItr);
			}

			aes_afp_objectfactory::Instance().relFileObj((*fileItr).second);
			if(fileItr == lastFileToBeTransfered_Itr)
			{
				lastFileToBeTransfered_Itr = fileMap_.end();			  
			}

			if(fileItr == nextFileToBeTransfered_Itr)
			{
				nextFileToBeTransfered_Itr = fileMap_.end();			  
			}

			fileMap_.erase(fileItr);
			protectTransferQueue();
			error = AES_NOERRORCODE;

			AES_AFP_TRACE_MESSAGE("File %s removed OK", filename.c_str() );
			return true;
		}

	}
	catch (...)
	{}

	return false;
}

//	---------------------------------------------------------
//	       restoreFromTmpFile()				- HI14735
//	---------------------------------------------------------

// Restore the configuration/state file of a file based transfer queue from the temporary copy
bool aes_afp_transferqueue::restoreFromTmpFile(std::string sourceFilePath,std::string targetFilePath)
{

	char *memblock;
	int num_of_bytes = 7563;	//any user specific number of bytes

	try
	{
		//source file
		ifstream src(sourceFilePath.c_str(),ios::in | ios::binary);

		//dest file
		ofstream dest(targetFilePath.c_str(),ios::trunc | ios::binary);

		//if we couldn't open the source file (it may not exist), means fail
		if(!src.is_open() || !dest.is_open() )
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Failed to open the files.");
			AES_AFP_TRACE_MESSAGE("Failed to open the files.");
			return false;
		}
		AES_AFP_TRACE_MESSAGE("The file successfully opened.");
		memblock = new char [num_of_bytes];

		while(true)
		{
			//reading "num_of_bytes" byte(s) from sourcefile
			src.read(memblock, num_of_bytes);

			//counting number of bytes actually read
			//using gcount() function
			num_of_bytes = src.gcount();

			//if zero bytes read, means End Of File
			//has reached, no bytes left to read
			if(num_of_bytes == 0)	break;

			//writing "num_of_bytes" byte(s) to destination file
			dest.write(memblock,num_of_bytes);
		}

		src.clear();	
		src.close();
		dest.clear();
		dest.close();

		delete[] memblock;/****************check once**************/
		memblock = 0;

		AES_AFP_TRACE_MESSAGE("The file was successfully copied.");

	}catch(...)
	{
		if(memblock)
			delete[] memblock;
		return false;
	}
	return true;

}

bool aes_afp_transferqueue::getTqDn(const std::string & tq, 
		std::string & tqDn)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	tqDn = "";
	OmHandler myOmHandler;
	if ( myOmHandler.Init() == ACS_CC_FAILURE )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Error occurred in omhandler initialize.");
		AES_AFP_TRACE_MESSAGE(" Error occurred in omhandler initialize.");
		return false;
	}

	string mytqDn = TQ_FILETQRDN;
	mytqDn += "=";
	mytqDn += tq;
	mytqDn += ",";
	mytqDn += AES_GCC_Util::dnOfFileTransferM;

	ACS_APGCC_ImmObject tqObj;
	tqObj.objName = mytqDn;

	if( myOmHandler.getObject(&tqObj) == ACS_CC_FAILURE )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Error occurred in getting TQ object from IMM");
		AES_AFP_TRACE_MESSAGE("Error occurred in getting TQ object from IMM");
		AES_AFP_TRACE_MESSAGE("Leaving");
		myOmHandler.Finalize();
		return false;
	}

	myOmHandler.Finalize();

	tqDn = mytqDn;
	AES_AFP_TRACE_MESSAGE("Leaving");
	return true;
}
void aes_afp_transferqueue::extractClassNmFromRdn( const string myObjRdn,string &myClassName)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	myClassName = "";
	string::size_type pos1 = string::npos;

	pos1 = myObjRdn.find_first_of("=");
	if ( pos1 != string::npos )
	{
		myClassName = myObjRdn.substr(0, pos1-2);
	}
	AES_AFP_TRACE_MESSAGE("Leaving");
}

bool aes_afp_transferqueue::dirDelete(std::string path)
{
	bool result = true;
	boost::filesystem::path dir_path(path);
	if(exists(dir_path))
	{
		try
		{	uintmax_t num_of_deleted_items = remove_all(dir_path);
			AES_AFP_TRACE_MESSAGE("deleted <%lu> items from directory <%s>", num_of_deleted_items, path.c_str());
			AES_AFP_LOG(LOG_LEVEL_INFO, "deleted <%lu> items from directory <%s>", num_of_deleted_items, path.c_str());
		}
		catch(const boost::filesystem::filesystem_error& ex)
		{
			AES_AFP_TRACE_MESSAGE("ERROR: cannot delete directory: <%s>. Error: <%s>", path.c_str(), ex.what());
			AES_AFP_LOG(LOG_LEVEL_ERROR, "cannot delete directory: <%s>. Error: <%s>", path.c_str(), ex.what());
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Creating empty directory: <%s> again!!",path.c_str());
			result = false;
			eventNoAccessAfpDir(path);
		}
	}
	else
	{
		AES_AFP_TRACE_MESSAGE("Directory <%s> not exists", path.c_str());
		AES_AFP_LOG(LOG_LEVEL_INFO, "Directory <%s> not exists", path.c_str());
	}
	return result;
}
bool aes_afp_transferqueue::createDirectoryStructure(AES_GCC_Errorcodes& error)
{

	// Create transfer queue_destinationset directory in data path for storing config file
	int result(0);
	int resultRemoveDir = 0;
	string 	tempObjPath = protocol_.getCharStringValue("TQOBJECTDIRECTORY");
	struct stat stat_buff;
	if(stat(tempObjPath.c_str(),&stat_buff)!= -1)
	{
		AES_AFP_TRACE_MESSAGE("Directory %s already exists , removing the directory",tempObjPath.c_str());
		dirDelete(tempObjPath);
	}
	result = ACE_OS::mkdir(tempObjPath.c_str());
	if (result == -1)
	{
		if (errno == EEXIST)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Directory %s already exists",
					(protocol_.getStringValue("TQOBJECTDIRECTORY")).c_str() );
			AES_AFP_TRACE_MESSAGE("Directory %s already exists",
					(protocol_.getStringValue("TQOBJECTDIRECTORY")).c_str() );
		}
		else
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Directory %s could not be created, trying to remove it", (protocol_.getStringValue("TQOBJECTDIRECTORY")).c_str() );
			AES_AFP_TRACE_MESSAGE("Directory %s could not be created, trying to remove it", (protocol_.getStringValue("TQOBJECTDIRECTORY")).c_str() );
			resultRemoveDir = ACE_OS::rmdir(protocol_.getStringValue("TQOBJECTDIRECTORY").c_str() );
			if (resultRemoveDir != 0)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ Directory  could not be removed as %d",  errno );
				AES_AFP_TRACE_MESSAGE("TQ Directory  could not be removed as %d",  errno );
			}
			eventNoAccessAfpDir(protocol_.getStringValue("TQOBJECTDIRECTORY") );
			error = AES_NOACCESS;
			return false;
		}
	}

	// Create files directory under transfer queue_destinationset directory
	result = ACE_OS::mkdir(filesDirectory_.c_str() );
	if (result == -1)
	{
		if (errno == EEXIST)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Senditems directory already exists");
			AES_AFP_TRACE_MESSAGE("Senditems directory already exists");
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("Could not create senditems dir, access fault");
			eventNoAccessAfpDir(filesDirectory_);
			resultRemoveDir = ACE_OS::rmdir(filesDirectory_.c_str() );
			if(resultRemoveDir != 0)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Directory  could not be removed as %d",  errno );
				AES_AFP_TRACE_MESSAGE("Directory  could not be removed as %d",  errno );
			}
			if(ACE_OS::rmdir(protocol_.getCharStringValue("TQOBJECTDIRECTORY") )!= 0)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Directory TQ OBJ could not be removed as %d", errno );
				AES_AFP_TRACE_MESSAGE("Directory TQ OBJ could not be removed as %d", errno );
			}
			error = AES_NOACCESS;
			return false;
		}
	}

	// Create directory for temporary transfer queue_destinationset in data area
	if(stat(tempDirectory_.c_str(),&stat_buff)!= -1)
	{
		AES_AFP_TRACE_MESSAGE("Directory %s already exists , removing the directory",tempDirectory_.c_str());
		dirDelete(tempDirectory_);
	}
	result = ACE_OS::mkdir(tempDirectory_.c_str());
	if (result == -1)
	{
		if (errno == EEXIST)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Directory %s already exists",
					tempDirectory_.c_str());
			AES_AFP_TRACE_MESSAGE("Directory %s already exists",
					tempDirectory_.c_str());
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("Directory %s could not be created, trying to remove it", tempDirectory_.c_str() );
			resultRemoveDir = ACE_OS::rmdir(tempDirectory_.c_str() );
			if(resultRemoveDir != 0)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "temp Directory  could not be removed as %d",  errno );
				AES_AFP_TRACE_MESSAGE("temp Directory  could not be removed as %d",  errno );
			}

			resultRemoveDir = ACE_OS::rmdir(filesDirectory_.c_str() );
			if(resultRemoveDir != 0)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "files Directory  could not be removed as %d", errno );
				AES_AFP_TRACE_MESSAGE("files Directory  could not be removed as %d", errno );
			}
			resultRemoveDir = ACE_OS::rmdir(protocol_.getCharStringValue("TQOBJECTDIRECTORY") );
			if(resultRemoveDir != 0)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "TQOBJECT Directory  could not be removed as %d", errno );
				AES_AFP_TRACE_MESSAGE("TQOBJECT Directory  could not be removed as %d", errno );
			}
			eventNoAccessAfpDir(tempDirectory_);
			error = AES_NOACCESS;
			return false;
		}
	}

	// Create directory for temporary files under temporary transfer queue_destinationset
	result = ACE_OS::mkdir(tempFilesDirectory_.c_str() );
	if (result == -1)
	{
		if (errno == EEXIST)
		{
			AES_AFP_TRACE_MESSAGE("Temporary senditems directory already exists");
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("Could not create temporary senditems dir, access fault");
			eventNoAccessAfpDir(tempFilesDirectory_);
			if((ACE_OS::rmdir(tempFilesDirectory_.c_str()) )!=0)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "temp Files Directory  could not be removed as %d",  errno );
				AES_AFP_TRACE_MESSAGE("temp Files Directory  could not be removed as %d",  errno );
			}
			if((ACE_OS::rmdir(tempDirectory_.c_str() )) !=0 )
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "temp Directory  could not be removed as %d", errno );
				AES_AFP_TRACE_MESSAGE("temp Directory  could not be removed as %d", errno );
			}
			if((ACE_OS::rmdir(filesDirectory_.c_str() )) != 0)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "files Directory  could not be removed as %d", errno );
				AES_AFP_TRACE_MESSAGE("files Directory  could not be removed as %d", errno );
			}
			if((ACE_OS::rmdir(protocol_.getCharStringValue("TQOBJECTDIRECTORY") )) != 0)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "TQOBJECT Directory could not be removed as %d", errno );
				AES_AFP_TRACE_MESSAGE("TQOBJECT Directory could not be removed as %d", errno );
			}
			error = AES_NOACCESS;
			return false;
		}
	}
	return true;
}

bool aes_afp_transferqueue::isSourceDir()
{
	bool ret = false;	
	string transferQ1 = getName();
	AES_AFP_TRACE_MESSAGE("transferQ1 is = %s ",transferQ1.c_str());
	string sourceDir = protocol_.getStringValue("SOURCEDIRECTORY");
	AES_AFP_TRACE_MESSAGE("source directroy is = %s ",sourceDir.c_str());	
	size_t index = sourceDir.find(transferQ1);
	if (index != std::string::npos)
	{
		ret = true;
	}
	return ret;
}

bool aes_afp_transferqueue::isAuditLogSourceDir()
{
	ACS_APGCC_DNFPath_ReturnTypeT returnType = ACS_APGCC_DNFPATH_FAILURE;
	ACS_APGCC_CommonLib objCommonLib;
	string auditLogsymName = AUDITLOG_SYM_NAME;
	if(auditLogDirPath.empty())
	{	
		AES_AFP_TRACE_MESSAGE("Fetching audit log directory path from IMM" );
		char folderPath[1024] = { 0 };
		int folderPathLen = 1024;       
		returnType = objCommonLib.GetFileMPath(auditLogsymName.c_str(),
				folderPath,
				folderPathLen );

		if( returnType == ACS_APGCC_DNFPATH_SUCCESS )
		{
			auditLogDirPath = folderPath;
		}
		else
		{ 
			AES_AFP_LOG(LOG_LEVEL_ERROR,"Failed to get the directory path" );
			AES_AFP_TRACE_MESSAGE("Failed to get the directory path" );
			return false;
		}
	}
	std::string sourceDirPath = protocol_.getStringValue("SOURCEDIRECTORY");
	if(!auditLogDirPath.empty())
	{
		size_t pos = sourceDirPath.find(auditLogDirPath);
		if(pos != std::string::npos)
		{
			AES_AFP_TRACE_MESSAGE("the source directory is %s ",sourceDirPath.c_str());
			return true;
		}
	}
	return false;
}
bool aes_afp_transferqueue::initCdhTask(AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	if (noDestination_ == false)
	{
		int openRepeat = 0;
		int  ret = AES_NOERRORCODE;
		cdhManageQueue_ = destTask_.askForManageQueue();
		while(openRepeat < MAX_RETRY_ON_DEST_OPEN)
		{
			if(aes_afp_services::afpStopEvt == true)
			{
				break;
			}
			ret = destTask_.open((void*)this);

			if(ret == AES_NOERRORCODE)
			{
				AES_AFP_LOG(LOG_LEVEL_INFO, "cdhTask thread started for TQ, %s and destination set, %s ",protocol_.getStringValue("TRANSFERQUEUE").c_str(), protocol_.getStringValue("DESTINATIONSET").c_str());
				break;
			}
			else
			{
				openRepeat++;
				sleep(1);
			}
		}
		if(ret != AES_NOERRORCODE)
		{
			error = AES_NOCDHSERVER;
			AES_AFP_LOG(LOG_LEVEL_ERROR, "NOT OK");
			AES_AFP_TRACE_MESSAGE("NOT OK and leaving");
			return false;
		}
	}
	else
	{
		AES_AFP_TRACE_MESSAGE("destiantionset is empty (-)");
	}
	AES_AFP_TRACE_MESSAGE("Leaving");
	return true;
}

std::string aes_afp_transferqueue::getSourceDirPath(aes_afp_file * fileObj) 
{
	std::string tmpSrcDirPath  = fileObj->getSourceDirPath();
	if(tmpSrcDirPath.empty())
	{
		tmpSrcDirPath = protocol_.getStringValue("SOURCEDIRECTORY");
		AES_AFP_TRACE_MESSAGE("SOURCEDIRECTORY path is empty in status file, path from config file is %s",tmpSrcDirPath.c_str());
	}
	return tmpSrcDirPath;
}

// TR HW88888
bool aes_afp_transferqueue::checkFileStatus()
{
	AES_AFP_TRACE_FUNCTION;
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(synchOnFileMap);

	if (fileMap_.empty() == true)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "File map is empty" );
		AES_AFP_TRACE_MESSAGE("File map is empty");
		return true;
	}

	AES_GCC_Filestates fileState;
	for (FileMap::iterator itr = fileMap_.begin();itr != fileMap_.end();++itr)
	{
		fileState = (*itr).second->getStatus(); 
		if(isShutdownSignaled_ || fileState == AES_FSREADY || fileState == AES_FSSEND)
			return false;
	}

	return true;
}

