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
#            2011-09-20   xdeecho   Created
#                                                            
# **********************************************************************
 
include /vobs/IO_Developments/Tools/linux_conf_file/common.mk

CXC_NAME?=MCS_AIAP
CXC_NR?=CXC1371379
CXC_VER?=R1A10
CXC_OLD_VER?=R1A09

CURDIR = $(shell pwd)
CXCPATH=$(CURDIR)
CXCDIR=$(CXCPATH)
export CXCDIR

DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=$(CXC_NAME)
BLOCK_VERSION=$(CXC_NAME)_$(CXC_VER)
BLOCK_SEARCH=../gohstadm_caa/src ../gohstadm_caa/inc 

export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

CAA_DIRS = \
		../gohstadm_caa
		



.PHONY: all
all: 
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) CXCDIR=$(CXCPATH) all) ;\
		if [ $$? != 0 ]; \
		then $(ECHO) "Error: please see the printout!!!";\
		exit $$?;\
		fi;\
		echo "+++++++++++++++++" ; \
		done

.PHONY: release
release: all rpm_package sdp_package ap_sdk documentation metrics

.PHONY: nightbuild
nightbuild: prebuild distclean all rpm_package sdp_package documentation metrics doc_package copy_package undobuild


.PHONY: prebuild
prebuild:
	$(SILENT)$(ECHO) 'prebuild ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

.PHONY: postbuild
postbuild:
	$(SILENT)$(ECHO) 'postbuild ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

.PHONY: undobuild
undobuild:
	$(SILENT)$(ECHO) 'undobuild ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'


.PHONY: documentation
documentation:
	#$(call doc_creation)

CPPCKECK_SRCS = ../gohstadm_caa/src/*.cpp  
CPPCKECK_INCS = -I../gohstadm_caa/inc  

.PHONY: cppcheck_m
cppcheck_m:
		$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))


CCCC_SRCS = ../gohstadm_caa/src/*  

.PHONY: cccc_m
cccc_m:
		$(call cccc_doc_creation,$(CCCC_SRCS))

SLOCCOUNT_SRCS = ../gohstadm_caa/ 

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
			 $(MAKE) $(MFLAGS) CXCDIR=$(CXCPATH) clean ) ;\
			 echo "+++++++++++++++++" ; \
		done

.PHONY: distclean
distclean:
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) $(MFLAGS) CXCDIR=$(CXCPATH) distclean ) ;\
		echo "+++++++++++++++++" ; \
	done

.PHONY: rpm_package
rpm_package:
	$(SILENT)$(ECHO) 'RPM packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

.PHONY: sdp_package
sdp_package:
	$(SILENT)$(ECHO) 'SDP packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

.PHONY: tar_package
tar_package:
	$(SILENT)$(ECHO) 'TAR packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

.PHONY: copy_package
copy_package:
	$(SILENT)$(ECHO) 'copy package ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'


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

		

