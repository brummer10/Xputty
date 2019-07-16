
	# set  name
	NAME = Xputty
	VER = 0.1
	# set link flags
	LDFLAGS += -I. -I../  -D DEBUG `pkg-config --static --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm

.PHONY : all  

all: simple-example.c example.c example1.c example2.c ../xputty.c
	$(CC) -o  $(LDFLAGS) simple-example.c ../xputty.c -o simple-example
	$(CC) -o  $(LDFLAGS) example.c ../xputty.c -o example
	$(CC) -o  $(LDFLAGS) example1.c ../xputty.c -o example1
	$(CC) -o  $(LDFLAGS) example2.c ../xputty.c -o example2

clean:
	@rm -f simple-example example example1 example2

