//   Read the documentation to learn more about C++ code generator
//   versioning.
//	INCLUDE aes_afp_def.h
//
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

#include "aes_afp_def.h"
#include "aes_afp_services.h"
#include <acs_apgcc_error.h>
#include "aes_afp_api_tracer.h"
AES_AFP_TRACE_DEFINE(aes_afp_Def);

bool isOptPresent = false; /*flag to check if the optional params are specified or not*/
//------------------------------------------------------------------------------
//	main ()
//------------------------------------------------------------------------------

int
main (int argc, char *argv [])
{
	aes_afp_def def (argc, argv);
	return def.launch ();
}
//	---------------------------------------------------------
//	       Constructor()
//	---------------------------------------------------------
aes_afp_def::aes_afp_def (int argc, char** argv)
: transferQueue_(""),
  destinationSet_(""),
  initdestinationSet_(""),
  respdestinationSet_(""),
  remBeforeString_(""),
  defaultStatus_(AES_FSREADY),
  removeDelay_(DEFAULT_REMOVEDELAY_),
  removeBefore_(false),
  sendRetries_(DEFAULT_NROFRETRIES_),
  sendRetriesDelay_(DEFAULT_RETRYTIME_),
  isADirectory_(false),
  sourceDirectory_(""),
  rename_(AES_NONE),
  userGroup_(""),
  nameTag_(""),
  startupSequenceNumber_(DEFAULT_STARTUPSEQUENCENUMBER_),
  changedefstatus_(false),
  changeRemoveDelay_(false),
  changeRemoveBefore_(false),
  changetemplate_(false),
  checkNameTag_(false),
  changeSendRetries_(false),
  changeSendRetriesDelay_(false)

{
	argC_ = argc;
	argV_ = argv;
	if( (omHandler.Init(REGISTERED_OI)) == ACS_CC_FAILURE )
	{
	}


}


aes_afp_def::~aes_afp_def()
{
	omHandler.Finalize();
}


//	---------------------------------------------------------
//	       parse()
//	---------------------------------------------------------
bool aes_afp_def::parse ()
{
	int c;
	option_t option;
	optpair_t optlist [] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};

	aes_afp_getopt getopt(argC_, argV_, "b:d:g:j:k:n:r:s:t:v:w:");
	// Parse the options

	while ((c = getopt()) != EOF)
	{
		switch (c)
		{
#if 0
		case 'b':
		{
			option = REMOVEBEFORE;
			break;
		}
#endif
		case 'd':
		{
			option = REMOVEDELAY;
			break;
		}
		case 'g':
		{
			option = TEMPLATE;
			//rename_ = AES_REMOTE;
			break;
		}
#if 0
		case 'j':
		{
			option = RENAME;
			break;
		}
		case 'k':
		{
			option = STARTUPSEQUENCENUMBER;
			break;
		}
#endif
		case 'n':
		{
			option = NAMETAG;
			break;
		}
		case 'r':
		{
			option = SENDRETRIES;
			break;
		}
		case 's':
		{
			option = DEFAULTSTATUS;
			break;
		}
		case 't':
		{
			option = SENDRETRIESDELAY;
			break;
		}
		case 'v':
		{
			option = USERGROUP;
			break;
		}
#if 0
		case 'w':
		{
			option = SOURCEDIRECTORY;
			break;
		}
#endif
		case '?':
		{
			usage ();
			return false;
		}
		default:
		{
			usage ();
			return false;
		}
		}
		if (!optlist[option].opt_)
		{
			optlist[option].opt_ = c;
			optlist[option].arg_ = getopt.optarg_;
		}
		else
		{
			usage ();
			return false;
		}
		isOptPresent = true;
	}

	// Parse transfer queue name -----------------------------------------------------------

	if (argC_ > getopt.optind_)
	{
		transferQueue_ = argV_[getopt.optind_];
		AES_AFP_TRACE_MESSAGE("parse()\n Transfer queue %s\n", transferQueue_.c_str());
		getopt.optind_++;
	}
	else
	{
		usage ();
		return false;
	}


	// Parse destination set name ----------------------------------------------------

	if (argC_ > getopt.optind_)
	{
		destinationSet_ = argV_[getopt.optind_];
		AES_AFP_TRACE_MESSAGE("parse()\n Destination set %s\n", destinationSet_.c_str());
		getopt.optind_++;
	}
	else
	{
		usage ();
		return false;
	}

	// No more names -------------------------------------------------------------

	if (argC_ != getopt.optind_)
	{
		usage ();
		return false;
	}


	// Check Default state -------------------------------------------------------

	if (optlist[DEFAULTSTATUS].opt_)
	{

		changedefstatus_ = true;
		std::string defstatus("");
		defstatus = toUpper(optlist[DEFAULTSTATUS].arg_);

		if (defstatus == "DELETE")
		{
			defaultStatus_ = AES_FSDELETE;    //FS_DELETE;
			// defaultStatus_ = AFP_FSDELETE;
		}
		else if (defstatus == "READY")
		{
			defaultStatus_ = AES_FSREADY;     // FS_READY;
			// defaultStatus_ = AFP_FSREADY;
		}
		else if (defstatus == "NONE")
		{
			defaultStatus_ = AES_FSNONE;      // FS_NONE;
			// defaultStatus_ = AFP_FSNONE;
		}
		else if (defstatus == "PENDING")
		{
			defaultStatus_ = AES_FSPENDING;   // FS_PENDING;
			// defaultStatus_ = AFP_FSPENDING;

		}
		else
		{
			printErrorMessage(AES_ILLEGALSTATUSVALUE);
			exit (AES_ILLEGALSTATUSVALUE);
		}
		AES_AFP_TRACE_MESSAGE("parse()\n Default status %s\n", defstatus.c_str());
	}

	// Check Remove delay -------------------------------------------------------

	if (optlist[REMOVEDELAY].opt_)
	{
		changeRemoveDelay_=true;
		if (checkNumericValue(optlist[REMOVEDELAY].arg_) == false)
		{
			printErrorMessage(AES_ILLEGALDELAYVALUE);
			exit(AES_ILLEGALDELAYVALUE);
		}
		unsigned int num = atoi(optlist[REMOVEDELAY].arg_);
		removeDelay_ = num;
		AES_AFP_TRACE_MESSAGE("parse()\n Remove delay %d\n", removeDelay_);
	}

	// Check remove before -------------------------------------------------------

	if (optlist[REMOVEBEFORE].opt_)
	{
		changeRemoveBefore_ = true;
		remBeforeString_ = toUpper(optlist[REMOVEBEFORE].arg_);

		if (remBeforeString_ == "YES")
		{
			//removeBefore_ = true;
			removeBefore_ = 1;
		}
		else if (remBeforeString_ == "NO")
		{
			//removeBefore_ = false;
			removeBefore_ = 0;
		}
		else
		{
			printErrorMessage(AES_ILLEGALREMBEFORE);
			exit (AES_ILLEGALREMBEFORE);
		}
		AES_AFP_TRACE_MESSAGE("parse()\n Remove before %s\n", remBeforeString_.c_str());
	}

	// Check send retries -------------------------------------------------------

	if (optlist[SENDRETRIES].opt_)
	{
		if (checkNumericValue(optlist[SENDRETRIES].arg_) == false)
		{
			printErrorMessage(AES_ILLEGALRETRYVALUE);
			exit(AES_ILLEGALRETRYVALUE);
		}
		int num = atoi(optlist[SENDRETRIES].arg_);
		sendRetries_ = num;
		changeSendRetries_ = true;
		AES_AFP_TRACE_MESSAGE( "parse()\n Send retries %d\n", sendRetries_);
	}

	// Check send retries delay -------------------------------------------------

	if (optlist[SENDRETRIESDELAY].opt_)
	{
		if (checkNumericValue(optlist[SENDRETRIESDELAY].arg_) == false)
		{
			printErrorMessage(AES_ILLEGALTIMEVALUE);
			exit(AES_ILLEGALTIMEVALUE);
		}
		unsigned int num = atoi(optlist[SENDRETRIESDELAY].arg_);
		sendRetriesDelay_ = num;
		changeSendRetriesDelay_ = true;
		AES_AFP_TRACE_MESSAGE( "parse()\n Send retries delay %d\n", sendRetriesDelay_);
	}
	// Check template --------------------------------------------------------

	if (optlist[TEMPLATE].opt_)
	{
		changetemplate_=true;
		template_ = optlist[TEMPLATE].arg_;
		AES_AFP_TRACE_MESSAGE( "parse()\n Template %s\n", template_.c_str());
	}

	// Check name tag ----------------------------------------------------------------
	if (optlist[NAMETAG].opt_)
	{
		checkNameTag_=true;
		nameTag_ = optlist[NAMETAG].arg_;
		AES_AFP_TRACE_MESSAGE("parse()\n Name tag %s\n", nameTag_.c_str());
	}
	// Check User group --------------------------------------------------------------

	if (optlist[USERGROUP].opt_)
	{
		userGroup_ = toUpper(optlist[USERGROUP].arg_);
		AES_AFP_TRACE_MESSAGE("parse()\n User group %s\n", userGroup_.c_str());
	}

	return true;

}


//	---------------------------------------------------------
//	       execute()
//	---------------------------------------------------------
unsigned int aes_afp_def::execute ()
{
	//## begin aes_afp_def::execute%14045781FEED.body preserve=yes
	int result(0);
	std::string p_errorText("");
	std::string transactionName("modifyTransaction");
	bool changeflag = false;
	bool initdestflag = false;
	bool respdestflag = false;
	bool isexist = false;

	AES_AFP_TRACE_MESSAGE("execute()\n Calling createTransferQueue\n");

	if((AES_GCC_Util::fetchDnOfFileTransferObjFromIMM(omHandler))==-1)
	{
		AES_AFP_TRACE_MESSAGE("Failed to fetch fileTransfer object dn name");
		result = AES_CATASTROPHIC;
		printErrorMessage(result);
		return result;
	}

	/*If a transfer queue, TQ, is already present in the system; afpdef should not be allowed to modify the paramaters of TQ.
         afpdef can only add a destination to a already existing TQ*/
	if (isOptPresent)
	{
		string tqDN("");
		tqDN += TQ_FILETQRDN;
		tqDN += "=";
		tqDN += transferQueue_;
		tqDN += ",";
		tqDN += AES_GCC_Util::dnOfFileTransferM;
		ACS_APGCC_ImmObject myTQObj;
		myTQObj.objName = tqDN;

		if( omHandler.getObject(&myTQObj) == ACS_CC_SUCCESS )
		{
			AES_AFP_TRACE_MESSAGE("execute()\n Transfer queue %s already exist Options not allowed\n", transferQueue_.c_str());
			printErrorMessage(AES_ILLMODTQATTRS);
			return AES_ILLMODTQATTRS;
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("execute()\n Transfer queue %s does not exist\n", transferQueue_.c_str());
		}
	}
	result = api_.validateRenameTemplate(nameTag_,template_);
	if(result != AES_NOERRORCODE)
	{
		AES_AFP_TRACE_MESSAGE("RenameTemplate %s , nameTag_ %s is not valid", template_.c_str(), nameTag_.c_str());
		printErrorMessage(result);
		return result;
	}

	result = api_.validateTransferQueueParam(
			nameTag_,
			template_,
			defaultStatus_,
			removeDelay_,
			sendRetriesDelay_,
			sendRetries_,
			//Start - HT50930
			startupSequenceNumber_
			//End - HT50930
	);
	if(result != AES_NOERRORCODE)
	{
		AES_AFP_TRACE_MESSAGE("--------------------\nValidation FAILED");
		printErrorMessage(result);
		return result;
	}
	result=api_.validateTqName(transferQueue_);
	if(result != AES_NOERRORCODE)
	{
		AES_AFP_TRACE_MESSAGE("TransferQueue is not valid %s",transferQueue_.c_str());
		printErrorMessage(result);
		return result;
	}

	result = api_.validateDestinationSet(destinationSet_);
	if(result != AES_NOERRORCODE)
	{
		AES_AFP_TRACE_MESSAGE("Destination validation failed %s ", destinationSet_.c_str());
		printErrorMessage(result);
		return result;
	}
	
	std::string rdn1 = TQ_FILETQRDN;
	rdn1 += "=";
	rdn1 += transferQueue_;

	vector<ACS_CC_ValuesDefinitionType> AttrList1;
	ACS_CC_ValuesDefinitionType *attributes1;

	ACE_INT32  numAttr1 = 2;

	attributes1 = new ACS_CC_ValuesDefinitionType[numAttr1];

	if(attributes1 == NULL)
	{
		AES_AFP_TRACE_MESSAGE( "Mem allocation failed for attributes1");
		return 	ACS_CC_FAILURE;
	}
	//Forming attributes for transfer queue
	attributes1[0].attrName = new char [ACE_OS::strlen(TQ_FILETQRDN) + 1];
	ACE_OS::strcpy( attributes1[0].attrName ,TQ_FILETQRDN);
	attributes1[0].attrType = ATTR_STRINGT;
	attributes1[0].attrValuesNum = 1;
	attributes1[0].attrValues = new void*[1];

	AES_AFP_TRACE_MESSAGE("RDN1 Value = %s", rdn1.c_str());
	attributes1[0].attrValues[0] = reinterpret_cast<void*>(const_cast<char*>(rdn1.c_str()));
	AttrList1.push_back(attributes1[0]);

	AES_AFP_TRACE_MESSAGE("RDN %s, Parent = %s\n",(char *)(attributes1[0].attrValues[0]), AES_GCC_Util::dnOfFileTransferM.c_str());
	string emtpy;
	//Forming bject for DESTSET
	if (destinationSet_.compare("-") == 0)
	{

		AES_AFP_TRACE_MESSAGE("Destination - #########################");
		if(defaultStatus_ != AES_FSDELETE)
		{
			AES_AFP_TRACE_MESSAGE("illegal status value");
			printErrorMessage(AES_ILLEGALSTATUSVALUE);
			if( attributes1 != 0 )
			{
				delete[] attributes1[0].attrName;
				attributes1[0].attrName = 0;
				delete[] attributes1[0].attrValues;
				attributes1[0].attrValues = 0;
				delete[] attributes1;
				attributes1 = 0;
			}
			return AES_ILLEGALSTATUSVALUE;
		}

		attributes1[1].attrName = new char [ACE_OS::strlen(TQ_FILETQINITDEST) + 1];
		ACE_OS::strcpy( attributes1[1].attrName ,TQ_FILETQINITDEST);
		attributes1[1].attrType = ATTR_STRINGT;
		attributes1[1].attrValuesNum = 1;
		attributes1[1].attrValues = new void*[1];
		attributes1[1].attrValues[0] = reinterpret_cast<void*>(const_cast<char*>(destinationSet_.c_str()));
		AttrList1.push_back(attributes1[1]);

	}
	else
	{

		//forming for initdestset
		ACS_CC_ImmParameter paramToFind;
		std::string fileDestination1("");

		fileDestination1 = TQ_FILETQINITDESTID;
		fileDestination1 += "=";
		fileDestination1 += destinationSet_;
		fileDestination1 += ",";
		fileDestination1 += AES_GCC_Util::dnOfFileTransferM;

		paramToFind.attrName = const_cast<char*>(TQ_FILETQINITDESTID);
		int result1 = omHandler.getAttribute(fileDestination1.c_str(), &paramToFind );
		if ( result1 == ACS_CC_FAILURE)
		{
			//checking for resp destset
			ACS_CC_ImmParameter paramToFind;
			std::string fileDestination2("");

			fileDestination2 = TQ_FILETQRESPDESTID;
			fileDestination2 += "=";
			fileDestination2 += destinationSet_;
			fileDestination2 += ",";
			fileDestination2 += AES_GCC_Util::dnOfFileTransferM;
			paramToFind.attrName = const_cast<char*>(TQ_FILETQRESPDESTID);


			if( omHandler.getAttribute(fileDestination2.c_str(), &paramToFind )==ACS_CC_FAILURE)
			{
				//given destset is niether Init or Resp, hence throw error
				AES_AFP_TRACE_MESSAGE(" Get attribute of resp dest failed due to %s ",omHandler.getInternalLastErrorText());
				//Free the memory allocated for attribute names.
				delete[] attributes1[0].attrName;
				attributes1[0].attrName = 0;
				delete[] attributes1[0].attrValues;
				attributes1[0].attrValues = 0;
				delete[] attributes1;
				attributes1 = 0;
				result=AES_NODESTINATION;
				printErrorMessage(result);
				return result;
			}
			else
			{

				respdestflag = true;
				AES_AFP_TRACE_MESSAGE("Destinationgroup = %s, respdestflag = %d",fileDestination2.c_str(),respdestflag);
				//setting attributes for respdest set
				attributes1[1].attrName = new char [ACE_OS::strlen(TQ_FILETQRESPDEST) + 1];
				ACE_OS::strcpy( attributes1[1].attrName ,TQ_FILETQRESPDEST);
				attributes1[1].attrType = ATTR_STRINGT;
				attributes1[1].attrValues = new void*[1];
				attributes1[1].attrValuesNum = 1;
				attributes1[1].attrValues[0] = reinterpret_cast<void*>(const_cast<char *>(destinationSet_.c_str()));
				AttrList1.push_back(attributes1[1]);
			}
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("Destinationgroup = %s",fileDestination1.c_str());
			initdestflag = true;

			//setting attributes for init dest set
			attributes1[1].attrName = new char [ACE_OS::strlen(TQ_FILETQINITDEST) + 1];
			ACE_OS::strcpy( attributes1[1].attrName ,TQ_FILETQINITDEST);
			attributes1[1].attrType = ATTR_STRINGT;
			attributes1[1].attrValuesNum = 1;
			attributes1[1].attrValues = new void*[1];
			attributes1[1].attrValues[0] = reinterpret_cast<void*>(const_cast<char*>(destinationSet_.c_str()));
			AttrList1.push_back(attributes1[1]);

		}
	}

	ACE_INT32 returnCode = omHandler.createObject(TQ_CL_FILETQ,AES_GCC_Util::dnOfFileTransferM.c_str(), AttrList1);
	delete[] attributes1[0].attrName;
	attributes1[0].attrName = 0;
	delete[] attributes1[0].attrValues;
	attributes1[0].attrValues = 0;

	delete[] attributes1[1].attrName;
	attributes1[1].attrName = 0;
	delete[] attributes1[1].attrValues;
	attributes1[1].attrValues = 0;
		
	delete[] attributes1;
	attributes1 = 0;
	if((returnCode != ACS_CC_SUCCESS) && (ACE_OS::strcmp( omHandler.getInternalLastErrorText(),"ERR_ELEMENT_ALREADY_EXIST") != 0))
	{
		AES_AFP_TRACE_MESSAGE("create object failed due to %s",omHandler.getInternalLastErrorText());
		AES_AFP_TRACE_MESSAGE("Creation of Object failed");
		if(omHandler.getInternalLastError()!=-12)
		{
			if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
			{
				result = AES_CATASTROPHIC;
				printErrorMessage(result);
				return result;
			}
		}
		else
		{
			result = AES_NOSERVERACCESS;
			printErrorMessage(result);
			return result;
		}

		printErrorMessage(p_errorText);
		return result;

	}


	if((returnCode != ACS_CC_SUCCESS) && (ACE_OS::strcmp( omHandler.getInternalLastErrorText(),"ERR_ELEMENT_ALREADY_EXIST")==0))
	{
		isexist = true;
		ACS_CC_ImmParameter paramToFind1;
		int noOfDestSet=0;
		std::string pObjName = rdn1;
		pObjName += ",";
		pObjName += AES_GCC_Util::dnOfFileTransferM;

		char initdestset[]=TQ_FILETQINITDEST;
		char respdestset[]=TQ_FILETQRESPDEST;
		ACS_CC_ImmParameter par;
		if(initdestflag)
		{
			par.attrName = initdestset;
			paramToFind1.attrName = new char [30];
			ACE_OS::strcpy(paramToFind1.attrName, TQ_FILETQINITDEST);
		}
		else
		{
			par.attrName = respdestset;
			paramToFind1.attrName = new char [30];
			ACE_OS::strcpy(paramToFind1.attrName, TQ_FILETQRESPDEST);
		}
		if(omHandler.getAttribute(pObjName.c_str(), &paramToFind1) == ACS_CC_SUCCESS)
		{
			noOfDestSet = paramToFind1.attrValuesNum;
			AES_AFP_TRACE_MESSAGE("Number of initiating destsets in current TQ : %d",noOfDestSet);
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("GetAttribute of Object failed");
			delete[] paramToFind1.attrName;
			result=AES_NODESTINATION;
			printErrorMessage(result);
			return result;
		}
		par.attrType= ATTR_STRINGT;
		par.attrValuesNum=1 + noOfDestSet;
		par.attrValues=new void*[par.attrValuesNum];
		for(int a=0; a < noOfDestSet; a++)
		{
			par.attrValues[a] = reinterpret_cast<void*>((char*)(paramToFind1.attrValues[a]));
		}
		par.attrValues[noOfDestSet] = reinterpret_cast<void*>((char*)(destinationSet_.c_str()));

		if(omHandler.modifyAttribute(pObjName.c_str(), &par) != ACS_CC_SUCCESS)
		{
			AES_AFP_TRACE_MESSAGE("Modify object failed due to %s",omHandler.getInternalLastErrorText());
			if(omHandler.getInternalLastError()!=-12)
			{
				if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
				{
					result=AES_CATASTROPHIC;
					delete[] paramToFind1.attrName;
					printErrorMessage(result);
					return result;
				}
			}
			else
			{
				result=AES_NOSERVERACCESS;
				delete[] paramToFind1.attrName;
				printErrorMessage(result);
				return result;
			}

			delete[] paramToFind1.attrName;
			printErrorMessage(p_errorText);
			return result;
		}
		delete[] paramToFind1.attrName;

	}

	std::string fileTQRdn = TQ_FILETQRDN;
	fileTQRdn +=  "=";
	fileTQRdn += transferQueue_;
	fileTQRdn += ",";
	fileTQRdn += AES_GCC_Util::dnOfFileTransferM.c_str();
	
	std::vector<std::string>  p_rdnList;	
	string myRDn("");
	omHandler.getChildren(fileTQRdn.c_str(),ACS_APGCC_SUBLEVEL,&p_rdnList);
	if(p_rdnList.size() == 0)
	{
		AES_AFP_TRACE_MESSAGE("Unable to retrieve advanced RDN name for fileTQRdn %s",fileTQRdn.c_str());
	}
	else
	{
		AES_AFP_TRACE_MESSAGE("Able to retrieve advanced RDN name for fileTQRdn %s and no of objects = %zu",fileTQRdn.c_str(),p_rdnList.size());
	}

	for(unsigned int rdnCntr = 0; rdnCntr < p_rdnList.size(); rdnCntr ++)
	{
		string myClassName("");
		extractClassNmFromRdn(p_rdnList[rdnCntr], myClassName);
		myRDn = myClassName;	
	}

	std::string advFileTQRdn = TQ_FILETQPARAMRDN;
	if(p_rdnList.size() == 0)
	{
		advFileTQRdn += "=";
		advFileTQRdn +=  transferQueue_;
	}
	else
		advFileTQRdn = myRDn;


	vector<ACS_CC_ValuesDefinitionType> AttrList2;
	ACS_CC_ValuesDefinitionType *attributes2;

	ACE_INT32  numAttr2 = 1;

	attributes2 = new ACS_CC_ValuesDefinitionType[numAttr2];

	if(attributes2 == NULL)
	{
		AES_AFP_TRACE_MESSAGE("Mem allocation failed for attributes2");
		return 	ACS_CC_FAILURE;
	}
	//Forming attributes for transfer queue
	attributes2[0].attrName = new char [ACE_OS::strlen(TQ_FILETQPARAMRDN) + 1];
	ACE_OS::strcpy( attributes2[0].attrName ,TQ_FILETQPARAMRDN);
	attributes2[0].attrType = ATTR_STRINGT;
	attributes2[0].attrValuesNum = 1;
	attributes2[0].attrValues = new void*[1];
	
	std::string rdn2 = advFileTQRdn;
	rdn2 += ",";
	rdn2 += TQ_FILETQRDN;
	rdn2 += "=";
	rdn2 += transferQueue_;
	rdn2 += ",";
	rdn2 += AES_GCC_Util::dnOfFileTransferM.c_str();

	AES_AFP_TRACE_MESSAGE( "RDN Value = %s", rdn2.c_str());

	attributes2[0].attrValues[0] = reinterpret_cast<void*>(const_cast<char*>(advFileTQRdn.c_str()));
	AttrList2.push_back(attributes2[0]);
	if(p_rdnList.size() == 0)
	{
		returnCode = omHandler.createObject(TQ_CL_FILETQPARAMNAME,fileTQRdn.c_str(), AttrList2);
	}
        
        delete[] attributes2[0].attrName;
        attributes2[0].attrName = 0;
        delete[] attributes2[0].attrValues;
        attributes2[0].attrValues = 0;

        delete[] attributes2;
        attributes2 = 0;

        if((returnCode != ACS_CC_SUCCESS) && (ACE_OS::strcmp( omHandler.getInternalLastErrorText(),"ERR_ELEMENT_ALREADY_EXIST") != 0))
        {
                AES_AFP_TRACE_MESSAGE("create object failed for AdvTQ due to %s",omHandler.getInternalLastErrorText());
                AES_AFP_TRACE_MESSAGE("Creation of Object failed");
                if(omHandler.getInternalLastError()!=-12)
                {
                        if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
                        {
                                result = AES_CATASTROPHIC;
                                printErrorMessage(result);
                                return result;
                        }
                }
                else
                {
                        result = AES_NOSERVERACCESS;
                        printErrorMessage(result);
                        return result;
                }

                printErrorMessage(p_errorText);
                return result;

        }

	AES_AFP_TRACE_MESSAGE("RDN Value = %s", rdn2.c_str());

	if(changedefstatus_)
	{
		char defstat[]=TQ_DEFSTAT;
		ACS_CC_ImmParameter par1;
		par1.attrName = defstat;
		par1.attrType= ATTR_INT32T;
		par1.attrValuesNum=1;
		par1.attrValues=new void*[par1.attrValuesNum];
		par1.attrValues[0] =reinterpret_cast<void*>(&(defaultStatus_));
		changeflag = true;

		if(omHandler.modifyAttribute(rdn2.c_str(), &par1, transactionName) != ACS_CC_SUCCESS)
		{
			AES_AFP_TRACE_MESSAGE("Modify object failed due to %s",omHandler.getInternalLastErrorText());
			if(omHandler.getInternalLastError()!=-12)
			{
				if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
				{
					result=AES_CATASTROPHIC;
					printErrorMessage(result);
					return result;
				}
			}
			else
			{
				result=AES_NOSERVERACCESS;
				printErrorMessage(result);
				return result;
			}

			printErrorMessage(p_errorText);
			return result;
		}
	}

	if(changeRemoveBefore_)
	{
		ACS_CC_ImmParameter par2;
		char defcnt[]= TQ_DEFCNTDWN;
		par2.attrName = defcnt;
		par2.attrType= ATTR_INT32T;
		par2.attrValuesNum=1;
		par2.attrValues=new void*[par2.attrValuesNum];
		int deletecndw = removeBefore_;
		par2.attrValues[0] =reinterpret_cast<void *>(&(deletecndw));
		changeflag = true;

		if(omHandler.modifyAttribute(rdn2.c_str(), &par2, transactionName) != ACS_CC_SUCCESS)
		{
			AES_AFP_TRACE_MESSAGE("Modify object failed due to %s",omHandler.getInternalLastErrorText());
			if(omHandler.getInternalLastError()!=-12)
			{
				if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
				{
					result=AES_CATASTROPHIC;
					printErrorMessage(result);
					return result;
				}
			}
			else
			{
				result=AES_NOSERVERACCESS;
				printErrorMessage(result);
				return result;
			}

			printErrorMessage(p_errorText);
			return result;

		}
	}

	if(checkNameTag_)
	{
		string buffer1("");
		string buffer2("");
		if(nameTag_!="")
		{

			int indexy = nameTag_.find_last_of('*');
			if (indexy != (ACE_INT32)nameTag_.npos)
			{
				buffer2 = nameTag_.substr(indexy+1,(strlen(nameTag_.c_str()))-1);
				buffer1 = nameTag_.substr(0, indexy);

			}
			else
			{
				buffer1 = nameTag_;
				buffer2 = "";
			}
		}
		if(!buffer1.empty())
		{
		ACS_CC_ImmParameter par3;
		char nTag1[] = TQ_NAMETAG1;
		par3.attrName = nTag1;
		par3.attrType= ATTR_STRINGT;
		par3.attrValuesNum=1;
		par3.attrValues=new void*[par3.attrValuesNum];
		par3.attrValues[0] =reinterpret_cast<void*>((char*)(buffer1.c_str()));
		changeflag = true;
		if(omHandler.modifyAttribute(rdn2.c_str(), &par3, transactionName) != ACS_CC_SUCCESS)
		{
			AES_AFP_TRACE_MESSAGE("Modify object failed due to %s",omHandler.getInternalLastErrorText());
			if(omHandler.getInternalLastError()!=-12)
			{
				if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
				{
					result=AES_CATASTROPHIC;
					printErrorMessage(result);
					return result;
				}
			}
			else
			{
				result=AES_NOSERVERACCESS;
				printErrorMessage(result);
				return result;
			}
			printErrorMessage(p_errorText);
			return result;

		}
		}
		if(!buffer2.empty())
		{
		ACS_CC_ImmParameter par4;
		string tempBuff = buffer2;
		char nTag2[] = TQ_NAMETAG2;
		par4.attrName = nTag2;
		par4.attrType= ATTR_STRINGT;
		par4.attrValuesNum=1;
		par4.attrValues=new void*[par4.attrValuesNum];
		par4.attrValues[0] =reinterpret_cast<void*>((char*)(tempBuff.c_str()));
		changeflag = true;
		if(omHandler.modifyAttribute(rdn2.c_str(), &par4, transactionName) != ACS_CC_SUCCESS)
		{
			AES_AFP_TRACE_MESSAGE("Modify object failed due to %s",omHandler.getInternalLastErrorText());
			if(omHandler.getInternalLastError()!=-12)
			{
				if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
				{
					result=AES_CATASTROPHIC;
					printErrorMessage(result);
					return result;
				}
			}
			else
			{
				result=AES_NOSERVERACCESS;
				printErrorMessage(result);
				return result;
			}
			printErrorMessage(p_errorText);
			return result;
		}
		}
	}
	if(changeRemoveDelay_)
	{
		ACS_CC_ImmParameter par5;
		char rDelay[] = TQ_REMOVEDELAY;
		par5.attrName = rDelay;
		par5.attrType= ATTR_INT32T;
		par5.attrValuesNum=1;
		par5.attrValues=new void*[par5.attrValuesNum];
		par5.attrValues[0] =reinterpret_cast<void*>(&(removeDelay_));
		changeflag = true;
		if(omHandler.modifyAttribute(rdn2.c_str(), &par5, transactionName) != ACS_CC_SUCCESS)
		{
			
			AES_AFP_TRACE_MESSAGE("Modify object failed due to %s",omHandler.getInternalLastErrorText());
			if(omHandler.getInternalLastError()!=-12)
			{
				if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
				{
					result=AES_CATASTROPHIC;
					printErrorMessage(result);
					return result;
				}
			}
			else
			{
				result=AES_NOSERVERACCESS;
				printErrorMessage(result);
				return result;
			}
			printErrorMessage(p_errorText);
			return result;
		}
	}

	if(changetemplate_)
	{
		ACS_CC_ImmParameter par6;
		char rtemplate[] = TQ_RENAMETEMPLATE;
		par6.attrName = rtemplate;
		par6.attrType= ATTR_STRINGT;
		par6.attrValuesNum=1;
		par6.attrValues=new void*[par6.attrValuesNum];
		par6.attrValues[0] =reinterpret_cast<void*>(reinterpret_cast<void*>((char*)(template_.c_str())));
		changeflag = true;
		if(omHandler.modifyAttribute(rdn2.c_str(), &par6, transactionName) != ACS_CC_SUCCESS)
		{
			AES_AFP_TRACE_MESSAGE("Modify object failed due to %s",omHandler.getInternalLastErrorText());
			if(omHandler.getInternalLastError()!=-12)
			{
				if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
				{
					result=AES_CATASTROPHIC;
					printErrorMessage(result);
					return result;
				}
			}
			else
			{
				result=AES_NOSERVERACCESS;
				printErrorMessage(result);
				return result;
			}
			printErrorMessage(p_errorText);
			return result;
		}
	}
	if(changeSendRetries_)
	{
		ACS_CC_ImmParameter par7;
		char numOfRetryOnFail[] = TQ_NUMOFRETRIES;
		par7.attrName = numOfRetryOnFail;
		par7.attrType= ATTR_INT32T;
		par7.attrValuesNum=1;
		par7.attrValues=new void*[par7.attrValuesNum];
		par7.attrValues[0] =reinterpret_cast<void*>(reinterpret_cast<void*>((int*)(&sendRetries_)));
		changeflag = true;
		if(omHandler.modifyAttribute(rdn2.c_str(), &par7, transactionName) != ACS_CC_SUCCESS)
		{
			AES_AFP_TRACE_MESSAGE("Modify of numberOfRetryOnFailure failed due to %s",omHandler.getInternalLastErrorText());
			if(omHandler.getInternalLastError()!=-12)
			{
				if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
				{
					result=AES_CATASTROPHIC;
					printErrorMessage(result);
					return result;
				}
			}
			else
			{
				result=AES_NOSERVERACCESS;
				printErrorMessage(result);
				return result;
			}
			printErrorMessage(p_errorText);
			return result;
		}
	}
	if(changeSendRetriesDelay_)
	{
		ACS_CC_ImmParameter par8;
		char retryInterval[] = TQ_RETRYINTERVAL;
		par8.attrName = retryInterval;
		par8.attrType= ATTR_INT32T;
		par8.attrValuesNum=1;
		par8.attrValues=new void*[par8.attrValuesNum];
		par8.attrValues[0] =reinterpret_cast<void*>(reinterpret_cast<void*>((int*)(&sendRetriesDelay_)));
		changeflag = true;
		if(omHandler.modifyAttribute(rdn2.c_str(), &par8, transactionName) != ACS_CC_SUCCESS)
		{
			AES_AFP_TRACE_MESSAGE("Modify of retryInterval failed due to %s",omHandler.getInternalLastErrorText());
			if(omHandler.getInternalLastError()!=-12)
			{
				if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
				{
					result=AES_CATASTROPHIC;
					printErrorMessage(result);
					return result;
				}
			}
			else
			{
				result=AES_NOSERVERACCESS;
				printErrorMessage(result);
				return result;
			}
			printErrorMessage(p_errorText);
			return result;
		}
	}
	if (changeflag != true)
	{
		AES_AFP_TRACE_MESSAGE("Creation of Object Success");
		return result;

	}
	result = omHandler.applyRequest(transactionName);
	if(result != ACS_CC_SUCCESS)
	{
		result = omHandler.resetRequest(transactionName);
		if(result == ACS_CC_SUCCESS)
		{
			AES_AFP_TRACE_MESSAGE("resetRequest SUCCESS");
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("resetRequest FAILED");
		}
		if(!isexist)
		{
			std::vector<std::string>  p_rdnList;
			string parent = rdn1;
			parent += ",";
			parent += AES_GCC_Util::dnOfFileTransferM;
			omHandler.getChildren(parent.c_str(), ACS_APGCC_SUBLEVEL,&p_rdnList);
			AES_AFP_TRACE_MESSAGE("Number of adv param objs under %s group is : %zu ",parent.c_str(),p_rdnList.size());
			if(p_rdnList.size()==0)
			{
				AES_AFP_TRACE_MESSAGE("There are no adv param objects");
			}
			else if (p_rdnList.size()==1)
			{
				AES_AFP_TRACE_MESSAGE("Size of Advanced parameters objects vector %zu ",p_rdnList.size());
				ACE_INT32 returnCode;
				string advParamDn("");

				advParamDn = p_rdnList[0].c_str();
				AES_AFP_TRACE_MESSAGE("Adv param dn =  %s ",advParamDn.c_str());
				returnCode = omHandler.deleteObject(const_cast<char*>(advParamDn.c_str()));
				if( returnCode != ACS_CC_SUCCESS )
				{
					AES_AFP_TRACE_MESSAGE("Deletion of advance param Object failed %s",omHandler.getInternalLastErrorText());
					//					result = AES_CATASTROPHIC;
					if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
					{
						result=AES_CATASTROPHIC;
						printErrorMessage(result);
						return result;
					}
					printErrorMessage(AES_NOERRORCODE);
					return result;
				}
				else
				{
					advParamDn.clear();
					AES_AFP_TRACE_MESSAGE("!Success : Deletion of advance param Object");
					returnCode = omHandler.deleteObject(const_cast<char*>(parent.c_str()));
					if( returnCode != ACS_CC_SUCCESS )
					{
						AES_AFP_TRACE_MESSAGE("Deletion of File transfer queue Object failed %s",omHandler.getInternalLastErrorText());
						//						result = AES_CATASTROPHIC;
						if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
						{
							result=AES_CATASTROPHIC;
							printErrorMessage(result);
							return result;
						}
						printErrorMessage(p_errorText);
						return result;
					}

					else
					{
						AES_AFP_TRACE_MESSAGE("!Success: Deletion of File transfer queue Object ");
						if(omHandler.getExitCode(result,p_errorText)!=ACS_CC_SUCCESS)
						{
							result=AES_CATASTROPHIC;
							printErrorMessage(result);
							return result;
						}
						printErrorMessage(p_errorText);
						return result;
					}
				}

			}
			else
			{
				AES_AFP_TRACE_MESSAGE("The TQ contains more than one advance param object");
				result = AES_CATASTROPHIC;
				printErrorMessage(result);
				return result;
			}
		}

	}
	AES_AFP_TRACE_MESSAGE("Creation of Object Success");
	return result;
}

//	---------------------------------------------------------
//	       usage()
//	---------------------------------------------------------
void aes_afp_def::usage ()
{
	string abc="afpdef";
	cout<<"Incorrect usage"<<endl;
	cout<<"Usage: \n"<<abc<<" [-d removedelay][-g template]" << endl;
	cout<<"       [-n nametags][-r retries]" << endl;
	cout<<"       [-s defaultstatus][-t retryinterval]" << endl;
	cout<<"       transferqueue destinationset" << endl<<endl;

	exit(AES_INCORRECTCOMMAND);
}

void aes_afp_def::extractClassNmFromRdn( const string myObjRdn,string &myClassName)
{
        myClassName = "";
        string::size_type pos1 = string::npos;

        pos1 = myObjRdn.find_first_of(",");
        if ( pos1 != string::npos )
        {
                myClassName = myObjRdn.substr(0, pos1);
        }
	
}


