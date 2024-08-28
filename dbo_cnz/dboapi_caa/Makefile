# **********************************************************************
#
# Short description:
# Makefile template for DBO library
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
# PA1        2012-08-10   xnadnar   Created
#                                                            
# **********************************************************************

CAA_NR = 1090870

CNZ_NAME ?= dbo_cnz
CAA_NAME ?= dboapi_caa
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
OUTDIR   = $(CXC_PATH)/bin/lib_ext
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INTINCDIR   = $(CURDIR)/inc_int
EXTINCDIR   = $(CURDIR)/inc_ext

AES_DBO_LIB_NAME = aes_dbo

AES_DBO_LIB_RELEASE_MAJOR ?= 2
AES_DBO_LIB_RELEASE_MINOR ?= 2
AES_DBO_LIB_RELEASE_BUILD ?= 0
AES_DBO_LIB_VERSION ?= $(AES_DBO_LIB_RELEASE_MAJOR).$(AES_DBO_LIB_RELEASE_MINOR).$(AES_DBO_LIB_RELEASE_BUILD)
AES_DBO_LIB_FILENAME = lib$(AES_DBO_LIB_NAME).so.$(AES_DBO_LIB_VERSION)
AES_DBO_LIB_FILENAME_DBG = lib$(AES_DBO_LIB_NAME).so.$(AES_DBO_LIB_VERSION)_dbg
AES_DBO_LIB_SONAME = lib$(AES_DBO_LIB_NAME).so.$(AES_DBO_LIB_RELEASE_MAJOR)
AES_DBO_LIB_LINKER_NAME = lib$(AES_DBO_LIB_NAME).so

NTAES = $(AES_ROOT)
INCCDHEXT= $(NTAES)/cdh_cnz/cdhapi_caa/inc_ext
INCGCCEXT= $(NTAES)/gcc_cnz/gccapi_caa/inc_ext
LIBGCCEXT= $(NTAES)/gcc_cnz/gcc_cxc/bin/lib_ext
LIBCDHEXT= $(NTAES)/cdh_cnz/cdh_cxc/bin/lib_ext

## # here you can add own Include paths and/or other includes
#CINCLUDES += -I$(INTINCDIR)  -I$(EXTINCDIR)  -I$(INCCDHEXT) -I$(INCGCCEXT)
CINCLUDES += -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC) -I$(BOOST_SDK_INC) 
CINCLUDES += -I$(INTINCDIR)  -I$(EXTINCDIR) 

# here you can add own lib paths
#LIBSDIR  += -L$(LIBGCCEXT) -L$(LIBCDHEXT) -L$(AP_SDK_LIB)
#LIBSDIR += -L"$(BOOST_SDK_LIB)"

LIBSDIR += -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(BOOST_SDK_LIB) 
LIBSDIR += -L$(AP_SDK_LIB)

# here you can add own compiler flags
#CFLAGS += -g

# here you can add own Assembler flags
ASMFLAGS += 

## here you can add own compiler flags
CFLAGS += -O2 -fPIC -fmessage-length=0
#CFLAGS += $(GCOV_FLAGS)

# here you can add own linker flags
LDFLAGS += -shared -Wl,-soname=$(AES_DBO_LIB_SONAME)

## # here you can add own libs 
LIBS +=  -lrt -lpthread -lboost_thread -lacs_tra -laes_gcc -lacs_apgcc

## here you can add own File paths
VPATH +=$(SRCDIR) $(OUTDIR) $(OBJDIR) $(INTINCDIR) $(EXTINCDIR)

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

SRCFILES =AES_DBO_AsynchComm.cpp AES_DBO_TQManager.cpp AES_DBO_TQSender.cpp DataMirror.cpp ParameterHandler.cpp \
AES_DBO_DataBlock.cpp AES_DBO_TQReceiver.cpp EventHandler.cpp CmdClient.cpp \

OBJFILES = $(OBJDIR)/AES_DBO_AsynchComm.obj $(OBJDIR)/AES_DBO_TQManager.obj $(OBJDIR)/AES_DBO_TQSender.obj $(OBJDIR)/DataMirror.obj \
$(OBJDIR)/ParameterHandler.obj $(OBJDIR)/AES_DBO_DataBlock.obj $(OBJDIR)/AES_DBO_TQReceiver.obj $(OBJDIR)/CmdClient.obj $(OBJDIR)/EventHandler.obj 

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

DBO_APLIB = $(OUTDIR)/$(AES_DBO_LIB_FILENAME)
DBO_APLIB_SO = $(OUTDIR)/$(AES_DBO_LIB_LINKER_NAME)

.PHONY: all
 
all: $(DBO_APLIB)

$(DBO_APLIB): $(OBJS) 
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    $(SILENT)$(ECHO) 'Creating library AES_DBO : $@'
		$(CC) -o $(DBO_APLIB) $(OBJFILES) $(LDFLAGS) $(LIBSDIR) $(LIBS) 
	    $(SEPARATOR_STR)
		$(call stripp,$(AES_DBO_LIB_FILENAME))
	    $(NEW_LINE)

.PHONY: clean 
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

.PHONY: distclean 
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(DBO_APLIB)'
		$(RM) $(DBO_APLIB)

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	$(CP) $(OUTDIR)/$(AES_DBO_LIB_FILENAME) $(AP_SDK_LIB)/.
	$(call ct_mkelem,$(AP_SDK_LIB)/$(AES_DBO_LIB_FILENAME))
	$(call ct_ln_ap_sdk_lib_link,$(AES_DBO_LIB_FILENAME),$(AES_DBO_LIB_SONAME))
	$(call ct_ln_ap_sdk_lib_link,$(AES_DBO_LIB_SONAME),$(AES_DBO_LIB_LINKER_NAME))

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

# DO NOT DELETE THIS LINE -- make depend needs it

