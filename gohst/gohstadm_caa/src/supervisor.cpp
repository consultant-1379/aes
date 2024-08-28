/**********************************************************************
 * File:		Supervisor.cpp                                        *
 * Date:		2000-07-06                                            *
 * Description: Class that handle vaules used in GHOST.               *        
 * Author:		QABMSOL                                               *
 **********************************************************************/
#include <supervisor.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <dirent.h>
#include <ace/ACE.h>

using namespace std;

/**********************************************************************
 * Supervisor() - Constructor                                         *
 **********************************************************************/
Supervisor::Supervisor(ACE_TCHAR * pCPName)
: aDefValue_(0), bDefValue_(-1), dDefValue_(1), fDefValue_(10000), lDefValue_(1024), sDefValue_(100),
  zDefValue_(2000), tDefValue_(0), lastSentFileDefValue_(1),
  maskDefValue_("*.*"), eventTextDefValue_("EventText"), thrStateDefValue_("Stopped"), 
  rpcType_("FTPV2"), defDisc_("/opt/ap/aes/bin"), debugEnabled_(false) 
{
	streamIDDefValue_ = pCPName;
	rescueFileName_ = "/gohst";
	defFolder_ = "gohst";

	if (!(streamIDDefValue_.empty()))
	{
		rescueFileName_ += "_";
		rescueFileName_ += pCPName;
	
		defFolder_ += "_"; 
		defFolder_ += pCPName; 
	}

	rescueFileName_ += ".rec";

	// Set default paths for gohst directory and log file directory
	std::string defGohstPath = this->currentDirectory();	 
	if( !defGohstPath.empty() )
	{
		
                defDisc_ = "/opt/ap/aes/bin/";
		defGohstPath = defDisc_ + defFolder_;   // Add default folder name
		pathDefValue_ = logPathDefValue_ = defGohstPath;   // Set default path and log path
	}
	else
	{
		pathDefValue_    = defDisc_ + defFolder_;
		logPathDefValue_ = defDisc_ + defFolder_;
	}

	// Set rescue directory
	rescueDirectory_ = defDisc_ + "/gohst_rescue"; 

	// Set default values
	this->defaultIntOptions();	
	this->defaultStrOptions();

	if (RESCUE)
		this->createRescueDirectory();
}

/**********************************************************************
 * ~Supervisor() - Destructor                                         *
 **********************************************************************/
Supervisor::~Supervisor()
{
}

/**********************************************************************
 * defaultValues() - Set all default values.                          *
 **********************************************************************/
void Supervisor::defaultValues()
{
	this->defaultIntOptions();
	this->defaultStrOptions();

	if (RESCUE)
		this->writeToRescueFile();
}

/**********************************************************************
 * defaultIntOptions() - Sets default values on  options -a, -f, -l,  *
 *                       -s & -t.                                     *
 **********************************************************************/
void Supervisor::defaultIntOptions()
{
	this->setNumberOfDest(1);

	for(ACE_INT32 i=0; i<NR_OF_THRDS; i++)
	{
		intOptions_[i][0] = i +1;
	
		for(ACE_INT32 j=0; j<NR_OF_INT_OPT; j++)
		{
			switch(j)
			{
				case 1:
					intOptions_[i][1] = aDefValue_;
					break;
				case 2:
					intOptions_[i][2] = fDefValue_;
					break;
				case 3:
					intOptions_[i][3] = lDefValue_;
					break;
				case 4:
					intOptions_[i][4] = sDefValue_;
					break;
				case 5:
					intOptions_[i][5] = zDefValue_;
					break;
				case 6:
					intOptions_[i][6] = tDefValue_;
					break;
				case 7:
					intOptions_[i][7] = 0;
					break;
				case 8:
					intOptions_[i][8] = bDefValue_;
					break;
				default:
					break;
			}
		}
	}
}

/**********************************************************************
 * defaultStrOptions()  - Sets default values on options -p, -pl, -m, *
 *                        State, -e & RPC type.                       *
 **********************************************************************/
void Supervisor::defaultStrOptions()
{
	ACE_TCHAR index[5];
	ACE_TCHAR path[30];
	
	for(ACE_INT32 i=0; i<NR_OF_THRDS; i++)
	{
		ACE_OS::sprintf( index, "%d", i + 1);
		strOptions_[i][0] = std::string(index);

		for(ACE_INT32 j=0; j<NR_OF_STR_OPT; j++)
		{
			switch(j)
			{
				case 1:
					ACE_OS::sprintf(path,"%s/gohstdest%i",pathDefValue_.c_str(),(i+1));
					strOptions_[i][1] = string(path);
					break;
				case 2:
					strOptions_[i][2] = logPathDefValue_;
					break;
				case 3:
					strOptions_[i][3] = maskDefValue_;
					break;
				case 4:
					strOptions_[i][4] = thrStateDefValue_;
					break;
				case 5:
					strOptions_[i][5] = eventTextDefValue_;
					break;
				case 6:
					strOptions_[i][6] = rpcType_;
					break;
				case 7:
					strOptions_[i][7] = streamIDDefValue_;
					break;
				default:
					break;
			}
		}
	}
}

/**********************************************************************
 * printex() - Prints extended values.                                *
 **********************************************************************/
void Supervisor::printex()
{
	if( this->getDebugMode() )
	{
		::cout << "Supervisor::printex(): NR_OF_THRDS= "   << NR_OF_THRDS   << endl;
		::cout << "Supervisor::printex(): NR_OF_INT_OPT= " << NR_OF_INT_OPT << endl; 
		::cout << "Supervisor::printex(): NR_OF_STR_OPT= " << NR_OF_STR_OPT << endl; 
		::cout << "Supervisor::printex(): Nr of defined destinations: " << this->getNumberOfDest() << endl;
	}
	
	::cout << "\n_____ EXTENDED LIST _______________________________________________________\n" << endl;
		
	for(ACE_INT32 i=0; i<dDefValue_; i++)
	{
		::cout << "Dest\t" << "-a\t" << "-f(ms)\t" << "-l\t" << "-s\t" << "-t\t" << "LSF\t" << "-m\t"<< "state\n" ;
		
		for(ACE_INT32 j=0; j<NR_OF_INT_OPT; j++)
		{
			if(j == 5 )
			{
				if(intOptions_[i][j] == 0)
					::cout << "bin\t";
				else if(intOptions_[i][j] == 1)
					::cout << "ascii\t";
				else
					::cout << "###\t";
			}
			else
				::cout << intOptions_[i][j] << "\t";
		}

		::cout << strOptions_[i][3].c_str() << "\t" << strOptions_[i][4].c_str() << endl << endl;
		::cout << "Path:\t\t" << strOptions_[i][1].c_str() << endl;
		::cout << "LogPath:\t" << strOptions_[i][2].c_str() << endl;
		::cout << "EventText:\t" << strOptions_[i][5].c_str() << endl;
		::cout << "RPC protocol:\t" << strOptions_[i][6].c_str() << endl;
		::cout << "streamID:\t" << strOptions_[i][7].c_str() << endl;
		::cout << "___________________________________________________________________________\n" << endl;
	}
}

/**********************************************************************
 * print() - Prints most used values.                                 *
 **********************************************************************/
void Supervisor::print()
{
	if( this->getDebugMode() )
	{
		::cout << "Supervisor::print(): NR_OF_THRDS= "   << NR_OF_THRDS   << endl;
		::cout << "Supervisor::print(): NR_OF_INT_OPT= " << NR_OF_INT_OPT << endl; 
		::cout << "Supervisor::print(): NR_OF_STR_OPT= " << NR_OF_STR_OPT << endl; 
		::cout << "Supervisor::print(): Nr of defined destinations: " << this->getNumberOfDest() << endl;
	}

	::cout << "\n_____ LIST ________________________________________________________________\n" << endl;
	::cout << "Dest\t" << "-a\t" << "-f(ms)\t" << "-l\t" << "-s\t" << "-t\t" << "LSF\t" << "-m\t"<< "state\n\n";
	
	for(ACE_INT32 i=0; i<dDefValue_; i++)
	{
		for(ACE_INT32 j=0; j<NR_OF_INT_OPT; j++)
		{
			if(j == 5 )
			{
				if(intOptions_[i][j] == 0)
					::cout << "bin\t";
				else if(intOptions_[i][j] == 1)
					::cout << "ascii\t";
				else
					::cout << "###\t";
			}
			else
				::cout << intOptions_[i][j] << "\t";
		}
		::cout << strOptions_[i][3].c_str() << "\t" << strOptions_[i][4].c_str() << endl;
	}
	
	::cout << "___________________________________________________________________________\n" << endl;
}

/**********************************************************************
 * setIntOptions() - Sets options -a, -f, -l, -s & -t.                 *
 **********************************************************************/
void Supervisor::setIntOptions( ACE_INT32 thrId, ACE_INT32 opt, ACE_INT32 value )
{ 
	// Check if value is resonable
	value = this->checkIntValue(thrId, opt, value);

	intOptions_[thrId][opt] = value;

	if( this->getDebugMode() )
	{
		::cout << "Supervisor::setIntOptions(): intOptions_[" << thrId << "][" << opt <<"] = " << value << endl;
	}

	if (RESCUE)
		this->writeToRescueFile();
}

/**********************************************************************
 * setStrOptions() - Gets options -p, -pl, -m, State, -e & RPC type.  *
 **********************************************************************/
void Supervisor::setStrOptions( ACE_INT32 thrId, ACE_INT32 opt, std::string value )
{
	strOptions_[thrId][opt] = std::string(value);

	if(this->getDebugMode())
	{
		::cout << "Supervisor::setStrOptions(): strOptions_[" << thrId << "][" << opt << "] = " << value.c_str() << endl;
	}

	if (RESCUE)
		this->writeToRescueFile();
}

/**********************************************************************
 * getIntOptions() - Gets options -a, -f, -l, -s & -t.                *
 **********************************************************************/
ACE_INT32 Supervisor::getIntOptions( ACE_INT32 thrId, ACE_INT32 opt )
{
	if( this->getDebugMode() )
	{
		::cout << "Supervisor::getIntOptions(): intOptions_[" << thrId << "][" << opt << "] return " << intOptions_[thrId][opt] << endl;
	}

	return intOptions_[thrId][opt];
}

/**********************************************************************
 * getStrOptions() - Gets options -p, -pl, -m, State, -e & RPC type.  *
 **********************************************************************/
std::string Supervisor::getStrOptions( ACE_INT32 thrId, ACE_INT32 opt )
{
	if( this->getDebugMode() )
	{
		::cout << "Supervisor::getStrOptions(): strOptions_[" << thrId << "][" << opt << "] return " << strOptions_[thrId][opt].c_str() << endl;
	}
	return strOptions_[thrId][opt];
}

/**********************************************************************
 * createRescueDirectoy() - Creates a directory.                      *
 **********************************************************************/
bool Supervisor::createRescueDirectory()
{
	if(ACE_OS::mkdir(rescueDirectory_.c_str(),ACE_DEFAULT_DIR_PERMS) == -1)//returns -1 or int
	{
		ACE_UINT64 lastError = ACE_OS::last_error();

		if(lastError == 183)
		{
			if( this->getDebugMode() )
				::cerr << "\aSupervisor::createRescueDirectory(): WARNING: Directory " << rescueDirectory_.c_str() << " already exists." << endl;
			return true;
		}
		else
		{
			if( this->getDebugMode() )
				::cerr << "Supervisor::createRescueDirectory(): Last error retrieved: " << lastError << endl;
			return false;
		}
	}
	else
	{
		if( this->getDebugMode() )
			::cout << "Supervisor::createRescueDirectory(): Directory " << rescueDirectory_.c_str() << " was created succesfully." << endl;
		
		return true;
	}
}

/**********************************************************************
 * writeToRescueFile() - Writes the current status (values) into the  *
 *                       gohst.rec file. If gohst is terminated       *
 *                       correctly this file is automatically removed,*
 *                       else if not the file is stored in            *
 *                       gohst_rescue directory.                      *
 **********************************************************************/
bool Supervisor::writeToRescueFile()
{
	string file = rescueDirectory_+rescueFileName_;
	ofstream out(file.c_str());

	if(!out)
	{
		return false;
	}
	else
	{
		out << "NrOfDestinations: " << this->getNumberOfDest() << endl;

		for(ACE_INT32 i=0; i<dDefValue_; i++)
		{
			out << "dest: "			<< this->getIntOptions(i, 0) << endl;
			out << "a: "			<< this->getIntOptions(i, 1) << endl;
			out << "f: "			<< this->getIntOptions(i, 2) << endl;
			out << "l: "			<< this->getIntOptions(i, 3) << endl;
			out << "s: "			<< this->getIntOptions(i, 4) << endl;
			out << "Z: "			<< this->getIntOptions(i, 5) << endl;
			out << "t: "			<< this->getIntOptions(i, 6) << endl;
			out << "b: "			<< this->getIntOptions(i, 8) << endl;
			out << "path: "			<< this->getStrOptions(i, 1) << endl;
			out << "logpath: "		<< this->getStrOptions(i, 2) << endl;
			out << "m: "			<< this->getStrOptions(i, 3) << endl;
			out << "state: "		<< this->getStrOptions(i, 4) << endl;
			out << "i: "			<< this->getStrOptions(i, 6) << endl;
			out << "streamID: "		<< this->getStrOptions(i, 7) << endl;  
			out << "<END" << i+1 << ">" << endl;
		}
		out.close();
	}

	return true;	
}

/**********************************************************************
 * recoverFromRescueFile() - In case of faulty closing of ghost, the  *
 *                           values that gohst was given before exit  *
 *                           are read from rescue file (gohst.rec).   *
 **********************************************************************/
bool Supervisor::recoverFromRescueFile()
{
	ACE_TCHAR id[20];
	ACE_INT32 intValue;
	ACE_TCHAR strValue[50];
	ACE_OS::memset (strValue, 0, 50);// analyse if any error ??

	string file = rescueDirectory_ + rescueFileName_;
	ifstream in(file.c_str());

	if(!in)
	{
		return false;
	}
	else
	{
		in >> id >> intValue;   // Nr of destinations
		this->setNumberOfDest(intValue);

		for(ACE_INT32 i=0; i<dDefValue_; i++)
		{
			// Read int values
			in >> id >> intValue;  //dest
			this->setIntOptions(i, 0, intValue);
	
			in >> id >> intValue;  //a
			this->setIntOptions(i, 1, intValue);
	
			in >> id >> intValue;  //f
			this->setIntOptions(i, 2, intValue);
	
			in >> id >> intValue;  //l
			this->setIntOptions(i, 3, intValue);
	
			in >> id >> intValue;  //s
			this->setIntOptions(i, 4, intValue);

			in >> id >> intValue;  //z
			this->setIntOptions(i, 5, intValue);

			in >> id >> intValue;  //t
			this->setIntOptions(i, 6, intValue);

			in >> id >> intValue;  //b
			this->setIntOptions(i, 8, intValue);

			// Read string values
			in >> id >> strValue;  //path
			this->setStrOptions(i, 1, strValue);

			in >> id >> strValue;  //logpath
			this->setStrOptions(i, 2, strValue);

			in >> id >> strValue;  //mask (m)
			this->setStrOptions(i, 3, strValue);

			in >> id >> strValue;  //state
			this->setStrOptions(i, 4, strValue);

			in >> id >> strValue;  //rpcType (i)
			this->setStrOptions(i, 6, strValue);

			in >> id >> strValue;  //streamID
			this->setStrOptions(i, 7, strValue);

			// Read end mark
			in >> strValue; 
		}
		
		in.close();

		if (RESCUE)
			this->writeToRescueFile();
	}

	return true;
}

/**********************************************************************
 * deleteRescueFile() - Gohst_rescue cleanup. Removes file gohst.rec  *
 *                      and directory gohst_rescue is they exist.     *                                   
 **********************************************************************/
ACE_INT32 Supervisor::deleteRescueFile()
{	
	bool dirRemoved  = false, fileRemoved = false;
	string dir  = rescueDirectory_;
	string file = rescueDirectory_ + rescueFileName_;

	const ACE_TCHAR* lpDirName = dir.c_str();
	const ACE_TCHAR* lpFileName = file.c_str();

	if(ACE_OS::unlink(lpFileName)) 
	{
		fileRemoved = true;
	}
	
	// Check if directory gohst_rescue exist and if it is empty. If true, remove directory!
	if(fileRemoved)   
	{
		if(ACE_OS::rmdir(lpDirName))
		{
			dirRemoved = true;
		}
	}
	
	if(fileRemoved && dirRemoved)
		return true;
	else
		return false;
}

/**********************************************************************
 * checkIntValue() - Check limits for options -d, -a, -f, -l & -s.    *
 **********************************************************************/
ACE_INT32 Supervisor::checkIntValue(ACE_INT32 thrId, ACE_INT32 opt, ACE_INT32 value)
{
		
	switch(opt)
	{
		case 0:  // Destinations
			{
				if(value < 1 || value > 100)
				{
					::cerr << "\a\nWARNING: Unresonable value for number of gohst destinations,\n" << 
						"         using default value " << this->dDefValue_ << " gohst destination.\n\n" <<
						"         [ Allowed values: 0 > d <= 100 ]\n" << endl;
					value = this->dDefValue_;
				}
				break;
			}
		case 1:   // Amount of files in each directory
			{
				if(value < 0 || value > 10000)
				{
					::cerr << "\a\nWARNING: Unresonable value for -a (amount of files in each\n" <<
						"         directory), using default value " << this->aDefValue_ << " file/dir.\n\n" <<
						"         [ Allowed values: 0 >= a <= 10000 ]\n" << endl;
					value = this->aDefValue_;
				}
				break;
			}
		case 2:   // Frequency between creation of files/blocks
			{
				if(value < 0 || value > 864000)
				{
					::cerr << "\a\nWARNING: Unresonable value for -f (file/block generation frequency),\n" << 
						"         using default value " << this->fDefValue_ << " ms.\n\n" <<
						"         [ Allowed values: 0 >= f <= 864000 (0 sec to 24h) ]\n" << endl;
					value = this->fDefValue_;
				}
				break;
			}
		case 3:   // Record length 
			{
				if(value != 16 && value != 1024 && value != 2048 && value != 4096 && value != 8192 && value != 16384 && value != 32768 && value != 49152 && value != 65536)
				{
					::cerr << "\a\nWARNING: Unresonable value for -l (record length of each record),\n" << 
						"         using default value " << this->lDefValue_ << " bytes.\n\n" <<
						"         [ Allowed values: 1024, 2048, 4096, 8192 or 16384 ]\n" << endl;
					value = this->lDefValue_;
				}
				break;
			}
		case 4:   // Number of records in each file/block
			{
				if(value < 0 || value > 50000 )
				{
					::cerr << "\a\nWARNING: Unresonable value for -s (number of records in each \n" << 
						"         file/block), using default value " << this->sDefValue_ << " records/(file/block).\n\n" <<
						"         [ Allowed values: 0 >= s <= 50000  ]\n" << endl;
					value = this->sDefValue_;
				}
				break;
			}
		case 5:   // Number of bytes in a block transaction
			{
				if(value < 1 || value > 1000000000 )
				{
					::cerr << "\a\nWARNING: Unresonable value for -z (transaction length \n" << 
						"         file/block), using default value " << this->zDefValue_ << " records/(file/block).\n\n" <<
						"         [ Allowed values: 1 >= z <= 1000000000  ]\n" << endl;
					value = this->zDefValue_;
				}
				break;
			}
	}
	return value;
}

/**********************************************************************
 * currentDirectory() - Returns current directroy from where the      *
 *                      binary is executed. If it fails an empty      *                                   
 *                      string is returned.                           *
 **********************************************************************/
std::string Supervisor::currentDirectory()
{
	#define CURR_DIR_BUF_LEN 100
	size_t curDir = CURR_DIR_BUF_LEN;   // size of directory buffer
	char szCurDir[curDir];   // directory buffer
	char* buf = 0;
	      
	buf = ACE_OS::getcwd(szCurDir,curDir);//this ACE fnt returns 0 or buf
	if (buf != 0)
	   return string(szCurDir);
	else
	   return "";
}

/**********************************************************************
 * clearDirectory() - Gohst directory cleanup. Removes files and      *
 *                    directories from the directory passed as an     *
 *                    argument (including it self).                   *                                   
 *                                                                    *
 *                    Returns true if succeded, else false.           *
 **********************************************************************/
bool Supervisor::clearDirectory(std::string dir)
{	
	static ACE_INT32 dirCounter = 0;
        DIR             *dip;
        struct dirent   *dit;

	string currentPath = dir + "/", tempPath = "";
		
        dip = opendir(currentPath.c_str());

	if( dip == NULL )
	{
		if ( this->getDebugMode() )
			this->errorText();
                return false;
	}

	dirCounter++;

	while((dit = readdir(dip)) != NULL )
	{
                if (!strcmp(dit->d_name,".") ||!strcmp(dit->d_name,".."))
                     continue;

                string filePath = currentPath + "/" + string(dit->d_name);
                struct stat buf;
                stat(filePath.c_str(), &buf);
                if(S_ISDIR(buf.st_mode))
		{
			tempPath = currentPath + string(dit->d_name);
			
			clearDirectory( tempPath );

			ACE_OS::rmdir( tempPath.c_str() ) ;
		}
		// Found item is a file
		else //if( fileDirExist != 0 )
		{
			tempPath = currentPath +  string(dit->d_name);;
			
			ACE_OS::unlink( tempPath.c_str() )  ;
		}
	}  // while fileDirExist

	dirCounter--;
	closedir(dip);

	if( dirCounter == 0 )
	{
		ACE_OS::rmdir( dir.c_str() ) ;
	}

	return true;
} 

/**********************************************************************
 * errorText() - Displays return value from GetLastError() in plain   *
 *               text.                                                *
 **********************************************************************/
void Supervisor::errorText()
{
      perror(NULL);
}
