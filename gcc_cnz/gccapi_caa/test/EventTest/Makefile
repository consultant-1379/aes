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
#            2011-06-11   xnadnar   Created
#                                                            
# **********************************************************************

#CXC_NR = CXC120333

CURDIR = $(shell pwd)
REPO_NAME = aes
AES_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(AES_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

BLOCKDIR = $(CURDIR)
OUTDIR   = $(BLOCKDIR)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own compiler flags

CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE  -g 
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

CINCLUDES +=  -I$(INCDIR) -I$(AES_ROOT)/gcc_cnz/gccapi_caa/inc_int

## # here you can add own Include paths and/or other includes -- Modified by SirishKumar
LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTC_SDK_LIB)

LIBSDIR +=  -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB) -L$(AES_ROOT)/gcc_cnz/gcc_cxc/bin/lib_int/
## # here you can add own libs 

LIBS += -lacs_tra -lacs_aeh -laes_gcc
## here you can add own File paths
VPATH +=	$(SRCDIR)

## Source files
#
SRCFILES = aes_gcc_event_test.cpp
IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 

## Build instructions
#
APNAME = aes_gcc_event_test

APEXE = $(OUTDIR)/$(APNAME)

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

$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
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
		$(CCCC) $(wildcard $(BLOCKDIR)/inc/*) 
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

lint:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'LINT file: $@'
#		$(LINT) $(wildcard $(BLOCKDIR)/inc/*) 
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

				
	
.PHONY: depend
depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

