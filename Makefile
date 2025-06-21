CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -Iinclude
LDFLAGS=-lssl -lcrypto
SRCDIR=src
OBJDIR=obj
DATADIR=data

# Source files
SOURCES=$(wildcard $(SRCDIR)/*.c)
# Object files
OBJECTS=$(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET=blockmed

.PHONY: all clean setup test

all: setup $(TARGET)

setup:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(DATADIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGET)
	rm -rf $(DATADIR)/*.dat
	rm -rf $(DATADIR)/*.log

test: $(TARGET)
	@echo "Running basic tests..."
	@./$(TARGET) < test_input.txt || true

install:
	sudo apt-get update
	sudo apt-get install -y libssl-dev

debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --track-origins=yes ./$(TARGET)
