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
#            2010-02-19   teiggam   Created
#                                                            
# **********************************************************************
CURDIR = $(shell pwd)
REPO_NAME = aes
AES_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(AES_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

BLOCKDIR = $(AES_ROOT)/ohi_cnz/ohiapi_caa
OUTDIR   = $(CURDIR)/bin
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCINTDIR   = $(BLOCKDIR)/inc_int
INCEXTDIR   = $(BLOCKDIR)/inc_ext
TESTDIR  = $(BLOCKDIR)/test/ohi_cute
NTACS = $(AES_ROOT)
CUTESRCDIR=$(CURDIR)/src

# here you can add own lib paths
LIBSDIR  += -L$(COREMW_SDK_LIB)  -L/usr/lib64 

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -fprofile-arcs -ftest-coverage

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(LOTC_SDK_INC) -I$(LOTC_SDK_INC)/c++/4.3 -I$(LOTC_SDK_INC)/c++/4.3/x86_64-suse-linux -I$(COREMW_SDK_INC)  -I$(INCDIR) -I$(TESTDIR)/cute -I$(COREMW_SDK_INC) -I/home1/SDK  -I$(INCINTDIR) -I$(INCEXTDIR) -I$(AES_ROOT)/gcc_cnz/gccapi_caa/inc_int

## # here you can add own libs 
LDFLAGS 	= -L. -L$(ACE_SDK_PATH)/lib -L$(LOTC_SDK_LIB) -lgcov

LIBSDIR += -L/usr/lib64/ -L$(LIB_LOGCPLUS_SDK_LIB)  -L$(AP_SDK_LIB) -L$(AES_ROOT)/ohi_cnz/ohi_cxc/bin/lib_ext -L$(AES_ROOT)/gcc_cnz/gcc_cxc/bin/lib_int

LIBS +=  -lxml2 -lgcov -lACE -laes_ohi -laes_gcc

## here you can add own File paths
VPATH +=$(SRCDIR) $(CUTESRCDIR) 

## Source files

L_FILES += $(SRCFILES:%=$(SRCDIR)/src/%) 

SRCFILES = aes_ohi_filehandler_r2.cpp ohi_cute_filehandler.cpp aes_ohi_blockhandler2_r1.cpp ohi_cute_blockhandler.cpp aes_ohi_directoryhandler_r1.cpp ohi_cute_directoryhandler.cpp aes_ohi_extfilehandler2_r1.cpp ohi_cute_extfilehandler.cpp Test.cpp
## Object files
#
OBJS = \
	$(SRCFILES:%.cpp=%.obj)
	
## Build instructions
#
APNAME = ohi_cute_test

APLIB = $(TESTDIR)/bin/$(APNAME)

#.PHONY: depend clean
 
all: $(APLIB)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(CUTESRCDIR)/../bin/*) $(wildcard $(OBJDIR)/*)
#		$(SILENT)$(RM) $(OBJDIR)/*.o*

$(APLIB): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating CUTE appl: $@'
		$(SILENT)$(RM) $(APLIB)
		$(CC) -o $(APLIB) $(LIBSDIR) $(LIBS) $(LDFLAGS) $(wildcard $(OBJDIR)/*.obj) $(CFLAGS)  
		$(SEPARATOR_STR)
	    $(NEW_LINE)

metrics : cccc lint

cccc:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
#		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*) 
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

lint:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'LINT file: $@'
#		$(LINT) $(wildcard $(BLOCKDIR)/inc/*) 
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

