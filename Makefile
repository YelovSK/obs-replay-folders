CC = x86_64-w64-mingw32-gcc
STRIP = strip -s
RM = rm

CFLAGS = -shared -O3

EXES = detect_game
EXT  = .dll

all: $(EXES)

$(EXES): %: %.c $(DEPS)
	$(CC) $^ -o $@$(EXT) $(CFLAGS)
	$(STRIP) $@$(EXT)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	@$(RM) *.o $(addsuffix $(EXT), $(EXES)) 2>/dev/null |:
