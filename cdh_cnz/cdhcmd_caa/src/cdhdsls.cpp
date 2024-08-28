#include <ace/Get_Opt.h>
#include "aes_cdh_destinationset.h"
#include "aes_cdh_resultcode.h"
#include <iostream>
#include <string>
#include <vector>
#include <list>
using namespace std;


//-------------------------------------------------------------------------
//  Function cleanUpArgv, used with long format listing
//  Remove arguments from the argument list
//-------------------------------------------------------------------------
void cleanUpArgv(int& argc, char* argv[], int firstOper)
{
	int i = 0;
	int j = 0;

	if (argv[firstOper-1] == NULL && firstOper < argc)
	{
		argv[firstOper-1] = new char [3];
		(void) strcpy(argv[firstOper-1], "--");
	}

	while (j < argc)
	{
		if (argv[j] != NULL)
		{
			argv[i] = argv[j];
			i++;
		}
		j++;
	}
	argc = i;
}


//-------------------------------------------------------------------------
//  Function parse_cdhdsls
//-------------------------------------------------------------------------
int parse_cdhdsls(int &argc, char *argv[], bool &longFormat)
{
	int opt;
	int r_code = AES_CDH_RC_OK;
	int lFlag = 0;

	ACE_Get_Opt getopt(argc, argv, "l");

	while ((opt = getopt()) != EOF)

	{
		switch (opt) 
		{
		// L
		case 'l':
		{
			longFormat = true;
			lFlag++;
			break;
		}
		case '?':
		{
			return AES_CDH_RC_INCUSAGE;
		}
		default:
		{
			return AES_CDH_RC_INTPROGERR;
		}
		}
	}

	if (lFlag > 1) return AES_CDH_RC_INCUSAGE;

	if (r_code == AES_CDH_RC_OK)
	{
		if (argv[getopt.optind-1][1] == '-')
		{
			if (strlen(argv[getopt.optind-1]) > 2)
			{
				return AES_CDH_RC_INCUSAGE;
			}
		}
		else if (argc > getopt.optind && argv[getopt.optind][0] == '-')
		{
			return AES_CDH_RC_INCUSAGE;
		}
	}

	if (longFormat)
	{
		argv[1] = NULL;
		cleanUpArgv(argc, argv, getopt.optind);
	}

	return r_code;
}


//------------------------------------------------------------------------------
//	Function printHeader
//------------------------------------------------------------------------------
void printHeader(const bool longFormat, const bool rowOne)
{
	// Create header
	char longHeader1[256];              // Header for long format
	char longHeader2[256];              // Header for long format
	char shortHeader[256];              // Header for short format
	memset(longHeader1, '\0', 256);     // clear header before use
	memset(longHeader2, '\0', 256);     // clear header before use
	memset(shortHeader, '\0', 256);     // clear header before use

	sprintf(longHeader1, "%-34s%-7s", "DESTINATION SET", "TYPE");
	sprintf(longHeader2, "%-44s%-6s", "        DESTINATION", "ACTIVE");
	sprintf(shortHeader, "%-34s%-4s", "DESTINATION SET", "TYPE");

	if (longFormat)
	{
		if (rowOne)
			cout << longHeader1 << endl;
		else
			cout << longHeader2 << endl;
	}
	else
	{
		cout << shortHeader << endl;
	}
}


//------------------------------------------------------------------------------
//	Main program cdhdsls
//------------------------------------------------------------------------------

int main(int argc, char *argv[]) 
{
	std::vector<AES_CDH_DestinationSet::destSetAttributes> destSetAttr;
	std::vector<AES_CDH_DestinationSet::destSetAttributes> tmpDestSetAttr;

	int exitcode = AES_CDH_RC_OK;
	AES_CDH_Result result;

	bool longFormat(false);
	bool rowOne(true);

	//AP_InitProcess ("AES_CDH_Cdhdsls", AP_COMMAND);
	//AP_SetCleanupAndCrashRoutine("AES_CDH_Cdhdsls",NULL);

	if (argc <= 32)
	{
	        int    newArgc;
	        char*  newArgv[32];
		newArgc = argc;
		int i=0;
		for (i = 0; i < argc; i++)
		{
			newArgv[i] = new char[strlen(argv[i])+1];
			(void) strcpy(newArgv[i], argv[i]);
		}

		exitcode = parse_cdhdsls(newArgc, newArgv, longFormat);

		if (exitcode == AES_CDH_RC_OK)
		{
			result = AES_CDH_DestinationSet::getAttr(newArgc, newArgv, tmpDestSetAttr);
			exitcode = result.code();
		}

		// Release newArgv
		for (i = 0; i < newArgc; i++)
		{
			delete [] newArgv[i];
		}
	}
	else // if (argc > 32)
	{
		exitcode = AES_CDH_RC_INCUSAGE;
	}

	if (exitcode == AES_CDH_RC_OK)
	{
		cout << "CDH DESTINATION SET TABLE" << endl << endl;

		std::vector<AES_CDH_DestinationSet::destSetAttributes>::iterator attrItr;
		std::vector<AES_CDH_DestinationSet::destSetAttributes>::iterator attrItr2;//for CPP check error

		// list to be used for sorting
		std::list<std::string> destSetNames;
		std::list<std::string>::iterator itr;
		std::string dsName("");

		char line[2048];
		string dsType("");

		if (!longFormat)
		{
			if (tmpDestSetAttr.size() > 0)
				printHeader(longFormat, false);
		}

		// Save the destination set names to a list
		for (attrItr2 = tmpDestSetAttr.begin(); attrItr2 != tmpDestSetAttr.end(); ++attrItr2)
		{
			destSetNames.push_back((*attrItr2).destSetName);
		}

		// Sort the list
		if (destSetNames.size() > 0)
			destSetNames.sort();

		// Put the destination sets to be listed into a vector
		// sorted by destination set name
		attrItr2 = tmpDestSetAttr.begin();
		for (itr = destSetNames.begin(); itr != destSetNames.end(); ++itr)
		{
			while (attrItr2 != tmpDestSetAttr.end())
			{
				if ((*attrItr2).destSetName == (*itr))
				{
					destSetAttr.push_back(*attrItr2);
					attrItr2 = tmpDestSetAttr.begin();
					break;;
				}
				++attrItr2;
			}
		}

		// print
		for (attrItr = destSetAttr.begin(); attrItr != destSetAttr.end(); ++attrItr)
		{
			//if ((*attrItr).destSetType == AES_CDH_DestinationSet::TQConnectAttributes::FILE)
			if ((*attrItr).destSetType == AES_CDH_DestinationSet::FILE)
				dsType.assign("FILE");
			else
				dsType.assign("BLOCK");

			if (longFormat == true)     // Long list format requested
			{
				printHeader(longFormat, rowOne);
				memset(line, '\0', 2048);

				sprintf(line, "%-33s %-6s \n",(*attrItr).destSetName.c_str(),
						dsType.c_str());

				std::cout << line << std::endl;

				rowOne = false;
				printHeader(longFormat, rowOne);
				memset(line, '\0', 2048);

				// active = primary destination
				if ((*attrItr).primDest == (*attrItr).active)
				{
					sprintf(line, "%s %-33s %s \n %s %-33s \n", 
							"        P",(*attrItr).primDest.c_str(), "YES",
							"       S", (*attrItr).secDest.c_str());
							/*"       B", (*attrItr).backDest.c_str());*/

				}

				// active = secondary destination
				else if ((*attrItr).secDest == (*attrItr).active)
				{
					sprintf(line, "%s %-33s \n %s %-33s %s \n", 
							"        P",(*attrItr).primDest.c_str(),
							"       S", (*attrItr).secDest.c_str(), "YES");
							/*"       B", (*attrItr).backDest.c_str());*/
				}

				// active = backup destination
				else
				{
					sprintf(line, "%s %-33s \n %s %-33s \n", 
							"        P",(*attrItr).primDest.c_str(),
							"       S", (*attrItr).secDest.c_str());
							/*"       B", (*attrItr).backDest.c_str(), "YES");*/
				}

				std::cout << line << std::endl;
				rowOne = true;
			}

			else // simple list requested
			{
				memset(line, '\0', 2048);

				sprintf(line, "%-33s %-6s",
						(*attrItr).destSetName.c_str(),
						dsType.c_str());

				std::cout << line << std::endl;

			}
		}
		std::cout<<std::endl;

	}
	else if (exitcode == AES_CDH_RC_INCUSAGE)
	{
		// "Incorrect usage"
		cout << AES_CDH_Result(AES_CDH_RC_INCUSAGE) << endl;
		cout << "Usage: cdhdsls [-l] [destinationset]" <<endl<<endl;
	}
	else
	{
		cout << result << endl<<endl;
	}

	if (exitcode == AES_CDH_RC_NOSERVER)
		return 117;

	return exitcode;
}
