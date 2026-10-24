CC = clang

SOURCE=src
BINARY=chip
LIBRARY=lib

SRCS      := $(wildcard $(SOURCE)/*.c)
OBJS      := $(SRCS:.c=.o)

OPT=-O0
DEPFLAGS=-MP -MD
CFLAGS=-Wall -Wextra -g -I./$(LIBRARY) $(OPT) $(DEPFLAGS) -ObjC
LDFLAGS = -framework QuartzCore -framework Cocoa -framework MetalKit -framework Metal -framework AudioToolbox

$(BINARY): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


