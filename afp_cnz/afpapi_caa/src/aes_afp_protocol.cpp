#include "aes_afp_protocol.h"
#include "aes_afp_events.h"
#include "aes_afp_renametypes.h"
#include "aes_afp_api_tracer.h"

AES_AFP_TRACE_DEFINE(AES_AFP_protocolbase);

#include <ace/Recursive_Thread_Mutex.h>


static const char empty[] = "";

aes_afp_protocolbase::aes_afp_protocolbase()
      : PROTOCOLVERSION_("PROTOCOLVERSION=VERSION_2.0"),
        longValue_(0),
        intValue_(0),
        tmpString_("")
{
	protocolPair_.first = "STATUS";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	//For TR HN35007- start
	protocolOptionPair_.first = "STATUS";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "REMOVEBEFORE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	//For TR HN35007- end

	protocolPair_.first = "REMOVEBEFORE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);
}


aes_afp_protocolbase::~aes_afp_protocolbase()
{
}

bool aes_afp_protocolbase::readFromFile (std::string& path, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("Path to file %s", path.c_str());

	std::fstream fin;

	ProtocolMap::iterator itr;
	//std::pair<std::string, std::string> parameterPair;


	int index = 0;
	fin.open(path.c_str(), std::ios::in);
	std::string tmpReadString;
	fin >> tmpReadString;
	if (!fin.good() || fin.eof())
	{
		AES_AFP_TRACE_MESSAGE("ERROR: No access to file <%s>", path.c_str());
		Event::report(EVENT_FILENOTOPEN, "OPEN FILE FAULT", path, "Not able to open file or directory: " + path);
		fin.close();
		error = AES_NOACCESS;
		return false;
	}

	do
	{
		index = tmpReadString.find_first_of('=');
		if(0 == (tmpReadString.substr(0, index)).compare("PROTOCOLVERSION"))
		{
			fin >> tmpReadString;
			continue;
		}

		if (index != -1)
		{
			// Find the parameter in protocol
			itr = protocolMap_.find(tmpReadString.substr(0, index) );
			if (itr != protocolMap_.end() )
			{
				// Set the value for parameter
				(*itr).second = tmpReadString.substr(index+1);
			}
			else
			{
				// If a parameter is not found it could be that it is removed in a another version, just skip this parameter.
				AES_AFP_TRACE_MESSAGE("Could not find parameter %s", tmpReadString.substr(0, index).c_str());
			}
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("Could not find delimeter = in file entry");
			error = AES_NOACCESS;
			fin.close();
			return false;
		}
		fin >> tmpReadString;
	} while (fin.good() );

	fin.close();
	error = AES_NOERRORCODE;
	return true;
}

bool aes_afp_protocolbase::addStringValue (std::string parameter, std::string value)
{
	ProtocolMap::iterator itr;

	itr = protocolMap_.find(parameter);
	if (itr != protocolMap_.end() )
	{
		(*itr).second = value;
		return true;
	}
	AES_AFP_TRACE_MESSAGE("Parameter %s not found", parameter.c_str());

	return false;
}

bool aes_afp_protocolbase::addIntValue (std::string parameter, int value)
{
	ProtocolMap::iterator itr;
	char intBuf[512];

	sprintf(intBuf, "%d", value);

	itr = protocolMap_.find(parameter);
	if (itr != protocolMap_.end() )
	{
		(*itr).second = intBuf;
		return true;
	}
	AES_AFP_TRACE_MESSAGE("Parameter %s not found", parameter.c_str());

	return false;
}

bool aes_afp_protocolbase::addLongValue (std::string parameter, unsigned long value)
{
	ProtocolMap::iterator itr;
	char intBuf[512];

	sprintf(intBuf, "%lu", value);

	itr = protocolMap_.find(parameter);
	if (itr != protocolMap_.end() )
	{
		(*itr).second = intBuf;
		return true;
	}
	AES_AFP_TRACE_MESSAGE("Parameter %s not found", parameter.c_str());

	return false;
}

bool aes_afp_protocolbase::addBoolValue (std::string parameter, bool value)
{
	ProtocolMap::iterator itr;

	itr = protocolMap_.find(parameter);
	if (itr != protocolMap_.end() )
	{
		if (value == false)
		{
			(*itr).second = "0";
		}
		else
		{
			(*itr).second = "1";
		}
		return true;
	}
	AES_AFP_TRACE_MESSAGE("Parameter %s not found", parameter.c_str());
	
	return false;
}

bool aes_afp_protocolbase::clearValues ()
{
	ProtocolMap::iterator itr;
	ProtocolOptionMap::iterator optionItr;

	for (itr = protocolMap_.begin();itr != protocolMap_.end();++itr)
	{
		(*itr).second = "";
	}

	//For TR HN35007: to change all entries of bool to false in map.
	for (optionItr = protocolOptionMap_.begin();optionItr != protocolOptionMap_.end();optionItr++)
	{
		(*optionItr).second = false;
	}

	return true;
}

bool aes_afp_protocolbase::getString (std::string& str)
{
	ProtocolMap::iterator itr;

	str = "";

	for (itr = protocolMap_.begin();itr != protocolMap_.end();++itr)
	{
		str += (*itr).first + '=' + (*itr).second + '\n';
	}

	return true;
}

bool aes_afp_protocolbase::setString (std::string& str)
{
	ProtocolMap::iterator itr;
	ProtocolOptionMap::iterator optionItr;

	unsigned short startValue(0);
	short index(0);
	short indexSub(0);
	std::string tmpSubString("");
	std::string tmpParameter("");
	std::string tmpValue("");
	bool optionSet(false);


	index = str.find_first_of('\n', index);

	while (index != -1)
	{
		if (index == 0)
		{
			return false;
		}

		tmpSubString = str.substr(startValue, index - startValue);
		indexSub = tmpSubString.find_first_of('=');
		if (indexSub == -1)
		{
			return false;
		}

		itr = protocolMap_.find(tmpSubString.substr(0, indexSub) );
		if (itr == protocolMap_.end() )
		{
			return false;
		}

		//tmpValue = tmpSubString.substr(indexSub+1);
		//For TR HN35007: Check if the attribute is already used or not by accessing
		//				  its bool flag from protocolOptionMap. Value will be not read
		//                if is already read and a non empty string is found.
		optionItr = protocolOptionMap_.find(tmpSubString.substr(0, indexSub) );
		if(optionItr != protocolOptionMap_.end() )
		{
			optionSet = (*optionItr).second;
		}
		else
		{
			optionSet = false;
		}

		if(optionSet == false )
		{
			tmpValue = tmpSubString.substr(indexSub+1);
			if (tmpValue.empty() )
			{
				(*itr).second = "";
			}
			else
			{
				(*itr).second = tmpValue;
				if(optionItr != protocolOptionMap_.end() )
				{
					(*optionItr).second = true;
				}
			}
		}

		startValue = index+1;
		index = str.find_first_of('\n', index+1);
	}

	return true;
}

std::string aes_afp_protocolbase::getStringValue (std::string parameter)
{
	ProtocolMap::iterator itr;

	itr = protocolMap_.find(parameter);
	if (itr == protocolMap_.end())
	{
		return tmpString_;
	}

	return (*itr).second;
}

int aes_afp_protocolbase::getIntValue (std::string parameter)
{
	int retValue = 0;
	if( 0 == parameter.compare("RENAMETYPE"))
	{
		retValue = AES_NONE;
	}
	ProtocolMap::iterator itr = protocolMap_.find(parameter);
	if (protocolMap_.end() != itr)
	{
		//Entry found
		std::string tmpValue = (*itr).second;
		if (tmpValue.empty() == false)
		{
			retValue = atoi(tmpValue.c_str() );
		}
	}
	intValue_ = retValue;
	return retValue;
}

long aes_afp_protocolbase::getLongValue (std::string parameter)
{

	long retValue = 0;
	ProtocolMap::iterator itr = protocolMap_.find(parameter);
	if (itr != protocolMap_.end())
	{
		std::string tmpValue = (*itr).second;
		if (false == tmpValue.empty())
		{
			retValue = atol(tmpValue.c_str() );
		}
	}
	longValue_ = retValue;
	return retValue;
}

bool aes_afp_protocolbase::getBoolValue (std::string parameter)
{
	ProtocolMap::iterator itr;
	std::string tmpValue("");

	itr = protocolMap_.find(parameter);
	if (itr == protocolMap_.end())
	{
		return false;
	}

	if ( (*itr).second == "1")
	{
		return true;
	}

	return false;
}

const char* aes_afp_protocolbase::getCharStringValue (std::string parameter)
{
	ProtocolMap::iterator itr;

	itr = protocolMap_.find(parameter);
	if (itr == protocolMap_.end())
	{
		return empty; 
	}
	return (*itr).second.c_str();
}

aes_afp_protocolapi::aes_afp_protocolapi()
{
	protocolPair_.first = "REMOVEDELAY";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);	

	protocolPair_.first = "SOURCEDIRECTORY";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "DESTINATIONSET";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "TRANSFERQUEUE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "USERNAME";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "FILENAME";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "USERGROUP";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "SENDRETRIES";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "SENDRETRIESDELAY";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "MANUALINITIATED";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "RANGEBEGIN";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "RANGEEND";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "NEWDESTINATIONSET";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "RENAMETYPE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "RENAMETEMPLATE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "TRANSFERMODE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "TRANSFERMASK";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "ISDIRECTORY";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "APPLICATION";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "NAMETAG";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "STARTUPSEQUENCENUMBER";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

		//For TR HN35007 - start
	protocolOptionPair_.first = "REMOVEDELAY";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "SOURCEDIRECTORY";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "DESTINATIONSET";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "TRANSFERQUEUE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "USERNAME";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "FILENAME";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "USERGROUP";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "SENDRETRIES";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "SENDRETRIESDELAY";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "MANUALINITIATED";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "RANGEBEGIN";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "RANGEEND";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "NEWDESTINATIONSET";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "RENAMETYPE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "RENAMETEMPLATE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "TRANSFERMODE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "TRANSFERMASK";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "ISDIRECTORY";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "APPLICATION";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "NAMETAG";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

	protocolOptionPair_.first = "STARTUPSEQUENCENUMBER";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);

   //For TR HN35007 - end

}


aes_afp_protocolapi::~aes_afp_protocolapi()
{
	ProtocolMap::iterator itr;
	itr = protocolMap_.begin();
	while( itr != protocolMap_.end() )
	{
		protocolMap_.erase(itr);
		itr = protocolMap_.begin();
	}
	protocolMap_.clear();
}

aes_afp_protocoltq::aes_afp_protocoltq()
{
	protocolPair_.first = "REMOVEDELAY";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);	

	protocolPair_.first = "SOURCEDIRECTORY";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "DESTINATIONSET";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "TRANSFERQUEUE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "TQOBJECTFILEPATH";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "TQOBJECTDIRECTORY";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "ROOTFILEPATH";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "SENDRETRIES";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "SENDRETRIESDELAY";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "MANUALINITIATED";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "TQLOCK";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "ALARM";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "APPLICATION";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "RENAMETYPE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "RENAMETEMPLATE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "ORIGINALFILENAME";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "GENERATEDFILENAME";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "USERGROUP";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "NAMETAG";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "STARTUPSEQUENCENUMBER";
	protocolPair_.second = "-1"; 
	protocolMap_.insert(protocolPair_); // uabmha, CNI 1135: -1 added for case where
	                                    // this tag is added by AFP server when an
	                                    // older TQ file (without this tag) is read
	                                    // and then written back to the file
		  //For TR HN35007 - start
	protocolOptionPair_.first = "REMOVEDELAY";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "SOURCEDIRECTORY";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "DESTINATIONSET";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "TRANSFERQUEUE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "TQOBJECTFILEPATH";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "TQOBJECTDIRECTORY";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "ROOTFILEPATH";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "SENDRETRIES";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "SENDRETRIESDELAY";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "MANUALINITIATED";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "TQLOCK";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "ALARM";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "APPLICATION";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "RENAMETYPE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "RENAMETEMPLATE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "ORIGINALFILENAME";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "GENERATEDFILENAME";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "USERGROUP";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "NAMETAG";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "STARTUPSEQUENCENUMBER";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	//For TR HN35007 - end


}


aes_afp_protocoltq::~aes_afp_protocoltq()
{
	ProtocolMap::iterator itr;
	itr = protocolMap_.begin();
	while( itr != protocolMap_.end() )
	{
		protocolMap_.erase(itr);
		itr = protocolMap_.begin();
	}
	protocolMap_.clear();
}

bool aes_afp_protocoltq::writeToFile (AES_GCC_Errorcodes& error)
{
	std::string filePath;
	ProtocolMap::iterator itr;
	std::string toFile;
	ssize_t written = 0;

	itr = protocolMap_.find("TQOBJECTFILEPATH");
	if (itr != protocolMap_.end() )
	{
		filePath = (*itr).second;
	}

	ACE_HANDLE fHandle = ACE_INVALID_HANDLE;

	//Create the file.
	fHandle = ACE_OS::open(filePath.c_str(), O_WRONLY|O_CREAT|O_BINARY);

	if(fHandle == ACE_INVALID_HANDLE)
	{
		AES_AFP_TRACE_MESSAGE("Could not open file,last error %d", ACE_OS::last_error());
		fHandle = ACE_OS::open(filePath.c_str(), O_WRONLY|O_CREAT);

		if(fHandle == ACE_INVALID_HANDLE)
		{
			AES_AFP_TRACE_MESSAGE("Could not create file,last error %d", ACE_OS::last_error());
			Event::report(EVENT_FILENOTOPEN, "OPEN FILE FAULT", filePath, "Not able to open file or directory: " + filePath);
			error = AES_NOACCESS;
			return false;
		} 
	}

	toFile.clear();

	//toFile += PROTOCOLVERSION_ + "\n";
	toFile.assign(PROTOCOLVERSION_);
	toFile.append("\n");

	for (itr = protocolMap_.begin();itr != protocolMap_.end();++itr)
	{
		if( (*itr).first == "ALARM" ||
			(*itr).first == "APPLICATION" ||
			(*itr).first ==  "GENERATEDFILENAME" ||
			(*itr).first == "MANUALINITIATED" ||
			(*itr).first ==  "ORIGINALFILENAME" ||
			(*itr).first ==  "TQLOCK" ||
			(*itr).first == "TQOBJECTDIRECTORY" ||
			(*itr).first == "SOURCEDIRECTORY" ||
			(*itr).first == "TQOBJECTFILEPATH" )
		{

			toFile.append((*itr).first);//toFile += (*itr).first;
			toFile.append("=");// toFile += "=";
			toFile.append((*itr).second);//toFile += (*itr).second;
			toFile.append("\n");//toFile += "\n";
		}
	}
	//toFile += "\0";

	const size_t fileSize = (toFile.size()+1 > 512)?(toFile.size()+1):512;


	AES_AFP_TRACE_MESSAGE("Going to write <%lu> bytes into <%s>", fileSize, filePath.c_str());

	if (fileSize > 512)
	{
		AES_AFP_TRACE_MESSAGE(
				"#######################\n"
				"   Going to write:\n"
				"#######################\n"
				"<START>%s<END>\n"
				"#######################\n",
				toFile.c_str());
	}

	//fileSize = (fileSize <1024 ? fileSize : 1024);
	char buf[fileSize];
	memset(buf, 0, fileSize);
	memcpy(buf,toFile.c_str(), toFile.size());

	written = ACE_OS::write(fHandle, buf , fileSize);

	if (written <= 0 )
	{
		AES_AFP_TRACE_MESSAGE("Could not write to file");
		Event::report(EVENT_FILENOTWRITE, "WRITE FILE FAULT", filePath, "Not able to write file or directory: " + filePath);
		ACE_OS::close(fHandle);
		error = AES_NOACCESS;
		return false;
	}
	ACE_OS::close(fHandle);

	error = AES_NOERRORCODE;
	return true;
}

aes_afp_protocolfile::aes_afp_protocolfile()
{
	protocolPair_.first = "CREATIONDATE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "READYDATE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "SENDDATE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "FAILEDDATE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "DELETEDATE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "NONEDATE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "ARCHIVEDATE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "ABSOLUTECREATIONTIME";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "ABSOLUTEDELETIONTIME";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "ISDIRECTORY";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "TRANSFERMODE";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "TRANSFERMASK";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);
	
	protocolPair_.first = "SOURCEDIRECTORY";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);

	protocolPair_.first = "ORIGFILENAME";
	protocolPair_.second = "";
	protocolMap_.insert(protocolPair_);	
	//For TR HN35007 - start
	protocolOptionPair_.first = "CREATIONDATE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "READYDATE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "SENDDATE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "FAILEDDATE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "DELETEDATE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "NONEDATE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "ARCHIVEDATE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "ABSOLUTECREATIONTIME";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "ABSOLUTEDELETIONTIME";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "ISDIRECTORY";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "TRANSFERMODE";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "TRANSFERMASK";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "SOURCEDIRECTORY";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	
	protocolOptionPair_.first = "ORIGFILENAME";
	protocolOptionPair_.second = false;
	protocolOptionMap_.insert(protocolOptionPair_);
	//For TR HN35007 - end


}


aes_afp_protocolfile::~aes_afp_protocolfile()
{
	ProtocolMap::iterator itr;
	itr = protocolMap_.begin();
	while( itr != protocolMap_.end() )
	{
		protocolMap_.erase(itr);
		itr = protocolMap_.begin();
	}
	protocolMap_.clear();
}

bool aes_afp_protocolfile::writeToFile (std::string& tempPath, std::string& path, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("Path to file %s", path.c_str());

	ACE_Recursive_Thread_Mutex  theRecursiveMutex;

	theRecursiveMutex.acquire();

	ProtocolMap::iterator itr;
	std::string toFile(""); 
	ssize_t written = 0;
	ACE_HANDLE fHandle = ACE_INVALID_HANDLE;

	//Create the file.
	fHandle = ACE_OS::open(tempPath.c_str(), O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, S_IRWXU | S_IRWXO | S_IRWXG);

	if(fHandle == ACE_INVALID_HANDLE)
	{
		AES_AFP_TRACE_MESSAGE("Could not open file,last error %d", ACE_OS::last_error());
		fHandle = ACE_OS::open(tempPath.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU | S_IRWXO | S_IRWXG);

		if(fHandle == ACE_INVALID_HANDLE)
		{
			AES_AFP_TRACE_MESSAGE("Could not create file,last error %d", ACE_OS::last_error());
			Event::report(EVENT_FILENOTOPEN, "OPEN FILE FAULT", tempPath, "Not able to open file or directory: " + tempPath);
			error = AES_NOACCESS;
			theRecursiveMutex.release();
			return false;
		} 
	}


	toFile += PROTOCOLVERSION_ + "\n";

	for (itr = protocolMap_.begin();itr != protocolMap_.end();++itr)
	{
		toFile += (*itr).first;
		toFile += "=";
		toFile += (*itr).second;
		toFile += "\n";
	}
	toFile += "\0";
	const size_t fileSize = (toFile.size()+1 > 512)?(toFile.size()+1):512;
	char buf[fileSize];
	memset(buf, 0, toFile.size());
	memcpy(buf,toFile.c_str(), toFile.size());
	written = ACE_OS::write(fHandle, buf , fileSize);

	if (written <= 0 )
	{
		AES_AFP_TRACE_MESSAGE("Could not write to file. errno = %d , fileSize = %zu", errno, toFile.size());
		Event::report(EVENT_FILENOTWRITE, "WRITE FILE FAULT", tempPath, "Not able to write file or directory: " + tempPath);
		ACE_OS::close(fHandle);
		error = AES_NOACCESS;
		theRecursiveMutex.release();
		return false;
	}
	ACE_OS::close(fHandle);

	if (AES_GCC_Util::copyFile(tempPath.c_str(), path.c_str()) != 0)
	{
		error = AES_NOACCESS;
		theRecursiveMutex.release();
		return false;
	}
	theRecursiveMutex.release();

	error = AES_NOERRORCODE;
	return true;
}

bool aes_afp_protocolfile::writeToFile_new (std::string& tempPath, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("Path to file %s", tempPath.c_str());

	ProtocolMap::iterator itr;
	std::string toFile("");
	ssize_t written = 0;
	ACE_HANDLE fHandle = ACE_INVALID_HANDLE;

	ACE_Recursive_Thread_Mutex  theRecursiveMutex;

	theRecursiveMutex.acquire();


	//Create the file.
	fHandle = ACE_OS::open(tempPath.c_str(), O_WRONLY|O_CREAT|O_BINARY);

	if(fHandle == ACE_INVALID_HANDLE)
	{
		AES_AFP_TRACE_MESSAGE("Could not open file,last error %d", ACE_OS::last_error());
		fHandle = ACE_OS::open(tempPath.c_str(), O_WRONLY|O_CREAT);

		if(fHandle == ACE_INVALID_HANDLE)
		{
			AES_AFP_TRACE_MESSAGE("Could not create file,last error %d", ACE_OS::last_error());
			Event::report(EVENT_FILENOTOPEN, "OPEN FILE FAULT", tempPath, "Not able to open file or directory: " + tempPath);
			error = AES_NOACCESS;
			theRecursiveMutex.release();
			return false;
		} 
	}

	toFile += PROTOCOLVERSION_ + "\n";

	for (itr = protocolMap_.begin();itr != protocolMap_.end();++itr)
	{
		toFile += (*itr).first;
		toFile += "=";
		toFile += (*itr).second;
		toFile += "\n";
	}
	const size_t fileSize = (toFile.size() + 1) > 512 ? (toFile.size() + 1) : 512;
	char buf[fileSize];
	memset(buf, 0, fileSize);
	memcpy(buf, toFile.c_str(), toFile.size());
	written = ACE_OS::write(fHandle, buf , fileSize);

	if (written <= 0 )
	{
		AES_AFP_TRACE_MESSAGE("Could not write to file. errno = %d , fileSize = %zu", errno, toFile.size());
		Event::report(EVENT_FILENOTWRITE, "WRITE FILE FAULT", tempPath, "Not able to write file or directory: " + tempPath);
		ACE_OS::close(fHandle);
		error = AES_NOACCESS;
		theRecursiveMutex.release();
		return false;
	}
	ACE_OS::close(fHandle);
	theRecursiveMutex.release();

	error = AES_NOERRORCODE;
	return true;
}
