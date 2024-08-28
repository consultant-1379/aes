
CNZ_NAME ?= ddt_cnz
CAA_NAME ?= ddtadm_caa
CXC_NAME ?= ddt_cxc

# Basic paths
VOB_PATH = $(AES_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(COMMON_ROOT)/common.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

#***********************************************************************************************
# ADDING HERE ALL SOFTWARE MODULES UNDER "src/" TO USE 
MODULES := common engine imm mtap alarm operation store store/checkpoint store/checkpoint/state
#************************************************************************************************

DOXYGEN_OUTPUT=$(DOCDIR)/docgen

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I$(INCDIR)

CINCLUDES += -I$(AP_SDK_INC)
CINCLUDES += -I$(3PP_SDK_PATH)
CINCLUDES += -I$(BOOST_SDK_INC)
CINCLUDES += -I$(SEC_SDK_INC)
CINCLUDES += -I$(LIB_SSH2_PATH)
CINCLUDES += -I$(LIB_CURL_SDK_INC)
CINCLUDES += -I$(COREMW_SDK_INC)
CINCLUDES += -I$(ACE_SDK_INC)
CINCLUDES += -I$(LOTC_SDK_INC)

## here you can add own lib paths
LIBSDIR += -L$(LIB_EXT)
LIBSDIR += -L$(AP_SDK_LIB)
LIBSDIR += -L$(LIB_LOGCPLUS_SDK_LIB)
LIBSDIR += -L$(BOOST_SDK_LIB)
LIBSDIR += -L$(SEC_SDK_LIB)
LIBSDIR += -L$(SEC_CREDU_SDK_LIB)
LIBSDIR += -L$(LIB_SSH2_SDK_LIB)
LIBSDIR += -L$(LIB_CURL_SDK_LIB)
LIBSDIR += -L$(LOTC_SDK_LIB)
LIBSDIR += -L$(ACE_SDK_LIB)
LIBSDIR += -L$(COREMW_SDK_LIB)

# here you can add own libs
LIBS += -lrt -lpthread -lACE 
LIBS += -lacs_tra -lcap -lnsl -lssh2 -lsec_credu_api -lcurl -lidn /lib64/libtirpc.so.3
LIBS += -lSaCkpt -lcrypto -lsec_crypto_api -lacs_aeh -lacs_apgcc -lacs_dsd -lacs_csapi
LIBS += -lboost_system -lboost_filesystem -lboost_serialization

VPATH += $(SRCDIR) $(INCDIR) $(OUTDIR) $(OBJDIR)

SRC_DIR := $(SRCDIR)
SRC_DIR += $(addprefix $(SRCDIR)/,$(MODULES))

BUILD_DIR := $(addprefix $(OBJDIR)/,$(MODULES))
OBJ_CLEAN := $(addsuffix /*.obj, $(BUILD_DIR))
DEP_CLEAN := $(addsuffix /*.d, $(BUILD_DIR))

## source files
SRC := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))

## Object files
MODULE_OBJS := $(patsubst src/%.cpp, $(OBJDIR)/%.obj, $(SRC))

DEPS := $(MODULE_OBJS:.obj=.d)

vpath %.cpp $(SRC_DIR)

ifeq ($(MAKECMDGOALS),all)
-include $(DEPS)
endif

# here you can add own linker flags
LDFLAGS += -Wl,-rpath-link,$(AP_SDK_LIB) -Wl,-rpath-link,$(COREMW_SDK_LIB) -Wl,-rpath-link,$(SEC_SDK_LIB) -Wl,-rpath-link,$(ACE_SDK_LIB)
LDFLAGS += -Wl,-rpath-link,$(SEC_CREDU_SDK_LIB) -Wl,-rpath-link,$(BOOST_SDK_LIB) -Wl,-rpath-link,$(LIB_LOGCPLUS_SDK_LIB) -Wl,-rpath-link,$(LIB_XERCES_SDK_LIB)

## here you can add own compiler flags
#CFLAGS += -O3 -fPIC -fmessage-length=0
CFLAGS = -g -fPIC -fmessage-length=0 -Wall

# here you can add more defines
# trace enabled
EXTRA_DEFINES += -DAES_DDT_API_HAS_TRACE
EXTRA_DEFINES += -DAES_DDT_TRACE_USE_SOURCE_INFO
EXTRA_DEFINES += -DAES_DDT_TRACE_HAS_FUNCTION_TRACE
#EXTRA_DEFINES += -DAES_DDT_TRACE_USE_PRETTY_FUNCTION
# log enabled
EXTRA_DEFINES += -DAES_DDT_HAS_LOGGING
EXTRA_DEFINES += -DAES_DDT_LOGGING_HAS_SOURCE_INFO
EXTRA_DEFINES += -DAES_DDT_LOGGING_USE_PRETTY_FUNCTION
EXTRA_DEFINES += -DAES_DDT_HAS_DUMPING

#EXTRA_DEFINES += -DFILE_DUMP #DUMP FILES TRANSMITTED OVER SFTP into /data/opt/ap/internal_root/tools
#EXTRA_DEFINES += -DFILE_WRITE #DUMP FILES WRITTEN BY THE CKPT  WRITER AND READ BY THE CKPT READER into /data/opt/ap/internal_root/tools

EXTRA_CFLAGS += -Wextra $(EXTRA_DEFINES) -Wno-ignored-qualifiers -Wno-empty-body -Wno-unused-parameter -Wno-unused-local-typedefs

# Setting application name
APPNAME = aes_ddtd
APPNAME_DBG = aes_ddtd_dbg

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: checkdirs $(OUTDIR)/$(APPNAME)

$(OUTDIR)/$(APPNAME): $(MODULE_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Building application: $(APPNAME)'
	$(SILENT)$(ECHO) "Linking $@..."
	$(SILENT)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $(OUTDIR)/$(APPNAME) $(MODULE_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	#$(SILENT)$(call stripp,$(APPNAME))
	$(SEPARATOR_STR)
	$(NEW_LINE)

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -pm 777 $@

.PHONY: clean
clean: checkdirs
	$(RM) -r $(OBJDIR)/*.obj
	$(RM) -r $(OBJDIR)/*.d
	$(RM) -r $(OBJ_CLEAN)
	$(RM) -r $(DEP_CLEAN)	

.PHONY: distclean
distclean: clean
	$(RM) -r $(OUTDIR)/$(APPNAME)
	$(RM) $(OUTDIR)/.debug/$(APPNAME_DBG)

######################################################################################
# DOCUMENTATION
# Generate HTML documentation with Doxygen for each CAA
######################################################################################	
.PHONY: documentation
documentation:
	$(call doc_creation)
    

