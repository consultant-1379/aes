# **********************************************************************
#
# Short description:
# Makefile template for CDH Library
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

CNZ_NAME ?= cdh_cnz
CAA_NAME ?= cdhcmd_caa
CXC_NAME ?= cdh_cxc

# Basic paths
VOB_PATH = $(AES_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(COMMON_ROOT)/common.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

BLOCKDIR = $(CAA_PATH)
#CXC_PATH   = $(AES_ROOT)/cdh_cnz/cdh_cxc
OUTDIR   = $(CXC_PATH)/bin
OBJDIR   = $(BLOCKDIR)/obj
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
APIINCINTDIR = $(CNZ_PATH)/cdhapi_caa/inc_int
APIINCEXTDIR = $(CNZ_PATH)/cdhapi_caa/inc_ext

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE 

# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes

#CINCLUDES +=  -I$(INCDIR) -I$(AES_ROOT)/gcc_cnz/gccapi_caa/inc_ext -I$(APIINCINTDIR) -I$(APIINCEXTDIR) -I$(AES_ROOT)/dbo_cnz/dboapi_caa/inc_ext
CINCLUDES +=  -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)  
CINCLUDES +=  -I$(INCDIR) -I$(APIINCINTDIR) -I$(APIINCEXTDIR)

## # here you can add own Include paths and/or other includes 
LDFLAGS      += 

#LIBSDIR += -L$(AES_ROOT)/gcc_cnz/gcc_cxc/bin/lib_ext -L$(AES_ROOT)/cdh_cnz/cdh_cxc/bin/lib_ext -L$(LIB_LOGCPLUS_SDK_LIB) -L$(AES_ROOT)/dbo_cnz/dbo_cxc/bin/lib_ext -L$(AP_SDK_LIB)
LIBSDIR += -L$(AES_ROOT)/cdh_cnz/cdh_cxc/bin/lib_ext
LIBSDIR += -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(SEC_SDK_LIB)
LIBSDIR += -L$(LIB_LOGCPLUS_SDK_LIB) -L$(AP_SDK_LIB)

## # here you can add own libs 
LIBS += -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf
LIBS += -lboost_serialization -lboost_filesystem  -lboost_system -lboost_thread -lsec_crypto_api 
LIBS += -lrt -lpthread -lacs_tra -laes_gcc -lacs_aeh -lacs_apgcc -laes_cdh -laes_dbo -lacs_dsd -lacs_prc -lacs_csapi

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OBJDIR) $(OUTDIR) 

## Source files
CDHLSSRCFILES = cdhls.cpp

CDHDSLSSRCFILES = cdhdsls.cpp

IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files

CDHLSOBJ = \
	$(CDHLSSRCFILES:%.cpp=%.obj) \
	$(SRCFILES:%.c=%.obj) 

CDHDSLSOBJ = \
	$(CDHDSLSSRCFILES:%.cpp=%.obj) \
	$(SRCFILES:%.c=%.obj) 

## Build instructions
#
CDHLSNAME  = cdhls
CDHDSLSNAME  = cdhdsls
CDHLSNAME_DBG  = cdhls_dbg
CDHDSLSNAME_DBG  = cdhdsls_dbg

CDHLSEXE = $(OUTDIR)/$(CDHLSNAME)
CDHDSLSEXE = $(OUTDIR)/$(CDHDSLSNAME)

CDHLSOBJFILES = $(OBJDIR)/cdhls.obj

CDHDSLSOBJFILES = $(OBJDIR)/cdhdsls.obj

DEPS := $(OBJFILES:.obj=.d) \

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

.PHONY: all

all: $(CDHLSEXE) $(CDHDSLSEXE) 

$(CDHLSEXE): $(CDHLSOBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: CDH Linker'
	    $(CC) -o $(CDHLSEXE) $(CDHLSOBJFILES) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(CDHLSNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

$(CDHDSLSEXE): $(CDHDSLSOBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: CDH Linker'
	    $(CC) -o $(CDHDSLSEXE) $(CDHDSLSOBJFILES) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(CDHDSLSNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

.PHONY: clean
clean:
		$(SILENT)$(ECHO) 'Cleaning object files '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*.obj)

.PHONY: distclean
distclean:
		$(SILENT)$(ECHO) 'Cleaning file: $(APEXE)
		$(SILENT)$(RM) $(APEXE)
		$(SILENT)$(RM) $(OUTDIR)/.debug/$(CDHLSNAME_DBG)
		$(SILENT)$(RM) $(OUTDIR)/.debug/$(CDHDSLSNAME_DBG)

	
