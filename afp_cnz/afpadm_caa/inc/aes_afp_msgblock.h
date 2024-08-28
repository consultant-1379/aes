#ifndef aes_afp_msgblock_h
#define aes_afp_msgblock_h 1

#include "aes_afp_datablock.h"
#include "aes_afp_msgtypes.h"
#include <string>
#include <ace/Message_Block.h>
#include <aes_gcc_errorcodes.h>
//#include "aes_gcc_errorcodes_r1.h"

//## Class: aes_afp_msgblock%373FF0ED00B5
//	This class is used in an Message queue for communication
//	between threads.

class aes_afp_msgblock : public ACE_Message_Block
{
public:

	aes_afp_msgblock (size_t size = 0, aes_afp_datablock* datablock = 0);

	virtual ~aes_afp_msgblock();

	//	Retrieves the data part from a aes_afp_msgblock.
	aes_afp_datablock* getData ();

	//	Set the caller id for this message.
	void setCallerId (void* id);

	//	Retrieves the caller id.
	void* getCallerId ();

	//	Set the callers name.
	void setCallerName (std::string name);

	//	Retrieves the name of the caller.
	std::string& getCallerName ();

	//	Set the message type.
	void setMsgType (aes_afp_msgtypes messageType);

	//	Retrieves the message type for this message.
	aes_afp_msgtypes getMsgType ();

	//	Sets the error code for this message block.
	void setErrorCode (AES_GCC_Errorcodes code);

	//	Retreives thoe error code for this message block.
	AES_GCC_Errorcodes getErrorCode ();

	//	Question if this message block has a data block.
	bool hasData ();

private:
	//## Get and Set Operations for Class Attributes (generated)

	const void* getcallerId () const;
	void setcallerId (void* value);
	//	True if message block contains data.
	bool getdata () const;
	void setdata (bool value);

private: //## implementation
	// Data Members for Class Attributes


	// Data Members for Associations

	aes_afp_datablock *dataBlock;
	aes_afp_msgtypes msgType;
	void* callerId;
	bool data;
	std::string callerName;
	AES_GCC_Errorcodes errorCode;
};

//## Get and Set Operations for Class Attributes (inline)

inline const void* aes_afp_msgblock::getcallerId () const
{
	return callerId;
}

inline void aes_afp_msgblock::setcallerId (void* value)
{
	callerId = value;
}

inline bool aes_afp_msgblock::getdata () const
{
	return data;
}

inline void aes_afp_msgblock::setdata (bool value)
{
	data = value;
}

#endif
