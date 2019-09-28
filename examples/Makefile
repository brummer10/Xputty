

	LIB_DIR := ../Build/
	HEADER_DIR := $(LIB_DIR)include/

	# include path for library headers
	INCLUDES := -I. -I$(HEADER_DIR) 
	# link flags to dynamic link cairo and X11 (default)
	LDFLAGS += -fPIC `pkg-config --static --cflags --libs cairo x11` -lm
	# set link flags to static link cairo and X11
	# (you could ignore ld warnings, but the resulting binary is huge
	# (6.4Mb for x11 and cairo) NOT RECOMMENDED!!
	STATIC_LDFLAGS += -fPIC -static `pkg-config --static --cflags --libs cairo ` \
	 -lX11 -ldl -lxcb -lpthread -lXau -lXdmcp
	# debug flags, use this to indicate a problem with libxputty
	DEBUG_LDFLAGS += -g -D DEBUG  -fPIC\
	 `pkg-config --static --cflags --libs cairo` \
	 -L/usr/X11/lib -lX11 -lm

	EXAMPLE_SOURCES := $(wildcard *.c)
	EXAMPLE_OBJ := $(patsubst %.c,%.o,$(EXAMPLE_SOURCES))
	EXEC_FILES := $(patsubst %.c,%,$(EXAMPLE_SOURCES))

.PHONY : all clean

all: $(EXEC_FILES) 

clean:
	rm -f $(EXAMPLE_OBJ) $(EXEC_FILES)

$(EXAMPLE_OBJ): $(EXAMPLE_SOURCES) 
	$(CC) -c $(patsubst %.o,%.c,$@) -o $@  $(INCLUDES) $(LDFLAGS)

$(EXEC_FILES): $(EXAMPLE_OBJ)
	$(CC)  $(patsubst %,%.c,$@) -L. $(LIB_DIR)libxputty.a -o $@  $(INCLUDES) $(LDFLAGS)

doc:

