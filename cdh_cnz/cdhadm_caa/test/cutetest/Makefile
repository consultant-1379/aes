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
#            1-11-11   xnadnar   Created
#                                                            
# **********************************************************************

CNZ_NAME ?= cdh_cnz
CAA_NAME ?= cdhadm_caa/test/cutetest/
CXC_NAME ?= cutetest_cxc

# Basic paths
VOB_PATH = $(AES_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)

include $(COMMON_ROOT)/common.mk

DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf 

BLOCKDIR = $(AES_ROOT)/cdh_cnz/cdhadm_caa/
SRCDIR   = $(BLOCKDIR)/src
INCDIR   = $(BLOCKDIR)/inc
TESTDIR  = $(BLOCKDIR)/test
CUTESRCDIR = $(TESTDIR)/cutetest/src
OBJDIR = $(TESTDIR)/cutetest/obj
OUTDIR = $(TESTDIR)/cutetest/bin

# Flexelint application
FL    = $(LINT)     # Global wrap-up mode
FL1   = $(LINT) -u  # Single unit mode

# here you can add own compiler flags
CFLAGS += $(CFLAGS_EXTRA) -DACE_NTRACE -fprofile-arcs -ftest-coverage -DCUTE_BT_SUITE
# here you can add own Assembler flags
ASMFLAGS += 

## # here you can add own Include paths and/or other includes
CINCLUDES += -I$(INCDIR) -I$(CUTESRCDIR)-I/home1/SDK -I$(CURDIR) -I$(CUTESRCDIR)/../cute \
		-I$(AES_ROOT)/gcc_cnz/gccapi_caa/inc_ext -I$(AES_ROOT)/cdh_cnz/cdhapi_caa/inc_int -I$(AES_ROOT)/cdh_cnz/cdhapi_caa/inc_ext \
		-I$(LIB_SSH2_SDK_INC)

## # here you can add own Include paths and/or other includes -- Modified by SirishKumar
LDFLAGS      = -L. -L$(ACE_ROOT)/lib -L$(LOTCROOT)/usr/lib64 -lgcov

# EDITED BY Sirish
LIBSDIR += -L$(AES_ROOT)/gcc_cnz/gcc_cxc/bin/lib_ext -L$(AES_ROOT)/cdh_cnz/cdh_cxc/bin/lib_ext \
			-L$(LIB_SSH2_SDK_LIB) -L$(AP_SDK_LIB) -L$(LIB_LOGCPLUS_SDK_LIB)

## # here you can add own libs 
LIBS += -lACE -lacs_apgcc -lgcov -lacs_tra -laes_gcc -laes_cdh -lssh2

## here you can add own File paths
VPATH +=	$(SRCDIR) $(OUTDIR)  $(CUTESRCDIR) $(CUTESRCDIR)/../cute

## Source files
#

#SRCFILES = cute_cdh_filedestset.cpp aes_cdh_filedestset_handler.cpp aes_cdh_file_init_dest_handler.cpp aes_cdh_file_resp_dest_handler.cpp  aes_cdh_destination_handler.cpp alarmhandler.cpp backupwriter.cpp cdhcriticalsection.cpp cmdhandler.cpp database.cpp destinationalarm.cpp event.cpp ftprv2.cpp ftprv2keep.cpp ftpv2agent.cpp messageblock.cpp notificationprotocol.cpp notificationrecord.cpp readypoll.cpp recordhandler.cpp resendnotification.cpp transferagent.cpp transdestset.cpp transdest.cpp transactionrules.cpp sshftprv2backup.cpp sshftprv2.cpp ftpiv2.cpp servr.cpp notification.cpp socketconnection.cpp ftprv2backup.cpp winftpiv2.cpp sshftpiv2.cpp aes_cdh_ha_service.cpp aes_cdh_sftp.cpp Test.cpp
SRCFILES =  cute_cdh_cmd_handler.cpp Test.cpp

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
#APNAME  = BLOCKNAME1API_CXC
APNAME = aes_cute_testsuite

APEXE = $(OUTDIR)/$(APNAME)

#.PHONY: depend clean
.PHONY: clean

 
all: $(APEXE)

clean:		
		$(SILENT)$(ECHO) 'Cleaning file: $@'
		$(SILENT)$(RM) $(wildcard $(OUTDIR)/*) $(wildcard $(OBJDIR)/*)

$(APEXE): $(OBJ) $(METRICS)
	    $(NEW_LINE)
	    $(SEPARATOR_STR)
		$(SILENT)$(ECHO) 'Building target: $@'
	    $(SILENT)$(ECHO) 'Invoking: GCC Linker'
	    $(CC) -o $(APEXE) $(wildcard $(OBJDIR)/*.obj) $(LIBSDIR) $(LIBS) $(CINCLUDES) $(LDFLAGS)
	    $(SILENT)$(ECHO) 'Finished building target: $@'
	    $(SEPARATOR_STR)
	    $(NEW_LINE)

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

depend:	$(SRCFILES)
		makedepend $(CINCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

