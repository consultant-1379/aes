/*
 * aes_afp_server.h
 *
 *  Created on: Sep 26, 2011
 *      Author: tcsnarn1
 */

#ifndef AES_AFP_SERVER_H_
#define AES_AFP_SERVER_H_
#include <ace/OS.h>
#include <ace/ACE.h>

#include <ACS_CS_API.h>
#include <ACS_DSD_Client.h>
#include <acs_aeh_signalhandler.h>

#include "aes_afp_datatask.h"
#include "aes_afp_apcom_serv.h"
#include "aes_afp_services.h"
#include "aes_afp_parameter.h"

#define AES_AFP_PROCESS_NAME "aes_afpd"

extern aes_afp_services * afpServices;

void usage();

#endif /* AES_AFP_SERVER_H_ */
