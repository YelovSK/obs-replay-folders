CC = x86_64-w64-mingw32-gcc
RM = rm

CFLAGS = -shared -lpsapi -luser32

EXES = detect_game
EXT  = .dll

all: $(EXES)

$(EXES): %: %.c $(DEPS)
	$(CC) $^ -o $@$(EXT) $(CFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	@$(RM) *.o $(addsuffix $(EXT), $(EXES)) 2>/dev/null |:
