__DIR_PATH := pkg/odom_driver/

__MY_SRCS :=

__MY_SRC_DIR := \
	src \

__MY_SRCS := $(addprefix $(__DIR_PATH),$(__MY_SRCS))
__MY_SRC_DIR := $(addprefix $(__DIR_PATH),$(__MY_SRC_DIR))
__MY_SRCS = $(wildcard $(addsuffix /*.c,$(__MY_SRC_DIR)))
__MY_SRCS += $(wildcard $(addsuffix /*.cpp,$(__MY_SRC_DIR)))


__INC_PATH := \
	inc \

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