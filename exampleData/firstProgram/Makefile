CC := riscv32-unknown-elf-gcc
LD := riscv32-unknown-elf-ld

CFLAGS += -gdwarf-4
CFLAGS += -march=rv32ima -mabi=ilp32
CFLAGS += -nostartfiles

all: main
sim: main
	symex-vp $<

main: bootstrap.o main.o symex.o
	$(LD) -o $@ $^
bootstrap.o: bootstrap.S
	$(CC) -c $(CPPFLAGS) -o $@ $< $(CFLAGS)

%.o: %.c
	$(CC) -c $(CPPFLAGS) -o $@ $< $(CFLAGS)

.PHONY: all sim
