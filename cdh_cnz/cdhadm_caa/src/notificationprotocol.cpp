/*=================================================================== */
/**
@file notificationprotocol.cpp

Class method implementation for notificationprotocol.h

DESCRIPTION
This class is responsible for the protocol between CDH and the application.

ERROR HANDLING
General rule:

The error handling is specified for each method.
No methods initiate or send error reports unless specified.

@version 1.1.1

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------

N/A       26/09/2011     xbhadur       Initial Release
=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <notificationprotocol.h>
#include <ace/ACE.h>
#include "aes_cdh_logger.h"
/*===================================================================
                        DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE:NotificationProtocol
=================================================================== */
NotificationProtocol::NotificationProtocol ()
{
  
}

/*===================================================================
   ROUTINE:~NotificationProtocol
=================================================================== */
NotificationProtocol::~NotificationProtocol()
{

}

/*===================================================================
   ROUTINE:add
=================================================================== */
bool NotificationProtocol::add (string parameter, string value)
{
	ProtocolPair prtPair;
	map<string, string, less<string> >::iterator itr;
	pair<map<string, string, less<string> >::iterator, bool> resultPair;

	// Insert the parameter and value in the parameter map.
	prtPair.first = parameter;
	prtPair.second = value;
	resultPair = parameterMap.insert(prtPair);

	// Return the result from the insertion, resultPair.second can either be
	// true or false.
	return resultPair.second;
}

/*===================================================================
   ROUTINE:remove
=================================================================== */
bool NotificationProtocol::remove (string parameter)
{
	map<string, string, less<string> >::iterator itr;

	// Search for the parameter
	itr = parameterMap.find(parameter);
	if (itr != parameterMap.end() )
	{
		parameterMap.erase(itr);
		return true;
	}
	else
	{
		return false;
	}
}

/*===================================================================
   ROUTINE:change
=================================================================== */
bool NotificationProtocol::change (string parameter, string value)
{
	map<string, string, less<string> >::iterator itr;

	// Search for the parameter
	itr = parameterMap.find(parameter);

	if (itr != parameterMap.end() )
	{
		(*itr).second = value;
		return true;
	}
	else
	{
		return false;
	}
}

/*===================================================================
   ROUTINE:get
=================================================================== */
string NotificationProtocol::get (string parameter)
{
	map<string, string, less<string> >::iterator itr;

	// Search for the parameter
	itr = parameterMap.find(parameter);

	if (itr != parameterMap.end() )
	{
		return (*itr).second;
	}
	else
	{
		return "";
	}
}

/*===================================================================
   ROUTINE:encode
=================================================================== */
bool NotificationProtocol::encode (string& encodedString)
{
	map<string, string, less<string> >::iterator itr;

	if (parameterMap.empty() )
	{
		encodedString = "";
		return false;
	}

	for (itr=parameterMap.begin();itr!=parameterMap.end(); ++itr)
	{
		encodedString += (*itr).first + '=' + (*itr).second + '\n';
	}
	return true;
}

/*===================================================================
   ROUTINE:decode
=================================================================== */
bool NotificationProtocol::decode (string codedString)
{
	string tmpValue("");
	string tmpPar("");
	string tmpParValue("");
	ACE_INT32 index(0);
	ACE_INT32 innerIndex(0);

	// Remove previous entries
	parameterMap.clear();

	index = codedString.find_first_of('\n');
	if (index == -1)
	{
		// Error in codedString
		return false;
	}
	while (index != -1)
	{
		tmpParValue = codedString.substr(0, index);
		innerIndex = tmpParValue.find_first_of('=');
		if (innerIndex == -1)
		{
			// Protocol error, missing "="
			return false;
		}
		tmpPar = tmpParValue.substr(0, innerIndex);
		tmpValue = tmpParValue.substr(innerIndex+1);

		if (add(tmpPar, tmpValue) == false)
		{
			return false;
		}

		codedString.erase(0, index+1);
		index = codedString.find_first_of('\n');
	}
	return true;
}
/*===================================================================
   ROUTINE:compare
=================================================================== */
bool NotificationProtocol::compare(NotificationProtocol recvProtocol)
{
	//if (this->get("FILE_FULL_PATH") != recvProtocol.get("FILE_FULL_PATH"))
	if (ACE_OS::strcmp(this->get("FILE_FULL_PATH").c_str(),
			recvProtocol.get("FILE_FULL_PATH").c_str()) != 0)
	{
		AES_CDH_LOG(LOG_LEVEL_TRACE, "NotificationProtocol::compare(), File full path problem: expected %s, received %s ", (this->get("FILE_FULL_PATH")).c_str(), (recvProtocol.get("FILE_FULL_PATH")).c_str());
		return false;
	}

	//if (this->get("STORE_FILE_NAME") != recvProtocol.get("STORE_FILE_NAME"))
	if (ACE_OS::strcmp(this->get("STORE_FILE_NAME").c_str(),
			recvProtocol.get("STORE_FILE_NAME").c_str()) != 0)
	{
		AES_CDH_LOG(LOG_LEVEL_TRACE, "NotificationProtocol::compare(), Store filename problem: expected %s, received %s ", (this->get("STORE_FILE_NAME")).c_str(), (recvProtocol.get("STORE_FILE_NAME")).c_str());
		return false;
	}

	if (this->get("MESSAGE_NUMBER") != recvProtocol.get("MESSAGE_NUMBER"))
	{
		AES_CDH_LOG(LOG_LEVEL_TRACE, "NotificationProtocol::compare(), File full path problem: expected %s, received %s ", (this->get("MESSAGE_NUMBER")).c_str(), (recvProtocol.get("MESSAGE_NUMBER")).c_str());
		return false;
	}

	return true;
}

void toUpper(string& strl)
{
	unsigned int i = 0;

	while(i < strl.length())
	{
		strl[i] = toupper(strl[i]);
		i++;
	}
}


/*===================================================================
   ROUTINE:ackReceived
=================================================================== */
bool NotificationProtocol::ackReceived(string parameter)
{
	string ackValue("YES");
	string tmpAckValue("");
	tmpAckValue = this->get(parameter);
	toUpper(tmpAckValue);
	if (tmpAckValue == ackValue) // ACK is sent
		return true;
	else
		return false;

}

/*===================================================================
   ROUTINE:nakReceived
=================================================================== */
bool NotificationProtocol::nakReceived(string parameter)
{
	string nakValue("YES");
	string tmpNakValue("");
	tmpNakValue = this->get(parameter);
	toUpper(tmpNakValue);
	if (tmpNakValue == nakValue) // NAK is sent
		return true;
	else
		return false;
}

/*===================================================================
   ROUTINE:clearPrt
=================================================================== */
void NotificationProtocol::clearPrt(void)
{
	parameterMap.clear();
}
