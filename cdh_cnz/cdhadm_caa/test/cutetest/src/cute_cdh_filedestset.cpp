#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "cute_cdh_filedestset.h"
#include <servr.h>
//#include "ACS_CC_Types.h"

#define FILEDESTSET_CMD_FILE "/opt/ap/aes/bin/fileDestSetCmdFile"

vector<string> CuteFileDestSet::cmdStringList;
vector<string> CuteFileDestSet::dataList;
vector<string> CuteFileDestSet::cmdResultList;
AES_CDH_FileDestSetCmdHandler * CuteFileDestSet::fileDestSetHandlerPtr[2];
AES_CDH_DestCmdHandler * CuteFileDestSet::localDestHandlerPtr;
AES_CDH_FileRespDestCmdHandler * CuteFileDestSet::respondingdestHandlerPtr[3];
void createMap();
//void getAttrData(string paramStr,ACS_APGCC_AttrValues **attr);
map<string,ACS_CC_AttrValueType> cmdTypeMap;
#if 0
void getAttrData(string paramStr,ACS_APGCC_AttrValues **attr)
{
	map<string,ACS_CC_AttrValueType>::iterator it;
	//	char * className = new char[50];
	string className("");
	//	strcpy(className,"FileDestGroupCategory");
	int dataStartPos=data.find_first_of('(');
	char * parentName= new char[50];
	strcpy(parentName,"dataTransferMId=1");
	className=data.substr(0,dataStartPos);
	cout<<"Class Name = "<<className.c_str()<<endl;

	string paramStr=data.substr(dataStartPos+1);
	cout<<" paramStr = "<<paramStr<<endl; //
	string::size_type dataClosePos=paramStr.find_last_of(')');
	string::size_type pos1=0;
	string::size_type commaPos;
	int i=0;
	while(pos1<dataClosePos)
	{
		attr[i]=new ACS_APGCC_AttrValues();
		commaPos=paramStr.find_first_of('&',pos1);
		cout<<"commaPosition = "<<commaPos<<endl;
		if(commaPos!=string::npos)
		{
			string tmpStr = paramStr.substr(pos1,commaPos-pos1);
			cout<<"tmpstr before attrType : "<<tmpStr<<endl;
			string::size_type colonPos=tmpStr.find_first_of(':');
			it = cmdTypeMap.find(tmpStr.substr(0,colonPos));
			attr[i]->attrType=((*it).second);
			cout<<"attrType = "<<tmpStr.substr(0,colonPos)<<"\t attr Type = "<<(*it).second <<" \t attr[i]->attrType : "<<attr[i]->attrType<<endl;

			tmpStr = tmpStr.substr(colonPos+1);
			cout<<"tmpstr before attrName : "<<tmpStr<<endl;
			colonPos=tmpStr.find_first_of(':');
			attr[i]->attrName = new char[100];
			strcpy(attr[i]->attrName,(tmpStr.substr(0,colonPos)).c_str());
			cout<<"attr Name = "<<(tmpStr.substr(0,colonPos)).c_str()<<endl;

			tmpStr = tmpStr.substr(colonPos+1);
			cout<<"tmpstr before attrValues : "<<tmpStr<<endl;
			colonPos=tmpStr.find_first_of(':');
			attr[i]->attrValues =  new void *[1];

			if(attr[i]->attrType == ATTR_INT32T || attr[i]->attrType == ATTR_UINT32T)
			{
				int intVal=atoi((tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<intVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(&intVal);
			}
			else if(attr[i]->attrType == ATTR_STRINGT || attr[i]->attrType == ATTR_NAMET  || attr[i]->attrType == ATTR_TIMET )
			{
				char * charVal = new char[10];
				strcpy(charVal,(tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<charVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(charVal);
			}
			else if(attr[i]->attrType == ATTR_INT64T || attr[i]->attrType == ATTR_UINT64T)
			{
				long longVal=atol((tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<longVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(&longVal);
			}
			else if(attr[i]->attrType == ATTR_FLOATT || attr[i]->attrType == ATTR_DOUBLET)
			{
				float floatVal=atof((tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<floatVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(&floatVal);

			}

			tmpStr = tmpStr.substr(colonPos+1);
			cout<<"tmpstr before attrValuesNum : "<<tmpStr<<endl;
			attr[i]->attrValuesNum=atoi(tmpStr.c_str());
			cout<<"Attr value Num: "<<atoi(tmpStr.c_str())<<endl;
			pos1=commaPos+1;
			i++;
		}
		else
		{
			cout<<"Searching for comma failed so it might be last attribute "<<endl;
			string tmpStr = paramStr.substr(pos1,dataClosePos);
			cout<<"tmpstr before attrType : "<<tmpStr<<endl;
			string::size_type colonPos=tmpStr.find_first_of(':');

			it = cmdTypeMap.find(tmpStr.substr(0,colonPos));
			attr[i]->attrType =((*it).second);
			cout<<"attrType = "<<tmpStr.substr(0,colonPos)<<" \t attr Type = "<<(*it).second <<" \t attr[i]->attrType : "<<attr[i]->attrType<<endl;

			attr[i]->attrName = new char[100];
			tmpStr = tmpStr.substr(colonPos+1);
			cout<<"tmpstr before attrName : "<<tmpStr<<endl;
			colonPos=tmpStr.find_first_of(':');
			strcpy(attr[i]->attrName,(tmpStr.substr(0,colonPos)).c_str());
			cout<<"attr Name = "<<(tmpStr.substr(0,colonPos)).c_str()<<endl;

			tmpStr = tmpStr.substr(colonPos+1);
			cout<<"tmpstr before attrValues : "<<tmpStr<<endl;
			colonPos=tmpStr.find_first_of(':');
			attr[i]->attrValues =  new void *[1];

			if(attr[i]->attrType == ATTR_INT32T || attr[i]->attrType == ATTR_UINT32T)
			{
				int intVal=atoi((tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<intVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(&intVal);
			}
			else if(attr[i]->attrType == ATTR_STRINGT || attr[i]->attrType == ATTR_NAMET  || attr[i]->attrType == ATTR_TIMET )
			{
				char * charVal1 = new char[10];
				strcpy(charVal1,(tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<charVal1<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(charVal1);
			}
			else if(attr[i]->attrType == ATTR_INT64T || attr[i]->attrType == ATTR_UINT64T)
			{
				long longVal=atol((tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<longVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(&longVal);
			}
			else if(attr[i]->attrType == ATTR_FLOATT || attr[i]->attrType == ATTR_DOUBLET)
			{
				float floatVal=atof((tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<floatVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(&floatVal);

			}

			tmpStr = tmpStr.substr(colonPos+1);
			colonPos=tmpStr.find_first_of(')');
			cout<<"tmpstr before attrValuesNum : "<<tmpStr<<endl;
			attr[i]->attrValuesNum=atoi(tmpStr.substr(0,colonPos).c_str());
			cout<<"Attr value Num as string  : "<<atoi(tmpStr.substr(0,colonPos).c_str())<<endl;
			cout<<"Attr value Num as num : "<<atoi(tmpStr.substr(0,colonPos).c_str())<<endl;
			pos1=dataClosePos;
			i++;
		}

	}
	attr[i] = 0;
}
#endif
void createMap()
{
	cmdTypeMap["string"]=ATTR_STRINGT;
	cout<<" ATTR_STRINGT = "<<ATTR_STRINGT<<endl;
	cmdTypeMap["int32"]=ATTR_INT32T;
	cout<<" ATTR_INT32T = "<<ATTR_INT32T<<endl;
	cmdTypeMap["uint32"]=ATTR_UINT32T;
	cout<<" ATTR_UINT32T = "<<ATTR_UINT32T<<endl;
	cmdTypeMap["int64"]=ATTR_INT64T;
	cout<<" ATTR_INT64T = "<<ATTR_INT64T<<endl;
	cmdTypeMap["uint64"]=ATTR_UINT64T;
	cout<<" ATTR_UINT64T = "<<ATTR_UINT64T<<endl;
	cmdTypeMap["time"]=ATTR_TIMET;
	cout<<" ATTR_TIMET = "<<ATTR_TIMET<<endl;
	cmdTypeMap["name"]=ATTR_NAMET;
	cout<<" ATTR_NAMET = "<<ATTR_NAMET<<endl;
	cmdTypeMap["float"]=ATTR_FLOATT;
	cout<<" ATTR_FLOATT = "<<ATTR_FLOATT<<endl;
	cmdTypeMap["double"]=ATTR_DOUBLET;
	cout<<" ATTR_DOUBLET = "<<ATTR_DOUBLET<<endl;
	cmdTypeMap["any"]=ATTR_ANYT;
	cout<<" ATTR_ANYT = "<<ATTR_ANYT<<endl;
}
bool CuteFileDestSet::fetchData(std::string fileName)
{
	std::string pszReadLine ="\0";
	ACE_TCHAR szTemp[512];
	ACE_OS::strncpy(szTemp, fileName.c_str(), 511);
	ifstream * commandFile = new ifstream(szTemp,ios::in);
	if ( commandFile == 0 )
	{
		cout<<"Failed to Open the file "<<szTemp<<endl;
		return false;
	}
	while((!(getline(*commandFile, pszReadLine)<=0)))
	{
		if ((pszReadLine.size()!=0))
		{
			int pos1=pszReadLine.find('>');
			int pos2=pszReadLine.find(')');
			int pos3=pszReadLine.find(';');
			std::string commandString=pszReadLine.substr(0,pos1);
			cout<<"commandString = "<<commandString<<endl;
			std::string dataStr = pszReadLine.substr(pos1+1,pos2-pos1);
			cout<<"dataStr = "<<dataStr<<endl;
			std::string commandResult=pszReadLine.substr(pos2+1,pos3-pos2-1);
			cout<<"commandResult = "<<commandResult<<endl;
			cmdStringList.push_back(commandString);
			dataList.push_back(dataStr);
			cmdResultList.push_back(commandResult);
		}

	}
	//content of the lists
	vector<string>::iterator it1;
	vector<string>::iterator it2;
	for(it1=dataList.begin(),it2=cmdResultList.begin();it1!=dataList.end(),it2!=cmdResultList.end();it1++,it2++)
	{
		cout<<"command pair = "<<(*it1)<<"\t"<<(*it2)<<endl;
	}
	return true;
}

void CuteFileDestSet::fileDestSetCmdexecute()
{
	//	std::list<std::string>::iterator cmdItr;
	//	std::list<std::string>::iterator dataStrItr;
	//	std::list<std::string>::iterator resultStrItr;
	//	cmdItr=cmdStringList.begin();
	//	dataStrItr = dataList.begin();
	//	resultStrItr = cmdResultList.begin();
	createMap();
	if(fetchData(FILEDESTSET_CMD_FILE))
	{
		string resultStr("");
		bool status=false;
		//createCommandPairLists(fileName);
		//		for(cdItr,dataStrItr,resultStrItr;	cmdItr!=cmdStringList.end(),dataStrItr!=dataList.end(),resultStrItr!=cmdResultList.end();cmdItr++,dataStrItr++,resultStrItr++)
		for(unsigned int i=0;i<cmdResultList.size();i++)
		{
			cout<<"######################################\n\n";
			cout<<"Executing command numbered : "<<i<<endl;
			cout<<"!!command : \n "<<cmdStringList[i]<<dataList[i]<<endl<<endl;

			//			if(*cmdItr == "CREATE")
			if(cmdStringList[i]=="CREATE")
			{
				status = vCreate(dataList[i]); //bCreate returns true or false
			}
			else if (cmdStringList[i]== "MODIFY")
			{
				vModify(dataList[i],resultStr);
			}
			else if (cmdStringList[i] == "DELETE")
			{
				vDeleted(dataList[i],resultStr);
			}
			else
			{
				cout<<"Please enter the valid command"<<endl;
			}

			cout<<"Result of the command= "<<status<<endl;
			if (status)
			{
				resultStr= "PASS";
			}
			else
			{
				resultStr= "FAIL";
			}
			if(resultStr==cmdResultList[i])
			{
				cout<<"\n ##Test case : PASSED##\n";
				cout<<"######################################\n\n";
				resultStr.clear();
			}
			else
			{
				cmdStringList.clear();
				dataList.clear();
				cmdResultList.clear();
				resultStr.clear();
				ASSERTM("##Test Case : FAILED##",false);
				break;
			}

		}
		cmdStringList.clear();
		dataList.clear();
		cmdResultList.clear();
	}
}

cute::suite CuteFileDestSet::make_suite_cute_cdh_filedestset()
{
	fileDestSetHandlerPtr[0] = new AES_CDH_FileDestSetCmdHandler(AES_CDH_RESPDESTSET_CLASSNAME,AES_CDH_RESPDESTSET_IMPLEMENTER);
	fileDestSetHandlerPtr[1] = new AES_CDH_FileDestSetCmdHandler(AES_CDH_INITDESTSET_CLASSNAME,AES_CDH_INITDESTSET_IMPLEMENTER);
	if(fileDestSetHandlerPtr[0]!=0 && fileDestSetHandlerPtr[1]!=0)
	{
		cout<<"FileDest set Command Handler ptr created"<<endl;
	}
	else
	{
		cout<<"!!Failed: FileDestset Command Handler ptr creation "<<endl;
	}

	localDestHandlerPtr = new AES_CDH_DestCmdHandler(AES_CDH_LOCALDEST_IMM_CLASS_NM,AES_CDH_LOCALDEST_IMPLEMENTER,ACS_APGCC_ONE);
	if(localDestHandlerPtr!=0)
	{
		cout<<"Local Destination Command Handler ptr created"<<endl;
	}
	else
	{
		cout<<"!!Failed: Local Destination Command Handler ptr creation "<<endl;
	}



	respondingdestHandlerPtr[0] = new AES_CDH_FileRespDestCmdHandler(AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM,AES_CDH_FILE_RESP_DEST_IMM_PRIM_IMPL,ACS_APGCC_ONE);
	respondingdestHandlerPtr[1] = new AES_CDH_FileRespDestCmdHandler(AES_CDH_FILE_RESP_DEST_IMM_SEC_CLASS_NM,AES_CDH_FILE_RESP_DEST_IMM_SEC_IMPL,ACS_APGCC_ONE);
	respondingdestHandlerPtr[2] = new AES_CDH_FileRespDestCmdHandler(AES_CDH_ADV_RESP_PARAMS_CLASS_NM,AES_CDH_ADV_RESP_DEST_IMM_IMPL,ACS_APGCC_ONE);
	if(respondingdestHandlerPtr[0]!=0 && respondingdestHandlerPtr[1]!=0 && respondingdestHandlerPtr[2]!=0)
	{
		cout<<"Responding Destination Command Handler ptrs created"<<endl;
	}
	else
	{
		cout<<"!!Failed: Responding Destination Command Handler ptrs creation "<<endl;
	}


	cute::suite s;
	s.push_back(CUTE(fileDestSetCmdexecute));
	return s;
}

bool CuteFileDestSet::vCreate(std::string data)
{

	map<string,ACS_CC_AttrValueType>::iterator it;
	//	char * className = new char[50];
	string className("");
	//	strcpy(className,"FileDestGroupCategory");
	int dataStartPos=data.find_first_of('(');
	char * parentName= new char[50];
	strcpy(parentName,"dataTransferMId=1");
	className=data.substr(0,dataStartPos);
	cout<<"Class Name = "<<className.c_str()<<endl;

	string paramStr=data.substr(dataStartPos+1);
	cout<<" paramStr = "<<paramStr<<endl; //
	string::size_type dataClosePos=paramStr.find_last_of(')');
	string::size_type pos1=0;

	//find number of attr
	string::size_type found;
	int count=0;
	found=paramStr.find_first_of('&');
	while(found!=string::npos)
	{
		++count;
		found=paramStr.find_first_of('&',found+1);
	}

	cout<<"Number of attr , count = "<<count<<endl;
	ACS_APGCC_AttrValues *attr[count];
	string::size_type commaPos;
	int i=0;
	while(pos1<dataClosePos)
	{
		attr[i]=new ACS_APGCC_AttrValues();
		commaPos=paramStr.find_first_of('&',pos1);
		cout<<"commaPosition = "<<commaPos<<endl;
		if(commaPos!=string::npos)
		{
			string tmpStr = paramStr.substr(pos1,commaPos-pos1);
			cout<<"tmpstr before attrType : "<<tmpStr<<endl;
			string::size_type colonPos=tmpStr.find_first_of(':');
			it = cmdTypeMap.find(tmpStr.substr(0,colonPos));
			attr[i]->attrType=((*it).second);
			cout<<"attrType = "<<tmpStr.substr(0,colonPos)<<"\t attr Type = "<<(*it).second <<" \t attr[i]->attrType : "<<attr[i]->attrType<<endl;

			tmpStr = tmpStr.substr(colonPos+1);
			cout<<"tmpstr before attrName : "<<tmpStr<<endl;
			colonPos=tmpStr.find_first_of(':');
			attr[i]->attrName = new char[100];
			strcpy(attr[i]->attrName,(tmpStr.substr(0,colonPos)).c_str());
			cout<<"attr Name = "<<(tmpStr.substr(0,colonPos)).c_str()<<endl;

			tmpStr = tmpStr.substr(colonPos+1);
			cout<<"tmpstr before attrValues : "<<tmpStr<<endl;
			colonPos=tmpStr.find_first_of(':');
			attr[i]->attrValues =  new void *[1];

			if(attr[i]->attrType == ATTR_INT32T || attr[i]->attrType == ATTR_UINT32T)
			{
				int intVal=atoi((tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<intVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(&intVal);
			}
			else if(attr[i]->attrType == ATTR_STRINGT || attr[i]->attrType == ATTR_NAMET  || attr[i]->attrType == ATTR_TIMET )
			{
				char * charVal = new char[10];
				strcpy(charVal,(tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<charVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(charVal);
			}
			else if(attr[i]->attrType == ATTR_INT64T || attr[i]->attrType == ATTR_UINT64T)
			{
				long longVal=atol((tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<longVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(&longVal);
			}
			else if(attr[i]->attrType == ATTR_FLOATT || attr[i]->attrType == ATTR_DOUBLET)
			{
				float floatVal=atof((tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<floatVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(&floatVal);

			}

			tmpStr = tmpStr.substr(colonPos+1);
			cout<<"tmpstr before attrValuesNum : "<<tmpStr<<endl;
			attr[i]->attrValuesNum=atoi(tmpStr.c_str());
			cout<<"Attr value Num: "<<atoi(tmpStr.c_str())<<endl;
			pos1=commaPos+1;
			i++;
		}
		else
		{
			cout<<"Searching for comma failed so it might be last attribute "<<endl;
			string tmpStr = paramStr.substr(pos1,dataClosePos);
			cout<<"tmpstr before attrType : "<<tmpStr<<endl;
			string::size_type colonPos=tmpStr.find_first_of(':');

			it = cmdTypeMap.find(tmpStr.substr(0,colonPos));
			attr[i]->attrType =((*it).second);
			cout<<"attrType = "<<tmpStr.substr(0,colonPos)<<" \t attr Type = "<<(*it).second <<" \t attr[i]->attrType : "<<attr[i]->attrType<<endl;

			attr[i]->attrName = new char[100];
			tmpStr = tmpStr.substr(colonPos+1);
			cout<<"tmpstr before attrName : "<<tmpStr<<endl;
			colonPos=tmpStr.find_first_of(':');
			strcpy(attr[i]->attrName,(tmpStr.substr(0,colonPos)).c_str());
			cout<<"attr Name = "<<(tmpStr.substr(0,colonPos)).c_str()<<endl;

			tmpStr = tmpStr.substr(colonPos+1);
			cout<<"tmpstr before attrValues : "<<tmpStr<<endl;
			colonPos=tmpStr.find_first_of(':');
			attr[i]->attrValues =  new void *[1];

			if(attr[i]->attrType == ATTR_INT32T || attr[i]->attrType == ATTR_UINT32T)
			{
				int intVal=atoi((tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<intVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(&intVal);
			}
			else if(attr[i]->attrType == ATTR_STRINGT || attr[i]->attrType == ATTR_NAMET  || attr[i]->attrType == ATTR_TIMET )
			{
				char * charVal1 = new char[10];
				strcpy(charVal1,(tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<charVal1<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(charVal1);
			}
			else if(attr[i]->attrType == ATTR_INT64T || attr[i]->attrType == ATTR_UINT64T)
			{
				long longVal=atol((tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<longVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(&longVal);
			}
			else if(attr[i]->attrType == ATTR_FLOATT || attr[i]->attrType == ATTR_DOUBLET)
			{
				float floatVal=atof((tmpStr.substr(0,colonPos)).c_str());
				cout<<"attr value = "<<floatVal<<endl;
				attr[i]->attrValues[0]=reinterpret_cast<void*>(&floatVal);

			}

			tmpStr = tmpStr.substr(colonPos+1);
			colonPos=tmpStr.find_first_of(')');
			cout<<"tmpstr before attrValuesNum : "<<tmpStr<<endl;
			attr[i]->attrValuesNum=atoi(tmpStr.substr(0,colonPos).c_str());
			cout<<"Attr value Num as string  : "<<atoi(tmpStr.substr(0,colonPos).c_str())<<endl;
			cout<<"Attr value Num as num : "<<atoi(tmpStr.substr(0,colonPos).c_str())<<endl;
			pos1=dataClosePos;
			i++;
		}

	}
	attr[i] = 0;

	if(className.compare(AES_CDH_RESPDESTSET_CLASSNAME)==0)
	{
		cout<<"Calling create of "<<AES_CDH_RESPDESTSET_CLASSNAME<<endl;
		ACS_CC_ReturnType result;
		result = fileDestSetHandlerPtr[0]->create(0, 0, className.c_str(), parentName, attr);
		if(result == ACS_CC_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if(className.compare(AES_CDH_INITDESTSET_CLASSNAME)==0)
	{
		cout<<"Calling create of "<<AES_CDH_RESPDESTSET_CLASSNAME<<endl;
		ACS_CC_ReturnType result;
		result = fileDestSetHandlerPtr[1]->create(0, 0, className.c_str(), parentName, attr);
		if(result == ACS_CC_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	else if(className.compare("LocalDestination")==0)
	{
		cout<<"Calling create of Local Destination"<<endl;
		ACS_CC_ReturnType result;
		result = localDestHandlerPtr->create(0, 0, className.c_str(), parentName, attr);
		if(result == ACS_CC_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(className.compare(AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM)==0)
	{
		ACS_APGCC_OiHandle oiHandle;
		ACS_APGCC_CcbId ccbId = 1000;
		ServR * gServR = new ServR();
		gServR->setDataPathForStatusFile();
		cout<<"Calling create of "<<AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM<<endl;
		ACS_CC_ReturnType result;
		result = respondingdestHandlerPtr[0] ->create(oiHandle, ccbId, className.c_str(), parentName, attr);
		if(result == ACS_CC_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(className.compare(AES_CDH_FILE_RESP_DEST_IMM_SEC_CLASS_NM)==0)
	{
		cout<<"Calling create of "<<AES_CDH_FILE_RESP_DEST_IMM_SEC_CLASS_NM<<endl;
		ACS_CC_ReturnType result;
		result = respondingdestHandlerPtr[1] ->create(0, 0, className.c_str(), parentName, attr);
		if(result == ACS_CC_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(className.compare(AES_CDH_ADV_RESP_PARAMS_CLASS_NM)==0)
	{
		cout<<"Calling create of "<<AES_CDH_ADV_RESP_PARAMS_CLASS_NM<<endl;
		ACS_CC_ReturnType result;
		result = respondingdestHandlerPtr[2] ->create(0, 0, className.c_str(), parentName, attr);
		if(result == ACS_CC_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}



	return false;
}
void CuteFileDestSet::vDeleted(std::string data,std::string status)
{

}
void CuteFileDestSet::vModify(std::string data,std::string status)
{
#if 0
	map<string,ACS_CC_AttrValueType>::iterator it;
	//	char * className = new char[50];
	string className("");
	//	strcpy(className,"FileDestGroupCategory");
	int dataStartPos=data.find_first_of('(');
	char * parentName= new char[50];
	strcpy(parentName,"dataTransferMId=1");
	className=data.substr(0,dataStartPos);
	cout<<"Class Name = "<<className.c_str()<<endl;

	string paramStr=data.substr(dataStartPos+1);
	cout<<" paramStr = "<<paramStr<<endl; //
	string::size_type dataClosePos=paramStr.find_last_of(')');
	string::size_type pos1=0;

	//find number of attr
	string::size_type found;
	int count=0;
	found=paramStr.find_first_of('&');
	while(found!=string::npos)
	{
		++count;
		found=paramStr.find_first_of('&',found+1);
	}

	cout<<"Number of attr , count = "<<count<<endl;
	ACS_APGCC_AttrValues *attr[count];


	if(className.compare(AES_CDH_RESPDESTSET_CLASSNAME)==0)
	{
		cout<<"Calling create of "<<AES_CDH_RESPDESTSET_CLASSNAME<<endl;
		ACS_CC_ReturnType result;
		result = fileDestSetHandlerPtr[0]->modify()(0, 0,objName.c_str(), attr);
		if(result == ACS_CC_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if(className.compare(AES_CDH_INITDESTSET_CLASSNAME)==0)
	{
		cout<<"Calling create of "<<AES_CDH_RESPDESTSET_CLASSNAME<<endl;
		ACS_CC_ReturnType result;
		result = fileDestSetHandlerPtr[1]->create(0, 0, className.c_str(), parentName, attr);
		if(result == ACS_CC_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	else if(className.compare("LocalDestination")==0)
	{
		cout<<"Calling create of Local Destination"<<endl;
		ACS_CC_ReturnType result;
		result = localDestHandlerPtr->create(0, 0, className.c_str(), parentName, attr);
		if(result == ACS_CC_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(className.compare(AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM)==0)
	{
		ACS_APGCC_OiHandle oiHandle;
		ACS_APGCC_CcbId ccbId = 1000;
		cout<<"Calling create of "<<AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM<<endl;
		ACS_CC_ReturnType result;
		result = respondingdestHandlerPtr[0] ->create(oiHandle, ccbId, className.c_str(), parentName, attr);
		if(result == ACS_CC_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(className.compare(AES_CDH_FILE_RESP_DEST_IMM_SEC_CLASS_NM)==0)
	{
		cout<<"Calling create of "<<AES_CDH_FILE_RESP_DEST_IMM_SEC_CLASS_NM<<endl;
		ACS_CC_ReturnType result;
		result = respondingdestHandlerPtr[1] ->create(0, 0, className.c_str(), parentName, attr);
		if(result == ACS_CC_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if(className.compare(AES_CDH_ADV_RESP_PARAMS_CLASS_NM)==0)
	{
		cout<<"Calling create of "<<AES_CDH_ADV_RESP_PARAMS_CLASS_NM<<endl;
		ACS_CC_ReturnType result;
		result = respondingdestHandlerPtr[2] ->create(0, 0, className.c_str(), parentName, attr);
		if(result == ACS_CC_SUCCESS )
		{
			return true;
		}
		else
		{
			return false;
		}
	}



	return false;
#endif

}
void CuteFileDestSet::vAdminOperationCallback(std::string data,std::string status)
{

}



