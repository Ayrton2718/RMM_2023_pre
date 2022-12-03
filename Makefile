CC ?= gcc
CPP ?= g++

MKDIR := mkdir -p
RM := rm -rf

# 'Debug' or 'Release'
BUILD_MODE  = Release

CFLAGS :=
CPPFLAGS :=
LFLAGS :=
LOCAL_INC_PATH :=
LOCAL_LIB_PATH :=
ALL_SRC :=
ALL_OBJ :=
ALL_DFP :=

-include os_define.mk

$(info UNAME = $(UNAME))
$(info SYSTEM = $(SYSTEM))
$(info OSX_VER = $(OSX_VER))
$(info BUILD_MODE = $(BUILD_MODE))
$(info CPP_VER = $(CPP_VER))
$(info OPTLIBS = $(OPTLIBS))
$(info SUBARCH = $(SUBARCH))

BUILD_PATH = $(addprefix build/,$(SYSTEM)/)
BUILD_PATH := $(addprefix $(BUILD_PATH),$(SUBARCH)/)
BUILD_PATH := $(addprefix $(BUILD_PATH),$(BUILD_MODE)/)
$(info BUILD_PATH = $(BUILD_PATH))

TARGET := main
TARGET := $(addprefix $(BUILD_PATH),$(TARGET))
TARGET := $(addsuffix $(FEXT),$(TARGET))
$(info TARGET = $(TARGET))

ifeq ($(BUILD_MODE),Debug)
CFLAGS += -g
else
CFLAGS += -O2
endif


SUBDIR_MAKEFILE := \
	subdir.mk

-include lib/library.mk
-include pkg/package.mk
-include user_lib.mk

-include $(SUBDIR_MAKEFILE)
$(info ----------------------)


LOCAL_INCLUDE = $(addprefix -I, $(LOCAL_INC_PATH))
LOCAL_LIBLARY = $(addprefix -I, $(LOCAL_LIB_PATH))
ALL_DFP = $(subst .o,.d,$(ALL_OBJ))

$(info CFLAGS = $(CFLAGS))
$(info CPPFLAGS = $(CPPFLAGS))
$(info LFLAGS = $(LFLAGS))
$(info LOCAL_INC_PATH = $(LOCAL_INC_PATH))
$(info LOCAL_LIB_PATH = $(LOCAL_LIB_PATH))
$(info ALL_SRC = $(ALL_SRC))
# $(info ALL_OBJ = $(ALL_OBJ))
# $(info ALL_DFP = $(ALL_DFP))


all: $(TARGET) build/main.out

build/main.out: $(TARGET)
	@cp $(TARGET) build/

$(TARGET): $(ALL_OBJ) Makefile $(SUBDIR_MAKEFILE)
	$(LK) $(CFLAGS) $(CPPFLAGS) $(ALL_OBJ) $(LOCAL_LIBLARY) $(LFLAGS) -o $(TARGET)

$(BUILD_PATH)%.c.o: %.c
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $(LOCAL_INCLUDE) -c $< -o $@

$(BUILD_PATH)%.cpp.o: %.cpp
	@$(MKDIR) $(dir $@)
	$(CPP) $(CFLAGS) $(CPPFLAGS) $(LOCAL_INCLUDE) -c $< -o $@

$(BUILD_PATH)%.c.d: %.c Makefile $(SUBDIR_MAKEFILE)
	@$(MKDIR) $(dir $@)
	$(CC) -MM $(CFLAGS) $(LOCAL_INCLUDE) $< | sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@

$(BUILD_PATH)%.cpp.d: %.cpp Makefile $(SUBDIR_MAKEFILE)
	@$(MKDIR) $(dir $@)
	$(CPP) -MM $(CFLAGS) $(CPPFLAGS) $(LOCAL_INCLUDE) $< | sed 's/$(notdir $*)\.o:/$(subst /,\/,$(patsubst %.d,%.o,$@) $@):/' > $@

clean:
	$(RM) build

.PHONY: clean

ifneq ($(filter clean,$(MAKECMDGOALS)),clean)
include $(ALL_DFP)
endif