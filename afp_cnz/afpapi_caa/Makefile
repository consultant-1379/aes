# **********************************************************************
#
# Short description:
# Makefile template for XXX library
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
#            2010-07-20   tcstanu   Created
#                                                            
# **********************************************************************

#CAA_NR = 

CNZ_NAME ?= afp_cnz
CAA_NAME ?= afpapi_caa
CXC_NAME ?= afp_cxc

# Basic paths
VOB_PATH = $(AES_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

CURDIR = $(CAA_PATH)
OUTDIR = $(CXC_PATH)/bin/lib_ext
OBJDIR = $(CURDIR)/obj
SRCDIR = $(CURDIR)/src
INTINCDIR = $(CURDIR)/inc_int
EXTINCDIR = $(CURDIR)/inc_ext
ADMINCDIR = $(CURDIR)/../afpadm_caa/inc
GCCEXTDIR = $(CURDIR)/../../gcc_cnz/gccapi_caa/inc_ext
TESTDIR = $(CURDIR)/test

AES_AFP_LIB_NAME = aes_afp

AES_AFP_LIB_RELEASE_MAJOR ?= 1
AES_AFP_LIB_RELEASE_MINOR ?= 10
AES_AFP_LIB_RELEASE_BUILD ?= 2
AES_AFP_LIB_VERSION ?= $(AES_AFP_LIB_RELEASE_MAJOR).$(AES_AFP_LIB_RELEASE_MINOR).$(AES_AFP_LIB_RELEASE_BUILD)
AES_AFP_LIB_FILENAME = lib$(AES_AFP_LIB_NAME).so.$(AES_AFP_LIB_VERSION)
AES_AFP_LIB_FILENAME_DBG = lib$(AES_AFP_LIB_NAME).so.$(AES_AFP_LIB_VERSION)_dbg
AES_AFP_LIB_SONAME = lib$(AES_AFP_LIB_NAME).so.$(AES_AFP_LIB_RELEASE_MAJOR)
AES_AFP_LIB_LINKER_NAME = lib$(AES_AFP_LIB_NAME).so

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)
CINCLUDES += -I$(GCCEXTDIR) -I$(EXTINCDIR) -I$(INTINCDIR)  -I$(ADMINCDIR)

# here you can add own lib paths
LIBSDIR  += -L$(AES_ROOT)/gcc_cnz/gcc_cxc/bin/lib_ext/     # TO BE REMOVED
LIBSDIR  += -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) 
LIBSDIR  += -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)

# here you can add own compiler flags
CFLAGS += -fPIC -fmessage-length=0
#CFLAGS += -O3
CFLAGS += -g -Wall
CFLAGS += $(EXTRA_DEFINES)

# here you can add more defines
EXTRA_DEFINES += -DAES_AFP_API_HAS_TRACE
EXTRA_DEFINES += -DAES_AFP_TRACE_USE_SOURCE_INFO
EXTRA_DEFINES += -DAES_AFP_TRACE_HAS_FUNCTION_TRACE
#EXTRA_DEFINES += -DAES_AFP_TRACE_USE_PRETTY_FUNCTION

# here you can add own Assembler flags
ASMFLAGS += 

LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)

## # here you can add own libs 
LIBS +=  -lrt -lpthread 

AES_AFP_LIBS = $(LIBS) -lacs_tra -lacs_apgcc -laes_gcc   

ADMSRCDIR = ../afpadm_caa/src

## here you can add own File paths
VPATH +=$(SRCDIR) $(OUTDIR) $(OBJDIR) $(ADMSRCDIR)

## Source files
#

L_FILES += $(AES_AFPOBJ:%=$(BLOCKDIR)/src/%) 

AES_AFPSRC = aes_afp_api.cpp aes_afp_apiinternal.cpp aes_afp_apimsg.cpp \
            aes_afp_apitask.cpp aes_afp_protocol.cpp ../afpadm_caa/src/aes_afp_events.cpp aes_afp_api_tracer.cpp \
            aes_afp_defines.cpp \
 
## Object files                 
AES_AFPOBJFILES = $(OBJDIR)/*.obj

## Object files
AES_AFPOBJ = \
	$(AES_AFPSRC:%.cpp=%.obj)

DEPS  = $(AES_AFPOBJ:.obj=.d)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

## Build instructions
#

AES_AFP_APLIB = $(OUTDIR)/$(AES_AFP_LIB_FILENAME)
AES_AFP_APLIB_SO = $(OUTDIR)/$(AES_AFP_LIB_SONAME)

.PHONY: clean 
.PHONY: distclean 
.PHONY: all
 
all: $(AES_AFP_APLIB)

clean:		
		$(SILENT)$(ECHO) 'Cleaning object files'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(AES_AFP_APLIB)'
		$(SILENT)$(RM) $(AES_AFP_APLIB) 

$(AES_AFP_APLIB): $(AES_AFPOBJ) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    $(SILENT)$(ECHO) 'Creating library AES AFP: $@'
		$(SILENT)$(RM) $(AES_AFP_APLIB)
		$(CC) -o $(AES_AFP_APLIB) $(AES_AFPOBJFILES) $(LDFLAGS) $(LIBSDIR) $(AES_AFP_LIBS) 
	    $(call stripp,$(AES_AFP_LIB_FILENAME))
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

	

	        

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	$(CP) $(OUTDIR)/$(AES_AFP_LIB_FILENAME) $(AP_SDK_LIB)/.
	$(call ct_mkelem,$(AP_SDK_LIB)/$(AES_AFP_LIB_FILENAME))
	$(call ct_ln_ap_sdk_lib_link,$(AES_AFP_LIB_FILENAME),$(AES_AFP_LIB_SONAME))
	$(call ct_ln_ap_sdk_lib_link,$(AES_AFP_LIB_SONAME),$(AES_AFP_LIB_LINKER_NAME))

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

