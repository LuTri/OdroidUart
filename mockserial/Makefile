GCC=gcc
LDLIBS = -lutil

ifndef UART_BUFFER_SIZE
	UART_BUFFER_SIZE=5
endif

CFLAGS = -DUNITTEST
CFLAGS += -DMOCKING
CFLAGS += -DBUFF_SIZE=$(UART_BUFFER_SIZE)

TARGET=mock

SRCS=$(TARGET).c mockregisters.c ../uart.c
OBJS=$(SRCS:.c=.o)

all: $(OBJS)
	$(GCC) $(OBJS) $(LDLIBS) -o $(TARGET)

%.o: %.c
	$(GCC) $(CFLAGS) -c $< -o $@

clean:
	@- $(RM) $(OBJS) $(TARGET)

.PHONY: all clean
