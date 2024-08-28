# **********************************************************************
#
# Short description:
# Makefile template for CDH library
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
#            2011-09-07   XCHEMAD   Created
#                                                            
# **********************************************************************
 
# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = aes
AES_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(AES_ROOT)/common

include $(COMMON_ROOT)/common.mk 

CXC_NAME?=AES_CDHBIN
CXC_NR?=CXC1371517_18
CXC_VER?=R1A01
#CXC_OLD_VER?='R1C|R1B|R1A14|R1A13|R1A12|R1A11|R1A10|R1A09|R1A08|R1A07|R1A06|R1A05|R1A04|R1A03|R1A02|R1A01|R1J|R1H|R1G|R1F|R1F03|R1E|R1D|R1C|R1B|R1A46|R1AK|R1AH'
BASE_SW_VER?= 4.0.0-R1A
CXC_BLK_NAME?=aes_cdh

CURDIR = $(shell pwd)
CXCPATH=$(CURDIR)
CXCDIR=$(CXCPATH)
export CXCDIR
FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*

DOXYGEN_OUTPUT=$(DOCDIR)/docgen
BLOCK_NAME=$(CXC_NAME)
BLOCK_VERSION=$(CXC_NAME)_$(CXC_VER)
BLOCK_SEARCH=../cdhapi_caa/src ../cdhapi_caa/inc_ext ../cdhapi_caa/inc_int ../cdhadm_caa/src ../cdhadm_caa/inc \
			../cdhcmd_caa/src ../cdhcmd_caa/inc

export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

CAA_API_DIR = ../cdhapi_caa/inc_int ../cdhapi_caa/inc_ext  
export CAA_API_DIR

CAA_DIRS = \
		../cdhapi_caa \
		../cdhadm_caa \
		../cdhcmd_caa
		

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
release: all rpm_package esm_package ap_sdk documentation metrics

.PHONY: nightbuild
nightbuild: del_update distclean all rpm_package sdp_package documentation metrics doc_package copy_package esm_package 

.PHONY: del_update
del_update:
	$(call rpm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call sdp_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_CXC_VER),$(CXC_NR))
	$(call esm_del_update,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(OLD_ESM_VER),$(CXC_NR))

.PHONY: documentation
documentation:
	$(call doc_creation)

#CPPCKECK_SRCS = ../cdhapi_caa/src/*.cpp ../cdhadm_caa/src/*.cpp ../cdhcmd_caa/src/*.cpp
CPPCKECK_SRCS = ../cdhapi_caa/src/aes_cdh_asynchreceiver.cpp ../cdhapi_caa/src/aes_cdh_cmdclient.cpp ../cdhapi_caa/src/aes_cdh_destination_r1.cpp ../cdhapi_caa/src/aes_cdh_destinationimplementation.cpp ../cdhapi_caa/src/aes_cdh_destinationset_r1.cpp ../cdhapi_caa/src/aes_cdh_destinationsetimplementation.cpp ../cdhapi_caa/src/aes_cdh_result_r1.cpp ../cdhapi_caa/src/aes_cdh_resultimplementation.cpp ../cdhadm_caa/src/aes_cdh_filedestset_handler.cpp ../cdhadm_caa/src/aes_cdh_file_init_dest_handler.cpp ../cdhadm_caa/src/aes_cdh_file_resp_dest_handler.cpp ../cdhadm_caa/src/alarmhandler.cpp ../cdhadm_caa/src/backupwriter.cpp ../cdhadm_caa/src/cdhcriticalsection.cpp ../cdhadm_caa/src/cmdhandler.cpp ../cdhadm_caa/src/database.cpp ../cdhadm_caa/src/destinationalarm.cpp ../cdhadm_caa/src/event.cpp ../cdhadm_caa/src/ftprv2.cpp ../cdhadm_caa/src/ftprv2keep.cpp ../cdhadm_caa/src/ftpv2agent.cpp ../cdhadm_caa/src/messageblock.cpp ../cdhadm_caa/src/notificationprotocol.cpp ../cdhadm_caa/src/notificationrecord.cpp ../cdhadm_caa/src/readypoll.cpp ../cdhadm_caa/src/recordhandler.cpp ../cdhadm_caa/src/resendnotification.cpp ../cdhadm_caa/src/transferagent.cpp ../cdhadm_caa/src/transdestset.cpp ../cdhadm_caa/src/transdest.cpp ../cdhadm_caa/src/transactionrules.cpp ../cdhadm_caa/src/sshftprv2backup.cpp ../cdhadm_caa/src/sshftprv2.cpp ../cdhadm_caa/src/ftpiv2.cpp ../cdhadm_caa/src/servr.cpp ../cdhadm_caa/src/notification.cpp ../cdhadm_caa/src/socketconnection.cpp ../cdhadm_caa/src/ftprv2backup.cpp ../cdhadm_caa/src/aes_cdh_service.cpp ../cdhadm_caa/src/winftpiv2.cpp ../cdhadm_caa/src/sshftpiv2.cpp ../cdhadm_caa/src/aes_cdh_ha_service.cpp ../cdhadm_caa/src/aes_cdh_sftp.cpp ../cdhcmd_caa/src/cdhdsls.cpp
CPPCKECK_INCS = -I../cdhapi_caa/inc_int -I../cdhapi_caa/inc_ext -I../cdhadm_caa/inc -I../cdhcmd_caa/inc

.PHONY: cppcheck_m
cppcheck_m:
	$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))

CCCC_SRCS = ../cdhapi_caa/src/* ../cdhadm_caa/src/* ../cdhcmd_caa/src/*

.PHONY: cccc_m
cccc_m:
	$(call cccc_doc_creation,$(CCCC_SRCS))

SLOCCOUNT_SRCS = ../cdhapi_caa/ ../cdhadm_caa/ ../cdhcmd_caa/

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
	$(call rpm_file_creation,cdh.spec,$(CXC_NAME),$(CXC_NR),$(CXC_VER))

.PHONY: sdp_package
sdp_package:
	$(SILENT)$(ECHO) 'SDP packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(call sdp_creation_US3,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(CXC_OLD_VER),$(CXC_NR))
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
	

