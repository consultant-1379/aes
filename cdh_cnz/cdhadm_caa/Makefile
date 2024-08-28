# **********************************************************************
#
# Short description:
# Makefile template for CDH Library
# **********************************************************************
#
# Ericsson AB 2010 All rights reserved.
# The information in this document is the property of Ericsson.
# Except as specifically authorized in writing by Ericsson, the receiver of this
# document shall keep the information contained herein confidential and shall protect
# the same in whole or in part from disclosure and dissemination to third parties.
# Disclosure and disseminations to the receivers employees shall only be made
# on a strict need to know basis.
#
# **********************************************************************
#
# Rev        Date         Name      What
# -----      -------      --------  --------------------------
#            2011-09-06   XCHEMAD   Created
#                                                            
# **********************************************************************

CNZ_NAME ?= cdh_cnz
CAA_NAME ?= cdhadm_caa
CXC_NAME ?= cdh_cxc

# Basic paths
VOB_PATH = $(AES_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(COMMON_ROOT)/common.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
APIINCINTDIR = ../cdhapi_caa/inc_int
APIINCEXTDIR = ../cdhapi_caa/inc_ext

LIBCDHEXT= ../cdh_cxc/bin/lib_ext

# trace enabled
EXTRA_DEFINES += -DAES_CDH_API_HAS_TRACE
EXTRA_DEFINES += -DAES_CDH_TRACE_USE_SOURCE_INFO
EXTRA_DEFINES += -DAES_CDH_TRACE_HAS_FUNCTION_TRACE
EXTRA_DEFINES += -DAES_CDH_TRACE_USE_PRETTY_FUNCTION

# log enabled
EXTRA_DEFINES += -DAES_CDH_HAS_LOGGING
EXTRA_DEFINES += -DAES_CDH_LOGGING_HAS_SOURCE_INFO
EXTRA_DEFINES += -DAES_CDH_LOGGING_USE_PRETTY_FUNCTION

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) $(EXTRA_DEFINES) -DACE_NTRACE 

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES +=  -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC) -I$(SEC_SDK_INC)
CINCLUDES +=  -I$(INCDIR) -I$(APIINCINTDIR) -I$(APIINCEXTDIR) -I$(LIB_SSH2_PATH) -I$(LIB_CURL_PATH) -I$(SEC_SDK_INC)/sec

## # here you can add own Include paths and/or other includes 
LDFLAGS      += 
LIBSDIR += -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(SEC_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)
LIBSDIR += -L$(LIBCDHEXT) -L$(LIB_SSH2_SDK_LIB) -L$(AP_SDK_LIB) -L$(LIB_CURL_SDK_LIB) -L$(LOTC_SDK_PATH)/lib64

## # here you can add own libs 
#LIBS += -lrt -lpthread -lacs_tra -lacs_csapi -laes_gcc -laes_dbo -lacs_aeh -lacs_apgcc -laes_cdh -lssh2 -lcurl -lidn -lnsl -ltirpc -lcap
LIBS += -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf
LIBS += -lsec_crypto_api -lboost_serialization -lboost_filesystem  -lboost_system -lboost_thread
LIBS += -lrt -lpthread -lacs_tra -lacs_csapi -laes_gcc -laes_dbo -lacs_aeh -lacs_apgcc -laes_cdh -lssh2 -lcurl -lnsl -lcap -lcrypto -lacs_dsd -lacs_prc /lib64/libtirpc.so.3 -lidn#-ltirpc

#/app/APG43L/SDK/LDE/4_0/lib64/libtirpc.so.1.0.10
## here you can add own File paths
VPATH +=	$(SRCDIR) $(OBJDIR) $(OUTDIR) 

## Source files
SRCFILES = blockpairmappermanager.cpp blockpairmapper.cpp blockpairindex.cpp AES_CDH_Contactor.cpp aes_cdh_block_dest_handler.cpp aes_cdh_block_destset_handler.cpp  aes_cdh_rpc_clnt.c aes_cdh_rpc_xdr.c aes_cdh_fileinitdestset_handler.cpp aes_cdh_filerespdestset_handler.cpp alarmhandler.cpp cdhcriticalsection.cpp cmdhandler.cpp database.cpp destinationalarm.cpp event.cpp ftprv2keep.cpp ftpv2agent.cpp messageblock.cpp notificationprotocol.cpp notificationrecord.cpp readypoll.cpp recordhandler.cpp resendnotification.cpp transferagent.cpp transdestset.cpp transdest.cpp transactionrules.cpp sshftprv2.cpp ftpiv2.cpp servr.cpp notification.cpp socketconnection.cpp aes_cdh_service.cpp aes_cdh_Linftpiv2.cpp sshftpiv2.cpp aes_cdh_ha_service.cpp aes_cdh_sftp.cpp aes_cdh_file_init_dest_handler.cpp aes_cdh_file_resp_dest_handler.cpp aes_cdh_daemon.cpp cdhrpc.cpp bgwrpc.cpp aes_cdh_common.cpp aes_cdh_logger.cpp

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files

OBJ = \
	$(SRCFILES:%.cpp=%.obj) \
	$(SRCFILES:%.c=%.obj) 

## Build instructions
#
APNAME  = aes_cdhd
APNAME_DBG  = aes_cdhd_dbg

APEXE = $(OUTDIR)/$(APNAME)

OBJFILES = $(OBJDIR)/blockpairmappermanager.obj $(OBJDIR)/blockpairmapper.obj $(OBJDIR)/blockpairindex.obj $(OBJDIR)/AES_CDH_Contactor.obj $(OBJDIR)/aes_cdh_block_dest_handler.obj $(OBJDIR)/aes_cdh_block_destset_handler.obj $(OBJDIR)/aes_cdh_rpc_clnt.obj $(OBJDIR)/aes_cdh_rpc_xdr.obj $(OBJDIR)/aes_cdh_fileinitdestset_handler.obj $(OBJDIR)/aes_cdh_filerespdestset_handler.obj $(OBJDIR)/alarmhandler.obj $(OBJDIR)/cdhcriticalsection.obj $(OBJDIR)/cmdhandler.obj $(OBJDIR)/database.obj $(OBJDIR)/destinationalarm.obj $(OBJDIR)/event.obj $(OBJDIR)/ftprv2keep.obj $(OBJDIR)/ftpv2agent.obj $(OBJDIR)/messageblock.obj $(OBJDIR)/notificationprotocol.obj $(OBJDIR)/notificationrecord.obj $(OBJDIR)/readypoll.obj $(OBJDIR)/recordhandler.obj  $(OBJDIR)/resendnotification.obj $(OBJDIR)/transferagent.obj $(OBJDIR)/transdestset.obj $(OBJDIR)/transdest.obj $(OBJDIR)/transactionrules.obj $(OBJDIR)/sshftprv2.obj $(OBJDIR)/ftpiv2.obj $(OBJDIR)/servr.obj $(OBJDIR)/notification.obj $(OBJDIR)/socketconnection.obj $(OBJDIR)/aes_cdh_service.obj $(OBJDIR)/aes_cdh_Linftpiv2.obj $(OBJDIR)/sshftpiv2.obj $(OBJDIR)/aes_cdh_ha_service.obj $(OBJDIR)/aes_cdh_sftp.obj $(OBJDIR)/aes_cdh_file_init_dest_handler.obj $(OBJDIR)/aes_cdh_file_resp_dest_handler.obj $(OBJDIR)/aes_cdh_daemon.obj $(OBJDIR)/cdhrpc.obj $(OBJDIR)/bgwrpc.obj $(OBJDIR)/aes_cdh_common.obj $(OBJDIR)/aes_cdh_logger.obj 

DEPS := $(OBJFILES:.obj=.d) \

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: all

all: $(APEXE) 

$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: CDH Linker'
	    $(CC) -o $(APEXE) $(OBJFILES) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: clean
clean:
		$(SILENT)$(ECHO) 'Cleaning object files '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(APEXE)
		$(SILENT)$(RM) $(APEXE)
		$(SILENT)$(RM) $(OUTDIR)/.debug/$(APNAME_DBG)

	
