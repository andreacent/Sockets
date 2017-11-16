# Makefile SVR

all: svr_c svr_s

svr_c: svr_c.o
	gcc -g -Wall -std=c11 svr_c.o -o svr_c

svr_s: svr_s.o
	gcc -g -Wall -std=c11 svr_s.o -o svr_s -pthread 

svr_c.o: svr_c.c 
	gcc -c -std=c11 svr_c.c

svr_s.o: svr_s.c 
	gcc -c -std=c11 svr_s.c
	
clean:
	rm -f *.o cript
