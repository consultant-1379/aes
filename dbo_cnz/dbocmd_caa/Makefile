# **********************************************************************
#
# Short description:
# Makefile template for DBO Library
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
#            2011-09-06   XCHEMAD   Created
#                                                            
# **********************************************************************

CNZ_NAME ?= dbo_cnz
CAA_NAME ?= dbocmd_caa
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
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
APIINCINTDIR = $(CURDIR)/../dboapi_caa/inc_int
APIINCEXTDIR = $(CURDIR)/../dboapi_caa/inc_ext

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE  -g

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

CINCLUDES +=  -I$(INCDIR) -I$(AES_ROOT)/gcc_cnz/gccapi_caa/inc_ext -I$(APIINCINTDIR) -I$(APIINCEXTDIR)

## # here you can add own Include paths and/or other includes 
LDFLAGS      += 

LIBSDIR += -L$(LIB_LOGCPLUS_SDK_LIB) -L$(AES_ROOT)/gcc_cnz/gcc_cxc/bin/lib_ext -L$(AES_ROOT)/dbo_cnz/dbo_cxc/bin/lib_ext -L$(AP_SDK_LIB)

## # here you can add own libs 
LIBS += -lrt -lpthread -llog4cplus -lacs_tra -laes_gcc -lacs_aeh -lacs_apgcc -laes_dbo

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OBJDIR) $(OUTDIR) 

## Source files
DBOLSSRCFILES = DBOls.cpp command.cpp

#DBODSLSSRCFILES = dbodsls.cpp

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files

DBOLSOBJ = \
	$(DBOLSSRCFILES:%.cpp=%.obj) \
	$(SRCFILES:%.c=%.obj) 

#DBODSLSOBJ = \
#	$(DBODSLSSRCFILES:%.cpp=%.obj) \
#	$(SRCFILES:%.c=%.obj) 

## Build instructions
#
DBOLSNAME  = dbols
#DBODSLSNAME  = dbodsls

DBOLSEXE = $(OUTDIR)/$(DBOLSNAME)
#DBODSLSEXE = $(OUTDIR)/$(DBODSLSNAME)

DBOLSOBJFILES = $(OBJDIR)/DBOls.obj $(OBJDIR)/command.obj

#DBODSLSOBJFILES = $(OBJDIR)/dbodsls.obj

DEPS := $(OBJFILES:.obj=.d) \

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: all

all: $(DBOLSEXE) $(DBODSLSEXE) 

$(DBOLSEXE): $(DBOLSOBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: DBO Linker'
	    $(CC) -o $(DBOLSEXE) $(DBOLSOBJFILES) $(LIBSDIR) $(LIBS)
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

#$(DBODSLSEXE): $(DBODSLSOBJ) $(METRICS)
#	    $(NEW_LINE)
#	    $(SEPARATOR_STR)
#		$(SILENT)$(ECHO) 'Building target: $@'
#	    $(SILENT)$(ECHO) 'Invoking: DBO Linker'
#	    $(CC) -o $(DBODSLSEXE) $(DBODSLSOBJFILES) $(LIBSDIR) $(LIBS)
#	    $(SILENT)$(ECHO) 'Finished building target: $@'
#	    $(SEPARATOR_STR)
#	    $(NEW_LINE)

.PHONY: clean
clean:
		$(SILENT)$(ECHO) 'Cleaning object files '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(APEXE)
#		$(SILENT)$(RM) $(APEXE)

