__DIR_PATH := webgl_map

__MY_SRCS :=

__MY_SRC_DIR := \
	\

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