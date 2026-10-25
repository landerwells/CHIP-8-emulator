CC = clang

SOURCE=src
BINARY=chip
LIBRARY=lib

SRCS      := $(wildcard $(SOURCE)/*.c)
OBJS      := $(SRCS:.c=.o)

OPT=-O0
DEPFLAGS=-MP -MD
CFLAGS=-Wall -Wextra -Wformat=2 -g -I./$(LIBRARY) $(OPT) $(DEPFLAGS) -ObjC
LDFLAGS = -framework QuartzCore -framework Cocoa -framework MetalKit -framework Metal -framework AudioToolbox

$(BINARY): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
	dsymutil $@

# -include $(OBJS:.o=.d)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BINARY) $(BINARY).dSYM $(OBJS) $(OBJS:.o=.d)
