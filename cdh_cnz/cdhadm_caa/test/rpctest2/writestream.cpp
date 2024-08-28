#ifdef WIN32
#pragma warning(disable:4786)
#endif

#define RECORD_SIZE 8192

#include <fstream>
#include "fms_cpf_stream.h"

using namespace std;

int main(int argc, char *argv[])
{

    FMS_CPF_Stream stream(FMS_CPF_Stream::SharedMem, true, 40, RECORD_SIZE);
    bool endOfFile = false;
	int returnCode = 0;
    char buf[RECORD_SIZE];
    int rc;
    EventMsgData msg;
    char ch;
    int recNr = 0;

    rc = stream.create(argv[1]);
    msg.MsgCmdCode = FMS_CPF_EventMsg::cmdHandleSubOpen;
    msg.MsgEventflag = 0;
    strcpy(msg.MsgFilename, "0001");
    msg.MsgFile2[0] = '\0';
    rc = stream.writeCtrl(msg);

    if (argc == 3) 
    {
        ifstream infile(argv[2], ios_base::in|ios_base::binary);

        if (!infile)
        {
            returnCode = 1;
        }

        while (!endOfFile && returnCode == 0)
        {
            recNr++;

            infile.read(buf, RECORD_SIZE);

            if (infile.gcount() != 0)
            {
                msg.MsgCmdCode = FMS_CPF_EventMsg::cmdHandleWrite;
                rc = stream.writeCtrl(msg);
               if ((rc = stream.write(buf, infile.gcount())) != infile.gcount()) returnCode = 2;
            }

            if (infile.gcount() < RECORD_SIZE)
            {
                if (infile.eof())
                {
                    endOfFile = true;
                }
                else
                {
                    returnCode = 3;
                }
                msg.MsgCmdCode = FMS_CPF_EventMsg::cmdHandleSubClose;
                rc = stream.writeCtrl(msg);
                msg.MsgCmdCode = FMS_CPF_EventMsg::cmdHandleClose;
                rc = stream.writeCtrl(msg);

            }

            if (recNr == 3)
            {
                msg.MsgCmdCode = FMS_CPF_EventMsg::cmdHandleSubClose;
                rc = stream.writeCtrl(msg);
                msg.MsgCmdCode = FMS_CPF_EventMsg::cmdHandleSubOpen;
                msg.MsgEventflag = 0;
                strcpy(msg.MsgFilename, "0002");
                msg.MsgFile2[0] = '\0';
                rc = stream.writeCtrl(msg);
            }

        }
        cout << "Press any key to close stream." << endl;
        cin.get(ch);
        rc = stream.close();
 
   }
    else
    {
        cout << "Usage: " << argv[0] << " streamname filename"
             << endl;
    }

    if (returnCode != 0) cout << "Error " << returnCode << endl;

	return returnCode;
}