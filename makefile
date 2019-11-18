CC      = gcc
CFLAGS = -g 
LDFLAGS = -lpthread -lm -lrt

src = $(wildcard *.c)
target = $(patsubst %.c, %, ${src})

.PHONY: all clean

%:%.c
	$(CC) $< $(CFLAGS) -o $@ ${LDFLAGS}


all: ${target}

$(target):%:%.c
		$(CC) $< $(CFLAGS) -o $@ ${LDFLAGS}
clean:
	rm -f ${target}
