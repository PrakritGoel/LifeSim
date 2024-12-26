CC=clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic

all: life

life: universe.o life.o
	$(CC) $(CFLAGS) -o life universe.o life.o -lncurses

life.o: life.c
	$(CC) $(CFLAGS) -c life.c

universe.o: universe.c
	$(CC) $(CFLAGS) -c universe.c

clean:
	rm -f *.o life

format:
	clang-format -i -style=file *.[c,h]

tst:
	./life -n 20 -i lists/unix.txt

valgrind_tst:
	valgrind -s --leak-check=full ./life -s -n 20 -i lists/unix.txt

