# **********************************************************************
#
# Short description:
# Makefile template for 
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
#            2010-12-07   tcsajgu   Created
#                                                            
# **********************************************************************

CNZ_PATH = /vobs/cm4ap/ntaes/gohst_cnz
COMMON_PATH = /vobs/IO_Developments/Tools/linux_conf_file
include $(COMMON_PATH)/common.mk

CAADIR = gohstadm_caa
#CURDIR = $(CNZ_PATH)/$(CAADIR)

BLOCKDIR = $(CURDIR)/
OUTDIR   = $(CXCDIR)/bin
OBJDIR   = $(CURDIR)/obj
SRCDIR   = $(CURDIR)/src
INCDIR   = $(CURDIR)/inc
TESTDIR  = $(CURDIR)/test


# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE 
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR) -I/vobs/cm4ap/ntaes/ohi_cnz/ohiapi_caa/inc_ext -I/vobs/cm4ap/ntaes/gcc_cnz/gccapi_caa/inc_int 

# here you can add own lib paths
LIBSDIR += -L$(LIB_LOGCPLUS_SDK_LIB) -L/vobs/cm4ap/ntaes/ohi_cnz/ohi_cxc/bin/lib_ext -L/vobs/cm4ap/ntaes/gcc_cnz/gcc_cxc/bin/lib_ext  -L/vobs/cm4ap/ntaes/afp_cnz/afp_cxc/bin/lib_ext -L$(AP_SDK_LIB)

## # here you can add own libs 
LIBS += -lacs_apgcc -lacs_tra -llog4cplus -lacs_jtp -lacs_csapi -lacs_dsd -laes_ohi -laes_gcc -laes_afp
# -lmcs_cc
# -lacs_prc -lacs_dsd

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR)

## Source files

SRCFILES = supervisor.cpp  test.cpp  main.cpp  threadgen.cpp

L_FILES += $(SRCFILES:%=$(BLOCKDIR)/src/%)



## Object files
#
OBJ = \
		$(SRCFILES:%.cpp=%.obj) \
		$(SRCFILES:%.c=%.obj) 


## Build instructions
#
APNAME = gohst 
APEXE = $(OUTDIR)/$(APNAME)


.PHONY: all 
all: $(APEXE)

.PHONY: clean
clean:		
		$(SILENT)$(ECHO) 'Cleaning object files: '
		$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)

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

.PHONY: prebuild
prebuild:
	$(call ct_co,$(OUTDIR)/$(APNAME))

.PHONY: postbuild
postbuild:
	$(call ct_ci,$(OUTDIR)/$(APNAME))

.PHONY: undobuild
undobuild:
	$(call ct_unco,$(OUTDIR)/$(APNAME))


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
