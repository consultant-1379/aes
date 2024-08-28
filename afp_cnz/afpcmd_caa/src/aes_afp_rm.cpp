//	INCLUDE aes_afp_rm.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002.
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
//
//	LINKAGE
//
//	SEE ALSO
#include "aes_afp_rm.h"
#include "aes_afp_services.h"
#include <acs_apgcc_adminoperation.h>
#include "acs_apgcc_omhandler.h"
#include "ACS_CC_Types.h"
#include "acs_apgcc_adminoperation.h"
#include "aes_afp_api_tracer.h"
AES_AFP_TRACE_DEFINE(aes_afp_Rm);

void* aes_afp_rm::immutil_new_attrValue(SaImmValueTypeT attrValueType, const char *str)
{
	void *attrValue = NULL;

	switch (attrValueType)
	{
	case SA_IMM_ATTR_SAINT32T:
		attrValue = malloc(sizeof(SaInt32T));
		*((SaInt32T *)attrValue) = strtol(str, NULL, 10);
		break;

	case SA_IMM_ATTR_SAUINT32T:
		attrValue = malloc(sizeof(SaUint32T));
		*((SaUint32T *)attrValue) = strtol(str, NULL, 10);
		break;

	case SA_IMM_ATTR_SAINT64T:
		attrValue = malloc(sizeof(SaInt64T));
		*((SaInt64T *)attrValue) = strtoll(str, NULL, 10);
		break;

	case SA_IMM_ATTR_SAUINT64T:
		attrValue = malloc(sizeof(SaUint64T));
		*((SaUint64T *)attrValue) = strtoll(str, NULL, 10);
		break;

	case SA_IMM_ATTR_SATIMET:
		attrValue = malloc(sizeof(SaTimeT));
		*((SaTimeT *)attrValue) = strtoll(str, NULL, 10);
		break;

	case SA_IMM_ATTR_SAFLOATT:
		SaFloatT myfloat;
		attrValue = malloc(sizeof(SaFloatT));
		sscanf(str, "%f", &myfloat);
		*((SaFloatT *)attrValue) = myfloat;
		break;

	case SA_IMM_ATTR_SADOUBLET:
		double mydouble;
		attrValue = malloc(sizeof(double));
		sscanf(str, "%lf", &mydouble);
		*((double *)attrValue) = mydouble;
		break;

	case SA_IMM_ATTR_SANAMET:
		SaNameT *mynamet;
		mynamet = (SaNameT*)malloc(sizeof(SaNameT));
		attrValue = mynamet;
		mynamet->length = strlen(str);
		strncpy((char *)mynamet->value, str, SA_MAX_NAME_LENGTH);
		break;

	case SA_IMM_ATTR_SASTRINGT:
		attrValue = malloc(sizeof(SaStringT));
		*((SaStringT *)attrValue) = strdup(str);
		break;


	case SA_IMM_ATTR_SAANYT:
		/* Cannot handle this one... */

	default:
		break;
	}

	return attrValue;
}

SaAisErrorT  aes_afp_rm::invokeAdminOperation(const std::string & i_dn,
		unsigned int i_operationId ,
		const SaImmAdminOperationParamsT_2 ** i_params,
		SaTimeT i_timeout,SaAisErrorT &aOIValidationReturnValue)
{
	SaAisErrorT rc;
	SaNameT objectName;
	objectName.length = i_dn.length();
	memcpy(objectName.value, i_dn.c_str(), objectName.length);

	const SaNameT *objectNames[2];
	objectNames[0] = &objectName;
	objectNames[1] = NULL;
	SaImmAdminOwnerHandleT m_ownerHandle;
	SaImmHandleT m_omHandle;

	if( i_params == NULL)
	{
		i_params = ( const SaImmAdminOperationParamsT_2 ** )realloc(NULL, sizeof(SaImmAdminOperationParamsT_2 *));
		i_params[0] = NULL;
	}

	SaVersionT s_immVersion = { 'A', 2, 1 };


	rc = saImmOmInitialize(&m_omHandle, NULL, &s_immVersion);
	if( rc != SA_AIS_OK)
	{
		free(i_params);
		i_params=0;
		return rc;
	}

	rc = saImmOmAdminOwnerInitialize(m_omHandle, (char*)"APG", SA_TRUE, &m_ownerHandle);
	if( rc != SA_AIS_OK)
	{
		saImmOmFinalize(m_omHandle);
		free(i_params);
		i_params=0;
		return rc;
	}


	rc = saImmOmAdminOwnerSet(m_ownerHandle, objectNames, SA_IMM_ONE);
	if( rc != SA_AIS_OK)
	{
		saImmOmAdminOwnerFinalize(m_ownerHandle);
		saImmOmFinalize(m_omHandle);
		free(i_params);
                i_params=0;
		return rc;
	}

	rc = saImmOmAdminOperationInvoke_2(m_ownerHandle, &objectName, 0, i_operationId, i_params,
			&aOIValidationReturnValue, SA_TIME_ONE_SECOND * i_timeout);

	// Clean Up the Admin Handle and OM Hanlde

	saImmOmAdminOwnerFinalize(m_ownerHandle);
	saImmOmFinalize(m_omHandle);
	return rc;
}

void aes_afp_rm::createIMMParameter(SaImmAdminOperationParamsT_2* & aParam , std::string aAttributeName,std::string aAttributeValue)
{
	aParam = (SaImmAdminOperationParamsT_2 *) malloc(sizeof(SaImmAdminOperationParamsT_2));
	aParam->paramName = (char *) strdup(aAttributeName.c_str());
	if(aAttributeName.compare(TQ_ENTITYNAME) == 0)
	{
		aParam->paramType = SA_IMM_ATTR_SASTRINGT;
	}
	else if(aAttributeName.compare(TQ_RMVFSRC) == 0)
	{
		aParam->paramType = SA_IMM_ATTR_SAINT32T;
		ACE_INT32 *tmp = new ACE_INT32(srcDir_);
		void* myMPIntervalValue[1]      = {reinterpret_cast<void*>(tmp)};
		aParam->paramBuffer       = myMPIntervalValue;
		return;
	}
	else if(aAttributeName.compare(TQ_DESTSETNAME) == 0)
	{
		aParam->paramType = SA_IMM_ATTR_SANAMET;
	}
	else if(aAttributeName.compare(TQ_NAME) == 0)
	{
		aParam->paramType = SA_IMM_ATTR_SANAMET;
	}
	else
	{
		return;
	}
	aParam->paramBuffer = immutil_new_attrValue(aParam->paramType, aAttributeValue.c_str());
	return;
}


//------------------------------------------------------------------------------
//	main ()
//------------------------------------------------------------------------------

int
main (int argc, char *argv [])
{
	aes_afp_rm rm (argc, argv);
	return rm.launch();
}

// Class aes_afp_rm 

//	---------------------------------------------------------
//	       Constructor()
//	---------------------------------------------------------
aes_afp_rm::aes_afp_rm (int argc, char** argv)
: fileDestination_(""),
  destination_(""),
  sendItem_(""),
  destinationSet_(false),
  fileSet_(false),
  dirSet_(false),
  srcDir_(false)
{
	argC_ = argc;
	argV_ = argv;
	if( (omHandler.Init(REGISTERED_OI)) == ACS_CC_FAILURE )
	{
	}
}


aes_afp_rm::~aes_afp_rm()
{
	omHandler.Finalize();
}

//	---------------------------------------------------------
//	       parse()
//	---------------------------------------------------------
bool aes_afp_rm::parse ()
{
	int c(0);
	optpair_t optlist [] = {{0,0},{0,0},{0,0}};

	aes_afp_getopt getopt (argC_, argV_, "c:z:f:");

	// Parse the options

	while ((c = getopt ()) != EOF)
	{
		switch (c)
		{
		case 'f':    // for removing SRCDIR(APZ21230/5-758)
		{
			srcDir_ = true;
			getopt.reInit();
			continue;
		}
		case '?':
		{
			usage();
			return false;
		}
		default:
		{
			usage();
			return false;
		}
		}
	} // while

	// Parse filedestination name -----------------------------------------------------------

	if (argC_ > getopt.optind_)
	{
		fileDestination_ = argV_[getopt.optind_];
		getopt.optind_++;
		if (fileDestination_.empty())
		{
			usage();
			return false;
		}
	}
	else
	{
		usage();
		return false;
	}

	// Parse destination ---------------------------------------------------------
	if (argC_ > getopt.optind_)
	{
		destination_ = argV_[getopt.optind_];
		destinationSet_ = true;
		getopt.optind_++;
	}

	// Parse filename ------------------------------------------------------------
	if (optlist[FILENAME].opt_)
	{
		sendItem_ = toUpper(optlist[FILENAME].arg_);
		fileSet_ = true;
	}

	// Parse directory -----------------------------------------------------------
	if (optlist[DIRECTORY].opt_)
	{
		sendItem_ = toUpper(optlist[DIRECTORY].arg_);
		dirSet_ = true;
	}

	if (fileSet_ == true && dirSet_ == true)
	{
		usage();
		return false;
	}

	// Parse no more arguments ---------------------------------------------------

	if (argC_ != getopt.optind_)
	{
		usage ();
		return false;
	}

	return true;
}

//	---------------------------------------------------------
//	       execute()
//	---------------------------------------------------------
unsigned int aes_afp_rm::execute ()
{
	unsigned int result(0);

	std::stringstream destStream;
	std::string tmpString("");
	std::string byWho("");
	result=api_.validateTqName(fileDestination_);
	if(result != AES_NOERRORCODE)
	{
		AES_AFP_TRACE_MESSAGE("TransferQueue is not valid %s",fileDestination_.c_str());
		printErrorMessage(result);
		return result;
	}
	if (destinationSet_ == true)
	{
		result = api_.validateDestinationSet(destination_);
		if(result != AES_NOERRORCODE)
		{
			AES_AFP_TRACE_MESSAGE("Destination validation failed %s ", destination_.c_str());
			printErrorMessage(result);
			return result;
		}
	}
	if((AES_GCC_Util::fetchDnOfFileTransferObjFromIMM(omHandler))==-1)
	{
		result = AES_CATASTROPHIC;
		printErrorMessage(result);
		return result;
	}
	string errText("");
	result =   executeAdminOperation(errText);
	if( result != AES_NOERRORCODE)
	{
		if( errText.empty())
		{
			printErrorMessage(result);
		}
		else
		{
			printErrorMessage(errText);
		}
	}
	else
	{
		cout<<endl;
	}
	return result;

}

//	---------------------------------------------------------
//	       usage()
//	---------------------------------------------------------
void aes_afp_rm::usage ()
{
	cout<<"Incorrect usage"<<endl;
	string abc="afprm";
	cout<<"Usage: "<<abc<<" transferqueue [destinationset]"<<endl;	//APZ21230/5-758
	cout<<"       "<<abc<<" [-f] transferqueue"<<endl;

	cout<<endl<<endl;
	exit(AES_INCORRECTCOMMAND);
}

unsigned int aes_afp_rm::executeAdminOperation(string& errorMessage)
{
	AES_AFP_TRACE_MESSAGE( "executeAdminOperation called");
	int result1(0);
	errorMessage = "";
	std::string p_errorText;
	vector<ACS_APGCC_AdminOperationParamType> paramVector;
	ACS_APGCC_AdminOperationParamType param1;

	bool initdestflag=false, respdestflag=true;

	std::string myDNNameOfTQ(TQ_FILETQRDN);
	myDNNameOfTQ += "=";
	myDNNameOfTQ += fileDestination_;
	myDNNameOfTQ += ",";
	myDNNameOfTQ += AES_GCC_Util::dnOfFileTransferM;

	AES_AFP_TRACE_MESSAGE("myDNNameOfTQ == %s",myDNNameOfTQ.c_str());

	param1.attrName=(char*)TQ_NAME;
	param1.attrType = ATTR_STRINGT;
	param1.attrValues=reinterpret_cast<void*>((char*)fileDestination_.c_str());

	paramVector.push_back(param1);
	long long int timeOutVal = 1200*(1000000000LL);//10 seconds timeout increased to 1200seconds to complete the Admin operation successfully
	int myOIValidationReturnValue;

	acs_apgcc_adminoperation admOp;

	ACS_CC_ReturnType returnCode = admOp.init();
	if(returnCode != ACS_CC_SUCCESS)
	{
		return AES_CATASTROPHIC;
	}

	ACS_CC_ImmParameter paramToFindTq;
	paramToFindTq.attrName = const_cast<char*>(TQ_FILETQRDN);
	if( omHandler.getAttribute(myDNNameOfTQ.c_str(), &paramToFindTq )==ACS_CC_FAILURE)
	{
		  AES_AFP_TRACE_MESSAGE(" TransferQueue does not exist %s ",omHandler.getInternalLastErrorText());
		  result1=AES_NOPROCORDER;
		  return result1;
	}

	if (destinationSet_ == false)
	{
		if(srcDir_ == true)
		{
			vector<ACS_APGCC_AdminOperationParamType> outVector;
			ACS_CC_ReturnType adminResult = admOp.adminOperationInvoke(AES_GCC_Util::dnOfFileTransferM.c_str(),0,1, paramVector,&myOIValidationReturnValue,timeOutVal,outVector);

			if (adminResult != ACS_CC_SUCCESS)
			{
/*				if(omHandler.getExitCode(result1,p_errorText) != ACS_CC_SUCCESS)
				{
					result1 = AES_NOSERVERACCESS;
					return result1;
				}
				errorMessage = p_errorText;
				return result1;
				*/
				errorMessage = admOp.getInternalLastErrorText();
				result1 = admOp.getInternalLastError();
				AES_AFP_TRACE_MESSAGE(" Admin operation invoke failed due to error code : %d and errortext : %s ",result1,errorMessage.c_str());
				return result1;
				
				
			}

			if( myOIValidationReturnValue == 1)
			{
				return 0;
			}

			if (myOIValidationReturnValue != 1)
			{
			    AES_AFP_TRACE_MESSAGE("outputvector size: %zu",outVector.size());

			    string errorText_temp = "";
			    string errorText = "";

				if ( outVector.size() !=0 )
				{
			        AES_AFP_TRACE_MESSAGE("Hereafter the received return parameters ");
					for(unsigned int k= 0; k<outVector.size();k++)
					{
						 switch ( outVector[k].attrType ) 
						 {
#if 0
							 case ATTR_INT32T:
							 result1 = *(reinterpret_cast<int *>(outVector[k].attrValues));
							 break;
#endif
							 case ATTR_STRINGT:
							 	errorText_temp = reinterpret_cast<char *>(outVector[k].attrValues);
								if ( errorText_temp.length() > 8 )
								{
									errorText = errorText_temp.substr(8);   // clipping the starting part @ComNbi@ in the actual error text
									//cout << errorText.c_str() << endl << endl;
								}
								else
								{
									cout << "Other Error"  << endl << endl;
								}

								if(strcmp(errorText.c_str(),  "Transfer in progress") == 0)
								{
									result1 =83;
								}
								else if(strcmp(errorText.c_str(),  "FileTransferQueue is used by Measurement program, it cannot be deleted") == 0)
								{
									result1 =152;
								}
								else if(strcmp(errorText.c_str(),  "FileTransferQueue is used by CP-file, it cannot be deleted") == 0)
								{
									result1 =153;
								}
								else if(strcmp(errorText.c_str(),  "FileTransferQueue is used by PDS-file, it cannot be deleted") == 0)
								{
									result1 =154;
								}
								else if(strcmp(errorText.c_str(),  "FileTransferQueue is used by ALOG-file, it cannot be deleted") == 0)
								{
									result1 =155;
								}
								else
									result1 =96;
								break;	
							 default:
								 break;
						 }
					}
					admOp.freeadminOperationReturnValue(outVector);
					return result1;
				}
			}
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("Trying to delete transfer queue without (destination set and source directory) options");
			
			std::vector<std::string>  p_rdnList;
			omHandler.getChildren(myDNNameOfTQ.c_str(), ACS_APGCC_SUBLEVEL,&p_rdnList);
			AES_AFP_TRACE_MESSAGE("Number of adv param objs under %s group is : %zu ",myDNNameOfTQ.c_str(),p_rdnList.size());
			if(p_rdnList.size()==0)
			{
				AES_AFP_TRACE_MESSAGE("There are no adv param objects");
				result1 = AES_CATASTROPHIC;
				return result1;

			}
			else if(p_rdnList.size()==1)
			{
				AES_AFP_TRACE_MESSAGE("Size of Advanced parameters objects vector %zu ",p_rdnList.size());
				ACE_INT32 returnCode;
				string advParamDn("");

				advParamDn = p_rdnList[0].c_str();
				AES_AFP_TRACE_MESSAGE("Adv param dn =  %s ",advParamDn.c_str());
				returnCode = omHandler.deleteObject(const_cast<char*>(advParamDn.c_str()));
				if( returnCode != ACS_CC_SUCCESS )
				{
					AES_AFP_TRACE_MESSAGE( "Deletion of advance param Object failed %s",omHandler.getInternalLastErrorText());
					if(omHandler.getInternalLastError() != -12)
					{
						if(omHandler.getExitCode(result1,p_errorText) != ACS_CC_SUCCESS)
						{
							result1 = AES_CATASTROPHIC;
							advParamDn.clear();
							return result1;
						}
					}
					else
					{
						result1 = AES_NOSERVERACCESS;
						advParamDn.clear();
						return result1;
					}
					advParamDn.clear();
					errorMessage = p_errorText;
					return result1;
				}
				else
				{
					advParamDn.clear();
					AES_AFP_TRACE_MESSAGE("!Success : Deletion of advance param Object");
					returnCode = omHandler.deleteObject(const_cast<char*>(myDNNameOfTQ.c_str()));
					if( returnCode != ACS_CC_SUCCESS )
					{
						AES_AFP_TRACE_MESSAGE("Deletion of advance param Object failed %d",omHandler.getInternalLastError());
						if(omHandler.getInternalLastError() != -12)
						{
							if(omHandler.getExitCode(result1,p_errorText)!=ACS_CC_SUCCESS) //Naresh check this
							{
								result1=AES_CATASTROPHIC;
								return result1;
							}
						}
						else
						{
							result1=AES_NOSERVERACCESS;
							return result1;
						}
						errorMessage = p_errorText;
						return result1;
					}
					else
					{
						AES_AFP_TRACE_MESSAGE("!Success: Deletion of File transfer queue Object");
						return AES_NOERRORCODE;
					}
				}
			}
			else
			{
				AES_AFP_TRACE_MESSAGE("!Error : File transfer queue Object has more than one Adv param object");
				return AES_CATASTROPHIC;
			}
		}
	}

	else //delete destset association
	{
		if(srcDir_ == true)
		{
			usage();
			return false;
		}	

		ACS_CC_ImmParameter paramToFind;
		std::string fileDestination1("");

		fileDestination1 = TQ_FILETQINITDESTID;
		fileDestination1 += "=";
		fileDestination1 += destination_;
		fileDestination1 += ",";
		fileDestination1 += AES_GCC_Util::dnOfFileTransferM;

		paramToFind.attrName = const_cast<char*>(TQ_FILETQINITDESTID);
		int result1 = omHandler.getAttribute(fileDestination1.c_str(), &paramToFind );

		if(result1 == ACS_CC_FAILURE && (strcmp(destination_.c_str(),"-")!=0) )
		{
			ACS_CC_ImmParameter paramToFind;
			std::string fileDestination2("");
			fileDestination2 = TQ_FILETQRESPDESTID;
			fileDestination2 += "=";
			fileDestination2 += destination_;
			fileDestination2 += ",";
			fileDestination2 += AES_GCC_Util::dnOfFileTransferM;
			paramToFind.attrName = const_cast<char*>(TQ_FILETQRESPDESTID);
			if((omHandler.getAttribute(fileDestination2.c_str(), &paramToFind )==ACS_CC_FAILURE )&& (strcmp(destination_.c_str(),"-")!=0))
			{
				AES_AFP_TRACE_MESSAGE(" DestinationSet does not exist %s ",omHandler.getInternalLastErrorText());
				result1 = AES_NODESTINATION;
				return result1;
			}
			else
			{
				respdestflag = true;
			}
		}
		else
		{
			initdestflag = true;
		}


		ACS_CC_ImmParameter par;
		int noOfDestSet=0;
		std::vector<ACS_APGCC_ImmAttribute *> attributes;
		ACS_APGCC_ImmAttribute paramToFind1;
		ACS_APGCC_ImmAttribute paramToFind2;
		bool checkflag=false;
		char initdestset[]=TQ_FILETQINITDEST;
		char respdestset[]=TQ_FILETQRESPDEST;

		if(initdestflag)
		{
			par.attrName = initdestset;
			paramToFind1.attrName = initdestset;
			paramToFind2.attrName = respdestset; 
			attributes.push_back(&paramToFind1);
			attributes.push_back(&paramToFind2);

		}
		else
		{
			AES_AFP_TRACE_MESSAGE("respdestflag = %d", respdestflag);
			par.attrName = respdestset;
			paramToFind1.attrName = respdestset;
			paramToFind2.attrName = initdestset;
			attributes.push_back(&paramToFind1);
			attributes.push_back(&paramToFind2);
		}
		if(omHandler.getAttribute(myDNNameOfTQ.c_str(), attributes) != ACS_CC_SUCCESS)
		{
			AES_AFP_TRACE_MESSAGE("GetAttribute of paramToFind1 Object failed");
			result1 = AES_CATASTROPHIC;
			return result1;
		}
		paramToFind1 = *(attributes[0]);
		paramToFind2 = *(attributes[1]);


		for (unsigned int h=0 ; h < attributes[0]->attrValuesNum ; h++)
		{
			AES_AFP_TRACE_MESSAGE("attr val = %s", (char*)attributes[0]->attrValues[h]);
		}

		/*get the total no of destset; if total no of destset is 1, call the business logic for TQ deletion*/
		ACE_UINT32 totalNumDestSet = paramToFind1.attrValuesNum + paramToFind2.attrValuesNum;
		if (totalNumDestSet == 1)
		{
			/*Check if the destset is associated with TQ*/
			if( paramToFind1.attrValuesNum != 0 && strcmp((char*)paramToFind1.attrValues[0], destination_.c_str())!= 0 )
			{
				AES_AFP_TRACE_MESSAGE("TQ with 1 destset: attrVal[0] = %s, destination_ = %s", (char*)paramToFind1.attrValues[0], destination_.c_str());
				return AES_TQDESTSETCOMBINATION;//TQ_destset does not exist
			}
			if( paramToFind2.attrValuesNum != 0 && strcmp((char*)paramToFind2.attrValues[0], destination_.c_str())!= 0 )
			{
				AES_AFP_TRACE_MESSAGE("TQ with 1 destset: attrVal[0] = %s, destination_ = %s", (char*)paramToFind2.attrValues[0], destination_.c_str());
				return AES_TQDESTSETCOMBINATION;//TQ_destset does not exist
			}

			/*call business logic for TQ deletion, when TQ has only one destset*/
			/*Business logic to delete transfer queue without (destination set and source directory) options*/
			AES_AFP_TRACE_MESSAGE("Trying to delete transfer queue without (destination set and source directory) options");
			std::vector<std::string>  p_rdnList;
			omHandler.getChildren(myDNNameOfTQ.c_str(), ACS_APGCC_SUBLEVEL,&p_rdnList);
			AES_AFP_TRACE_MESSAGE("Number of adv param objs under %s group is : %zu\n ",myDNNameOfTQ.c_str(),p_rdnList.size());
			if(p_rdnList.size()==0)
			{
				AES_AFP_TRACE_MESSAGE("There are no adv param objects");
				result1 = AES_CATASTROPHIC;
				return result1;

			}
			else if(p_rdnList.size()==1)
			{
				AES_AFP_TRACE_MESSAGE("Size of Advanced parameters objects vector %zu\n ",p_rdnList.size());
				ACE_INT32 returnCode;
				string advParamDn("");

				advParamDn = p_rdnList[0].c_str();
				AES_AFP_TRACE_MESSAGE("Adv param dn =  %s ",advParamDn.c_str());
				returnCode = omHandler.deleteObject(const_cast<char*>(advParamDn.c_str()));
				if( returnCode != ACS_CC_SUCCESS )
				{
					AES_AFP_TRACE_MESSAGE("Deletion of advance param Object failed %s",omHandler.getInternalLastErrorText());
					if(omHandler.getInternalLastError() != -12)
					{
						if(omHandler.getExitCode(result1,p_errorText) != ACS_CC_SUCCESS)
						{
							result1 = AES_CATASTROPHIC;
							advParamDn.clear();
							return result1;
						}
					}
					else
					{
						result1 = AES_NOSERVERACCESS;
						advParamDn.clear();
						return result1;
					}
					advParamDn.clear();
					errorMessage = p_errorText;
					return result1;
				}
				else
				{
					advParamDn.clear();
					AES_AFP_TRACE_MESSAGE("!Success : Deletion of advance param Object");
					returnCode = omHandler.deleteObject(const_cast<char*>(myDNNameOfTQ.c_str()));
					if( returnCode != ACS_CC_SUCCESS )
					{
						AES_AFP_TRACE_MESSAGE("Deletion of advance param Object failed %d",omHandler.getInternalLastError());
						if(omHandler.getInternalLastError() != -12)
						{
							if(omHandler.getExitCode(result1,p_errorText)!=ACS_CC_SUCCESS) //Naresh check this
							{
								result1=AES_CATASTROPHIC;
								return result1;
							}
						}
						else
						{
							result1=AES_NOSERVERACCESS;
							return result1;
						}
						errorMessage = p_errorText;
						return result1;
					}
					else
					{
						AES_AFP_TRACE_MESSAGE("!Success: Deletion of File transfer queue Object");
						return AES_NOERRORCODE;
					}
				}
			}
			else
			{
				AES_AFP_TRACE_MESSAGE("!Error : File transfer queue Object has more than one Adv param object");
				return AES_CATASTROPHIC;
			}
			/*Business logic ends*/
		}

		/*call the business logic for TQ Destset association deletion*/
		noOfDestSet = paramToFind1.attrValuesNum;
		if(noOfDestSet <= 0)
		{
			AES_AFP_TRACE_MESSAGE("DestinationSet is not associated to TransferQueue");
			result1 = AES_TQDESTSETCOMBINATION;
			return result1;
		}

		AES_AFP_TRACE_MESSAGE("Number of initiating destset in current TQ : %d",noOfDestSet);
		par.attrType= ATTR_STRINGT;
		par.attrValuesNum = noOfDestSet - 1;
		par.attrValues=new void*[par.attrValuesNum];
		char tmpdest[100];
		for(int a=0; a < noOfDestSet; a++)
		{
			ACE_OS::strcpy(tmpdest ,(char *)paramToFind1.attrValues[a]);
			if( ACE_OS::strcmp(destination_.c_str(),tmpdest) == 0)
			{
				checkflag =  true;
				break;
			}
		}
		if (checkflag == false)
		{
			AES_AFP_TRACE_MESSAGE("DestinationSet is not associated to TransferQueue");
			result1 = AES_TQDESTSETCOMBINATION;
			//delete[] par.attrValues;
			return result1;
		}
		std::string tempstr[noOfDestSet];
		int k=0;
		for(int a=0; a < noOfDestSet; a++)
		{

			ACE_OS::strcpy((char *)tempstr[a].c_str() ,(char *)paramToFind1.attrValues[a]);
			if( ACE_OS::strcmp(destination_.c_str(),tempstr[a].c_str()) != 0)
			{
				par.attrValues[k] = reinterpret_cast<void*>((char *)paramToFind1.attrValues[a]);
				k++;
			}
		}

		if(omHandler.modifyAttribute(myDNNameOfTQ.c_str(), &par) != ACS_CC_SUCCESS)
		{
			AES_AFP_TRACE_MESSAGE("Modify object failed due to %s",omHandler.getInternalLastErrorText());
			if(omHandler.getInternalLastError()!=-12)
			{
				if(omHandler.getExitCode(result1,p_errorText)!=ACS_CC_SUCCESS) //Naresh check this
				{
					result1=AES_CATASTROPHIC;
					//delete[] par.attrValues;
					return result1;
				}
			}
			else
			{
				result1=AES_NOSERVERACCESS;
				//delete[] par.attrValues;
				return result1;
			}
			errorMessage = p_errorText;
			//delete[] par.attrValues;
			return result1;
		}
		//delete[] par.attrValues;
	}

	return result1;
}

