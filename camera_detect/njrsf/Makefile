CC=gcc
CFLAGS=-O3 -I/local/include/SDL -I/local/include -I/usr/local/lib -I"/c/Program Files/OpenCV/cv/include" -I"/c/Program Files/OpenCV/cvaux/include" -I"/c/Program Files/OpenCV/cxcore/include" -I"/c/Program Files/OpenCV/ml/include" -I"/c/Program Files/OpenCV/otherlibs/cvcam/include" -I"/c/Program Files/OpenCV/otherlibs/highgui" -L"/c/Program Files/OpenCV/lib"  -L/usr/local/lib
LFLAGS=-lm -lcxcore -lcv -lcvaux -lhighgui -lml -lmingw32 -lSDLmain -lSDL_ttf -lfreetype -lsprig -lSDL -mwindows -lSDL_image 
SRCS=main.c tolerance.c depth.c target.c blob.c
OBJS=main.o tolerance.o depth.o target.o blob.o

all : njrsf

njrsf : $(OBJS) $(SRCS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $@

.c.o : $(SRCS)
	$(CC) -c $(CFLAGS) $*.c

clean : 
	rm -rf *.o *.a *.exe *.txt
    
