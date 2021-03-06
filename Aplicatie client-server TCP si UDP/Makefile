# Makefile

CFLAGS = -Wall  -Wextra -g 

all: server subscriber

# Compileaza server.c
server: server.c -lm

# Compileaza subscriber.c
subscriber: subscriber.c

.PHONY: clean run_server run_subscriber


clean:
	rm -f server subscriber
