CC=gcc
CFLAGS=-O3 -I/local/include/SDL -I/local/include -I/usr/local/lib -L/usr/local/lib
LFLAGS=-lm -lmingw32 -lSDLmain -lSDL_ttf -lfreetype -lsprig -lSDL -mwindows -lSDL_image -lws2_32
SRCS=main.c axis206.c blob_sdl.c depth.c target.c
OBJS=main.o axis206.o blob_sdl.o depth.o target.o

all : axis

axis : $(OBJS) $(SRCS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $@

.c.o : $(SRCS)
	$(CC) -c $(CFLAGS) $*.c

clean : 
	rm -rf *.o *.a *.exe *.txt
