#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "cute_cdh_cmd_handler.h"
#include <string>
using namespace std;
#define FILEREMOTEDEST_COMMAND_FILE "/opt/ap/aes/bin/fileremotecmdfile"
#define RESPDEST_COMMAND_FILE "/opt/ap/aes/bin/respdestcmdfile"
#define FILEDESTSET_COMMAND_FILE "/opt/ap/aes/bin/filedestsetcmdfile"

list<string> Cute_Command_Handler::commandStringList;
list<string> Cute_Command_Handler::commandResultList;
list<string>::iterator it1;
list<string>::iterator it2;
vector<string>Macros_Name;
vector<string>Macros_Val;
vector<int>Macros_len;
vector<string>::iterator it3;
vector<string>::iterator it4;
vector<int>::iterator it5;
int countstar=0;


void Cute_Command_Handler:: getMacros(string fileName)
{
std::string myString;
char szTemp[512];
strncpy(szTemp, fileName.c_str(), 511);
ifstream * commandfile = new ifstream(szTemp,ios::in);
string MacroVal;
string Macro;

int flag=0;
while((!(getline(*commandfile,myString )<=0)))
        {
       //etline(pszReadLine,myString); // Saves the line in STRING.

        string MACROENDLINE="*";
        string CHECK=myString.substr(0,1);
                if (MACROENDLINE.compare(CHECK)!=0 && flag==0)
                {
                int pos1;
        //cout<<myString<<endl; // Prints our STRING.
                string MacroStart="=";

                //SETING MACRO LENGTH IN VECTOR
                pos1=myString.find(MacroStart);
                Macros_len.push_back(pos1);
                string MacroEnd="\n";
                int pos2=myString.find(MacroEnd);

                //SETING MACRO VALUES IN VECTOR
                MacroVal=myString.substr(pos1+1,pos2);
                Macros_Val.push_back(MacroVal);

                //SETING MACRO NAMES IN VECTOR
                Macro=myString.substr(0,pos1);
                Macros_Name.push_back(Macro);
                //cout<<MacroVal<<endl;
                //cout<<Macro<<endl;
                countstar++;
                }


                else
                {
               flag=1;
               break;

                }
}
}

void Cute_Command_Handler:: replaceMacros(string fileName)
{
string Name;
string Value;
int len;
int count;
int flag;
string myString;
std::string pszReadLine;
char szTemp[512];
strncpy(szTemp, fileName.c_str(), 511);
ifstream * commandfile = new ifstream(szTemp,ios::in);
int check_count_star=0;
string IMMCOM="imm";
                       while((!(getline(*commandfile,myString )<=0)))
                        {
                        string CHECK=myString.substr(0,3);
                        check_count_star++;
                        if( check_count_star > countstar+1)
                        {

                        for(it3=Macros_Name.begin(),it4=Macros_Val.begin(),it5=Macros_len.begin();it3!=Macros_Name.end(),it4!=Macros_Val.end(),it5!=Macros_len.end();it3++,it4++,it5++)
                        {
                        Name.assign(*it3);
                        Value.assign(*it4);
                        len=*it5;



                        int pos3= myString.find(Name);
                        flag=0;
                        count++;
                        int n=0;


                                if (pos3>0)
                                {

                                flag=1;
                                pos3=0;
                                while( (pos3= myString.find(Name, pos3 ))
                                            != std::string::npos )
                                        {
                                        if ( myString.at(pos3-1) == '=' )
                                                {n++;
                                        int len = Name.size();

                //                      cout<<pos<<endl;

                                        myString.replace(pos3,len,Value);
                                        }
                                        pos3 +=Value.size();

                                }





                                }
                        }


                           int posA=myString.find('>');
                        int posB=myString.find('@');
                        std::string commandString=myString.substr(posA+1,posB-posA-1);
                        std::string commandResult=myString.substr(posB+1);
                        commandStringList.push_back(commandString);
                        commandResultList.push_back(commandResult); }
                        else
                        {}

}
}

bool Cute_Command_Handler::immExecuteCommand(string cmd)
{
	FILE *fp1,*fp2;
	char * data = new char[512];
	int status = -1;
	int ret = -1;
	bool bstatus = false;

	fp1 = popen(cmd.c_str(),"r");

	if (fp1 == NULL)
	{
		bstatus = false;
	}

	status = pclose(fp1);

	if (WIFEXITED(status))
	{
		ret=WEXITSTATUS(status);
	}

	if (ret == 0)
	{
		bstatus = true;
	}
	else
	{
		bstatus = false;
	}
	return bstatus;
}

bool Cute_Command_Handler::createCommandPairLists(string fileName)
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
       getMacros(fileName);
       replaceMacros(fileName);
	
/*while((!(getline(*commandFile, pszReadLine)<=0)))
	{
		if ((pszReadLine.size()!=0))
		{
			replaceMacros(pszReadLine);
			
			int pos1=pszReadLine.find('>');
			int pos2=pszReadLine.find('@');
			std::string commandString=pszReadLine.substr(pos1+1,pos2-pos1-1);
			std::string commandResult=pszReadLine.substr(pos2+1);
			commandStringList.push_back(commandString);
			commandResultList.push_back(commandResult);
        
		}

	}
*/
	//content of the lists

	for(it1=commandStringList.begin(),it2=commandResultList.begin();it1!=commandStringList.end(),it2!=commandResultList.end();it1++,it2++)
	{
		cout<<"command pair = "<<(*it1)<<"\t"<<(*it2)<<endl;
	}
	return true;
}

bool Cute_Command_Handler::executeTest(string fileName)
{
	int i=1;
	string resultStr;
	bool bStatus = false;
	createCommandPairLists(fileName);
	for(it1=commandStringList.begin(),it2=commandResultList.begin();it1!=commandStringList.end(),it2!=commandResultList.end();it1++,it2++)
	{
		cout<<"######################################\n\n";
		cout<<"Executing command numbered : "<<i<<endl;
		cout<<"!!command : \n "<<(*it1)<<endl<<endl;
		bStatus = immExecuteCommand((*it1));
		if (bStatus == 0)
		{
			resultStr = "fail";
		}
		else
		{
			resultStr = "pass";
		}
		if(resultStr==(*it2))
		{
			cout<<"\n ##Test case : PASSED##\n";
			cout<<"######################################\n\n";
		}
		else
		{
//			ASSERTM("##Test Case : FAILED##",false);
			cout<<"##Test Case : FAILED##\n";
			commandStringList.clear();
			commandStringList.clear();
			return false;
		}

		i++;
	}
	commandStringList.clear();
	commandResultList.clear();
	return true;
}
void Cute_Command_Handler::fileRemoteTestCases()
{
	bool status = executeTest(FILEREMOTEDEST_COMMAND_FILE);
	ASSERTM("##FileRemoteDestination Test case Failed##",status);
}

void Cute_Command_Handler::respDestTestCases()
{
	bool status = executeTest(RESPDEST_COMMAND_FILE);
	ASSERTM("##RespondingDestination Test case Failed##",status);
}

void Cute_Command_Handler::FileDestSetTestCases()
{
	bool status = executeTest(FILEDESTSET_COMMAND_FILE);
	ASSERTM("##FileDestSet Test case Failed##",status);
}

cute::suite Cute_Command_Handler::make_suite_cute_command_handler()
{
	cute::suite s;
	s.push_back(CUTE(fileRemoteTestCases));
	s.push_back(CUTE(respDestTestCases));
//	s.push_back(CUTE(FileDestSetTestCases));
	return s;
}



