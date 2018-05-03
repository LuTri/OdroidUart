# Hey Emacs, this is a -*- makefile -*-

# Inherit parameters from a parent's Makefile
ifndef MCU # default target MCU to atmega328p
	MCU = atmega328p
endif

ifndef F_OSC # default quartz speed to 16MHz
	F_OSC = 16000000
endif

# Desired BAUD-rate
ifndef BAUD_INT
	BAUD_INT = 500000
endif
ifndef USART_PORT
	USART_PORT = /dev/ttyACM99
endif
BAUD = $(BAUD_INT)UL

# Target file name (without extension).
TARGET = uart

# List C source files here. (C dependencies are automatically generated.)
SRC = $(TARGET).c

UNIT_TARGET = unittest

UNIT_SRCS = $(UNIT_TARGET).c $(SRC)

# Optimization level, can be [0, 1, 2, 3, s].
# 0 = turn off optimization. s = optimize for size.
# (Note: 3 is not always the best optimization level. See avr-libc FAQ.)
OPT = s

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
#DEBUG = stabs
DEBUG = dwarf-2

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99

# Compiler flags.
#  -g*:          generate debugging information
#  -O*:          optimization level
#  -f...:        tuning, see GCC manual and avr-libc documentation
#  -Wall...:     warning level
#  -Wa,...:      tell GCC to pass this to the assembler.
#    -adhlns...: create assembler listing
CFLAGS = -g$(DEBUG)
CFLAGS += $(CDEFS) $(CINCS)
CFLAGS += -O$(OPT)
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += $(CSTANDARD)
CFLAGS += -DF_OSC=$(F_OSC)
CFLAGS += -DF_CPU=$(F_OSC)
CFLAGS += -DBAUD=$(BAUD)

# Define programs and commands.
CC = avr-gcc
GCC = gcc
REMOVE = rm -f

# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_COMPILING = Compiling:
MSG_CLEANING = Cleaning project:




# Define all object files.
OBJ = $(SRC:.c=.o) $(ASRC:.S=.o)

# Define all object files.
UNIT_OBJ = $(UNIT_SRCS:.c=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(SRC:.c=.lst)


# Compiler flags to generate dependency files.
### GENDEPFLAGS = -Wp,-M,-MP,-MT,$(*F).o,-MF,.dep/$(@F).d
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS) $(GENDEPFLAGS)

# Default target.
all: begin gccversion build finished end settings

build: $(OBJ)

settings:
	echo "{\"port\":\"$(USART_PORT)\", \"baud\":$(BAUD_INT)}" > .usart.ini

# Eye candy.
# AVR Studio 3.x does not check make's exit code but relies on
# the following magic strings to be generated by the compile job.
begin:
	@echo
	@echo $(MSG_BEGIN)

finished:
	@echo $(MSG_ERRORS_NONE)

end:
	@echo $(MSG_END)
	@echo

# Display compiler version information.
gccversion :
	@$(CC) --version

# Compile: create object files from C source files.
%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(ALL_CFLAGS) $< -o $@

# Target: clean project.
clean: begin clean_list finished end

clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(OBJ)

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

unittest : CC=$(GCC)
unittest : ALL_CFLAGS = $(CFLAGS) -DUNITTEST
unittest : $(UNIT_OBJ)
	$(GCC) $(UNIT_OBJ) -o $@
	./unittest
uart.o: .FORCE

.PHONY: .FORCE
.FORCE:

# Listing of phony targets.
.PHONY : all begin finish end sizebefore sizeafter gccversion \
build elf hex eep lss sym coff extcoff clean clean_list unittest
