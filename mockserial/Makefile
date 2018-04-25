GCC=gcc
LDLIBS = -lutil

CFLAGS = -DUNITTEST
CFLAGS += -DMOCKING
CFLAGS += -DBUFF_SIZE=2000

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
