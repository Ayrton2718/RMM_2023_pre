__DIR_PATH := 

__CURRENT_MKFILES := \
	lrf_localization/subdir.mk \
	webgl_map/subdir.mk \

SUBDIR_MAKEFILE += $(addprefix $(__DIR_PATH),$(__CURRENT_MKFILES))