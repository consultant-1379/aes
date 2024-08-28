#include <aes_afp_msgblock.h>

// Class aes_afp_msgblock 

aes_afp_msgblock::aes_afp_msgblock (size_t size, aes_afp_datablock* datablock)
      : ACE_Message_Block(size),msgType(UNKNOWN),
        callerId(0),
        data(false),
		errorCode(AES_NOERRORCODE)

{
  dataBlock = datablock;
  if (dataBlock != 0)
    {
      data = true;
    }
  else
    {
      data = false;
    }
}

aes_afp_msgblock::~aes_afp_msgblock()
{
	 if (dataBlock != 0)
   	 {
		delete dataBlock;
		dataBlock =0;
	}
}


aes_afp_datablock * aes_afp_msgblock::getData ()
{
  return dataBlock;
}

void aes_afp_msgblock::setCallerId (void* id)
{
  callerId = id;
}

void* aes_afp_msgblock::getCallerId ()
{
  return callerId;
}

void aes_afp_msgblock::setCallerName (std::string name)
{
  callerName = name;
}

std::string& aes_afp_msgblock::getCallerName ()
{
  return callerName;
}

void aes_afp_msgblock::setMsgType (aes_afp_msgtypes messageType)
{
  msgType = messageType;
}

aes_afp_msgtypes aes_afp_msgblock::getMsgType ()
{
  return msgType;
}

void aes_afp_msgblock::setErrorCode (AES_GCC_Errorcodes code)
{
  errorCode = code;
}

AES_GCC_Errorcodes aes_afp_msgblock::getErrorCode ()
{
  return errorCode;
}

bool aes_afp_msgblock::hasData ()
{
  return data;
}
