
	# set  name
	NAME = Xputty
	VER = 0.1
	# set link flags
	LDFLAGS += -I. -I../header `pkg-config --static --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm
	LD_DBUGFLAGS += -I. -I../header  -D DEBUG `pkg-config --static --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm
	SRC=$(wildcard *.c)
	LIB_SOURCES=$(wildcard ../libxputty/*.c)
	EXAMPLE_SOURCES=$(wildcard *.c)
	OBJECTS=simple-example.o example.o example1.o example2.o 
	EXEC_FILES=simple-example example example1 example2

.PHONY : all build-static clean

all: $(LIB_SOURCES) $(EXAMPLE_SOURCES)
	$(CC) simple-example.c $(LIB_SOURCES) -o simple-example $(LDFLAGS)
	$(CC) example.c $(LIB_SOURCES)  -o example $(LDFLAGS)
	$(CC) example1.c $(LIB_SOURCES) -o example1 $(LDFLAGS)
	$(CC) example2.c $(LIB_SOURCES) -o example2 $(LDFLAGS)

build-static: $(EXEC_FILES)

clean:
	@rm -f simple-example.o example.o example1.o example2.o
	@rm -f simple-example example example1 example2
	@rm -f libxputty.a xputty.o

libxputty.a: $(LIB_SOURCES)
	$(CC) -c ../libxputty/xwidget.c -o xwidget.o $(LDFLAGS)
	$(CC) -c ../libxputty/xadjustment.c -o xadjustment.o $(LDFLAGS)
	$(CC) -c ../libxputty/xchildlist.c -o xchildlist.o $(LDFLAGS)
	ar rcs libxputty.a xwidget.o xadjustment.o xchildlist.o

$(OBJECTS): $(EXAMPLE_SOURCES) libxputty.a
	$(CC) -c simple-example.c -o simple-example.o $(LDFLAGS)
	$(CC) -c example.c -o example.o $(LDFLAGS)
	$(CC) -c example1.c -o example1.o $(LDFLAGS)
	$(CC) -c example2.c -o example2.o $(LDFLAGS)

$(EXEC_FILES): $(OBJECTS)
	$(CC) simple-example.o -L. libxputty.a -o simple-example $(LDFLAGS)
	$(CC) example.o -L. libxputty.a -o example $(LDFLAGS)
	$(CC) example1.o -L. libxputty.a -o example1 $(LDFLAGS)
	$(CC) example2.o -L. libxputty.a -o example2 $(LDFLAGS)

doc:

