## @file
#  SPDM library.
#
#  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

#
# Platform Macro Definition
#

include $(WORKSPACE)/GNUmakefile.Flags

#
# Module Macro Definition
#
MODULE_NAME = SpdmResponderTest
BASE_NAME = $(MODULE_NAME)

#
# Build Directory Macro Definition
#
BUILD_DIR = $(WORKSPACE)/Build
BIN_DIR = $(BUILD_DIR)/$(TARGET)_$(TOOLCHAIN)/$(ARCH)
OUTPUT_DIR = $(BIN_DIR)/OsTest/$(MODULE_NAME)

SOURCE_DIR = $(WORKSPACE)/OsTest/$(MODULE_NAME)

#
# Build Macro
#

OBJECT_FILES =  \
    $(OUTPUT_DIR)/SpdmResponder.o \
    $(OUTPUT_DIR)/SpdmResponderMeasurement.o \
    $(OUTPUT_DIR)/SpdmResponderTest.o \
    $(OUTPUT_DIR)/SpdmTestCommand.o \
    $(OUTPUT_DIR)/SpdmTestKey.o \
    $(OUTPUT_DIR)/SpdmTestSupport.o \


STATIC_LIBRARY_FILES =  \
    $(BIN_DIR)/OsStub/BaseMemoryLib/BaseMemoryLib.a \
    $(BIN_DIR)/OsStub/DebugLib/DebugLib.a \
    $(BIN_DIR)/OsStub/BaseCryptLib$(CRYPTO)/BaseCryptLib$(CRYPTO).a \
    $(BIN_DIR)/OsStub/$(CRYPTO)Lib/$(CRYPTO)Lib.a \
    $(BIN_DIR)/OsStub/MemoryAllocationLib/MemoryAllocationLib.a \
    $(BIN_DIR)/Library/SpdmLib/SpdmCommonLib/SpdmCommonLib.a \
    $(BIN_DIR)/Library/SpdmLib/SpdmResponderLib/SpdmResponderLib.a \
    $(BIN_DIR)/Library/SpdmLib/SpdmEncodingLib/SpdmEncodingLib.a \
    $(OUTPUT_DIR)/$(MODULE_NAME).a \


STATIC_LIBRARY_OBJECT_FILES =  \
    $(BIN_DIR)/OsStub/BaseMemoryLib/*.o \
    $(BIN_DIR)/OsStub/DebugLib/*.o \
    $(BIN_DIR)/OsStub/BaseCryptLib$(CRYPTO)/*.o \
    $(BIN_DIR)/OsStub/$(CRYPTO)Lib/*.o \
    $(BIN_DIR)/OsStub/MemoryAllocationLib/*.o \
    $(BIN_DIR)/Library/SpdmLib/SpdmCommonLib/*.o \
    $(BIN_DIR)/Library/SpdmLib/SpdmResponderLib/*.o \
    $(BIN_DIR)/Library/SpdmLib/SpdmEncodingLib/*.o \
    $(OUTPUT_DIR)/*.o \


INC =  \
    -I$(SOURCE_DIR) \
    -I$(WORKSPACE)/OsTest/SpdmTestCommon \
    -I$(WORKSPACE)/Include \
    -I$(WORKSPACE)/Include/Hal \
    -I$(WORKSPACE)/Include/Hal/$(ARCH)

#
# Overridable Target Macro Definitions
#
INIT_TARGET = init
CODA_TARGET = $(OUTPUT_DIR)/$(MODULE_NAME)

#
# Default target, which will build dependent libraries in addition to source files
#

all: mbuild

#
# ModuleTarget
#

mbuild: $(INIT_TARGET) gen_libs $(CODA_TARGET)

#
# Initialization target: print build information and create necessary directories
#
init:
	-@$(MD) $(OUTPUT_DIR)

#
# GenLibsTarget
#
gen_libs:
	@"$(MAKE)" $(MAKE_FLAGS) -f $(WORKSPACE)/OsStub/BaseMemoryLib/$(MAKEFILE) ARCH=$(ARCH) TARGET=$(TARGET) TOOLCHAIN=$(TOOLCHAIN) CRYPTO=$(CRYPTO)
	@"$(MAKE)" $(MAKE_FLAGS) -f $(WORKSPACE)/OsStub/DebugLib/$(MAKEFILE) ARCH=$(ARCH) TARGET=$(TARGET) TOOLCHAIN=$(TOOLCHAIN) CRYPTO=$(CRYPTO)
	@"$(MAKE)" $(MAKE_FLAGS) -f $(WORKSPACE)/OsStub/BaseCryptLib$(CRYPTO)/$(MAKEFILE) ARCH=$(ARCH) TARGET=$(TARGET) TOOLCHAIN=$(TOOLCHAIN) CRYPTO=$(CRYPTO)
	@"$(MAKE)" $(MAKE_FLAGS) -f $(WORKSPACE)/OsStub/$(CRYPTO)Lib/$(MAKEFILE) ARCH=$(ARCH) TARGET=$(TARGET) TOOLCHAIN=$(TOOLCHAIN) CRYPTO=$(CRYPTO)
	@"$(MAKE)" $(MAKE_FLAGS) -f $(WORKSPACE)/OsStub/MemoryAllocationLib/$(MAKEFILE) ARCH=$(ARCH) TARGET=$(TARGET) TOOLCHAIN=$(TOOLCHAIN) CRYPTO=$(CRYPTO)
	@"$(MAKE)" $(MAKE_FLAGS) -f $(WORKSPACE)/Library/SpdmLib/SpdmCommonLib/$(MAKEFILE) ARCH=$(ARCH) TARGET=$(TARGET) TOOLCHAIN=$(TOOLCHAIN) CRYPTO=$(CRYPTO)
	@"$(MAKE)" $(MAKE_FLAGS) -f $(WORKSPACE)/Library/SpdmLib/SpdmResponderLib/$(MAKEFILE) ARCH=$(ARCH) TARGET=$(TARGET) TOOLCHAIN=$(TOOLCHAIN) CRYPTO=$(CRYPTO)
	@"$(MAKE)" $(MAKE_FLAGS) -f $(WORKSPACE)/Library/SpdmLib/SpdmEncodingLib/$(MAKEFILE) ARCH=$(ARCH) TARGET=$(TARGET) TOOLCHAIN=$(TOOLCHAIN) CRYPTO=$(CRYPTO)

#
# Individual Object Build Targets
#
$(OUTPUT_DIR)/SpdmResponder.o : $(SOURCE_DIR)/SpdmResponder.c
	$(CC) $(CC_FLAGS) -o $@ $(INC) $^

$(OUTPUT_DIR)/SpdmResponderMeasurement.o : $(SOURCE_DIR)/SpdmResponderMeasurement.c
	$(CC) $(CC_FLAGS) -o $@ $(INC) $^

$(OUTPUT_DIR)/SpdmResponderTest.o : $(SOURCE_DIR)/SpdmResponderTest.c
	$(CC) $(CC_FLAGS) -o $@ $(INC) $^

$(OUTPUT_DIR)/SpdmTestCommand.o : $(SOURCE_DIR)/../SpdmTestCommon/SpdmTestCommand.c
	$(CC) $(CC_FLAGS) -o $@ $(INC) $^

$(OUTPUT_DIR)/SpdmTestKey.o : $(SOURCE_DIR)/../SpdmTestCommon/SpdmTestKey.c
	$(CC) $(CC_FLAGS) -o $@ $(INC) $^

$(OUTPUT_DIR)/SpdmTestSupport.o : $(SOURCE_DIR)/../SpdmTestCommon/SpdmTestSupport.c
	$(CC) $(CC_FLAGS) -o $@ $(INC) $^

$(OUTPUT_DIR)/$(MODULE_NAME).a : $(OBJECT_FILES)
	$(RM) $(OUTPUT_DIR)/$(MODULE_NAME).a
	$(SLINK) cr $@ $(SLINK_FLAGS) $^ $(SLINK_FLAGS2)

$(OUTPUT_DIR)/$(MODULE_NAME) : $(STATIC_LIBRARY_FILES)
	@echo $(BIN_DIR)/OsStub/BaseMemoryLib/BaseMemoryLib.a > $(OUTPUT_DIR)/tmp.list
	@echo $(BIN_DIR)/OsStub/DebugLib/DebugLib.a >> $(OUTPUT_DIR)/tmp.list
	@echo $(BIN_DIR)/OsStub/BaseCryptLib$(CRYPTO)/BaseCryptLib$(CRYPTO).a >> $(OUTPUT_DIR)/tmp.list
	@echo $(BIN_DIR)/OsStub/$(CRYPTO)Lib/$(CRYPTO)Lib.a >> $(OUTPUT_DIR)/tmp.list
	@echo $(BIN_DIR)/OsStub/MemoryAllocationLib/MemoryAllocationLib.a >> $(OUTPUT_DIR)/tmp.list
	@echo $(BIN_DIR)/Library/SpdmLib/SpdmCommonLib/SpdmCommonLib.a >> $(OUTPUT_DIR)/tmp.list
	@echo $(BIN_DIR)/Library/SpdmLib/SpdmResponderLib/SpdmResponderLib.a >> $(OUTPUT_DIR)/tmp.list
	@echo $(BIN_DIR)/Library/SpdmLib/SpdmEncodingLib/SpdmEncodingLib.a >> $(OUTPUT_DIR)/tmp.list
	@echo $(OUTPUT_DIR)/$(MODULE_NAME).a >> $(OUTPUT_DIR)/tmp.list
	$(DLINK) $(DLINK_FLAGS) $(DLINK_SPATH) $(DLINK_OBJECT_FILES) $(DLINK_FLAGS2)

#
# clean all intermediate files
#
clean:
	$(RD) $(OUTPUT_DIR)


