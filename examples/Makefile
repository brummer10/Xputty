
	# set  name
	NAME = Xputty
	VER = 0.1
	# set link flags
	LDFLAGS += -I. -I../  `pkg-config --static --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm
	LD_DBUGFLAGS += -I. -I../  -D DEBUG `pkg-config --static --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm

.PHONY : all  

all: simple-example.c example.c example1.c example2.c ../xputty.c
	$(CC) simple-example.c ../xputty.c ../xadjustment.c -o simple-example $(LDFLAGS)
	$(CC) example.c ../xputty.c ../xadjustment.c -o example $(LDFLAGS)
	$(CC) example1.c ../xputty.c ../xadjustment.c -o example1 $(LDFLAGS)
	$(CC) example2.c ../xputty.c ../xadjustment.c -o example2 $(LDFLAGS)

clean:
	@rm -f simple-example.o example.o example1.o example2.o
	@rm -f simple-example example example1 example2
	@rm -f xputty.a xputty.o


static-lib: ../xputty.c ../xadjustment.c
	gcc -c ../xputty.c -o xputty.o $(LDFLAGS)
	gcc -c ../xadjustment.c -o xadjustment.o $(LDFLAGS)
	ar rcs xputty.a xputty.o xadjustment.o

build-static: static-lib
	gcc -c example.c -o example.o $(LDFLAGS)
	gcc -c example1.c -o example1.o $(LDFLAGS)
	gcc -c example2.c -o example2.o $(LDFLAGS)
	gcc example.o -L. xputty.a -o example $(LDFLAGS)
	gcc example1.o -L. xputty.a -o example1 $(LDFLAGS)
	gcc example2.o -L. xputty.a -o example2 $(LDFLAGS)
