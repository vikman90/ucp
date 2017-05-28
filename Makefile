# Makefile for UCP tool
# by Vikman
# May 28, 2017
#
# Syntax: make [ DEBUG=1 ] [ PROFILE=1 ] [ all | clean ]

CC = gcc
RM = rm -f
CFLAGS = -pipe -Wall

TARGET = ucp-server ucp-client

ifeq ($(DEBUG), 1)
	CFLAGS += -g -Wextra -DDEBUG
else
	CFLAGS += -O2
endif

ifeq ($(PROFILE), 1)
	CFLAGS += -DPROFILE
endif

.PHONY: all clean

ucp-%: %.c
	$(CC) $(CFLAGS) -o $@ $<

all: $(TARGET)

clean:
	$(RM) $(TARGET) *.o
