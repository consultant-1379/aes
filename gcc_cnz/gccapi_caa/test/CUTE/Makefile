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

BLOCKDIR = $(AES_ROOT)/gcc_cnz/gccapi_caa/
OUTDIR   = $(CURDIR)/bin
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc_int
TESTDIR  = $(BLOCKDIR)/test/CUTE
NTACS = $(AES_ROOT)
CUTESRCDIR=$(CURDIR)/src

# here you can add own lib paths
LIBSDIR  += -L$(COREMW_SDK_LIB)  -L/usr/lib64 

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -fprofile-arcs -ftest-coverage

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(LOTC_SDK_INC) -I$(LOTC_SDK_INC)/c++/4.3 -I$(LOTC_SDK_INC)/c++/4.3/x86_64-suse-linux -I$(COREMW_SDK_INC)  -I$(INCDIR) -I$(TESTDIR)/cute \
-I$(COREMW_SDK_INC) -I/home1/SDK  -I$(AES_ROOT)/gcc_cnz/gccapi_caa/inc_int \

## # here you can add own libs 
#LIBS += -lrt -lpthread -lxml2 -lACE -lcurl 
LDFLAGS 	= -L. -L$(ACE_SDK_PATH)/lib -L$(LOTC_SDK_LIB) -lgcov
LIBSDIR += -L/usr/lib64/ -L$(LIB_LOGCPLUS_SDK_LIB)  -L$(AP_SDK_LIB)
LIBS +=  -lxml2 -lgcov -lACE -lacs_apgcc -lacs_aeh

## here you can add own File paths
VPATH +=$(SRCDIR) $(CUTESRCDIR) 

## Source files

L_FILES += $(SRCFILES:%=$(SRCDIR)/src/%) 

SRCFILES =  aes_gcc_thread.cpp gcc_cute_threadImpl.cpp aes_gcc_eventhandlerimplementation.cpp aes_gcc_eventhandler_r1.cpp \
aes_gcc_eventimplementation.cpp aes_gcc_event_r1.cpp  gcc_cute_eventhandler.cpp event.cpp\
aes_gcc_variableimplementation.cpp aes_gcc_variable_r1.cpp gcc_cute_variableimplementation.cpp \
gcc_cute_message_queue.cpp aes_gcc_stream.cpp gcc_cute_stream.cpp aes_gcc_log.cpp Test.cpp\
## Object files
#
OBJS = \
	$(SRCFILES:%.cpp=%.obj)
	
## Build instructions
#
APNAME = gcc_cute_test

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

