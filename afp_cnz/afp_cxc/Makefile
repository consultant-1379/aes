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
# -----      ----------   --------  --------------------------
#            2018-04-23   xcsrajn   ESM adoptations
#            2011-09-22   xnadnar   Created
#                                                            
# **********************************************************************
# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = aes
AES_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(AES_ROOT)/common

include $(COMMON_ROOT)/common.mk 

CXC_NAME?=AES_AFPBIN
CXC_NR?=CXC1371518_9
CXC_VER?=R1L
#CXC_OLD_VER?='R1C|R1B|R1A25|R1A24|R1A23|R1A22|R1A21|R1A20|R1A19|R1A18|R1A17|R1A16|R1A15|R1A14|R1A13|R1A12|R1A11|R1A10|R1A09|R1A08|R1A07|R1A06|R1A05|R1A04|R1A03|R1A02|R1A01|R1G|R1F'
BASE_SW_VER?=3.6.0-R1A
CXC_BLK_NAME?=aes_afp

CXCPATH=$(CURDIR)
CXCDIR=$(CXCPATH)
export CXCDIR
FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*

DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=$(CXC_NAME)
BLOCK_VERSION=$(CXC_NAME)_$(CXC_VER)
BLOCK_SEARCH= ../afpadm_caa/src ../afpadm_caa/inc \
	../afpcmd_caa/src ../afpcmd_caa/inc \
	../afpapi_caa/src ../afpapi_caa/inc_int ../afpapi_caa/inc_ext 
		

export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

CAA_API_DIR = ../afpapi_caa/inc_ext ../afpapi_caa/inc_int
export CAA_API_DIR

CAA_DIRS = \
		../afpapi_caa \
		../afpadm_caa \
		../afpcmd_caa

.PHONY: all
all: 
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) AES_ROOT=$(AES_ROOT) all) ;\
		if [ $$? != 0 ]; \
		then $(ECHO) "Error: please see the printout!!!";\
		exit $$?;\
		fi;\
		echo "+++++++++++++++++" ; \
		done
.PHONY: release
release: all rpm_package sdp_package esm_package tar_package ap_sdk documentation metrics

.PHONY: nightbuild
nightbuild: del_update distclean all rpm_package sdp_package  esm_package documentation metrics doc_package copy_package 

.PHONY: del_update
del_update:
	$(call rpm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call sdp_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call esm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_ESM_VER),$(CXC_NR))

.PHONY: documentation
documentation:
		$(call doc_creation)

CPPCKECK_SRCS = ../afpapi_caa/src/*.cpp ../afpadm_caa/src/*.cpp ../afpcmd_caa/src/*.cpp
CPPCKECK_INCS = -I../afpapi_caa/inc_int -I../afpapi_caa/inc_ext -I../afpadm_caa/inc -I../afpcmd_caa/inc

.PHONY: cppcheck_m
cppcheck_m:
	$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))

CCCC_SRCS = ../afpapi_caa/src/* ../afpadm_caa/src/* ../afpcmd_caa/src/*

.PHONY: cccc_m
cccc_m:
	$(call cccc_doc_creation,$(CCCC_SRCS))

SLOCCOUNT_SRCS = ../afpapi_caa/ ../afpadm_caa/ ../afpcmd_caa/

.PHONY: sloccount_m
sloccount_m:
	$(call sloccount_doc_creation,$(SLOCCOUNT_SRCS))

.PHONY: metrics
metrics: cppcheck_m cccc_m sloccount_m
		
.PHONY: clean
clean:
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		 echo "cd $$i" ; \
		 cd $$i ; \
		 $(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) AES_ROOT=$(AES_ROOT) clean ) ;\
		 echo "+++++++++++++++++" ; \
	done
	$(call FX_CLEAN_TMP)

.PHONY: distclean
distclean:
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) AES_ROOT=$(AES_ROOT) distclean ) ;\
		echo "+++++++++++++++++" ; \
	done
	$(call FX_CLEAN_TMP)

.PHONY: rpm_package
rpm_package:
	$(call rpm_file_creation,afp.spec,$(CXC_NAME),$(CXC_NR),$(CXC_VER))
	echo "RPM CREATION  ";

.PHONY: sdp_package
sdp_package:
	$(call sdp_creation_US3,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(CXC_OLD_VER),$(CXC_NR))
	$(call up_creation,$(CXC_NAME),$(CXC_NR),$(CXC_VER),$(CXC_OLD_VER),$(CXCPATH),$(BASE_SW_VER))
#	echo " SDP CREATION NOT IMPLEMENTED ";
.PHONY: esm_package
esm_package:
		$(call esm_pkg_creation,$(CXCPATH),$(CXC_BLK_NAME),$(CXC_VER),$(BASE_SW_VER),$(CXC_NR),$(CXC_NAME))
                      
.PHONY: upload_package 
upload_package: 
	$(call upload_package,$(CXC_BLK_NAME),$(CXC_NR),$(CXC_NAME),$(CXC_VER))

.PHONY: tar_package
tar_package:
	$(SILENT)$(ECHO) 'TAR packaging ...'
#	$(call tar_creation,$(BLOCK_NAME),$(CXC_NR),$(CXC_VER))
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

.PHONY: copy_package
copy_package:
	$(call copy_file_to_integration,$(RPMDIR)/$(CXC_NAME)-$(CXC_NR)-$(CXC_VER).x86_64.rpm)
	$(call copy_file_to_integration,$(CXC_NAME)-$(CXC_NR)-$(CXC_VER)-documentation.tar)

.PHONY: doc_package
doc_package: documentation metrics
	$(SILENT)$(ECHO) 'DOC and Metrics packaging ...'
	$(call doc_tarfile_creation,$(CXC_NAME)-$(CXC_NR)-$(CXC_VER)-documentation.tar)

.PHONY: ap_sdk
ap_sdk:
	$(SILENT)$(ECHO) 'AP SDK updating ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

		
