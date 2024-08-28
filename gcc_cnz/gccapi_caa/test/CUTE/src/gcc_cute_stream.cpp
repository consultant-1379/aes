#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "gcc_cute_stream.h"
using namespace std;

void AES_GCC_CUTE_Stream::StreamFunctionality()
{
		AES_GCC_Stream mystream;
		const std::string name("streamBufer");
		string buff("10-AAAAAAA");
		string buff0("5-AAA");
		string buff1("6-AAAB");
		string buff3("21-123456789123456ABC");
		string buff2("20-12345678912345678");
		ACE_TCHAR * buffin = new ACE_TCHAR[20];
		ACE_TCHAR * readbuff= new ACE_TCHAR[30];
		ACE_TCHAR * readbuff0= new ACE_TCHAR[30];
		ACE_TCHAR * readbuff1= new ACE_TCHAR[30];
		ACE_TCHAR * readbuff2= new ACE_TCHAR[30];
		ACE_UINT32 len= 10;
		int status = mystream.create(name,20);
		ASSERTM("Failed to create stream ",status==1);
		if(status)
		{

			strcpy(buffin,buff.c_str());
			status = mystream.write(buffin,strlen(buffin));
			ASSERTM("Failed to write into stream ",status==0);

			strcpy(buffin,buff0.c_str());
			status = mystream.write(buffin,strlen(buffin));
			ASSERTM("Failed to write into stream ",status==0);

			strcpy(buffin,buff1.c_str());
			status = mystream.write(buffin,strlen(buffin));
			ASSERTM("Failed: wrote into stream even though buffer full ",status==5);

			status = mystream.read(readbuff,len);
			ASSERTM("Failed to read from stream ",status==0);

			strcpy(buffin,buff1.c_str());
			status = mystream.write(buffin,strlen(buffin));
			ASSERTM("Failed to write into stream through turn around ",status==0);

			status = mystream.read(readbuff0,len);
			ASSERTM("Failed to read from stream ",status==0);

			mystream.clear();


			status = mystream.read(readbuff1,len);
			ASSERTM("Failed: Read from stream even though buffer empty",status==7);
/*
			strcpy(buffin,buff2.c_str());
			status = mystream.write(buffin,strlen(buffin));
			ASSERTM("Failed to write into stream ",status==0);


			len=20;
			status = mystream.read(readbuff2,len);
			ASSERTM("Failed to read from stream ",status==0);
*/
			mystream.clear();

			strcpy(buffin,buff3.c_str());
			status = mystream.write(buffin,strlen(buffin));
			ASSERTM("Failed to write into stream ",status==5);


			mystream.clear();
			mystream.close();
		}
}


cute::suite AES_GCC_CUTE_Stream::make_suite_gcc_cute_stream()
{

	cute::suite s;
	s.push_back(CUTE(StreamFunctionality));
	return s;
}
