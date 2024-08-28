#include "aes_gcc_stream.h"
#include <ace/ACE.h>

#include <iostream>
using namespace std;
int main()
{

	AES_GCC_Stream mystream;
	const std::string name("streamBufer");
	string buff("10-AAAAAAA");
	string buff0("5-AAA");
	string buff1("6-AAAB");
	string buff3("21-123456789123456ABC");
	string buff2("20-1234567891234567");
	int status;
	ACE_TCHAR * buffin = new ACE_TCHAR[20];
	ACE_TCHAR * readbuff= new ACE_TCHAR[30];
	ACE_TCHAR * readbuff0= new ACE_TCHAR[30];
	ACE_TCHAR * readbuff1= new ACE_TCHAR[30];
	ACE_TCHAR * readbuff2= new ACE_TCHAR[30];
	ACE_UINT32 len= 10;
	bool result = mystream.create(name,20);
	cout<<"result of create :"<<result<<endl;
	if(result)
	{

		strcpy(buffin,buff.c_str());
		cout<<"buffin = "<<buff.c_str()<<" buffer length: "<<strlen(buffin)<<endl;
		status = mystream.write(buffin,strlen(buffin));
		cout<<"Status of write1 :"<<status<<endl;
		if(status==0)
		{
			cout<<"successful write to stream"<<endl;
		}
		else if(status ==5)
		{
			cout<<"Failed to write to stream : buffer full "<<endl;
		}


		strcpy(buffin,buff0.c_str());
		cout<<endl<<"buffin = "<<buff0.c_str()<<" buffer length: "<<strlen(buffin)<<endl;
		status = mystream.write(buffin,strlen(buffin));
		cout<<"Status of write2 :"<<status<<endl;
		if(status==0)
		{
			cout<<"successful write to stream"<<endl;
		}
		else if(status ==5)
		{
			cout<<"Failed to write to stream : buffer full "<<endl;
		}


		strcpy(buffin,buff1.c_str());
		cout<<endl<<"buffin = "<<buff1.c_str()<<" buffer length: "<<strlen(buffin)<<endl;
		status = mystream.write(buffin,strlen(buffin));
		cout<<"Status of write3 :"<<status<<endl;
		if(status==0)
		{
			cout<<"successful write to stream"<<endl;
		}
		else if(status ==5)
		{
			cout<<"Failed to write to stream : buffer full "<<endl;
		}

		status = mystream.read(readbuff,len);
		cout<<endl<<"Status of read1 :"<<status<<endl;
		if(status==0)
		{
			cout<<"successful read "<<endl;
		}
		else if(status==1)
		{
			cout<<"Error while executing Read "<<endl;
		}
		else if(status==3)
		{
			cout<<"Timeout while performing read"<<endl;
		}
		else if(status==7)
		{
			cout<<"Found buffer empty"<<endl;
		}
		cout<<"read buffer is: "<<readbuff<<" length given: "<<len<<endl;

		strcpy(buffin,buff1.c_str());
		cout<<endl<<"buffin = "<<buff1.c_str()<<" buffer length: "<<strlen(buffin)<<endl;
		status = mystream.write(buffin,strlen(buffin));
		cout<<"Status of write3B :"<<status<<endl;
		if(status==0)
		{
			cout<<"successful write to stream"<<endl;
		}
		else if(status ==5)
		{
			cout<<"Failed to write to stream : buffer full "<<endl;
		}

		status = mystream.read(readbuff0,len);
		cout<<endl<<"Status of read2 :"<<status<<endl;
		if(status==0)
		{
			cout<<"successful read "<<endl;
		}
			else if(status==1)
		{
			cout<<"Error while executing Read "<<endl;
		}
			else if(status==3)
		{
			cout<<"Timeout while performing read"<<endl;
		}
			else if(status==7)
		{
			cout<<"Found buffer empty"<<endl;
		}
		cout<<"read buffer is: "<<readbuff0<<" length given: "<<len<<endl;

		mystream.clear();

		status = mystream.read(readbuff1,len);
		cout<<endl<<"Status of read3 (immediate after clear0) :"<<status<<endl;
		if(status==0)
		{
				cout<<"successful read "<<endl;
		}
			else if(status==1)
		{
			cout<<"Error while executing Read "<<endl;
		}
			else if(status==3)
		{
			cout<<"Timeout while performing read"<<endl;
		}
			else if(status==7)
		{
			cout<<"Found buffer empty"<<endl;
		}
		cout<<"read buffer is: "<<readbuff1<<" length given: "<<len<<endl;

		strcpy(buffin,buff2.c_str());
		status = mystream.write(buffin,strlen(buffin));
		cout<<endl<<"buffin = "<<buff2.c_str()<<" buffer length: "<<strlen(buffin)<<endl;
		cout<<"Status of write4 (after clear1) :"<<status<<endl;
		if(status==0)
		{
			cout<<"successful write to stream"<<endl;
		}
			else if(status ==5)
		{
			cout<<"Failed to write to stream : buffer full "<<endl;
		}


		len=20;
		status = mystream.read(readbuff2,len);
		cout<<endl<<"Status of read4 (after clear2 (after write)) :"<<status<<endl;
		if(status==0)
		{
			cout<<"successful read "<<endl;
		}
			else if(status==1)
		{
			cout<<"Error while executing Read "<<endl;
		}
			else if(status==3)
		{
			cout<<"Timeout while performing read"<<endl;
		}
			else if(status==7)
		{
			cout<<"Found buffer empty"<<endl;
		}
		cout<<"read buffer is: "<<readbuff2<<" length given: "<<len<<endl;

		mystream.clear();

		strcpy(buffin,buff3.c_str());
		cout<<endl<<"buffin = "<<buff3.c_str()<<" buffer length: "<<strlen(buffin)<<endl;
		status = mystream.write(buffin,strlen(buffin));
		cout<<"Status of write4 (after clear00) :"<<status<<endl;
		if(status==0)
		{
			cout<<"successful write to stream"<<endl;
		}
			else if(status ==5)
		{
			cout<<"Failed to write to stream : buffer full "<<endl;
		}

		mystream.clear();
		mystream.close();
		cout<<"close ended in main"<<endl;
	}
	cout<<"end of program"<<endl;
	return 0;
}

