
	# set  name
	NAME = Xputty
	VER = 0.1
	# set link flags
	LDFLAGS += -I. -I../header `pkg-config --static --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm
	LD_DBUGFLAGS += -I. -I../header -g -D DEBUG `pkg-config --static --cflags --libs cairo` -L/usr/X11/lib -lX11 -lm
	SRC=$(wildcard *.c)
	LIB_DIR := ../libxputty/
	OBJ_DIR := 
	LIB_SOURCES := $(wildcard $(LIB_DIR)/*.c)
	LIB_OBJ := $(notdir $(patsubst %.c,%.o,$(LIB_SOURCES)))
	EXAMPLE_SOURCES := $(wildcard *.c)
	OBJECTS := $(patsubst %.c,%.o,$(EXAMPLE_SOURCES))
	EXEC_FILES := $(patsubst %.c,%,$(EXAMPLE_SOURCES))

.PHONY : all build-static clean

all: $(LIB_SOURCES) $(EXAMPLE_SOURCES)
	$(CC) simple-example.c $(LIB_SOURCES) -o simple-example $(LDFLAGS)
	$(CC) example.c $(LIB_SOURCES)  -o example $(LDFLAGS)
	$(CC) example1.c $(LIB_SOURCES) -o example1 $(LDFLAGS)
	$(CC) example2.c $(LIB_SOURCES) -o example2 $(LDFLAGS)

build-static: $(EXEC_FILES)

clean:
	@rm -f $(OBJECTS) $(EXEC_FILES)
	@rm -f libxputty.a $(LIB_OBJ)

$(LIB_OBJ): $(LIB_SOURCES)
	$(CC) -c $(addprefix $(LIB_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(LDFLAGS)

libxputty.a: $(LIB_OBJ)
	ar rcs libxputty.a xwidget.o xadjustment.o xchildlist.o xputty.o

$(OBJECTS): $(EXAMPLE_SOURCES) libxputty.a
	$(CC) -c $(patsubst %.o,%.c,$@) -o $@  $(LDFLAGS)

$(EXEC_FILES): $(OBJECTS)
	$(CC)  $(patsubst %,%.c,$@) -L. libxputty.a -o $@  $(LDFLAGS)

doc:
