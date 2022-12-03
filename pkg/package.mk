__DIR_PATH := pkg/

__CURRENT_MKFILES := \
	can_smbus/subdir.mk \
	odom_driver/subdir.mk \
	robomas/subdir.mk \
	controller_client/subdir.mk \

SUBDIR_MAKEFILE += $(addprefix $(__DIR_PATH),$(__CURRENT_MKFILES))


__MY_SRCS := package.cpp

__MY_SRC_DIR := 

__MY_SRCS := $(addprefix $(__DIR_PATH),$(__MY_SRCS))
__MY_SRC_DIR := $(addprefix $(__DIR_PATH),$(__MY_SRC_DIR))
__MY_SRCS += $(wildcard $(addsuffix /*.c,$(__MY_SRC_DIR)))
__MY_SRCS += $(wildcard $(addsuffix /*.cpp,$(__MY_SRC_DIR)))


__INC_PATH := \

ALL_SRC += $(__MY_SRCS)

__INC_PATH := $(addprefix $(__DIR_PATH),$(__INC_PATH))
LOCAL_INC_PATH += $(__INC_PATH)

__MY_ObJS := $(addsuffix .o,$(__MY_SRCS))
__MY_ObJS := $(addprefix $(BUILD_PATH), $(__MY_ObJS))
ALL_OBJ += $(__MY_ObJS)

$(info -----------$(__DIR_PATH)-----------)
$(info __INC_PATH = $(__INC_PATH))
$(info __MY_SRCS = $(__MY_SRCS))
# $(info __MY_ObJS = $(__MY_ObJS))