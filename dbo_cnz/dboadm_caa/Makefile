# **********************************************************************
#
# Short description:
# Makefile template for DBO Library
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
#            2012-04-09   tcsgabh   Created
#                                                            
# **********************************************************************

CNZ_NAME ?= dbo_cnz
CAA_NAME ?= dboadm_caa
CXC_NAME ?= dbo_cxc

# Basic paths
VOB_PATH = $(AES_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

CURDIR = $(CAA_PATH)
BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
INCCMDDIR= $(CURDIR)/../dbocmd_caa/inc
NTAES = $(AES_ROOT)

INCCDHEXT= $(NTAES)/cdh_cnz/cdhapi_caa/inc_ext
INCCDHINT= $(NTAES)/cdh_cnz/cdhapi_caa/inc_int
INCGCCEXT= $(NTAES)/gcc_cnz/gccapi_caa/inc_ext
INCOHIEXT= $(NTAES)/ohi_cnz/ohiapi_caa/inc_ext
INCOHIINT= $(NTAES)/ohi_cnz/ohiapi_caa/inc_int
INCDBOEXT= $(NTAES)/dbo_cnz/dboapi_caa/inc_ext
INCDBOINT= $(NTAES)/dbo_cnz/dboapi_caa/inc_int
INCGCCEXT= $(NTAES)/gcc_cnz/gccapi_caa/inc_ext
LIBGCCEXT= $(NTAES)/gcc_cnz/gcc_cxc/bin/lib_ext
LIBDBOEXT= $(NTAES)/dbo_cnz/dbo_cxc/bin/lib_ext
LIBCDHEXT= $(NTAES)/cdh_cnz/cdh_cxc/bin/lib_ext

# here you can add own compiler flags
#CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE  

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

#CINCLUDES +=  -I$(INCDIR) -I$(INCCMDDIR) -I$(INCGCCEXT) -I$(INCCDHEXT) -I$(INCCDHINT)-I$(INCOHIEXT) -I$(INCOHIINT) -I$(INCGCCEXT) -I$(INCDBOEXT) -I$(INCDBOINT) -I$(LIB_SSH2_PATH) -I$(LIB_CURL_PATH)
CINCLUDES +=  -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC) -I$(BOOST_SDK_INC) 
CINCLUDES +=  -I$(INCDIR) -I$(INCCMDDIR) -I$(INCCDHINT) -I$(INCOHIINT) -I$(INCDBOINT) -I$(LIB_SSH2_PATH) -I$(LIB_CURL_PATH)

## # here you can add own Include paths and/or other includes 
LDFLAGS      =  -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64

#LIBSDIR += -L$(LIBGCCEXT) -L$(LIBDBOEXT)  -L$(LIBCDHEXT) -L$(LIB_SSH2_SDK_LIB)  -L$(LIB_LOGCPLUS_SDK_LIB) -L$(AP_SDK_LIB) -L$(LIB_CURL_SDK_LIB) -L"$(BOOST_SDK_LIB)"
LIBSDIR += -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(SEC_SDK_LIB)
LIBSDIR += -L$(AES_ROOT)/cdh_cnz/cdh_cxc/bin/lib_ext/  #TO BE REMOVED
LIBSDIR += -L$(LIB_SSH2_SDK_LIB)  -L$(LIB_LOGCPLUS_SDK_LIB) -L$(AP_SDK_LIB) -L$(LIB_CURL_SDK_LIB) -L"$(BOOST_SDK_LIB)"
 

## # here you can add own libs  #-lssh2 -lcurl -lidn
LIBS += -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf
LIBS += -lboost_serialization -lboost_filesystem  -lboost_system -lboost_thread -lsec_crypto_api
LIBS += -lrt -lpthread -llog4cplus -lacs_tra -laes_gcc -lacs_aeh -lacs_apgcc -laes_dbo -laes_cdh -lboost_filesystem -lcap -lacs_dsd -lacs_csapi -lacs_prc

## here you can add own File paths
VPATH +=$(SRCDIR) $(OBJDIR) $(OUTDIR) $(INCDIR) $(CURDIR)/../dbocmd_caa/src

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Source files
SRCFILES = aes_dbo_daemon.cpp \
aes_dbo_server.cpp \
aes_dbo_alarmdatabase.cpp \
aes_dbo_clientsession.cpp \
aes_dbo_destinationset.cpp \
aes_dbo_mirrormaintainer.cpp \
aes_dbo_mirrorqueue.cpp \
aes_dbo_opentqsessions.cpp \
aes_dbo_tqdatabase.cpp \
aes_dbo_tqsession.cpp \
aes_dbo_blocktransferm_handler.cpp \
aes_dbo_blocktransfer_queue_handler.cpp \
aes_dbo_ha_service.cpp \
aes_dbo_service.cpp \

OBJFILES = $(OBJDIR)/aes_dbo_daemon.obj $(OBJDIR)/aes_dbo_server.obj $(OBJDIR)/aes_dbo_alarmdatabase.obj $(OBJDIR)/aes_dbo_clientsession.obj $(OBJDIR)/aes_dbo_destinationset.obj $(OBJDIR)/aes_dbo_mirrormaintainer.obj $(OBJDIR)/aes_dbo_mirrorqueue.obj $(OBJDIR)/aes_dbo_opentqsessions.obj $(OBJDIR)/aes_dbo_tqdatabase.obj $(OBJDIR)/aes_dbo_tqsession.obj $(OBJDIR)/aes_dbo_blocktransferm_handler.obj $(OBJDIR)/aes_dbo_blocktransfer_queue_handler.obj $(OBJDIR)/aes_dbo_ha_service.obj $(OBJDIR)/aes_dbo_service.obj 

DEPS := $(OBJFILES:.obj=.d) \

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

## Object files

OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj)

## Build instructions
#

APNAME  = aes_dbod
APNAME_DBG  = aes_dbod_dbg

APEXE = $(OUTDIR)/$(APNAME)
.PHONY: all

all: $(APEXE) 

$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: DBO Linker'
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

.PHONY: metrics
metrics : cccc lint

.PHONY: cccc
cccc:
	$(NEW_LINE)
	$(SEPARATOR_STR)
        $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*)
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*)
	$(SEPARATOR_STR)

lint:
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'LINT file: $@'
#               $(LINT) $(wildcard $(BLOCKDIR)/inc/*)
		$(LINT) $(wildcard $(BLOCKDIR)/src/*)
	$(SEPARATOR_STR)

	
.PHONY: depend
depend: $(SRCFILES)
	makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

