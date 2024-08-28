# **********************************************************************
#
# Short description:
# Makefile template for OHI library
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
# PA1        2011-06-17   xchemad   Created
# PA2        2014-08-26   xmahban   Removed -s flag from LDFLAGS
# PB1 		 2016-01-08	  enungai	update for SLES12
#                                                            
# **********************************************************************

CAA_NR = 1090870

#include $(COMMON_MK)
CNZ_PATH = $(AES_ROOT)/ohi_cnz

CNZ_NAME ?= ohi_cnz
CAA_NAME ?= ohiapi_caa
CXC_NAME ?= ohi_cxc

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
INTAFPDIR = $(AES_ROOT)/afp_cnz/afpapi_caa/inc_int
EXTAFPDIR = $(AES_ROOT)/afp_cnz/afpapi_caa/inc_ext
EXTGCCDIR = $(AES_ROOT)/gcc_cnz/gccapi_caa/inc_ext
AFPLIBDIR = $(AES_ROOT)/afp_cnz/afp_cxc/bin/lib_ext
INTDBODIR = $(AES_ROOT)/dbo_cnz/dboapi_caa/inc_int
EXTDBODIR = $(AES_ROOT)/dbo_cnz/dboapi_caa/inc_ext
DBOLIBDIR = $(AES_ROOT)/dbo_cnz/dbo_cxc/bin/lib_ext
TESTDIR  = $(CURDIR)/test

LIB_NAME = aes_ohi

LIB_RELEASE_MAJOR ?= 1
LIB_RELEASE_MINOR ?= 6
LIB_RELEASE_BUILD ?= 1
LIB_VERSION ?= $(LIB_RELEASE_MAJOR).$(LIB_RELEASE_MINOR).$(LIB_RELEASE_BUILD)
LIB_FILENAME = lib$(LIB_NAME).so.$(LIB_VERSION)
LIB_FILENAME_DBG = lib$(LIB_NAME).so.$(LIB_VERSION)_dbg
LIB_SONAME = lib$(LIB_NAME).so.$(LIB_RELEASE_MAJOR)
LIB_LINKER_NAME = lib$(LIB_NAME).so

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INTINCDIR)  -I$(EXTINCDIR) -I$(EXTGCCDIR) -I$(INTAFPDIR) -I$(EXTAFPDIR) -I$(EXTDBODIR) -I$(INTDBODIR)

# here you can add own lib paths
LIBSDIR  += -L$(EXTGCCDIR) -L$(AFPLIBDIR) -L$(DBOLIBDIR) -L$(AP_SDK_LIB)

# here you can add own compiler flags
#CFLAGS += -O3 -fPIC -fmessage-length=0
#CFLAGS += -g

# here you can add own Assembler flags
ASMFLAGS += 

LDFLAGS += -s -shared -Wl,-soname=$(LIB_SONAME)

## # here you can add own libs 
LIBS +=  -lrt -lpthread -lacs_tra -laes_afp -laes_dbo

#ACE SDK
LIBSDIR += -L$(ACE_SDK_LIB)
LIBS += -lACE
CINCLUDES += -I$(ACE_SDK_INC)

## here you can add own File paths
VPATH +=$(SRCDIR) $(OUTDIR) $(OBJDIR) $(INTINCDIR) $(EXTINCDIR)

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

SRCFILES = aes_ohi_blockhandler2_r1.cpp aes_ohi_blockhandlerimplementation.cpp \
aes_ohi_filehandler_r1.cpp aes_ohi_filehandler_r2.cpp  aes_ohi_filehandlerimplementation.cpp ohiafpcomm.cpp ohiextafpcomm.cpp aes_ohi_directoryhandler_r1.cpp \
aes_ohi_extfilehandler2_r1.cpp aes_ohi_extfilehandlerimplementation2.cpp \

OBJFILES = $(OBJDIR)/aes_ohi_blockhandler2_r1.obj $(OBJDIR)/aes_ohi_blockhandlerimplementation.obj \
$(OBJDIR)/aes_ohi_filehandler_r2.obj \
$(OBJDIR)/aes_ohi_filehandlerimplementation.obj $(OBJDIR)/ohiafpcomm.obj $(OBJDIR)/ohiextafpcomm.obj $(OBJDIR)/aes_ohi_filehandler_r1.obj $(OBJDIR)/aes_ohi_directoryhandler_r1.obj \
$(OBJDIR)/aes_ohi_extfilehandler2_r1.obj $(OBJDIR)/aes_ohi_extfilehandlerimplementation2.obj \

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

OHI_APLIB = $(OUTDIR)/$(LIB_FILENAME)
OHI_APLIB_SO = $(OUTDIR)/$(LIB_LINKER_NAME)

.PHONY: all
 
all: $(OHI_APLIB)

$(OHI_APLIB): $(OBJS) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    $(SILENT)$(ECHO) 'Creating library: $@'
		$(CC) -o $(OHI_APLIB) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS) 
	    $(call stripp,$(LIB_FILENAME))
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: clean 
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

.PHONY: distclean 
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(OHI_APLIB)'
		$(RM) $(OHI_APLIB)
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

