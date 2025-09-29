
# makefile to compile Sniffer for SONOFF Zigbee 3.0 USB DONGLE Plus-E
# intended to be called from project base directory
# basic usage:
# make all -f .\Sources\Target\Sonoff_USB_Dongle_Plus_E\makefile

# Please note this makefile contains some comments not relevant
# to the project compilation but are kept for references

# All C files are compiled to same object folder OUTPUT_DIR
# Source files filename must be unique among project scope

# To perform a dry run that shows the evaluation of make, use:
#make.exe build -f makefile -p -n -r > test_output.txt

# useful method to print makefile variable
# $(info    var is $(VAR))
# or alternatively to print all make variables
# $(foreach v, $(.VARIABLES), $(info $(v) = $($(v))))

# adding element to path under windows
# PowerSheel:
# $Env:Path = $Env:Path + ";D:/prog/GnuWin32/bin"
# cmd.exe:
# set PATH=%PATH%;D:/prog/GnuWin32/bin

#ignore implicit rules
.SUFFIXES:
MAKEFLAGS += --no-builtin-rules

#make sure "clean" "all" "rebuild" "build" are never interpreted as a filename
.PHONY: all clean rebuild build

#Default rule (aka target)
#This is the target that will be built if make is called without any target specified
all : rebuild

DEFINES = TARGET_SONOFF_USB_DONGLE_PLUS_E

OUTPUT_NAME = Sniffer_802.15.4_SONOFF_USB_Dongle_Plus_E


actual_path := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
sources_location :=           $(dir $(realpath $(lastword $(MAKEFILE_LIST))))../..

$(info    actual_path is $(actual_path))
$(info    sources_location is $(sources_location))

#uncomment to serialize make execution to avoid console output mess
.NOTPARALLEL:

####################################################################
# Source files                                                     #
####################################################################
SRCS := 		\
		./Sources/main.c														\
		./Sources/SnifferSharedComponents/802.15.4/phy.c						\
		./Sources/SnifferSharedComponents/802.15.4/mac.c						\
		./Sources/SnifferSharedComponents/802.15.4/mac_unpack.c					\
		./Sources/SnifferSharedComponents/Console/console.c						\
		./Sources/SnifferSharedComponents/Console/printf.c						\
		./Sources/SnifferSharedComponents/crc/crc.c								\
		./Sources/HAL/SiliconLabs/SDK/gecko_sdk_3.1.1/platform/emlib/src/em_assert.c	\
		./Sources/HAL/SiliconLabs/SDK/gecko_sdk_3.1.1/platform/emlib/src/em_cmu.c 		\
		./Sources/HAL/SiliconLabs/SDK/gecko_sdk_3.1.1/platform/emlib/src/em_core.c 		\
		./Sources/HAL/SiliconLabs/SDK/gecko_sdk_3.1.1/platform/emlib/src/em_gpio.c 		\
		./Sources/HAL/SiliconLabs/SDK/gecko_sdk_3.1.1/platform/emlib/src/em_system.c 	\
		./Sources/HAL/SiliconLabs/SDK/gecko_sdk_3.1.1/platform/emlib/src/em_usart.c 	\
		./Sources/HAL/SiliconLabs/EFR32MG21/Source/GCC/startup_efr32mg21.c				\
		./Sources/HAL/SiliconLabs/EFR32MG21/Source/system_efr32mg21.c					\
		./Sources/HAL/SiliconLabs/Hal_Clocks.c									\
		./Sources/HAL/SiliconLabs/Hal_Radio.c									\
		./Sources/HAL/SiliconLabs/Hal_Console.c									\
		./Sources/Target/Sonoff_USB_Dongle_Plus_E/BSP_Sonoff_USB_Dongle_Plus_E.c


####################################################################
# Included directories											   #
# Directories searched for included files                          #
####################################################################
INCLUDE_DIR := 	\
		./Sources                        							\
		./Sources/SnifferSharedComponents/802.15.4					\
		./Sources/SnifferSharedComponents/Console					\
		./Sources/SnifferSharedComponents/crc						\
		./Sources/SnifferSharedComponents/json_parser				\
		./Sources/HAL												\
		./Sources/HAL/SiliconLabs   								\
		./Sources/HAL/SiliconLabs/bootloaderInterface				\
		./Sources/HAL/SiliconLabs/EFR32MG21/Include					\
		./Sources/HAL/SiliconLabs/SDK/gecko_sdk_3.1.1/platform/emlib/inc								\
		./Sources/HAL/SiliconLabs/SDK/gecko_sdk_3.1.1/platform/radio/rail_lib/chip/efr32/efr32xg2x		\
		./Sources/HAL/SiliconLabs/SDK/gecko_sdk_3.1.1/platform/radio/rail_lib/common					\
		./Sources/HAL/SiliconLabs/SDK/gecko_sdk_3.1.1/platform/radio/rail_lib/protocol/ieee802154		\
		./Sources/HAL/CMSIS/Include


####################################################################
# Toolchain														   #
####################################################################
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
OBJSIZE = arm-none-eabi-size
COMMANDER = commander

$(info    OS is $(OS))
# Check if gcc is accessible
# https://stackoverflow.com/questions/5618615/check-if-a-program-exists-from-a-makefile
ifeq ($(OS),Windows_NT)
  # Under windows, user can specify a specific path to gcc by using an environement variable named ARM_GCC_DIR_WIN
  # To add a temporary environment variable use the following commands
  # set ARM_GCC_DIR_WIN=D:/prog/gcc/gcc-arm-none-eabi-10.3-2021.10/bin/
  # To add a permanent system variable use "edit system environment variables"
  # verify if gcc is runnable using environment variable ARM_GCC_DIR_WIN
  CC := $(CC).exe
  OBJCOPY := $(OBJCOPY).exe
  OBJDUMP := $(OBJDUMP).exe
  OBJSIZE := $(OBJSIZE).exe

  # Commander used to package GBL file
  # Silicon Labs commander.exe must be installed on the local machine
  # Windows: the PATH variable must have an entry to the folder containing commander.exe
  # 		   or have an environement variable named SILABS_COMMANDER_DIR_WIN specifying the path
  #		   to commander folder
  COMMANDER := $(COMMANDER).exe

  ifneq ($(ARM_GCC_DIR_WIN),)
    ## use command "where" with arguments using path:pattern to find gcc
	ifeq (,$(shell where $(ARM_GCC_DIR_WIN):$(CC) 2>NUL))
      $(info Can't find $(CC))
      $(info Verify location specified by system variable ARM_GCC_DIR_WIN)
      $(error "Actual ARM_GCC_DIR_WIN is $(ARM_GCC_DIR_WIN)")
    endif
    CC := $(ARM_GCC_DIR_WIN)/$(CC)
    OBJCOPY := $(ARM_GCC_DIR_WIN)/$(OBJCOPY)
    OBJDUMP := $(ARM_GCC_DIR_WIN)/$(OBJDUMP)
	OBJSIZE := $(ARM_GCC_DIR_WIN)/$(OBJSIZE)
  else
  # verify if gcc is accessible in PATH environement variable, using "where"
    ifeq (,$(shell where $(CC) 2>NUL))
      $(info Can't find $(CC))
      $(info Install gcc and add its location to system environment variable PATH, or add a system variable named ARM_GCC_DIR_WIN that specify the gcc install location)
      $(error "Actual PATH is $(PATH)")
    endif
  # command line in windows powershell to an entry to path
  # $Env:Path = $Env:Path + ";D:/prog/gcc/gcc-arm-none-eabi-10.3-2021.10/bin/"
  endif

  # Verify if we can run Silabs commander.exe utility
  # commander is used to package a .gbl file
  ifneq ($(SILABS_COMMANDER_DIR_WIN),)
    ## use command "where" with arguments using path:pattern
	ifeq (,$(shell where $(SILABS_COMMANDER_DIR_WIN):$(COMMANDER) 2>NUL))
      $(info Can't find $(COMMANDER))
      $(info Verify location specified by system variable SILABS_COMMANDER_DIR_WIN)
      $(error "Actual SILABS_COMMANDER_DIR_WIN is $(SILABS_COMMANDER_DIR_WIN)")
    endif
    COMMANDER := $(SILABS_COMMANDER_DIR_WIN)/$(COMMANDER)
  else
  # verify if commander is accessible in PATH environement variable, using "where"
    ifeq (,$(shell where $(COMMANDER) 2>NUL))
      $(info Can't find $(COMMANDER))
      $(info Install Silicon Labs commander.exe and add its location to system environment variable PATH, or add a system variable named SILABS_COMMANDER_DIR_WIN that specify the install location)
      $(error "Actual PATH is $(PATH)")
    endif
  endif

else
  #building under linux and/or inside docker container
endif


#compiler option
#-Og : optimize for debugging
#-g : generate debug information
#-ggdb : generate debug information for use by gdb
#-fdata-sections and -ffunction-sections tells the compiler to put data and function in different sections
#so linker will not link unused function with -Wl,--gc-sections
CFLAGS = -Og \
		 -ggdb \
		 -mthumb \
		 -mcpu=cortex-m33 \
		 -mfpu=fpv5-sp-d16 \
		 -mfloat-abi=hard \
		 -include "./Sources/buildCfg.h" \
		 -save-temps=obj \
		 -fdata-sections \
		 -ffunction-sections \
		 -fomit-frame-pointer \
		 -fno-builtin \
		 -std=c99 \
		 -Wall	\
		 -Wextra \
		 -Wno-comment	\
		 -Wno-int-in-bool-context \
		 --specs=nano.specs \
		 -MMD -MP -MF $(@:.o=.d)

#Linker options
#-nostdlib
#-lm : search for library libm  (prefix lib is automatically added with -l)
#-Wl,--gc-sections : garbage collect unused sections
LDFLAGS = -mthumb \
		  -mcpu=cortex-m33 \
		  -mfpu=fpv5-sp-d16 \
		  -mfloat-abi=hard \
		  -Wl,-Map="$(OUTPUT_NAME).map" \
		  -Wl,--gc-sections \
		  -nostartfiles \
		  -nodefaultlibs \
		  -L"./Sources/HAL/SiliconLabs/SDK/gecko_sdk_3.1.1/platform/radio/rail_lib/autogen/librail_release" \
		  -l:librail_efr32xg21_gcc_release.a \
		  -lgcc \
		  -lc_nano \
		  -T"Sources/Target/Sonoff_USB_Dongle_Plus_E/EFR32MG21A020F768IM32_for_bootloader.ld"

		  #use this linker file to build without bootloader support
		  #-T"Sources/Target/Sonoff_USB_Dongle_Plus_E/EFR32MG21A020F768IM32_flash.ld"

		  #use this linker file to build with bootloader support
		  #-T"Sources/Target/Sonoff_USB_Dongle_Plus_E/EFR32MG21A020F768IM32_for_bootloader.ld"

		  #-lm -lc_nano \
		  #--specs=nano.specs \
		  #-specs=nosys.specs \
		  #-Xlinker -Map=$(OUTPUT_DIR)/$(PROJECTNAME).map 	## is an alternative to -Wl,-Map="$(OUTPUT_DIR)/$(PROJECTNAME).map

#libraries are copied from:
# D:/prog/gcc/lib/gcc/arm-none-eabi/7.3.1/thumb
#read this for
#https://stackoverflow.com/questions/44932748/printf-in-newlib-is-not-working-in-baremetal-arm-environment
#Supporting OS subroutines required: close, fstat, isatty, lseek, read, sbrk, write.

#libraries: How to include a library not using a standard name having "lib" prefix
# https://stackoverflow.com/questions/10234208/how-do-i-link-a-library-file-in-gcc-that-does-not-start-with-lib

rebuild :
	$(MAKE) clean -f Sources/Target/Sonoff_USB_Dongle_Plus_E/makefile
	$(MAKE) -j build -f Sources/Target/Sonoff_USB_Dongle_Plus_E/makefile

###############################################################################
# below should remain identical between different hardware
###############################################################################
#store all objects in a single folder
OUTPUT_DIR := Output/$(OUTPUT_NAME)
OUTPUT_NAME := $(OUTPUT_DIR)/$(OUTPUT_NAME)
OBJECT_DIR := $(OUTPUT_DIR)/OBJS
SRC_DIR := $(dir $(SRCS))

DUMPFLAGS = -d
DUMPFLAGS2 = -h -S

#same as SRCS except change file extension to .o example "main.c foo.c" will be "main.o foo.o"
#addprefix add a prefix to each object
#notdir keep only the filename
# OBJECTS := $(SRCS:%.c=%.o)
OBJECTS := $(addprefix $(OBJECT_DIR)/,$(notdir $(SRCS:%.c=%.o)))

#dependencies
#same as SRCS except change file extension to .d example "main.c foo.c" will be "main.d foo.d"
# .d file contains the list of header files being referenced by the .c file
# The .d file is created by gcc with -MMD option (alternative exist TIMTOWTDI)
#CDEPS := $(addprefix $(OBJECT_DIR)/,$(notdir $(SRCS:%.c=%.d)))
CDEPS := $(OBJECTS:%.o=%.d)

#put a -D in front of all defines
DEFINES_D := $(DEFINES:%=-D%)

#put a -I in front of all include folders
INCLUDE_DIR_I := $(INCLUDE_DIR:%=-I%)

#convert source path to vpath format (Separated with :, no leading ./, and no trailing /)
empty:=
space := $(empty) $(empty)
#remove current ./ folder
VPATH_DIR:=$(filter-out ./, $(SRC_DIR))
#remove leading ./ to each path
VPATH_DIR:=$(subst ./,, $(VPATH_DIR))
# #replace space with :
VPATH_DIR:=$(subst /$(space),:, $(VPATH_DIR))
#remove trailing / slash
VPATH_DIR:=$(VPATH_DIR:%/=%)
VPATH:=$(VPATH_DIR)

#goal if this makefile is to produce $(OUTPUT_NAME).elf
build : $(OUTPUT_NAME).elf

#Rule for .elf file
$(OUTPUT_NAME).elf : $(OBJECTS)
	$(CC) $(INCLUDE_DIR_I) -o $(OUTPUT_NAME).elf $(OBJECTS) $(LDFLAGS)
	$(OBJCOPY) -I elf32-littlearm -O binary "$(OUTPUT_NAME).elf" "$(OUTPUT_NAME).bin"
	$(OBJCOPY) -I elf32-littlearm -O ihex "$(OUTPUT_NAME).elf" "$(OUTPUT_NAME).hex"
	$(OBJDUMP) $(DUMPFLAGS) $(OUTPUT_NAME).elf > $(OUTPUT_NAME).S
	$(OBJDUMP) $(DUMPFLAGS2) $(OUTPUT_NAME).elf > $(OUTPUT_NAME).lss
	$(OBJSIZE) -x "$(OUTPUT_NAME).elf"
	$(COMMANDER) gbl create "$(OUTPUT_NAME).gbl" --app "$(OUTPUT_NAME).hex"

#eso notes
#$@ = actual target
#$(@D) = the directory the current target resides in
#$(@F) = the file name of the current target or prerequisite
# the | in the dependencies tell make that it should not check the modification date of this element and make it every time
$(OBJECT_DIR)/%.o : %.c | $(OBJECT_DIR)
	$(CC) -c $(CFLAGS) $(DEFINES_D) $(INCLUDE_DIR_I) $< -o $(OBJECT_DIR)/$(@F)
#	$(CC) -c $(CFLAGS) $(DEFINES_D) $(INCLUDE_DIR_I) $< -o $(OBJECT_DIR)/$(@F) 2>$(OBJECT_DIR)/$(@F:%.o=%.stderr)


# print_compilation_stderr :
# 	COMPILE_STDERR := $(OBJECTS:%.o=%.stderr)

#Windows note: mkdir requires " when using / as a char separator for folders
$(OBJECT_DIR):
ifeq ($(OS),Windows_NT)
	mkdir "$(OBJECT_DIR)"
else
	mkdir -p "$(OBJECT_DIR)"
endif

clean:
#only erase directory if it exist
ifneq ($(wildcard $(OUTPUT_DIR)),)
ifeq ($(OS),Windows_NT)
	-rmdir /q /s "$(OUTPUT_DIR)"
else
	-rm -r "$(OUTPUT_DIR)"
endif
endif


-include $(CDEPS)


#	@echo objects: $(OBJECTS)
#	@echo src dir: $(SRC_DIR)
#	@echo objects dir: $(OBJECT_DIR)
#	@echo vpath_dir: $(VPATH_DIR)
#	@echo vpath: $(VPATH)

