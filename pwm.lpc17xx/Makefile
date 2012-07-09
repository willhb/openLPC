# OpenLPC Makefile for Executables
#
# This file should be copied into a project
# and renamed to 'Makefile'. This Makefile only contains the
# targets and rules to build a project. The source files
# and nonstandard compiler options need to be specified
# in a seperate file called 'sources.inc'. This is so that
# the Makefile can be upgraded simply by overwriting 
# the old one with a new one. The file 'sources.program.inc'
# accompanies this sourcefile and can be used as a template for
# new projects.
#
# You should not need to edit this file. Try to make your changes
# in sources.inc, and only edit this file as a last result.
# Project-specific data is contained in sources.inc, which will not
# change between Makefile upgrades. Unless you have made changes
# to this file, it is save to clobber this with a new version
# of the Makefile.
#
# Makefile targets:
#   all: compile source files, link, create binary, and create
#        extended listing
#
#   program: copy the binary file onto the device
#
#   clean: remove all generated and temporary files
#          created in the build process
#

# What is the target chip for the compilation? Currently supported values are lpc17xx,lpc2148
CHIP = lpc17xx

# A subsystem consists of startup code and linker script
# for a particular operating environment. The currently
# supported subsystems are:
#
#    openlpc - this is the default. Use this subsystem
#      for applications running on top of the OpenLPC supervisor.
#      The output file is a .bin file which can be loaded onto
#      the board using the 'make program' command. Also contains
#      newlib stubs that use the supervisor for usb serial IO.
#
#    bare - subsystem for applications running bare metal.
#      The output file is a .hex file which can be loaded
#      onto the board using FlashMagic and the Philips on-chip
#      serial bootloader. Also contains IRQ wrappers.
#
#    custom - you must provide your own linker script and 
#      startup code. If you specify this option, you
#      MUST provide startup code and linker script or your
#      code will not link!! Startup assembly code can
#      be specifed in the ASRC list, and a linker script
#      can be specified with the LDSCRIPT variable
#
SUBSYS = openlpc

# Common flags shared by AFLAGS, CFLAGS, and CPPFLAGS
lpc17xx_FLAGS = -mcpu=cortex-m3  -mthumb -D__thumb2__=1 -msoft-float \
	 -fno-hosted  -mtune=cortex-m3 -march=armv7-m -mfix-cortex-m3-ldrd

lpc2148_FLAGS = -mcpu=arm7tdmi-s

# Compiler flags to generate dependency files.
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

# Assembler flags.
#  -Wa,...:   tell GCC to pass this to the assembler.
#  -ahlms:    create listing
#  -gstabs:   have the assembler create line number information; note that
#             for use in COFF files, additional information about filenames
#             and function names needs to be present in the assembler source
#             files -- see avr-libc docs [FIXME: not yet described there]
ASFLAGS = -Wa,-adhlns=$(<:.S=.lst) $(GENDEPFLAGS)

# Compiler flags.
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
#
# Flags for C
CFLAGS = -std=gnu99 -Wall -Wa,-adhlns=$(subst $(suffix $<),.lst,$<) $(GENDEPFLAGS)

# flags for C++
CPPFLAGS = -Wall -fno-exceptions -Wa,-adhlns=$(subst $(suffix $<),.lst,$<) $(GENDEPFLAGS)

# Linker flags.
#  -Wl,...:     tell GCC to pass this to linker.
#    -Map:      create map file
LDFLAGS = -nostartfiles

# Include the user's files.
include sources.inc

ifeq ($(strip $(SUBSYS)),openlpc)
	LDSCRIPT=$(strip $(CHIP))-$(strip $(SUBSYS)).ld
	LIBS += -lc -lm -lstartup.$(strip $(CHIP)) -l$(strip $(CHIP)).$(strip $(CHIP))
	OUTPUTFORMAT=bin
endif

ifeq ($(strip $(SUBSYS)),bare)
	LDSCRIPT=$(strip $(CHIP))-$(strip $(SUBSYS)).ld
	LIBS += -lc -lm -lstartupbare.$(strip $(CHIP)) -l$(strip $(CHIP)).$(strip $(CHIP))
	OUTPUTFORMAT=hex
endif

TARGET = $(strip $(NAME)).$(strip $(CHIP))
FLAGS=$($(strip $(CHIP))_FLAGS)

# Define programs and commands.
SHELL = sh
PFX = arm-none-eabi
CC = $(PFX)-gcc
CPP = $(PFX)-g++
OBJCOPY = $(PFX)-objcopy
OBJDUMP = $(PFX)-objdump
SIZE = $(PFX)-size
NM = $(PFX)-nm
REMOVE = rm -f
COPY = cp

# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_AFTER = Size after:
MSG_FLASH = Creating load file for Flash:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_COMPILING_ARM = "Compiling C:"
MSG_COMPILINGCPP = Compiling C++:
MSG_COMPILINGCPP_ARM = "Compiling C++:"
MSG_ASSEMBLING = Assembling:
MSG_ASSEMBLING_ARM = "Assembling:"
MSG_CLEANING = Cleaning project:

# Define all object files.
AOBJ      = $(ASRC:.S=.o)
COBJ      = $(CSRC:.c=.o) 
CPPOBJ    = $(CPPSRC:.cpp=.o) 

# Define all listing files.
LST = $(ASRC:.S=.lst) $(CSRC:.c=.lst) $(CPPSRC:.cpp=.lst)

# Default target.
.PHONY: all
all: $(OUTPUTFORMAT) lss size

elf: $(TARGET).elf
bin: $(TARGET).bin
hex: $(TARGET).hex
lss: $(TARGET).lss 
sym: $(TARGET).sym

.PHONY: program
program: $(TARGET).bin
	@echo
	@openlpc-program $(TARGET).bin

# Create final output files (.bin, .eep) from ELF output file.
%.bin: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O binary $< $@

# Create hex file from elf.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@	
	$(OBJCOPY) -O ihex $< $@

# Create extended listing file from ELF output file.
# Grrrr, for some reason this gives an error when invoked
# from the Makefile, so we have to put it in a cmd file
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@	
	$(OBJDUMP) -h -S $(TARGET).elf > $(TARGET).lss

# Link: create ELF output file from object files.
%.elf: sources.inc $(AOBJ) $(COBJ) $(CPPOBJ)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) $(FLAGS) $(LDFLAGS) -Wl,-Map=$(TARGET).map $(OPT) -T$(LDSCRIPT) --output $@ $(AOBJ) $(COBJ) $(CPPOBJ) $(LIBS)

.PHONY: size
size: $(TARGET).elf
	@echo
	@echo $(MSG_SIZE_AFTER)
	@$(SIZE) -B -d $(TARGET).elf 

# Compile: create object files from C source files. ARM/Thumb
$(COBJ) : %.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(FLAGS) $(CFLAGS) $(OPT) $(CDEFS) $(CINCS) $< -o $@ 

# Compile: create object files from C++ source files. ARM/Thumb
$(CPPOBJ) : %.o : %.cpp
	@echo
	@echo $(MSG_COMPILINGCPP) $<	
	$(CPP) -c $(FLAGS) $(CPPFLAGS) $(OPT) $(CDEFS) $(CINCS) $< -o $@ 

# Assemble: create object files from assembler source files. ARM/Thumb
$(AOBJ) : %.o : %.S
	@echo
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(FLAGS) $(ASFLAGS) $(OPT) $(ADEFS) $< -o $@

.PHONY: clean
clean:
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).bin $(TARGET).hex $(TARGET).elf $(TARGET).map $(TARGET).lss $(TARGET).sym
	$(REMOVE) $(COBJ) $(CPPOBJ) $(AOBJ)
	$(REMOVE) $(LST)
	$(REMOVE) $(CSRC:.c=.s) $(CPPSRC:.cpp=.s)
	$(REMOVE) $(CSRC:.c=.d) $(CPPSRC:.cpp=.d)
	$(REMOVE) index.txt program.cmd .program_begin .program_end .dep/*

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


