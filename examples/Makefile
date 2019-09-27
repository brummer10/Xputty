

	LIB_DIR := ../libxputty/
	HEADER_DIR := ../header/
	WIDGET_DIR := $(LIB_DIR)widgets/
	WIDGET_HEADER_DIR := $(HEADER_DIR)widgets/
	RESOURCES_DIR :=$(LIB_DIR)resources/

	# include path for library headers
	INCLUDES := -I. -I$(HEADER_DIR) -I$(WIDGET_HEADER_DIR) -I$(RESOURCES_DIR)
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
	$(CC)  $(patsubst %,%.c,$@) -L. ../Build/libxputty.a -o $@  $(INCLUDES) $(LDFLAGS)

doc:

