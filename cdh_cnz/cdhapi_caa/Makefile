# **********************************************************************
#
# Short description:
# Makefile template for CDH library
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
# PA1        2011-09-06   xchemad   Created
#                                                            
# **********************************************************************

CAA_NR = 1090870

CNZ_NAME ?= cdh_cnz
CAA_NAME ?= cdhapi_caa
CXC_NAME ?= cdh_cxc

# Basic paths
VOB_PATH = $(AES_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(COMMON_ROOT)/common.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

CAADIR = cdhapi_caa
#CXC_PATH = $(CNZ_PATH)/cdh_cxc
#CURDIR = $(CNZ_PATH)/$(CAADIR)

BLOCKDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin/lib_ext
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INTINCDIR   = $(CURDIR)/inc_int
EXTINCDIR   = $(CURDIR)/inc_ext

LIB_NAME = aes_cdh

LIB_RELEASE_MAJOR ?= 1
LIB_RELEASE_MINOR ?= 5
LIB_RELEASE_BUILD ?= 1
LIB_VERSION ?= $(LIB_RELEASE_MAJOR).$(LIB_RELEASE_MINOR).$(LIB_RELEASE_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_RELEASE_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

INCCDH= ../cdhadm_caa/inc

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)
CINCLUDES += -I$(INTINCDIR)  -I$(EXTINCDIR) -I$(INCCDH) 

# here you can add own lib paths
LIBSDIR  += -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB)
LIBSDIR  += -L$(AP_SDK_LIB) 

# trace enabled
EXTRA_DEFINES += -DAES_CDH_API_HAS_TRACE
EXTRA_DEFINES += -DAES_CDH_TRACE_USE_SOURCE_INFO
EXTRA_DEFINES += -DAES_CDH_TRACE_HAS_FUNCTION_TRACE
EXTRA_DEFINES += -DAES_CDH_TRACE_USE_PRETTY_FUNCTION

# here you can add own compiler flags
CFLAGS += $(EXTRA_DEFINES)
CFLAGS += -g

# here you can add own Assembler flags
ASMFLAGS += 

LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)

## # here you can add own libs 
LIBS +=  -lrt -lpthread -lacs_tra -laes_gcc -lacs_apgcc 

## here you can add own File paths
VPATH +=$(SRCDIR) $(OUTDIR) $(OBJDIR) $(INTINCDIR) $(EXTINCDIR)

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

SRCFILES = aes_cdh_asynchreceiver.cpp aes_cdh_cmdclient.cpp aes_cdh_destination_r1.cpp aes_cdh_destinationimplementation.cpp aes_cdh_destinationset_r1.cpp aes_cdh_destinationsetimplementation.cpp aes_cdh_result_r1.cpp aes_cdh_resultimplementation.cpp aes_cdh_tracer.cpp

OBJFILES = $(OBJDIR)/aes_cdh_asynchreceiver.obj $(OBJDIR)/aes_cdh_cmdclient.obj $(OBJDIR)/aes_cdh_destination_r1.obj $(OBJDIR)/aes_cdh_destinationimplementation.obj $(OBJDIR)/aes_cdh_destinationset_r1.obj $(OBJDIR)/aes_cdh_destinationsetimplementation.obj $(OBJDIR)/aes_cdh_result_r1.obj $(OBJDIR)/aes_cdh_resultimplementation.obj $(OBJDIR)/aes_cdh_tracer.obj

DEPS := $(OBJFILES:.obj=.d)
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

## Object files
#
OBJS =	\
	$(SRCFILES:%.cpp=%.obj) 

## Build instructions
#

CDH_APLIB = $(OUTDIR)/$(LIB_FILENAME)
CDH_APLIB_SO = $(OUTDIR)/$(LIB_LINKER_NAME)

.PHONY: all
 
all: $(CDH_APLIB)

$(CDH_APLIB): $(OBJS) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    $(SILENT)$(ECHO) 'Creating library: $@'
		$(CC) -o $(CDH_APLIB) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS) 
	    #$(call stripp,$(LIB_FILENAME))
	    $(SEPARATOR_STR) 
	    $(NEW_LINE)

.PHONY: clean 
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

.PHONY: distclean 
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(CDH_APLIB)'
		$(RM) $(CDH_APLIB)

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	$(CP) $(OUTDIR)/$(LIB_FILENAME) $(AP_SDK_LIB)/.
	$(call ct_mkelem,$(AP_SDK_LIB)/$(LIB_FILENAME))
	$(call ct_ln_ap_sdk_lib_link,$(LIB_FILENAME),$(LIB_SONAME))
	$(call ct_ln_ap_sdk_lib_link,$(LIB_SONAME),$(LIB_LINKER_NAME))

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

# DO NOT DELETE THIS LINE -- make depend needs it

