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
#            2011-07-13   tcschem   Created
#                                                            
# **********************************************************************

CNZ_PATH = $(AES_ROOT)/ohi_cnz
CURDIR = $(shell pwd)
REPO_NAME = aes
AES_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(AES_ROOT)/common
 
include $(COMMON_ROOT)/common.mk 

CXCDIR = ohi_cxc
CAADIR = ohiapi_caa
CURDIR = $(CNZ_PATH)/$(CAADIR)/test/BlockTestApp

BLOCKDIR = $(CURDIR)
OUTDIR   = $(CURDIR)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own lib paths
LIBSDIR  +=  -L$(CNZ_PATH)/$(CXCDIR)/bin/lib_ext  -L$(AES_ROOT)/gcc_cnz/gcc_cxc/bin/lib_ext -L$(AES_ROOT)/dbo_cnz/dbo_cxc/bin/lib_ext -L$(LIB_LOGCPLUS_SDK_LIB)  -L$(AP_SDK_LIB)

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -g
 

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(ACE_SDK_PATH)  -I$(LOTC_SDK_INC) -I$(LOTC_SDK_INC)/c++/4.3 -I$(LOTC_SDK_INC)/c++/4.3/x86_64-suse-linux -I$(COREMW_SDK_INC) -I$(AES_ROOT)/ohi_cnz/ohiapi_caa/inc_ext -I$(AES_ROOT)/dbo_cnz/dboapi_caa/inc_ext  -I$(AP_SDK_INC)

LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTC_SDK_LIB) -L/usr/lib 

## # here you can add own libs 

LIBS +=  -lacs_tra -laes_ohi -laes_gcc -laes_dbo

## here you can add own File paths
VPATH += 	$(SRCDIR) $(OUTDIR) $(OBJDIR)

## Source files
#

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
SRCFILES = BlockTest.cpp 

OBJS = \
	 $(SRCFILES:%.cpp=%.obj)

$(OBJDIR)/%.obj : $(SRCFILES)
	$(ECHO) 'Compiling SRCFILES'
	$(CC) $(CFLAGS) $(CINCLUDES) $(APGINCLUDE) $*.cpp -o $@

## Build instructions
#
APNAME = ohi_block_test

APEXE = $(OUTDIR)/$(APNAME)

#.PHONY: depend clean
 
.PHONY: all
all: $(APEXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $(APEXE)'
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

.PHONY: distclean
distclean:		
		$(SILENT)$(ECHO) 'Cleaning binary file: $(APEXE)'
		$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) 
		
		

$(APEXE): $(OBJS) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'Creating OHI Block Test Application: $@'
		$(SILENT)$(RM) $(APEXE)
		$(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*) $(LIBSDIR) $(LIBS)
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

METRICS : cccc lint

cccc:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'C/C++ Code Counter file: $@'
		$(CCCC) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

lint:
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
	    $(SILENT)$(ECHO) 'LINT file: $@'
		$(LINT) $(wildcard $(BLOCKDIR)/src/*) 
	    $(SEPARATOR_STR)

depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

