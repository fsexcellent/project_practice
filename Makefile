CROSS_COMPLIE :=
CC = gcc
C_FLAGS = -Wall -g
LD_FLAGS = -lpthread -lsqlite3
TARGET = program 
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJFILES = $(SOURCES:%.c=%.o)

all: clean $(TARGET)

$(TARGET):$(OBJFILES)
	$(CROSS_COMPLIE)$(CC) -o $@ $^ $(LD_FLAGS) 

$(OBJFILES): %.o: %.c $(HEADERS)
	$(CROSS_COMPLIE)$(CC) $(C_FLAGS) -c -o $@ $< 

clean:
	rm -rf $(OBJFILES) $(TARGET)




