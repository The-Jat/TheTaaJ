

# Makefile with common rules and defines for the system
# Defines paths


# Sanitize for architecture
ifndef ARCH
ARCH = i386
# $(error ARCH is not set)
endif

# Sanitize for toolchain
ifndef CROSS
CROSS = ""
# $(error CROSS is not set)
endif

# := Immediate Assignment (can't changed later on)
# = Deferred Assignment (can be changed later on before using it)

# Setup project tools
CC := gcc
CXX := g++
LD := ld
AS := nasm
# LIB := $(CROSS)/bin/llvm-lib
# ANALYZER := scan-build --use-cc=$(CC) --use-c++=$(CXX)

# Setup project paths
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
config_path := $(abspath $(dir $(mkfile_path)))
workspace_path := $(abspath $(config_path)/../)
# arch_path := $(abspath $(workspace_path)/kernel/arch/$(VALI_ARCH))
# userspace_path := $(abspath $(workspace_path)/userspace)
# include_path := $(userspace_path)/include
# lib_path := $(userspace_path)/lib


# Include correct arch file
include $(dir $(mkfile_path))/$(ARCH)/rules.mk
