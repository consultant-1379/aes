# **********************************************************************
#
# Short description:
# Makefile template for OHI library
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
# PA1        2011-06-17   xchemad   Created
#                                                            
# **********************************************************************
# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = aes
AES_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(AES_ROOT)/common

include $(COMMON_ROOT)/common.mk 

CXC_NAME?=AES_OHIBIN
CXC_NR?=CXC1371446_9
CXC_VER?=R1A05
CXC_OLD_VER?=''
BASE_SW_VER?=3.6.0-R1A
CXC_BLK_NAME?=aes_ohi

CURDIR = $(shell pwd)
CXCPATH=$(CURDIR)
CXCDIR=$(CXCPATH)
export CXCDIR
DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=$(CXC_NAME)
BLOCK_VERSION=$(CXC_NAME)_$(CXC_VER)
BLOCK_SEARCH=../ohiapi_caa/src ../ohiapi_caa/inc_ext ../ohiapi_caa/inc_int
FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*
export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

CAA_API_DIR = ../ohiapi_caa/inc_ext
export CAA_API_DIR

CAA_DIRS = \
		../ohiapi_caa

.PHONY: all
all: 
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) AES_ROOT=$(AES_ROOT) all) ;\
		if [ $$? != 0 ]; \
		then $(ECHO) "Error: please see the printout!!!";\
		exit 3;\
		fi;\
		echo "+++++++++++++++++" ; \
		done

.PHONY: release
release: all rpm_package sdp_package esm_package tar_package documentation metrics

.PHONY: nightbuild
nightbuild: del_update distclean all rpm_package sdp_package esm_package documentation metrics doc_package copy_package 

.PHONY: del_update
del_update:
	$(call rpm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call sdp_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call esm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_ESM_VER),$(CXC_NR))

.PHONY: documentation
documentation:
	$(call doc_creation)

CPPCKECK_SRCS = ../ohiapi_caa/src/*.cpp
CPPCKECK_INCS = -I../ohiapi_caa/inc_int -I../ohiapi_caa/inc_ext

.PHONY: cppcheck_m
cppcheck_m:
	$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))

CCCC_SRCS = ../ohiapi_caa/src/*

.PHONY: cccc_m
cccc_m:
	$(call cccc_doc_creation,$(CCCC_SRCS))

SLOCCOUNT_SRCS = ../ohiapi_caa/

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
	$(SILENT)$(ECHO) 'RPM packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(call rpm_file_creation,ohi.spec, $(CXC_NAME), $(CXC_NR), $(CXC_VER))
	$(SILENT)$(ECHO) '**********************************'

.PHONY: sdp_package
sdp_package:
	$(SILENT)$(ECHO) 'SDP packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(call sdp_creation_US1,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(CXC_OLD_VER),$(CXC_NR))
	$(call up_creation,$(CXC_NAME),$(CXC_NR),$(CXC_VER),$(CXC_OLD_VER),$(CXCPATH),$(BASE_SW_VER))
	$(SILENT)$(ECHO) '**********************************'

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
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) AES_ROOT=$(AES_ROOT) ap_sdk ) ;\
		if [ $$? != 0 ] ; \
		then $(ECHO) "Error: please see the printout !!!";\
		exit 10;\
		fi;\
		echo "+++++++++++++++++" ; \
	done

