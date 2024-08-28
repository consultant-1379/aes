/*=================================================================== */
/**
   @file   aes_cdh_cmdclient.cpp

   Class method implementationn for AES_CDH_CmdClient type module.

   This module contains the implementation of class declared in
   the aes_cdh_cmdclient.h module

   Handles communication with AES_CDH_Server using DSD.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011   XCHEMAD     Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_cdh_cmdclient.h>

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: AES_CDH_CmdClient
=================================================================== */
AES_CDH_CmdClient::AES_CDH_CmdClient(const ACS_APGCC_DSD_Addr& addr)
: m_address(addr),
  stream_(),
  server_()
{
}  

AES_CDH_CmdClient::AES_CDH_CmdClient(const char* serviceName)
: m_address(serviceName),
  stream_(),
  server_()
{

}
/*===================================================================
   ROUTINE: ~AES_CDH_CmdClient
=================================================================== */
AES_CDH_CmdClient::~AES_CDH_CmdClient()
{
}


/*===================================================================
   ROUTINE: connect
=================================================================== */
int AES_CDH_CmdClient::connect(void)
{
	int ret = server_.connect(stream_, m_address);
    if (ret != 0) 
    {
        return -1;
    }
    return 0;
}

/*===================================================================
   ROUTINE: send
=================================================================== */
int AES_CDH_CmdClient::send(ACS_APGCC_Command& cmd)
{
    cmd.result = -1;
    
    cmd.encode();
    
    if (cmd.send(stream_) < 0) 
    {
        return -1;
    }
    return 0;
}

/*===================================================================
   ROUTINE: receive
=================================================================== */
int AES_CDH_CmdClient::receive(ACS_APGCC_Command& cmd)
{
    if (cmd.recv(stream_) < 0) 
    {
        return -1;
    }
    return 0;
}

/*===================================================================
   ROUTINE: getHandle
=================================================================== */
ACE_HANDLE AES_CDH_CmdClient::getHandle(void)     // ++
{
  ACE_HANDLE handles[1] = {ACE_INVALID_HANDLE};
  int handle_count = 1;
  stream_.get_handles(handles, handle_count) ;
  return handles[0];

}

/*===================================================================
   ROUTINE: close
=================================================================== */
void AES_CDH_CmdClient::close(void)
{
    stream_.close();
    server_.close();
}
