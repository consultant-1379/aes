/*=================================================================== */
/**
   @file   cdhls.cpp

   This module handles the command cdhls.

   @version 1.0.0
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       01/11/2011   XCHEMAD     Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_cdh_destination.h>
#include <iostream>
#include <string>
#include <map>
#include <ace/ACE.h>
#include <getopt.h>
#include <ace/Get_Opt.h>
#include <aes_gcc_util.h>

using namespace std;

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
const string AES_DATA_PATH = "/data_transfer/";

/*=================================================================== */
/**
   @brief optarg

          Definition for option argument.
 */
/*=================================================================== */
extern ACE_TCHAR* optarg;
/*=================================================================== */
/**
   @brief optind

          Definition for option index
 */
/*=================================================================== */
extern ACE_INT32  optind;
/*=================================================================== */
/**
   @brief opterr

          Definition for option error
 */
/*=================================================================== */
extern ACE_INT32  opterr;

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: cleanUpArgv
=================================================================== */
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

/*===================================================================
   ROUTINE: parse_cdhls
=================================================================== */
int parse_cdhls(int& argc, char *argv[], bool& longFormat, bool& optDformat, bool& optPformat)
{
    	int opt;
    	int r_code = AES_CDH_RC_OK;
    	int lFlag(0);
	int dFlag(0);
	int pFlag(0);


	char *argv1[64];
	int ctr = 0;
	for( ctr = 0;  ctr < argc ; ctr++)
	{
		argv1[ctr] = new char[strlen(argv[ctr]) + 1];
		strcpy(argv1[ctr], argv[ctr]);
	}
	argv1[ctr] = 0;
	strcpy(argv1[0], "cdhls");//argv1[0]="cdhls";

	while ((opt = getopt(argc, argv1, "ldp")) != EOF)

	{
		switch (opt)
        	{
			case 'l':
			{
                		longFormat = true;
                		lFlag++;
			}
			break;
			
			case 'd':
			{
				optDformat = true;
				dFlag++;
			}
			break;

			case 'p':
			{
				optPformat = true;
				pFlag++;
			}
			break;
			
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

    	if (lFlag > 1 || dFlag > 1 || pFlag > 1)
	{
		for( ctr = 0;  ctr < argc ; ctr++)
		{
			delete[] argv1[ctr];
			argv1[ctr] = 0;
		}
		return AES_CDH_RC_INCUSAGE;
	}
	
	if ((longFormat && optDformat) || (longFormat && optPformat) || 
		(optDformat && optPformat) || (longFormat && optDformat && optPformat))
	{
		for( ctr = 0;  ctr < argc ; ctr++)
		{
			delete[] argv1[ctr];
			argv1[ctr] = 0;
		}
		return AES_CDH_RC_INCUSAGE;
	}

    	if (r_code == AES_CDH_RC_OK)
    	{
        	if (argv[optind-1][1] == '-')
        	{
            		if (ACE_OS::strlen(argv[optind-1]) > 2)
            		{
				for( ctr = 0;  ctr < argc ; ctr++)
				{
					delete[] argv1[ctr];
					argv1[ctr] = 0;
				}
                		return AES_CDH_RC_INCUSAGE;
            		}
        	}
        	else if (argc > optind && argv[optind][0] == '-')
        	{
			for( ctr = 0;  ctr < argc ; ctr++)
			{
				delete[] argv1[ctr];
				argv1[ctr] = 0;
			}
            		return AES_CDH_RC_INCUSAGE;
        	}
		else if (optPformat && argc <= optind)
		{
			for( ctr = 0;  ctr < argc ; ctr++)
			{
				delete[] argv1[ctr];
				argv1[ctr] = 0;
			}
			return AES_CDH_RC_INCUSAGE; // No destname is given! 
		}
    	}
	for( ctr = 0;  ctr < argc ; ctr++)
	{
		delete[] argv1[ctr];
		argv1[ctr] = 0;
	}
    
    	if (longFormat || optDformat || optPformat)
    	{
        	argv[1] = NULL;
        	cleanUpArgv(argc, argv, optind);
    	}

    	return r_code;
}

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     listRecord

                 This class holds the attributes of the destination
*/
/*=================================================================== */
class listRecord
{
/*===================================================================
                        PRIVATE DECLARATION SECTION
=================================================================== */
private:
/*===================================================================
                        PRIVATE ATTRIBUTE
=================================================================== */

/*=================================================================== */
   /**
      @brief   transferType

               Transfer type of the destination
   */
/*=================================================================== */
    	std::string transferType;

/*=================================================================== */
   /**
      @brief   destSetName

               Destination Set Name
   */
/*=================================================================== */
	std::string destSetName;

/*=================================================================== */
   /**
      @brief   destPath

               Destination Path
   */
/*=================================================================== */
	std::string destPath;

/*=================================================================== */
   /**
      @brief   parameters

               Parameters
   */
/*=================================================================== */
    	std::string parameters;

/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       Sets the transfer Type

      @param	   transType
			Transfer type

      @return      void

      @exception   none
    */
/*=================================================================== */
    	void setTransferType(const std::string &transType)
        { 
		transferType.assign(transType);
	}

/*=================================================================== */
   /**
      @brief       Sets the destination set name

      @param       dsName
                        Destination set name

      @return      void

      @exception   none
    */
/*=================================================================== */
	void setDestinationSet(const std::string &dsName)
        {
		destSetName.assign(dsName); 
	}

/*=================================================================== */
   /**
      @brief       Sets the destination path

      @param       pathName
                        Destination path

      @return      void

      @exception   none
    */
/*=================================================================== */
	void setDestPath(const std::string &pathName)
        { 
		destPath.assign(pathName); 
	}
    
/*=================================================================== */
   /**
      @brief       Sets the parameters

      @param       params
                        Parameters

      @return      void

      @exception   none
    */
/*=================================================================== */
    	void setParameters(const std::string &params)
        { 
		parameters.assign(params); 
	}

/*=================================================================== */
   /**
      @brief       Gets the transfer type

      @return      string
			Transfer type

      @exception   none
    */
/*=================================================================== */
    	std::string getTransferType()const
        {
	 	return transferType; 
	}

/*=================================================================== */
   /**
      @brief       Gets the destination set name

      @return      string
                        Destination set name

      @exception   none
    */
/*=================================================================== */
	std::string getDestinationSet()const
        { 
		return destSetName; 
	}

/*=================================================================== */
   /**
      @brief       Gets the destination path

      @return      string
                        Destination path

      @exception   none
    */
/*=================================================================== */
	std::string getDestPath()const
        { 
		return destPath; 
	}
    
/*=================================================================== */
   /**
      @brief       Gets the parameters

      @return      string
                        Parameters

      @exception   none
    */
/*=================================================================== */
    	std::string getParameters()const
        { 
		return parameters; 
	}
};

/*===================================================================
   ROUTINE: printHeader
=================================================================== */
void printHeader(const bool longFormat, const bool optDformat)
{
    	// Create header
    	char longHeader[256];               // Header for long format
    	char shortHeader[256];              // Header for short format
	char dsHeader[256];		    // Header for opt -d, destination set
	ACE_OS::memset(longHeader, '\0', 256);      // clear header before use
    	ACE_OS::memset(shortHeader, '\0', 256);     // clear header before use
	ACE_OS::memset(dsHeader, '\0', 256);        // clear header before use
	
    	ACE_OS::sprintf(longHeader, "%-34s%-15s%-12s", "DESTINATION", "TRANSFER TYPE", "PARAMETERS");
    	ACE_OS::sprintf(shortHeader, "%-34s%-15s", "DESTINATION", "TRANSFER TYPE");
	ACE_OS::sprintf(dsHeader, "%-34s%-17s", "DESTINATION", "DESTINATION SET");
	
    	if (longFormat == true)
    	{
        	cout << longHeader << endl;
    	}
	else if (optDformat == true)
	{
		cout << dsHeader << endl;
	}
	else
    	{	
        	cout << shortHeader << endl;
    	}
}

/*===================================================================
   ROUTINE: main
=================================================================== */
int main(int argc, char *argv[]) 
{
	AES_CDH_Result result;
	char*  newArgv[32] = { 0 };
	vector<AES_CDH_Destination::destAttributes>	attr;
	vector<string>::iterator iter;
    
	int exitcode = AES_CDH_RC_OK;
    
	bool longFormat(false); // -l
	bool optDformat(false); // -d
	bool optPformat(false); // -p
	

    	listRecord *record = NULL;
    	std::map<std::string, listRecord *> listMap;

    	if (argc <= 32)
    	{
        	int newArgc = argc;
			int i = 0;

        	for (i = 0; i < argc; i++)
	    	{
            		newArgv[i] = new char[ACE_OS::strlen(argv[i])+1];
            		(void) ACE_OS::strcpy(newArgv[i], argv[i]);
        	}
			if( argc == 2  )
			{
				if( strlen(argv[1]) >  30 )
				{
					exitcode = AES_CDH_RC_UNREAS ;
					cout << AES_CDH_Result(AES_CDH_RC_UNREAS) << endl;
					return exitcode;
				}
			}
			else if( argc == 3 )
			{

				if( ((strcmp(argv[1],"-l") == 0)  || (strcmp(argv[1] , "-p" ) == 0 ) )&&
					   	strlen(argv[2] ) > 30  )
				{
					exitcode = AES_CDH_RC_UNREAS ;
					cout << AES_CDH_Result(AES_CDH_RC_UNREAS) << endl;
					return exitcode;
				}
			}


	    	exitcode = parse_cdhls(newArgc, newArgv, longFormat, optDformat, optPformat);

	    	if (exitcode == AES_CDH_RC_OK)
	    	{
		    	result = AES_CDH_Destination::getAttr(newArgc, newArgv, attr);
				exitcode = result.code();
        	} 

        	// Release newArgv
	    	for (i = 0; i < argc; i++)
        	{
            		delete [] newArgv[i];
        	}
    	}
    	else // if (argc <= 32)
    	{
        	exitcode = AES_CDH_RC_INCUSAGE;
    	}

		if (optPformat && exitcode == AES_CDH_RC_OK)
		{
			if (attr[0].destPath == "")
			{
				result = AES_CDH_RC_NOTRESPDEST;
				exitcode = result.code();
			}
		}

    	if (exitcode == AES_CDH_RC_OK)
    	{
        	cout << "CDH DESTINATION TABLE" << endl << endl;
	
			if (attr.size() > 0 && !optPformat)
			{
				printHeader(longFormat, optDformat);
			}
		
			std::string dest("");
			std::string parameters("");
        	std::string transferType("");
			std::string dsName("");
			std::string path("");

			const std::string pad("                                                     ");
        
        	for (unsigned int counter = 0; counter < attr.size(); ++counter)
        	{
				dest = attr[counter].destName;
				transferType = attr[counter].transferType;
				dsName = attr[counter].destSetName;
				path = attr[counter].destPath;

				if (longFormat == true)     // Long list format requested
				{
					for (iter = attr[counter].otherAttrs.begin(); iter != attr[counter].otherAttrs.end(); ++iter)
					{
						parameters.append((*iter)).append(" ");
					}	
					record = new listRecord;
					record->setTransferType(transferType);
					record->setParameters(parameters);
					listMap.insert(std::make_pair(dest, record));
				}
				else if (optDformat == true)
				{
					record = new listRecord;
					record->setDestinationSet(dsName);
					listMap.insert(std::make_pair(dest, record));
				}
				else if (optPformat == true)
				{
					record = new listRecord;
					record->setDestPath(path);
					listMap.insert(std::make_pair(dest, record));
				}
				else
				{
					record = new listRecord;
					record->setTransferType(transferType);
					record->setParameters("");
					listMap.insert(std::make_pair(dest, record));
				}
				parameters.assign("");
		}
        
		// Create header
		char line[2048];
		ACE_OS::memset(line, '\0', 2048);     // clear header before use
		std::map<std::string, listRecord *>::iterator itr = listMap.begin();
		int pos = 0;
		while (itr != listMap.end())
		{
			if (optDformat)
			{
				ACE_OS::sprintf(line, "%-34s%-32s", (*itr).first.c_str(), (*itr).second->getDestinationSet().c_str());
			}
			else if (optPformat)
			{
				string rpath("");
				string path = (*itr).second->getDestPath();

				if( AES_GCC_Util::datapath_trn(path,AES_DATA_PATH, rpath) == false )
				{
						rpath = "Not Available";
				}

				ACE_OS::sprintf(line, "%s %s",
					        "DESTINATION:", (*itr).first.c_str());
				cout << line << endl;
				ACE_OS::memset(line, '\0', 2048);     // clear header before use
				ACE_OS::sprintf(line, "%s %s",
					        "PATH:       ",rpath.c_str());
			}
			else
			{
				ACE_OS::sprintf(line, "%-34s%-15s", (*itr).first.c_str(), (*itr).second->getTransferType().c_str());
			}

			cout << line;

			if (longFormat == true)
			{   

				std::string split = (*itr).second->getParameters();
				pos = split.find(' ');
				pos = split.find(' ', ++pos);
				std::string oneOption = split.substr(0, pos);
				split = split.substr(pos + 1);
				cout << oneOption.c_str() << endl;
				pos = split.find(' ');
				while (pos != -1)
				{
					pos = split.find(' ');

					if (!strcmp(split.substr(0,2).c_str(),"-r")) // TR HO27423 code change start here
					{											 
						pos = split.rfind("/ -");                
						if (pos != -1)                           
							pos++;					// TR HO27423 code change end here
					}
					else
					{
							pos = split.find(' ', ++pos);
					}
					std::string slask = split.substr(0, pos);
					ACE_OS::sprintf(line, "%-49s%s", " ", slask.c_str());
					cout << line << endl;
					split= split.substr(pos + 1);
					pos = split.find(' ');
				}
			}
			else
			{
				cout << endl;
			}

			++itr;
		}
		cout<<endl;
		std::map<std::string, listRecord *>::iterator listMapItr;
		for( listMapItr = listMap.begin(); listMapItr != listMap.end(); listMapItr++)
		{
			delete (*listMapItr).second;
			listMap.erase(listMapItr);
		}

	}
	else if (exitcode == AES_CDH_RC_INCUSAGE)
	{
		cout << AES_CDH_Result(AES_CDH_RC_INCUSAGE) << endl;
		cout << "Usage:" << endl; 
		cout << "cdhls [-d | -l] [destination]" << endl;
		cout << "cdhls -p destination" << endl<<endl;
	}
	else if ( exitcode == AES_CDH_RC_UNREAS )
	{
		cout << AES_CDH_Result(AES_CDH_RC_UNREAS) << endl;
	}	
	else
	{
		cout << result << endl;
	}

	if (exitcode == AES_CDH_RC_NOSERVER)
	{
       		return 117;
	}

	return exitcode;
}
