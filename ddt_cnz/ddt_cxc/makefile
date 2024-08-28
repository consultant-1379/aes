

# BEGIN: GIT INTRO.
CURDIR = $(shell pwd)
REPO_NAME = aes
AES_ROOT = $(shell echo $(CURDIR) | sed 's@'/$(REPO_NAME)'.*@'/$(REPO_NAME)'@g')
COMMON_ROOT = $(AES_ROOT)/common

include $(COMMON_ROOT)/common.mk 

CXC_NAME ?= AES_DDTBIN
CXC_NR ?= CXC1371515_18
CXC_VER ?= R1B
#CXC_OLD_VER ?= NOT USED ANYMORE
BASE_SW_VER ?= 3.8.0-R1A
CXC_BLK_NAME?=aes_ddt

RPM_SPEC = ddt.spec

CXCPATH = $(CURDIR)
CXCDIR = $(CXCPATH)
export CXCDIR
export CXCPATH
FX_CLEAN_TMP := rm -rf /tmp/$(USER)/*

#$(MAKE) $(MFLAGS) CXCDIR=$(CXCPATH) release

CAA_ADM_DIR = ../ddtadm_caa
export CAA_ADM_DIR
CAA_DIRS = $(CAA_ADM_DIR)

DOXYGEN_OUTPUT=./docgen
BLOCK_NAME=$(CXC_NAME)
BLOCK_VERSION=$(CXC_NAME)_$(CXC_VER)
BLOCK_SEARCH=$(CAA_ADM_DIR)/$(SRCDIR) $(CAA_ADM_DIR)/inc

FX_CLEAN_CONTRIB := find ../ -name \*.contrib* | xargs -I {} rm {}

export DOXYGEN_OUTPUT
export BLOCK_NAME
export BLOCK_VERSION
export BLOCK_SEARCH

.PHONY: clean_and_build
clean_and_build: clean all

.PHONY: all
all:
	@for i in $(CAA_DIRS) ; do \
		(echo "*****************" ; \
	     echo "***    ALL    ***" ; \
		 echo "*****************" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) AES_ROOT=$(AES_ROOT) checkdirs ;\
		$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) AES_ROOT=$(AES_ROOT) all) ;\
		if [ $$? != 0 ] ; \
		then $(ECHO) "Error: please see the printout !!!";\
		exit $$?;\
		fi;\
		echo "===================" ; \
		echo "====  END ALL  ====" ; \
		echo "===================" ; \
		done
		$(call FX_CLEAN_TMP)
    

.PHONY: release
release: all rpm_package sdp_package esm_package ap_sdk documentation metrics

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
	
CPPCKECK_SRCS = $(CAA_ADM_DIR)/$(SRCDIR)/*.cpp
CPPCKECK_INCS = -I$(CAA_ADM_DIR)/inc

.PHONY: cppcheck_m
cppcheck_m:
	$(call cppcheck_doc_creation,$(CPPCKECK_SRCS),$(CPPCKECK_INCS))

CCCC_SRCS = $(CAA_ADM_DIR)/$(SRCDIR)/*

.PHONY: cccc_m
cccc_m:
cccc_m:
	$(call cccc_doc_creation,$(CCCC_SRCS))

SLOCCOUNT_SRCS = $(CAA_ADM_DIR)/

.PHONY: sloccount_m
sloccount_m:
	$(call sloccount_doc_creation,$(SLOCCOUNT_SRCS))

.PHONY: metrics
metrics: cppcheck_m cccc_m sloccount_m
		
.PHONY: clean
clean:
		$(SILENT)$(ECHO) '===================================================================='
		$(SILENT)$(ECHO) 'CLEANING OBJECT FILES FOR BLOCK $(BLOCK_NAME)'
		$(SILENT)$(ECHO) '===================================================================='
		$(NEW_LINE)
		$(call FX_CLEAN_CONTRIB)
		$(call FX_CLEAN_TMP)
		@for i in $(CAA_DIRS) ; do \
			(echo "*****************" ; \
			 echo "***   CLEAN   ***" ; \
			 echo "*****************" ; \
			 echo "cd $$i" ; \
			 cd $$i ; \
			 $(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) AES_ROOT=$(AES_ROOT) clean ) ;\
			 if [ $$? != 0 ] ; \
			then $(ECHO) "Error: please see the printout !!!";\
			exit $$?;\
			fi;\
			 echo "=====================" ; \
			 echo "====  END CLEAN  ====" ; \
			 echo "=====================" ; \
		done

.PHONY: distclean
distclean:
	@for i in $(CAA_DIRS) ; do \
		(echo "+++++++++++++++++" ; \
		echo "cd $$i" ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) COMMON_ROOT=$(COMMON_ROOT) AES_ROOT=$(AES_ROOT) distclean ) ;\
		if [ $$? != 0 ] ; \
		then $(ECHO) "Error: please see the printout !!!";\
		exit $$?;\
		fi;\
		echo "+++++++++++++++++" ; \
	done
	$(call FX_CLEAN_TMP)

.PHONY: rpm_package
rpm_package:
	$(call rpm_file_creation,$(RPM_SPEC),$(CXC_NAME),$(CXC_NR),$(CXC_VER))

.PHONY: sdp_package
sdp_package:
	$(SILENT)$(ECHO) 'SDP packaging ...'
	$(call sdp_creation_US3,$(CXCPATH),$(BLOCK_NAME),$(CXC_VER),$(CXC_OLD_VER),$(CXC_NR))
	$(SILENT)$(ECHO) 'UP packaging ...' 
	$(call up_creation,$(CXC_NAME),$(CXC_NR),$(CXC_VER),$(CXC_OLD_VER),$(CXCPATH),$(BASE_SW_VER))

.PHONY: esm_package
esm_package:
		$(call esm_pkg_creation,$(CXCPATH),$(CXC_BLK_NAME),$(CXC_VER),$(BASE_SW_VER),$(CXC_NR),$(CXC_NAME))
                      
.PHONY: upload_package 
upload_package: 
	$(call upload_package,$(CXC_BLK_NAME),$(CXC_NR),$(CXC_NAME),$(CXC_VER))

.PHONY: copy_package
copy_package:
	$(call copy_file_to_integration,$(RPMDIR)/$(CXC_NAME)-$(CXC_NR)-$(CXC_VER).x86_64.rpm)
	$(call copy_file_to_integration,$(CXC_NAME)-$(CXC_NR)-$(CXC_VER)-documentation.tar)

.PHONY: doc_package
doc_package: documentation metrics
	$(SILENT)$(ECHO) 'DOC and Metrics packaging ...'
	$(call doc_tarfile_creation,$(CXC_NAME)-$(CXC_NR)-$(CXC_VER)-documentation.tar)

.PHONY: tar_package
tar_package:
	$(SILENT)$(ECHO) 'TAR packaging ...'
	$(call tar_creation,$(BLOCK_NAME),$(CXC_NR),$(CXC_VER))

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
		exit $$?;\
		fi;\
		echo "+++++++++++++++++" ; \
	done

.PHONY: undo_ap_sdk
undo_ap_sdk:
	$(SILENT)$(ECHO) 'UNDO AP SDK packaging ...'
	$(SILENT)$(ECHO) '**********************************'
	$(SILENT)$(ECHO) '****** NOT YET IMPLEMENTED *******'
	$(SILENT)$(ECHO) '**********************************'

