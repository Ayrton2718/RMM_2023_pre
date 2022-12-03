# platform switcher (Windows, Linux, OS-X)
ifeq ($(CROSS_COMPILE), Rpi4B)
    FEXT  = .out
    ICON_RC =
    UNAME := Raspberry
    SYSTEM := Linux
    CPP_VER := -std=c++17
    CC      = aarch64-linux-gnu-gcc
    CPP     = aarch64-linux-gnu-g++
    LK      = aarch64-linux-gnu-g++
    LFLAGS  += -pthread
    CFLAGS += -march=armv8-a -mcpu=cortex-a72 -mtune=cortex-a72
    CPPFLAGS +=

    LFLAGS += \
        -lm \

    SUBARCH := arm64v8_rpi4B
    OPTLIBS =
else
    ifeq ($(OS),Windows_NT)
        $(error Windows wasn't Supported.)
        # FEXT    = .exe
        # ICON_RC = icon.rc
        # SYSTEM := Win
        # CPP_VER := -std=c++17
        # LOCAL_INC_PATH := /mingw64/include
        # LOCAL_LIB_PATH := /mingw64/lib
        # OPTLIBS =
        # CP    =   g++
        # CC    =   gcc
        # CFLAGS  += -DWIN32
        # PFLAGS  += -DWIN32 -DBOOST_USE_WINDOWS_H
        # LFLAGS +=
    else
        FEXT  = .out
        ICON_RC =
        UNAME := $(shell uname -s)
        ifeq ($(UNAME),Linux)
            SYSTEM := Linux
            CPP_VER := -std=c++17
            CC      = gcc
            CPP     = g++
            LK      = g++
            LFLAGS  += -pthread
            CFLAGS += 
            CPPFLAGS +=
            # LFLAGS += -ldns_sd \

        endif
        ifeq ($(UNAME),Darwin)
            SYSTEM := OSX
            # OSX_VER := $(shell sw_vers -productVersion | sed 's/^\([0-9]*.[0-9]*\).[0-9]*/\1/')
            CPP_VER := -std=c++14
            CC      = gcc
            CPP     = g++
            LK      = g++
            CFLAGS += 
            CPPFLAGS +=
            LFLAGS += -lpthread \
            
        endif

        LFLAGS += \
            -lm \

        SUBARCH ?= $(shell uname -m)
        OPTLIBS =
    endif
endif