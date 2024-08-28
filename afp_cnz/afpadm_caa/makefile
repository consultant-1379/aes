# **********************************************************************
#
# Short description:
# Makefile template for XXX library
# **********************************************************************
#
# Ericsson AB 2011 All rights reserved.
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
#            2011-09-07   xnadnar  Created
#                                                            
# **********************************************************************

#CXC_NR = CXC120333
CNZ_NAME ?= afp_cnz
CAA_NAME ?= afpadm_caa
CXC_NAME ?= afp_cxc

# Basic paths
VOB_PATH = $(AES_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

CURDIR   = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INCDIR   = $(CURDIR)/inc
INCCMDDIR= $(CURDIR)/../afpcmd_caa/inc

INCCDHEXT= $(CURDIR)/../../cdh_cnz/cdhapi_caa/inc_ext
INCCDHINT= $(CURDIR)/../../cdh_cnz/cdhapi_caa/inc_int
INCGCCEXT= $(CURDIR)/../../gcc_cnz/gccapi_caa/inc_ext
INCOHIEXT= $(CURDIR)/../../ohi_cnz/ohiapi_caa/inc_ext
INCOHIINT= $(CURDIR)/../../ohi_cnz/ohiapi_caa/inc_int
INCAFPEXT= $(CURDIR)/../afpapi_caa/inc_ext
INCAFPINT= $(CURDIR)/../afpapi_caa/inc_int
TESTDIR  = $(CURDIR)/test

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add more defines
# trace enabled
EXTRA_DEFINES += -DAES_AFP_API_HAS_TRACE
EXTRA_DEFINES += -DAES_AFP_TRACE_USE_SOURCE_INFO
EXTRA_DEFINES += -DAES_AFP_TRACE_HAS_FUNCTION_TRACE
EXTRA_DEFINES += -DAES_AFP_TRACE_USE_PRETTY_FUNCTION
# log enabled
EXTRA_DEFINES += -DAES_AFP_HAS_LOGGING
EXTRA_DEFINES += -DAES_AFP_LOGGING_HAS_SOURCE_INFO
EXTRA_DEFINES += -DAES_AFP_LOGGING_USE_PRETTY_FUNCTION

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE
CFLAGS += -g -Wall##CFLAGS += -O2
CFLAGS += $(EXTRA_DEFINES)	 

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

#CINCLUDES +=  -I$(INCDIR) -I$(INCCMDDIR) -I$(INCCDHEXT) -I$(INCCDHINT) -I$(INCGCCEXT) -I$(INCAFPEXT) -I$(INCAFPINT) -I$(INCOHIEXT) -I$(INCOHIINT) 
#-I$(INCAFPCMDINT)
CINCLUDES += -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC) -I$(BOOST_SDK_INC)
CINCLUDES += -I$(INCGCCEXT) -I$(INCDIR) -I$(INCCMDDIR) -I$(INCCDHINT) -I$(INCAFPEXT) -I$(INCAFPINT) -I$(INCOHIINT) 

## # here you can add own Include paths and/or other includes -- 
LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 

#LIBSDIR += -L$(AES_ROOT)/gcc_cnz/gcc_cxc/bin/lib_ext -L../afp_cxc/bin/lib_ext  -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)
LIBSDIR +=  -L$(AES_ROOT)/gcc_cnz/gcc_cxc/bin/lib_ext -L$(AES_ROOT)/cdh_cnz/cdh_cxc/bin/lib_ext/   #TO BE REMOVED
LIBSDIR +=  -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(SEC_SDK_LIB)
LIBSDIR +=  -L../afp_cxc/bin/lib_ext  -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) 
## # here you can add own libs 

LIBS += -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf
LIBS += -lpthread -lboost_serialization -lboost_filesystem  -lboost_system -lsec_crypto_api
LIBS += -lacs_apgcc -laes_gcc -lacs_tra -llog4cplus -laes_afp -laes_cdh -lcap -lacs_prc -lacs_dsd -lacs_csapi -lacs_aeh
## here you can add own File paths
VPATH +=	$(SRCDIR)  $(OBJDIR) $(OUTDIR) $(CURDIR)/../afpcmd_caa/src  

## Source files
#
SRCFILES =  aes_afp_ha_service.cpp aes_afp_command_handler.cpp aes_afp_date_time.cpp aes_afp_file.cpp aes_afp_cdhtask.cpp \
aes_afp_server.cpp aes_afp_services.cpp aes_afp_apcom_hand.cpp \
aes_afp_apcom_serv.cpp  aes_afp_criticalsection.cpp aes_afp_datablock.cpp \
aes_afp_dsdsessionmap.cpp aes_afp_events.cpp aes_afp_msgblock.cpp \
aes_afp_objectfactory.cpp aes_afp_threadmap.cpp  \
aes_afp_datatask.cpp aes_afp_destination.cpp aes_afp_database_fd.cpp \
aes_afp_parameter.cpp aes_afp_template.cpp\
aes_afp_timestamp.cpp aes_afp_transferqueue.cpp aes_afp_datatransferm_handler.cpp aes_afp_logger.cpp\

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(CURDIR)/src/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 

## Build instructions
#
APNAME = aes_afpd
APNAME_DBG = aes_afpd_dbg

APEXE = $(OUTDIR)/$(APNAME)

OBJFILES =$(OBJDIR)/*.obj

DEPS := $(OBJFILES:.obj=.d) \

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: all
all: $(APEXE)

.PHONY: clean
clean:	
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT) rm -f  $(wildcard $(OBJDIR)/*)

.PHONY: distclean
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(APNAME)'
		$(RM) $(OUTDIR)/$(APNAME)
		$(RM) $(OUTDIR)/.debug/$(APNAME_DBG)

$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(APEXE) $(OBJFILES) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: metrics
metrics : cccc lint

.PHONY: cccc
cccc:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
		$(CCCC) $(wildcard $(CURDIR)/inc/*) 
		$(CCCC) $(wildcard $(CURDIR)/src/*) 
	    $(SEPARATOR_STR)

lint:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'LINT file: $@'
#		$(LINT) $(wildcard $(CURDIR)/inc/*) 
		$(LINT) $(wildcard $(CURDIR)/src/*) 
	    $(SEPARATOR_STR)

				
	
.PHONY: depend
depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

