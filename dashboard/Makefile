CC=gcc
CFLAGS=-O3 -I/local/include/SDL -I/local/include -I/usr/local/lib -L/usr/local/lib
LFLAGS=-lm -lmingw32 -lSDLmain -lSDL_ttf -lfreetype -lsprig -lSDL -mwindows -lSDL_image -lws2_32
SRCS=main.c draw.c draw.h log.c log.h net.c net.h
OBJS=main.o draw.o log.o net.o

all : dashboard

dashboard : $(OBJS) $(SRCS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $@

.c.o : $(SRCS)
	$(CC) -c $(CFLAGS) $*.c

clean : 
	rm -rf *.o *.a *.exe *.txt
