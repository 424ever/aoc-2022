SOLDIR  = src/sol
SRCDIR  = src
PROG    = aoc-2022
CFLAGS  = -Wall -Wextra -Werror -I $(SRCDIR) -g

SOLSRCS = $(wildcard $(SOLDIR)/*.c)
SOLOBJS = $(patsubst %.c, %.o, $(SOLSRCS))

OBJ     = $(SOLOBJS)
OBJ    += $(SRCDIR)/runner.o

.PHONY: all clean

all: $(PROG)

clean:
	@$(RM) -f $(PROG) $(OBJ)

$(PROG): $(OBJ)
	@printf "%b" " \033[0;34mLD\t\033[0;35m$@\033[m\n"
	@$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c
	@printf "%b" " \033[0;34mCC\t\033[0;36m$@\033[m\n"
	@$(CC) -c -o $@ $^ $(CFLAGS)

