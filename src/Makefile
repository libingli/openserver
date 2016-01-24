CC = gcc
LD = gcc
CFLAGS = -D _DEBUG_ -g -Wall -c -Dgliethttp -I.
LIBS = -lpthread -ldl

ROOT_DIR = ..
SRCS = $(wildcard *.c)
OBJS = $(patsubst %c,%o,$(SRCS))
TARGET = openserverd

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(LD) -o $@ $^ $(LIBS)
	@mv ${TARGET} ${ROOT_DIR}/sbin

%o: %c
	@$(CC) $(CFLAGS) -o $@ $<
clean:
	@rm -f *.o 
