CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99
LDFLAGS = -lcurl -ljson-c

SRCS = main.c network.c search.c download.c json.c logs.c
OBJS = $(SRCS:.c=.o)
EXECUTABLE = mget

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXECUTABLE)

.PHONY: all clean
