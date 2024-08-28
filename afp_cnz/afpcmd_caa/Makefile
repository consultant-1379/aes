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
#            2010-11-24   xnadnar   Created
#                                                            
# **********************************************************************

CNZ_NAME ?= afp_cnz
CAA_NAME ?= afpcmd_caa
CXC_NAME ?= afp_cxc

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
AFPADMINCDIR = $(CURDIR)/../afpadm_caa/inc
AFPAPIINCDIR = $(CURDIR)/../afpapi_caa/inc_ext
AFPAPIINTDIR = $(CURDIR)/../afpapi_caa/inc_int
INCCDHEXT = $(CURDIR)/../../cdh_cnz/cdhapi_caa/inc_ext
GCCEXTDIR = $(CURDIR)/../../gcc_cnz/gccapi_caa/inc_ext
AFPLIBDIR = $(CURDIR)/../afp_cxc/bin/lib_ext
GCCLIBDIR = $(CURDIR)/../../gcc_cnz/gcc_cxc/bin/lib_ext
TESTDIR  = $(BLOCKDIR)/test

EXTRA_DEFINES += -DAES_AFP_API_HAS_TRACE
EXTRA_DEFINES += -DAES_AFP_TRACE_USE_SOURCE_INFO
EXTRA_DEFINES += -DAES_AFP_TRACE_HAS_FUNCTION_TRACE
EXTRA_DEFINES += -DAES_AFP_TRACE_USE_PRETTY_FUNCTION
# log enabled
EXTRA_DEFINES += -DAES_AFP_HAS_LOGGING
EXTRA_DEFINES += -DAES_AFP_LOGGING_HAS_SOURCE_INFO
EXTRA_DEFINES += -DAES_AFP_LOGGING_USE_PRETTY_FUNCTION

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE
CFLAGS += $(EXTRA_DEFINES)
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
#CINCLUDES+= -I$(INCDIR) -I$(AFPADMINCDIR) -I$(GCCEXTDIR) -I$(AFPAPIINTDIR) -I$(AFPAPIINCDIR) -I$(INCCDHEXT) 
CINCLUDES += -I$(ACE_SDK_INC) -I$(COREMW_SDK_INC)
CINCLUDES+= -I$(GCCEXTDIR) -I$(INCDIR) -I$(AFPADMINCDIR) -I$(AFPAPIINTDIR) -I$(AFPAPIINCDIR) 

## # here you can add own Include paths and/or other includes 
LDFLAGS      =  -L.  -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 

#LIBSDIR += -L$(GCCLIBDIR) -L$(AFPLIBDIR) -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)  
LIBSDIR  += -L$(COREMW_SDK_LIB) -L$(ACE_SDK_LIB) -L$(SEC_CREDU_SDK_LIB) -L$(BOOST_SDK_LIB) -L$(SEC_SDK_LIB)
LIBSDIR += -L$(GCCLIBDIR) -L$(AFPLIBDIR) -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)  

## # here you can add own libs
LIBS += -lACE -lSaImmOm -lSaImmOi -lSaAmf -lSaCkpt -lSaClm -lSaLog -lSaNtf
LIBS += -lboost_serialization -lboost_filesystem  -lboost_system -lsec_crypto_api 
LIBS += -lacs_apgcc -lacs_tra -llog4cplus -laes_afp -laes_gcc -lacs_dsd -lacs_aeh -lacs_csapi -lacs_prc

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR) $(NTAES)/afp_cnz/afpadm_caa/src/

## Source files
#
SRCFILES =  aes_afp_command.cpp aes_afp_getopt.cpp aes_afp_def.cpp aes_afp_rm.cpp aes_afp_ls.cpp
		     
IFUFILES = \

ASMFILES = \

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%) 

## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 

## Build instructions
AFPDEF_APNAME = afpdef
AFPRM_APNAME = afprm
AFPLS_APNAME = afpls
AFPDEF_APNAME_DBG = afpdef_dbg
AFPRM_APNAME_DBG = afprm_dbg
AFPLS_APNAME_DBG = afpls_dbg

AFPDEF_APEXE = $(OUTDIR)/$(AFPDEF_APNAME)
AFPRM_APEXE = $(OUTDIR)/$(AFPRM_APNAME)
AFPLS_APEXE = $(OUTDIR)/$(AFPLS_APNAME)

AFPDEF_OBJDIR = $(OBJDIR)/aes_afp_command.obj $(OBJDIR)/aes_afp_getopt.obj $(OBJDIR)/aes_afp_def.obj
AFPRM_OBJDIR = $(OBJDIR)/aes_afp_command.obj $(OBJDIR)/aes_afp_getopt.obj $(OBJDIR)/aes_afp_rm.obj
AFPLS_OBJDIR = $(OBJDIR)/aes_afp_command.obj $(OBJDIR)/aes_afp_getopt.obj $(OBJDIR)/aes_afp_ls.obj

 
.PHONY: all
all: $(OUTDIR)/$(AFPDEF_APNAME) \
	 $(OUTDIR)/$(AFPRM_APNAME) $(OUTDIR)/$(AFPLS_APNAME)

	 

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT) rm -f $(wildcard $(OBJDIR)/*)
		

.PHONY: distclean
distclean: 
		$(SILENT)$(ECHO) 'Cleaning file: $(APNAME)'
		$(RM) $(OUTDIR)/$(AFPDEF_APNAME)
		$(RM) $(OUTDIR)/$(AFPRM_APNAME)
		$(RM) $(OUTDIR)/$(AFPLS_APNAME)
		$(RM) $(OUTDIR)/.debug/$(AFPDEF_APNAME_DBG)
		$(RM) $(OUTDIR)/.debug/$(AFPRM_APNAME_DBG)
		$(RM) $(OUTDIR)/.debug/$(AFPLS_APNAME_DBG)
				
$(AFPDEF_APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(AFPDEF_APEXE) $(AFPDEF_OBJDIR) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(AFPDEF_APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)
	    
$(AFPRM_APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(AFPRM_APEXE) $(AFPRM_OBJDIR) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(AFPRM_APNAME))
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)	    
	    
$(AFPLS_APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(AFPLS_APEXE) $(AFPLS_OBJDIR) $(LIBSDIR) $(LIBS)
	    $(call stripp,$(AFPLS_APNAME))
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

