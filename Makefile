CC = gcc
CFLAGS = -Wall -O2
COMPILE = $(CC) $(CFLAGS) -c

all: test

test: main.o parser.o xmlbare.o sh_bighash.o sh_hash.o sh_hash_func.o sh_page.o sh_page_manager.o
	$(CC) -o test main.o parser.o xmlbare.o sh_bighash.o sh_hash.o sh_hash_func.o sh_page.o sh_page_manager.o

parser.o: parser.c
	$(COMPILE) -o parser.o parser.c

main.o: main.c
	$(COMPILE) -o main.o main.c

xmlbare.o: xmlbare.c
	$(COMPILE) -o xmlbare.o xmlbare.c

sh_bighash.o: sh_bighash.c
	$(COMPILE) -o sh_bighash.o sh_bighash.c

sh_hash.o: sh_hash.c
	$(COMPILE) -o sh_hash.o sh_hash.c

sh_hash_func.o: sh_hash_func.c
	$(COMPILE) -o sh_hash_func.o sh_hash_func.c

sh_page.o: sh_page.c
	$(COMPILE) -o sh_page.o sh_page.c

sh_page_manager.o: sh_page_manager.c
	$(COMPILE) -o sh_page_manager.o sh_page_manager.c

clean:	
	rm -rf *.o test.exe test
