
	# set  name
	NAME = Xputty
	VER = 0.1
	# set link flags
	LDFLAGS += -I. -I../  `pkg-config --static --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm
	LD_DBUGFLAGS += -I. -I../  -D DEBUG `pkg-config --static --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm
	OBJECTS = simple-example.o example.o example1.o example2.o 
	EXEC_FILES = simple-example example example1

.PHONY : all build-static clean

all:
	$(CC) simple-example.c ../xputty.c ../xadjustment.c -o simple-example $(LDFLAGS)
	$(CC) example.c ../xputty.c ../xadjustment.c -o example $(LDFLAGS)
	$(CC) example1.c ../xputty.c ../xadjustment.c -o example1 $(LDFLAGS)
	$(CC) example2.c ../xputty.c ../xadjustment.c -o example2 $(LDFLAGS)

build-static: $(EXEC_FILES)

clean:
	@rm -f simple-example.o example.o example1.o example2.o
	@rm -f simple-example example example1 example2
	@rm -f xputty.a xputty.o

xputty.a: ../xputty.c ../xadjustment.c
	$(CC) -c ../xputty.c -o xputty.o $(LDFLAGS)
	$(CC) -c ../xadjustment.c -o xadjustment.o $(LDFLAGS)
	ar rcs xputty.a xputty.o xadjustment.o

$(OBJECTS): xputty.a
	$(CC) -c simple-example.c -o simple-example.o $(LDFLAGS)
	$(CC) -c example.c -o example.o $(LDFLAGS)
	$(CC) -c example1.c -o example1.o $(LDFLAGS)
	$(CC) -c example2.c -o example2.o $(LDFLAGS)

$(EXEC_FILES): $(OBJECTS)
	$(CC) simple-example.o -L. xputty.a -o simple-example $(LDFLAGS)
	$(CC) example.o -L. xputty.a -o example $(LDFLAGS)
	$(CC) example1.o -L. xputty.a -o example1 $(LDFLAGS)
	$(CC) example2.o -L. xputty.a -o example2 $(LDFLAGS)

