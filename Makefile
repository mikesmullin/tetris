# Tetris CLI Makefile
# Portable C99 implementation

CC = clang
CFLAGS = -std=c99 -O2 -Wall -Wextra -pedantic -Wno-unused-function
LDFLAGS =

# Debug build flags
DEBUG_CFLAGS = -std=c99 -O0 -g -Wall -Wextra -pedantic -Wno-unused-function -DDEBUG_SLOW

SRC = src/main.c
TARGET = tetris

.PHONY: all clean debug install

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

debug: $(SRC)
	$(CC) $(DEBUG_CFLAGS) -o $(TARGET) $< $(LDFLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

test: $(TARGET)
	@echo "=== Testing tetris CLI ==="
	@./$(TARGET) show
	@echo ""
	@echo "=== Pressing 'a' (left) ==="
	@./$(TARGET) press a
	@echo ""
	@echo "=== Advancing frame ==="
	@./$(TARGET) next
