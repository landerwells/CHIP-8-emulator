CC = clang

SOURCE=src
BINARY=chip
LIBRARY=lib

SRCS      := $(wildcard $(SOURCE)/*.c)
OBJS      := $(SRCS:.c=.o)

DEPFLAGS=-MP -MD
CFLAGS= -std=c99 -g -O3 \
				-Wall -Wextra -Wpedantic \
				-Wformat=2 -Wstrict-prototypes -Wmissing-prototypes \
				-fsanitize=address,undefined -fno-omit-frame-pointer \
				-I./$(LIBRARY) $(OPT) $(DEPFLAGS) -ObjC

LDFLAGS = -fsanitize=address,undefined \
					-framework QuartzCore -framework Cocoa -framework MetalKit -framework Metal -framework AudioToolbox

$(BINARY): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
	dsymutil $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(OBJS:.o=.d)

clean:
	rm -rf $(BINARY) $(BINARY).dSYM $(OBJS) $(OBJS:.o=.d)
