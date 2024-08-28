
CNZ_NAME ?= cdh_cnz
CAA_NAME ?= cdhadm_caa
CXC_NAME ?= cdh_cxc

# Basic paths
VOB_PATH = $(AES_ROOT)
CNZ_PATH = $(VOB_PATH)/$(CNZ_NAME)
CAA_PATH = $(CNZ_PATH)/$(CAA_NAME)
CXC_PATH = $(CNZ_PATH)/$(CXC_NAME)
                                  
include $(COMMON_ROOT)/common.mk
                                 
DOXYGENFILECONFIG := $(COMMON_ROOT)/doxygen_common.conf

CURDIR = $(shell pwd)
CXCPATH= $(CURDIR)
CXC_PATH=$(CXCPATH)
OUTDIR = ./bin

MODULE_SEARCH = ./src ./inc

# Setting the application name
APP_FILENAME = cdhftp

## Source directories. Here you can list your source directories. Example: SRCDIR src1 src2 src3 (use spaces)
SRCDIRS ?= src

## Source files
SRCFILES = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
SRCFILES += $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))

## Object files
MODULE_OBJS =	\
	$(OBJDIR)/aes_cdh_Linftpiv2.obj \
	$(OBJDIR)/main_ftp.obj \
	
OUT_FILENAME = $(OUTDIR)/$(APP_FILENAME)

INCGCCEXT= $(AES_ROOT))/gcc_cnz/gccapi_caa/inc_ext
LIBGCCEXT= $(AES_ROOT)/gcc_cnz/gcc_cxc/bin/lib_ext

# here you can add own Include paths and/or other includes before those included by common.mk file
CINCLUDES += -I"$(CAA_CDHFTP_DIR)/inc" -I$(LIB_CURL_PATH) -I"./inc"
CINCLUDES += -I"$(AP_SDK_INC)"

LIBSDIR += -L$(LIB_CURL_SDK_LIB) -L$(LIBGCCEXT)

# here you can add own libs -lfms_cpf
LIBS = -lstdc++ -lrt -lpthread  -lcurl -lidn -laes_gcc

VPATH += $(SRCDIR) $(OUTDIR) $(INCDIR) $(OBJDIR)

.PHONY: all CFLAGS += $(GCOV_FLAGS)
all: $(OUTDIR)/$(OUT_FILENAME)

$(OUTDIR)/$(OUT_FILENAME): $(MODULE_OBJS)
	$(NEW_LINE)
	$(SEPARATOR_STR)
	$(SILENT)$(ECHO) 'Creating Application: $(OUT_FILENAME)'
	$(SILENT)$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -o $(OUT_FILENAME) $(MODULE_OBJS) $(LDFLAGS) $(LIBSDIR) $(LIBS)
	$(SEPARATOR_STR)
	$(NEW_LINE)

	
.PHONY: clean
clean:
	$(SILENT)$(RM) $(wildcard $(OBJDIR)/*)
	

	
