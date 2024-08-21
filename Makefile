CC    = x86_64-w64-mingw32-gcc
STRIP = strip -s
RM    = rm

CFLAGS = -shared -O3

SRC = detect_game
EXT = .dll

all: $(SRC)

$(SRC): %: %.c $(DEPS)
	$(CC) $^ -o $@$(EXT) $(CFLAGS)
	$(STRIP) $@$(EXT)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	@$(RM) $(addsuffix $(EXT), $(SRC)) 2>/dev/null |:
