//****************************************************************
//
// NAME
//	rpctest.cpp
//
//
//	This program sends block towards rpcserver 
//

#include <string>
#include <iostream>
#include <time.h>
#include <sys/timeb.h>
#include <AES_GCC_GetOpt.h>
#include "aes_cdh_rpc.h"

using namespace std;

void printUsage();

int main(int argc, char* argv[])
{
    HCLIENT *client;
    string hostAddress = "172.17.139.249";
    unsigned long int bgwProgramNumber = 770703857;
    unsigned long blnr;
    unsigned int nrOfSend;
    unsigned int blockSize = 16384;
    char fileName[11] = "myRpcTest";
    BlockInfo cdhOutVar;
    unsigned long totaltime;
    int speed;
    time_t time1, time2; 
    unsigned long t1, t2;
    struct _timeb tstruct1,tstruct2;
    unsigned int summillisec;
    unsigned int tmp1;

// ------------ operator i/o --------------------------------

    int a_flag = 0;
    int s_flag = 0;
    int n_flag = 0;
//    int r_flag = 0;

	AES_GCC_GetOpt getopt(argc, argv,"");

	// parse blocksize name
    if (argc > getopt.optind)
    { 
		blockSize = atoi(argv[getopt.optind]);
		s_flag++;
		getopt.optind++;
	}
	else
	{
        printUsage();
        return 2;
	}

	// parse address name
    if (argc > getopt.optind)
    { 
		hostAddress = argv[getopt.optind];
		a_flag++;
		getopt.optind++;
	}
	else
	{
        printUsage();
        return 2;
	}

	// number of send
    if (argc > getopt.optind)
    { 
		nrOfSend = atoi(argv[getopt.optind]);
		n_flag++;
		getopt.optind++;
	}
	else
	{
        printUsage();
        return 2;
	}

    /*
	// remote name
    if (argc > getopt.optind)
    { 
		remoteName = argv[getopt.optind];
		r_flag++;
		getopt.optind++;
	}
	else
	{
        printUsage();
        return 2;
	}*/

	// No more names
	if (argc != getopt.optind)
    {
        printUsage();
        return 2;
    }

    // blocksize, address and nrOfSend must be specified
    if ((s_flag =! 1) || 
        (a_flag =! 1) || 
        (n_flag =! 1) 
       // ||(r_flag =! 1)
        )
    {
        printUsage();
        return 2;
    }

// ------------ end operator i/o ----------------------------

    cout <<"send blocks to myRpcTest"<<endl;

    // create clnt
    char* tmpHostAddress;
	tmpHostAddress = new char[hostAddress.size() + 1];
    (void) ::strcpy(tmpHostAddress, hostAddress.c_str());
    client = ::clnt_create(tmpHostAddress, bgwProgramNumber, A, "tcp");

    // get block number
    lastTransactionAnswer answer;
    int rpcReturnCode;
    char* ptr = fileName;
    char** ptr2 = &ptr;
    rpcReturnCode = ::lasttransaction_1(ptr2, &answer, client);
    blnr = answer.lastTransactionID + 1;
    cout <<"lasttransaction_1: " << rpcReturnCode ;
    cout <<" blnr: "<< blnr-1 << endl;

    // create block
	cout << "create block with size " << blockSize << " byte" << endl ;
	char *myBlock = new char[blockSize];
	memset((char*)myBlock, 'A', blockSize);
	unsigned int nrOfBytes = blockSize;

    // send block
    answerCode returnCode;
    cdhOutVar.theSourceID = fileName;
    cdhOutVar.theBlockData.theBlockData_len = blockSize;
    cdhOutVar.theBlockData.theBlockData_val = (unsigned char *)myBlock;
    cout << "Put the blocks "<< blnr <<" to "<<blnr+nrOfSend-1 <<endl;

    time(&time1);
    _ftime( &tstruct1 );
    t1 = (time1*1000)+tstruct1.millitm;

    for (int i=1; i<nrOfSend+1; i++)
    {
        cdhOutVar.theBlockID = blnr;
        rpcReturnCode = ::put_1(&cdhOutVar, &returnCode, client);
        blnr++;
        if(rpcReturnCode != 0)
        {
            cout << endl;
            cout << "rpcReturnCode: " << rpcReturnCode <<"  exit! " << endl;
            return 2;
        }
    }

    time(&time2);
    _ftime( &tstruct2 );
    t2 = (time2*1000)+tstruct2.millitm;
    
    totaltime = t2 - t1;
//    cout <<"t1 " << t1 << endl;
//    cout <<"t2 " << t2 << endl;
//    cout <<"totaltime " <<totaltime << endl;

    tmp1 = tstruct2.millitm - tstruct1.millitm;
    if (tmp1 < 1000)
    {
        summillisec = tmp1;
    }
    else
    {
        summillisec = tmp1 + 1000;
    }

    if (totaltime > 2)
    {
        cout << endl;
        cout << nrOfSend*blockSize/1000 <<"K bytes sent within ";
        cout << totaltime/1000 <<" sek and "<<summillisec<<" millisec"<< endl;
        speed = (nrOfSend*blockSize)/(totaltime);
        cout <<"The speed was " << speed << " Kbyte/sek" << endl;
    }
    else
    {
        cout << endl;
        cout <<"send more than 2 sec to get speed calculation!!" << endl;
    }

    cout << endl;
    return rpcReturnCode;
}

//*******************************************************************
//
void printUsage()
{
   cerr << "Incorrect usage" << endl;
   cerr << "Usage:" << endl;
   cerr << "rpctest blockSize address nrOfSend"<<endl;
}

