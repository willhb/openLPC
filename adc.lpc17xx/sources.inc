# Sources.inc: This file is included by the Makefile and contains
# the list of data required to build the software. The Makefile
# can be upgraded without loss of data. Customizations to the
# build process should be made in this file.
#
# This sources.inc file should be paired with the Program Makefile.
# There is a seperate sources.inc template for the Library Makefile.
#
# Target file name (without extension). The resulting binary
# will be called target.bin
NAME=blinky.1769

# List Assembler source files here.
# Make them always end in a capital .S.  Files ending in a lowercase .s
# will not be considered source files but generated files (assembler
# output from the compiler), and will be deleted upon "make clean"!
# Even though the DOS/Win* filesystem matches both .s and .S the same,
# it will preserve the spelling of the filenames, and gcc itself does
# care about how the name is spelled on its command-line.
ASRC = 

# List C source files here.
# use file-extension c for "c-only"-files
CSRC =  main.c 

# List C++ source files here.
# use file-extension cpp for C++-files
CPPSRC =

# C Defines. Place -D's here
CDEFS=

# Place -D or -U options for ASM here
ADEFS =

# Include directories. Place -I options here
CINCS = 

# Libraries. Place -l options here.
# Default libraries are added for each subsystem. Custom subsystem
# does not specify any default libraries.
LIBS =

# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization. s = optimize for size.
OPT = -O2

# You can append flags to the defaults here, or completely
# overwrite the default flags by using '=' instead of '+='

# Flags passed to the assembler
ASFLAGS +=

# Flags passed to gcc, affecting compilation of .c files
CFLAGS +=

# Flags passed to g++, affecting compilation of .cpp files
CPPFLAGS +=

# Flags passed to the linker.
LDFLAGS +=

# Select target chip. Currently supported values are: lpc17xx, lpc2148
CHIP=lpc17xx

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
#      serial bootloader.
#
#    custom - you must provide your own linker script and 
#      startup code. If you specify this option, you
#      MUST provide startup code and linker script or your
#      code will not link!! Startup assembly code can
#      be specifed in the ASRC list, and a linker script
#      can be specified with the LDSCRIPT variable
#
SUBSYS = openlpc
