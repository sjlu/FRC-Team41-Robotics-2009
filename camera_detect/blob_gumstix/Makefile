CC=gcc
CFLAGS=-O3 -I/local/include/SDL -I/local/include -I/usr/local/lib -L/usr/local/lib
LFLAGS=-lm -lmingw32 -lbgd -lws2_32
SRCS=main.c blob_gd.c depth.c target.c tolerance.c log.c
OBJS=main.o blob_gd.o depth.o target.o tolerance.o log.o

all : blob_gumstix

blob_gumstix : $(OBJS) $(SRCS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $@

.c.o : $(SRCS)
	$(CC) -c $(CFLAGS) $*.c

clean : 
	rm -rf *.o *.a *.exe *.txt
