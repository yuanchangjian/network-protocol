# Main makefile: run make in all subdirectories
# and perform few checks on what needs to be here
# to be able to build Mobilygen applications

#===============================================================================
#
# The content of this file or document is CONFIDENTIAL and PROPRIETARY
# to Mobilygen Corporation.  It is subject to the terms of a
# License Agreement between Licensee and Mobilygen Corporation.
# restricting among other things, the use, reproduction, distribution
# and transfer.  Each of the embodiments, including this information and
# any derivative work shall retain this copyright notice.
#
# Copyright 2006 Mobilygen Corporation.
# All rights reserved.
#
# QuArc is a registered trademark of Mobilygen Corporation.
#
#===============================================================================

# MODULE_NAME: Name of the module, this will be used to generate the name of the resulting file
MODULE_NAME=NetworkProtocol

# MODULE_TYPE: What is this makefile suppose to build. Can be: exec, lib, solib or none
MODULE_TYPE=solib

# Content is public
MODULE_PUBLIC_SRC=1

# MODULE_SUPPORTED_TARGETS: If this module cannot run on every target
# the supported target should be listed here
MODULE_SUPPORTED_TARGETS=

MODULES_DEF_TAGS=

# MODULE_SUBDIRS: Subdirectories that the make system should enter
MODULE_SUBDIRS=

# MODULE_SRC:
# The common files needed by controller.

MODULE_SRC= $(wildcard *.cpp)

# MODULE_PUBLIC_HEADERS: Include files that are part of this module
MODULE_PUBLIC_HEADERS=

# MODULE_LIBS: Additional libraries (without the path, nor lib, nor .a or .so)
MODULE_LIBS= rt dl pthread dbus-c++-1 dbus-1 xml2 moon-1 moon-1-luabind configuredb lua53 readline osports

MOON1LOC= 

MODULE_INCLUDE_PATH= .
MODULE_INCLUDE_PATH+= $(MODULE_DIR)
MODULE_INCLUDE_PATH+= $(MOON1LOC)\
					$(INSTALL_INC_DIR)/dbus-c++-1\
					$(INSTALL_INC_DIR)/dbus-1.0\
					$(INSTALL_INC_DIR)/libxml2

# MODULE_DEFINES: Additional defines
MODULE_DEFINES=

#if needed:
#MODULE_DEFINES+= CONTROLLER_AS_MOBI_PLUGINS

# MODULE_CPPFLAGS: Module specific Flags to give to the pre-processor
MODULE_CPPFLAGS=

# MODULE_CFLAGS: Module specific Flags to give to the compiler when building C files
MODULE_CFLAGS=

# MODULE_CXXFLAGS: Module specific Flags to give to the compiler when building C++ files
MODULE_CXXFLAGS=-rdynamic

# MODULE_ASFLAGS: Module specific Flags to give to the compiler when building assembly files
MODULE_ASFLAGS=

# MODULE_LDFLAGS: Module specific Flags to give to the linker when linking files
MODULE_LDFLAGS=-rdynamic

ifneq ($(EMAKE_DIR),)
include $(EMAKE_DIR)/config.mk
else
include $(QUARC_ROOT)/merlinsw/host/make/config.mk
endif

# TARGET_SUPERCLEAN: list of targets run when user call superclean, AFTER superclean is run
TARGET_SUPERCLEAN+=

# TARGET_CLEAN: list of targets run when user call clean, BEFORE clean is run
TARGET_CLEAN+=

# TARGET_DEPS: list of targets run BEFORE dependencies are generated
TARGET_DEPS+=

# TARGET_BUILD: list of targets run when user call build  (or all), AFTER build is run
TARGET_BUILD+=

# TARGET_INSTALL: list of targets run when user call install (or all), BEFORE install is run
#TARGET_INSTALL+=

include $(RULES_MK)



