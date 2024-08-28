//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3BE1320C0284.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3BE1320C0284.cm

//## begin module%3BE1320C0284.cp preserve=no
//	INCLUDE aes_afp_template.h
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
//	   19089-CAA 109 0503
//
//	AUTHOR
//	   2003-01-22  UAB/KB/AU DAPA
//
//	REVISION
//	   A 2003-01-22 DAPA
//	   B 2003-05-09 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3BE1320C0284.cp


#include <aes_afp_template.h>
#include <ACS_CS_API_Common_R1.h>
#include <ACS_CS_API_NetworkElement_R1.h>
#include <aes_afp_defines.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"


// uabmha: CNI 1135, why was the _ removed from this one ?? :
extern int errno_;

extern bool scpMcpFlag; 

AES_AFP_TRACE_DEFINE(AES_AFP_template);

// Class aes_afp_template 

//## Operation: aes_afp_template%3BE13489019A; C++
//## Semantics:
//	---------------------------------------------------------
//	       Constructor()
//	---------------------------------------------------------
// uabmha: CNI 1135, added startupSequenceNumber for afpdef -k option
//
aes_afp_template::aes_afp_template (std::string pathToPersistentFile, std::string nameTag, long startupSequenceNumber)
//## begin aes_afp_template::aes_afp_template%3BE13489019A.hasinit preserve=no
: transientNr_((startupSequenceNumber == -1) ? startupSequenceNumber = 0 : startupSequenceNumber),
  persistentNr_(startupSequenceNumber),
  usePersNr_(false),
  persistentFilePath_(pathToPersistentFile),
  newFilename_(""),
  nameTag_(nameTag),
  nameTagPart1_(""),
  nameTagPart2_(""),
  rtrFileFlag(false), //becomes true if the user chooses the argument 'b' in rename template -- HO99988
  multiNameTag_(false),
  startupSequenceNumber_(startupSequenceNumber)
{
	persistentFilePath_ += "/persist.nr";

	// uabmha: CNI 1135, added split up of name tag into
	// two separate name tags, if orig. tag contains an asterisk.
	// Ex: nameTag_ = "aaa*.cdr" --> nameTagPart1_ = "aaa",  nameTagPart2_ = ".cdr"
	// Pos. :          01234567
	int ipos = 0;
	int totlen = 0;

	totlen = nameTag_.length();

	// 
	if ((ipos = nameTag_.find_first_of("*")) != (int)(std::string::npos))
	{
		// Asterisk found in string, split string into two name tags
		multiNameTag_ = true;

		// First calculate length of part2 (aaa*.cdr : 8 - 3 - 1 = 4 characters
		int part2len = nameTag_.length() - ipos - 1;

		// then copy from pos. 0 to ipos into nameTagPart1_ ...
		nameTagPart1_ = nameTag_.substr(0, ipos);
		// terminate with NULL;
		nameTagPart1_[ipos] = NULL;

		// then copy from pos. after ipos to end into nameTagPart2_
		nameTagPart2_ = nameTag_.substr(ipos+1, part2len);
	}
	else
	{
		// Find method returned npos (= -1),
		// so no asterisk found in string, then do nothing
	}

	AES_AFP_TRACE_MESSAGE("Constructor nameTag_ = %s",nameTag_.c_str());	
        //START - HV85900
        ifstream fin(persistentFilePath_.c_str() ); 
        if(fin.good())
        {
                unsigned long val = 0;
                if(fin >> val)
                {
                        persistentNr_ = val;
                        fin.close();
                }
                else
                {
                        fin.close();
                        persistentNr_ = startupSequenceNumber;
                }

        }
        else
        {
                persistentNr_ = startupSequenceNumber;
                AES_AFP_LOG(LOG_LEVEL_WARN, "Error in reading persistentNr_ value %lu from file",persistentNr_);
        }
        //END - HV85900

}


aes_afp_template::~aes_afp_template()
{
	AES_AFP_TRACE_MESSAGE("Entering");
	templateVector_.clear();
	commitPersNr(); //HV40150
}



//## Other Operations (implementation)
//## Operation: setTemplate%3BE13529026C; C++
//## Semantics:
//	---------------------------------------------------------
//	       setTemplate()
//	---------------------------------------------------------
int aes_afp_template::setTemplate (std::string templateString)
{
	//## begin aes_afp_template::setTemplate%3BE13529026C.body preserve=yes

	unsigned int index(0);
	bool year(false);
	bool month(false);
	bool day(false);
	bool hour(false);
	bool min(false);
	bool sec(false);
	bool pers(false);
	bool ners(false);
	bool fsiz(false);
	bool orig(false);
	//bool tag(false);
	bool tagfirst(false);  // uabmha: CNI 1135
	bool tagsecond(false); // uabmha: CNI 1135
	//TR HO99988	
	bool bladeid(false);  
	bool isMultipleCPSystem = false;	

	for (index=0;index < templateString.length();index++)
	{
		switch (templateString[index] )
		{
		case 'y':
		{
			if (year == true)
			{
				return false;
			}
			year = true;
			index++;

			// Check for 2 digit year
			if (templateString[index] == 'y')
			{
				index++;
			}
			else
			{
				return false;
			}
			if (templateString[index] == 'y')
			{
				index++;
			}
			else
			{
				// Two digit year
				templateVector_.push_back("TWOYEAR");
				index--;
				break;
			}

			// Check for 4 digit year
			if (templateString[index] == 'y')
			{
				templateVector_.push_back("FOURYEAR");
			}
			else
			{
				return false;
			}
			break;
		}
		case 'm':
		{
			if (month == true)
			{
				return false;
			}
			month = true;
			index++;
			if (templateString[index] == 'm')
			{
				templateVector_.push_back("MONTH");
			}
			else
			{
				return false;
			}
			break;
		}
		case 'd':
		{
			if (day == true)
			{
				return false;
			}
			day = true;
			index++;
			if (templateString[index] == 'd')
			{
				templateVector_.push_back("DAY");
			}
			else
			{
				return false;
			}
			break;
		}
		case 'H':
		{
			if (hour == true)
			{
				return false;
			}
			hour = true;
			index++;
			if (templateString[index] == 'H')
			{
				templateVector_.push_back("HOUR");
			}
			else
			{
				return false;
			}
			break;
		}
		case 'M':
		{
			if (min == true)
			{
				return false;
			}
			min = true;
			index++;
			if (templateString[index] == 'M')
			{
				templateVector_.push_back("MINUTE");
			}
			else
			{
				return false;
			}
			break;
		}
		case 'S':
		{
			if (sec == true)
			{
				return false;
			}
			sec = true;
			index++;
			if (templateString[index] == 'S')
			{
				templateVector_.push_back("SECOND");
			}
			else
			{
				return false;
			}
			break;
		}
		case 'p':
		{
			if (pers == true || ners == true)
			{
				return false;
			}
			pers = true;
			usePersNr_ = true;
			// Check for one p
			index++;
			if (templateString[index] == 'p')
			{
				index++;
			}
			else
			{
				index--;
				templateVector_.push_back("ONEPERS");
				break;
			}
			if (templateString[index] == 'p')
			{
				index++;
			}
			else
			{
				return false;
			}
			// Check for four p
			if ((templateString[index] == 'p') && (templateString[index + 1] != 'p'))
			{
				templateVector_.push_back("FOURPERS");
				break;
			}
			else if ((templateString[index] == 'p') && (templateString[index + 1] == 'p'))
			{
				index += 2;
			}
			else
			{
				return false;
			}
			// For HL79347 : Check for six p
			if ((templateString[index] == 'p') && (templateString[index + 1] != 'p'))
			{
				templateVector_.push_back("SIXPERS");
				break;
			}
			else if ((templateString[index] == 'p') && (templateString[index + 1] == 'p'))
			{
				index += 2;
			}
			else
			{
				return false;
			}
			// For HL79347 : Check for eight p
			if ((templateString[index] == 'p') && (templateString[index + 1] != 'p'))
			{
				templateVector_.push_back("EIGHTPERS");
				break;
			}
			else
			{
				return false;
			}

			break;
		}
		case 'n':
		{
			if (ners == true || pers == true)
			{
				return false;
			}
			ners = true;
			// Check for one n
			index++;
			if (templateString[index] == 'n')
			{
				index++;
			}
			else
			{
				index--;
				templateVector_.push_back("ONENERS");
				break;
			}

			if (templateString[index] == 'n')
			{
				index++;
			}
			else
			{
				return false;
			}
			if (templateString[index] != 'n')
			{
				return false;
			}
			// Check for four n
			if ((templateString[index] == 'n') && (templateString[index + 1] != 'n'))
			{
				templateVector_.push_back("FOURNERS");
				break;
			}
			else if ((templateString[index] == 'n') && (templateString[index + 1] == 'n'))
			{
				index += 2;
			}
			else
			{
				return false;
			}
			// For HL79347 : Check for six n
			if ((templateString[index] == 'n') && (templateString[index + 1] != 'n'))
			{
				templateVector_.push_back("SIXNERS");
				break;
			}
			else if ((templateString[index] == 'n') && (templateString[index + 1] == 'n'))
			{
				index += 2;
			}
			else
			{
				return false;
			}
			// For HL79347 : Check for eight n
			if ((templateString[index] == 'n') && (templateString[index + 1] != 'n'))
			{
				templateVector_.push_back("EIGHTNERS");
				break;
			}
			else
			{
				return false;
			}
			break;
		}
		case 'z':
		{
			if (fsiz == true)
			{
				return false;
			}
			fsiz = true;
			templateVector_.push_back("FILESIZE");
			break;
		}
		case 'f':
		{
			if (orig == true)
			{
				return false;
			}
			orig = true;
			templateVector_.push_back("ORIGINAL");
			break;
		}
		case 'F':
		{
			// uabmha: CNI 1135, Added handling of 2 name tags
			if (tagsecond == true)
			{
				// No more than two F elements allowed
				// regardless of name tag content.
				return false;
			}

			else if (tagfirst == true)
			{
				if (multiNameTag_ == false)
				{
					// No asterisk present, so no second F allowed
					return false;
				}
				tagsecond = true;
				templateVector_.push_back("NAMETAG");
			}
			else
			{
				tagfirst = true;
				templateVector_.push_back("NAMETAG");
			}

			break;
		}// end of case 'F':

		/* START:: CNI 55/1-970:: introducing new option 'b' for blade id. if it is bc1 considers as '01' and if
						it is cp01 considers as '91' */
		case 'b':
		{
			if (scpMcpFlag == false)
			{
				ACS_CS_API_NS::CS_API_Result multiCP = ACS_CS_API_NetworkElement_R1::isMultipleCPSystem(isMultipleCPSystem);
				if ((multiCP == ACS_CS_API_NS::Result_Success) && (isMultipleCPSystem))
				{
					if (bladeid == true)
					{
						return false;
					}
					templateVector_.push_back("BID");
					bladeid = true;
					rtrFileFlag = true;
				}
				else
				{
					return 8;
				}
			}
			else
			{
				if (bladeid == true)
				{
					return false;
				}
				templateVector_.push_back("BID");
				bladeid = true;
				rtrFileFlag = true;
			}

			break;
		}
		/* END of new option 'b' */



		default:
		{
			return false;
		}

		}// end of switch

	}// end of for

	if ((nameTag_.length() > 0) && (tagfirst != true))
	{
		// Name tag is set, at least one F element should have existed
		return false;
	}
	if ((tagsecond != true) && (multiNameTag_ == true))
	{
		// Less than two F elements found, not enough when asterisk
		// is present in name tag...
		return false;
	}
	if ((tagfirst == true) && (nameTag_.empty() == true))
	{
		// At least one F element is present, but no name tag
		return false;
	}

	return true;

	//## end aes_afp_template::setTemplate%3BE13529026C.body
}

//## Operation: renameFileLocal%3BE1359402C0; C++
//## Semantics:
//	---------------------------------------------------------
//	       renameFile()
//	---------------------------------------------------------
bool aes_afp_template::renameFileLocal (std::string oldFileName, std::string& newFileName)
{
	//## begin aes_afp_template::renameFileLocal%3BE1359402C0.body preserve=yes
	int index(0);

	std::string tmpSearchPath("");
	std::string tmpOldName("");
	std::string tmpFileName("");
	std::string tmpTimeStamp("");
	int result(0);

	index = oldFileName.find_last_of('/');
	if (index != -1)
	{
		tmpSearchPath = oldFileName.substr(0, index);
		tmpOldName = oldFileName.substr(index +1);
	}

	// Construct new filename

	std::string tmpNewFileName(tmpSearchPath);
	tmpNewFileName += "/" + newFilename_;
	//check if the old file name and new file name are same(can happen if template have option only "f")
	if(0 == tmpOldName.compare(newFilename_))
	{
		AES_AFP_TRACE_MESSAGE("Old and new are same. RETURN TRUE");
		return true;
	}
	// Rename file
	//result = ACE_OS::rename(oldFileName.c_str(), tmpNewFileName.c_str() );

	//Check the existance of new file
	ACE_stat f_stat;
	result = ACE_OS::stat(tmpNewFileName.c_str(), &f_stat);

	if (result == 0)//file already present in src directory
	{
		int counter(0);
		// File already exist, add time stamp
		counter++;
		tmpTimeStamp = aes_afp_timestamp::Instance().getTimeString();

		tmpFileName = newFilename_;
		tmpFileName += '_' + tmpTimeStamp;
		tmpNewFileName += '_' + tmpTimeStamp;
		//result = rename(oldFileName.c_str(), tmpNewFileName.c_str() );
		result = ACE_OS::stat(tmpNewFileName.c_str(), &f_stat);
		while (result == 0 && counter < 10)
		{
			// File still exist, system clock could be adjusted backwards, try with 2 sec interval to rename
			sleep(2);
			counter++;
			tmpTimeStamp = aes_afp_timestamp::Instance().getTimeString();
			tmpFileName = newFilename_;
			tmpFileName += '_' + tmpTimeStamp;
			tmpNewFileName += '_' + tmpTimeStamp;
			//result = ACE_OS::rename(oldFileName.c_str(), tmpNewFileName.c_str() );
			result = ACE_OS::stat(tmpNewFileName.c_str(), &f_stat);
		}
		if (result != 0)
		{
			AES_AFP_LOG(LOG_LEVEL_INFO, "OriginalFileName = %s Renamed File = %s",oldFileName.c_str(), tmpNewFileName.c_str());
			result = ACE_OS::rename(oldFileName.c_str(), tmpNewFileName.c_str());
		}
		// Change supplied file name to the one with a time stamp.
		newFilename_ = tmpFileName;
		newFileName = tmpFileName;

	}
	else
	{
		result = ACE_OS::rename(oldFileName.c_str(), tmpNewFileName.c_str() );
	}



	if (result != 0)
	{
		return false;
	}

	return true;
	//## end aes_afp_template::renameFileLocal%3BE1359402C0.body
}

//## Operation: createFileName%3BE135E502D1; C++
//## Semantics:
//	---------------------------------------------------------
//	       createFileName()
//	---------------------------------------------------------
std::string aes_afp_template::createFileName (std::string oldFileName)
{
	TemplateVector::iterator itr;
	ACE_Time_Value currentTime(ACE_OS::time());
	struct tm *newtime;
	long long_time(currentTime.sec() );
	char buf[32];
	//struct _stat buffer;
	ACE_stat buffer;
	int result;
	int index(0);
	std::string originalFileName("");
	int originalFileSize(0);

	bool tagfirst(false);  // uabmha: CNI 1135
	bool tagsecond(false); // uabmha: CNI 1135

	//Start of TR HO99988
	std::string bladeid;	
	std::string bladefirst;	
	int ipos = 0;	
	int ipos1 = 0;
	bool rtrTraffic(false);	
	std::string trafficType;

	//End of TR HO99988

	AES_AFP_TRACE_MESSAGE("Entering , nameTag_ = %s", nameTag_.c_str());

	// uabmha, CNI 1135, convert startup sequence number
	// from -1 (internal repr.) to 0, when applicable.
	// (Internal repr. needed to distinguish between when
	// set to 0 from afpdef and when default value 0 is used.
	// afpls needs this to suppress header printout when value
	// not set from afpdef.
	if (startupSequenceNumber_ == -1)
	{
		startupSequenceNumber_ = 0;
	}

	newtime = ACE_OS::localtime(&long_time);
	newFilename_ = "";
	index = oldFileName.find_last_of('/');
	if (index != -1)
	{
		originalFileName = oldFileName.substr(index +1);
	}

	result = ACE_OS::stat(oldFileName.c_str(), &buffer);
	if (result == 0)
	{
		originalFileSize = ((buffer.st_size-1)/1024)+1;
	}
	/* START:: TR HO99988 checking is the file is rtr or not */
	//if ((ipos1 = originalFileName.find_first_of("+")) != (std::string::npos))
	ipos1 =  originalFileName.find_first_of("+");
	if(ipos1 != -1)
	{
		trafficType = originalFileName.substr(ipos1+1,3);
		if (trafficType.compare("RTR") == 0)
		{

			rtrTraffic = true;
		}
	}
	AES_AFP_TRACE_MESSAGE("value of rtrTraffic = %d",rtrTraffic);
	/* END:: HO99988 */


	for (itr = templateVector_.begin();itr != templateVector_.end();++itr)
	{
		if ((*itr) == "TWOYEAR")
		{
			sprintf(buf, "%02d", (newtime->tm_year + 1900) - 2000);
			newFilename_ += buf;
		}
		if ((*itr) == "FOURYEAR")
		{
			sprintf(buf, "%04d", (newtime->tm_year + 1900) );
			newFilename_ += buf;
		}
		if ((*itr) == "MONTH")
		{
			sprintf(buf, "%02d", (newtime->tm_mon + 1));
			newFilename_ += buf;
		}
		if ((*itr) == "DAY")
		{
			sprintf(buf, "%02d", newtime->tm_mday);
			newFilename_ += buf;
		}
		if ((*itr) == "HOUR")
		{
			sprintf(buf, "%02d", newtime->tm_hour);
			newFilename_ += buf;
		}
		if ((*itr) == "MINUTE")
		{
			sprintf(buf, "%02d", newtime->tm_min);
			newFilename_ += buf;
		}
		if ((*itr) == "SECOND")
		{
			sprintf(buf, "%02d", newtime->tm_sec);
			newFilename_ += buf;
		}
		if ((*itr) == "ONEPERS")
		{
		        ifstream fin(persistentFilePath_.c_str() );            //HV40150
                        if(fin.good())
                        {
                                unsigned long val = 0;
                                if(fin >> val)
                                {
                                        persistentNr_ = val;
                                        fin.close();
                                }
                                else
                                {
                                        fin.close();
                                        AES_AFP_LOG(LOG_LEVEL_WARN, "Error in reading persistentNr_ value %lu from file",persistentNr_);
                                }

                        }
                        else
                        {
                                AES_AFP_LOG(LOG_LEVEL_WARN, "Error : stream is not good");
                        }
                        sprintf(buf, "%04lu", persistentNr_);
                        persistentNr_++;

                        if (persistentNr_ > 9999)
                        {
                                persistentNr_  = startupSequenceNumber_; //uabmha: For CNI 1135
                        }

                        newFilename_ += buf;
                        AES_AFP_LOG(LOG_LEVEL_INFO, "newFilename_ is %s",newFilename_.c_str());  //HV40150
		}
		if ((*itr) == "FOURPERS")
		{
			ifstream fin(persistentFilePath_.c_str() );                  //HV40150
			if(fin.good())
                	{
                                unsigned long val = 0;
                        	// if(fin >> persistentNr_)
                        	if(fin >> val)
				{
                                        persistentNr_ = val;
                        		fin.close();
				}
				else
				{
					fin.close();
					AES_AFP_LOG(LOG_LEVEL_WARN, "Error in reading persistentNr_ value %lu from file",persistentNr_);
				}
                        	
                	}
                	else
                	{
                        	AES_AFP_LOG(LOG_LEVEL_WARN, "Error : stream is not good");
                	}
			sprintf(buf, "%04lu", persistentNr_);
			persistentNr_++;

			if (persistentNr_ > 9999)
			{
				persistentNr_  = startupSequenceNumber_; //uabmha: For CNI 1135
			}

			newFilename_ += buf;
			AES_AFP_LOG(LOG_LEVEL_INFO, "newFilename_ is %s",newFilename_.c_str()); //HV40150
		}
		//For HL79347 : Check for sixpers
		if ((*itr) == "SIXPERS")
		{
		        ifstream fin(persistentFilePath_.c_str() );                 //HV40150
                        if(fin.good())
                        {
                                unsigned long val = 0;
                                // if(fin >> persistentNr_)
                                if(fin >> val)
                                {
                                        persistentNr_ = val;
                                        fin.close();
                                }
                                else
                                {
                                        fin.close();
                                        AES_AFP_LOG(LOG_LEVEL_WARN, "Error in reading persistentNr_ value %lu from file",persistentNr_);
                                }

                        }
                        else
                        {
                                AES_AFP_LOG(LOG_LEVEL_WARN, "Error : stream is not good");
                        }
                        sprintf(buf, "%06lu", persistentNr_);
                        persistentNr_++;

                        if (persistentNr_ > 999999)
                        {
                                persistentNr_  = startupSequenceNumber_; //uabmha: For CNI 1135
                        }

                        newFilename_ += buf;
                        AES_AFP_LOG(LOG_LEVEL_INFO, "newFilename_ is %s",newFilename_.c_str());  //HV40150
		}
		//For HL79347 : Check for eightpers
		if ((*itr) == "EIGHTPERS")
		{
		        ifstream fin(persistentFilePath_.c_str() );                  //HV40150
                        if(fin.good())
                        {
                                unsigned long val = 0;
                                // if(fin >> persistentNr_)
                                if(fin >> val)
                                {
                                        persistentNr_ = val;
                                        fin.close();
                                }
                                else
                                {
                                        fin.close();
                                        AES_AFP_LOG(LOG_LEVEL_WARN, "Error in reading persistentNr_ value %lu from file",persistentNr_);
                                }

                        }
                        else
                        {
                                AES_AFP_LOG(LOG_LEVEL_WARN, "Error : stream is not good");
                        }
                        sprintf(buf, "%08lu", persistentNr_);
                        persistentNr_++;

                        if (persistentNr_ > 99999999)
                        {
                                persistentNr_  = startupSequenceNumber_; //uabmha: For CNI 1135
                        }

                        newFilename_ += buf;
                        AES_AFP_LOG(LOG_LEVEL_INFO, "newFilename_ is %s",newFilename_.c_str());   //HV40150
		}
		if ((*itr) == "ONENERS")
		{
			sprintf(buf, "%lu", transientNr_);
			transientNr_++;
			if (transientNr_ > 9999)
			{
				transientNr_ = startupSequenceNumber_; //uabmha: For CNI 1135
			}
			newFilename_ += buf;
		}
		if ((*itr) == "FOURNERS")
		{
			sprintf(buf, "%04lu", transientNr_);
			transientNr_++;
			if (transientNr_ > 9999)
			{
				transientNr_ = startupSequenceNumber_; //uabmha: For CNI 1135
			}
			newFilename_ += buf;
		}
		//For HL79347 : Check for sixners
		if ((*itr) == "SIXNERS")
		{
			sprintf(buf, "%06lu", transientNr_);
			transientNr_++;
			if (transientNr_ > 999999)
			{
				transientNr_ = startupSequenceNumber_; //uabmha: For CNI 625
			}
			newFilename_ += buf;
		}
		//For HL79347 : Check for eightners
		if ((*itr) == "EIGHTNERS")
		{
			sprintf(buf, "%08lu", transientNr_);
			transientNr_++;
			if (transientNr_ > 99999999)
			{
				transientNr_ = startupSequenceNumber_; //uabmha: For CNI 625
			}
			newFilename_ += buf;
		}
		if ((*itr) == "FILESIZE")
		{
			sprintf(buf,"%ld",(unsigned long)originalFileSize);
			newFilename_ += buf;
		}
		if ((*itr) == "ORIGINAL")
		{
			newFilename_ += originalFileName;
		}
		if ((*itr) == "NAMETAG")
		{
			// uabmha: CNI 1135, Added handling of two name tags
			AES_AFP_TRACE_MESSAGE("Entering NAMETAG");
			if (multiNameTag_ == false)
			{
				// No asterisk found in name tag...
				// use full name tag string.
				newFilename_ += nameTag_;
				AES_AFP_TRACE_MESSAGE("NAMETAG, multiNametag FALSE, nameTag_ = %s", nameTag_.c_str());
			}
			else if( tagsecond == true)
			{
				AES_AFP_TRACE_MESSAGE("NAMETAG, tagsecond TRUE");
				// Finished; nameTagPart2_ has been used
			}
			else if( tagfirst == true)
			{
				// nameTagPart1_ was used in previous loop
				newFilename_ += nameTagPart2_;
				tagsecond = true;
				AES_AFP_TRACE_MESSAGE("NAMETAG, nameTagPart2_ = %s", nameTagPart2_.c_str());
			}
			else
			{
				// First round, nameTagPart1_ to be used
				newFilename_ += nameTagPart1_;
				tagfirst = true;
				AES_AFP_TRACE_MESSAGE( "NAMETAG, nameTagPart1_ = %s", nameTagPart1_.c_str());
			}

			//newFilename_ += nameTag_;
		}		
		//TR H099988:: Ths is the RTR with blade id
		if( rtrFileFlag && rtrTraffic)
		{
			if ((*itr) == "BID")
			{
				//if ((ipos = originalFileName.find_first_of("+")) != (std::string::npos))
				ipos = originalFileName.find_first_of("+");
				if(ipos != -1)
				{
					bladeid = originalFileName.substr(0, ipos);
					// terminate with NULL;
					bladeid[ipos] = NULL;
					bladefirst = bladeid.substr(0,2);
					if (bladefirst.compare("BC") == 0)
					{
						if((bladeid.length()) == 3)
						{
							bladeid.replace(0,2,"0");
							newFilename_+= bladeid;
						}
						if ((bladeid.length()) == 4)
						{
							bladeid = bladeid.substr(2,2);
							newFilename_+= bladeid;
						}
					}
					if (bladefirst.compare("CP") == 0)
					{
						if((bladeid.length()) == 3)
						{
							bladeid.replace(0,2,"9");
							newFilename_+= bladeid;
						}

					}	
				}
			}

		}
		else
		{
			AES_AFP_TRACE_MESSAGE("Not rtr MCP");	
		}

	} // end of for loop
	AES_AFP_TRACE_MESSAGE("Leaving newFilename_ = %s", newFilename_.c_str());
	return newFilename_;
	//## end aes_afp_template::createFileName%3BE135E502D1.body
}

//## Operation: commitPersNr%3BEBA86F002D; C++
//## Semantics:
//	---------------------------------------------------------
//	       commitPersNr()
//	---------------------------------------------------------
void aes_afp_template::commitPersNr ()
{
	//## begin aes_afp_template::commitPersNr%3BEBA86F002D.body preserve=yes
	if (usePersNr_ == true)
	{
		ofstream fout(persistentFilePath_.c_str() );
		AES_AFP_LOG(LOG_LEVEL_INFO, "Value of persistentNr_ is %lu",persistentNr_);
		if(fout.good())
		{
			if(fout << persistentNr_)
			{
				fout.close();
			}
			else
			{
				fout.close();
				AES_AFP_LOG(LOG_LEVEL_WARN, "Error in writing file %lu",persistentNr_);
			}
		}
		else
		{
			AES_AFP_LOG(LOG_LEVEL_WARN, "Error : ofstream is not good");
		}
	}
	//## end aes_afp_template::commitPersNr%3BEBA86F002D.body
}
// Additional Declarations
//## begin aes_afp_template%3BE1316F027F.declarations preserve=yes
//## end aes_afp_template%3BE1316F027F.declarations

//## begin module%3BE1320C0284.epilog preserve=yes
//## end module%3BE1320C0284.epilog
