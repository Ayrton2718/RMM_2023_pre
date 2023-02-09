__DIR_PATH := lrf_localization

__MY_SRCS :=

__MY_SRC_DIR := \
	\

LRF_SIM := ON
LRF_SIM ?= OFF
ifeq ($(LRF_SIM),OFF)
LFLAGS += $(shell pkg-config YDLIDAR_SDK --libs)
CPPFLAGS += $(shell pkg-config YDLIDAR_SDK --cflags)
CPPFLAGS += -std=c++11
else
LFLAGS += $(shell pkg-config opencv4 --libs)
# LFLAGS += -lpynq
# LFLAGS += -lcma
CPPFLAGS += $(shell pkg-config opencv4 --cflags)
CPPFLAGS += -std=c++11
CFLAGS += -DLL_LRF_SIM
endif

# __MY_SRCS := $(addprefix $(__DIR_PATH),$(__MY_SRCS))
__MY_SRC_DIR := $(addprefix $(__DIR_PATH),$(__MY_SRC_DIR))
__MY_SRC_DIR += $(__DIR_PATH)
__MY_SRCS = $(wildcard $(addsuffix /*.c,$(__MY_SRC_DIR)))
__MY_SRCS += $(wildcard $(addsuffix /*.cpp,$(__MY_SRC_DIR)))

ALL_SRC += $(__MY_SRCS)

__MY_ObJS := $(addsuffix .o,$(__MY_SRCS))
__MY_ObJS := $(addprefix $(BUILD_PATH), $(__MY_ObJS))
ALL_OBJ += $(__MY_ObJS)

$(info -----------$(__DIR_PATH)-----------)
$(info __MY_SRCS = $(__MY_SRCS))
# $(info __MY_ObJS = $(__MY_ObJS))