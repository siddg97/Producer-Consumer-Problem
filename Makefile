CC=cc
CXX=CC
CCFLAGS= -g -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Werror -lrt -pthread
sources = $(wildcard *.c)
objs = $(patsubst %.c, %.o, $(sources))

candykids: $(objs)
	$(CC) $(CCFLAGS) $^ -o candykids

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f core *.o candykids
