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
# PA1        2011-06-16   xnadnar   Created
# PA2	     2014-09-01	  xmahban   Removed -s option from LDFLAGS
#                                                            
# **********************************************************************

CAA_NR = 1090870

#include $(COMMON_MK)
CNZ_NAME ?= gcc_cnz
CAA_NAME ?= gccapi_caa
CXC_NAME ?= gcc_cxc

# Basic paths
VOB_PATH = $(AES_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

CURDIR = $(CAA_PATH)
OUTDIR   = $(CXC_PATH)/bin/lib_ext
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INTINCDIR   = $(CURDIR)/inc_int
EXTINCDIR   = $(CURDIR)/inc_ext
TESTDIR  = $(CURDIR)/test

LIB_NAME = aes_gcc

LIB_RELEASE_MAJOR ?= 2
LIB_RELEASE_MINOR ?= 6
LIB_RELEASE_BUILD ?= 0
LIB_VERSION ?= $(LIB_RELEASE_MAJOR).$(LIB_RELEASE_MINOR).$(LIB_RELEASE_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_RELEASE_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INTINCDIR)  -I$(EXTINCDIR)
CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)

# here you can add own lib paths
LIBSDIR  +=  -L$(AP_SDK_LIB)
LIBSDIR  +=  -L$(ACE_SDK_LIB)
LIBSDIR  +=  -L$(COREMW_SDK_LIB)
LIBSDIR  +=  -L$(SEC_CREDU_SDK_LIB)

# here you can add own compiler flags
#CFLAGS += -O3 -fPIC -fmessage-length=0
CFLAGS += -O2 -fPIC -fmessage-length=0

# here you can add own Assembler flags
ASMFLAGS += 

LDFLAGS += -shared -Wl,-soname=$(LIB_SONAME)

## # here you can add own libs 
LIBS +=  -lrt -lpthread -lACE -lacs_tra -lacs_aeh

## here you can add own File paths
VPATH +=$(SRCDIR) $(OUTDIR) $(OBJDIR) $(INTINCDIR) $(EXTINCDIR)

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

SRCFILES = aes_gcc_stream.cpp aes_gcc_log.cpp aes_gcc_tracer.cpp\
aes_gcc_variableimplementation.cpp aes_gcc_variable_r1.cpp \
aes_gcc_eventhandlerimplementation.cpp aes_gcc_eventhandler_r1.cpp \
event.cpp aes_gcc_eventimplementation.cpp aes_gcc_event_r1.cpp aes_gcc_thread.cpp \
aes_gcc_util.cpp 

OBJFILES =$(OBJDIR)/aes_gcc_stream.obj  $(OBJDIR)/aes_gcc_log.obj \
$(OBJDIR)/aes_gcc_tracer.obj \
$(OBJDIR)/aes_gcc_eventhandlerimplementation.obj $(OBJDIR)/aes_gcc_thread.obj\
$(OBJDIR)/aes_gcc_eventhandler_r1.obj $(OBJDIR)/aes_gcc_eventimplementation.obj \
$(OBJDIR)/aes_gcc_event_r1.obj $(OBJDIR)/aes_gcc_variableimplementation.obj \
$(OBJDIR)/aes_gcc_variable_r1.obj  $(OBJDIR)/event.obj  \
$(OBJDIR)/aes_gcc_util.obj 

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

GCC_APLIB = $(OUTDIR)/$(LIB_FILENAME)
GCC_APLIB_SO = $(OUTDIR)/$(LIB_LINKER_NAME)

.PHONY: all
 
all: $(GCC_APLIB)

$(GCC_APLIB): $(OBJS) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    $(SILENT)$(ECHO) 'Creating library: $@'
		$(CC) -o $(GCC_APLIB) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS) 
	    $(call stripp,$(LIB_FILENAME))
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: clean 
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

.PHONY: distclean 
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(GCC_APLIB)'
		$(RM) $(GCC_APLIB)
		$(RM) $(OUTDIR)/.debug/$(LIB_FILENAME_DBG)

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

