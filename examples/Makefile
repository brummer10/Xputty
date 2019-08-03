
	# set  name
	NAME = Xputty
	VER = 0.1

	LIB_DIR := ../libxputty/
	HEADER_DIR := ../header/
	WIDGET_DIR := $(LIB_DIR)widgets/
	WIDGET_HEADER_DIR := $(HEADER_DIR)widgets/
	RESOURCES_DIR :=$(LIB_DIR)resources/
	OBJ_DIR := .

	# include path for library headers
	INCLUDES := -I. -I$(HEADER_DIR) -I$(WIDGET_HEADER_DIR) -I$(RESOURCES_DIR)
	# link flags to dynamic link cairo and X11 (default)
	LDFLAGS += `pkg-config --static --cflags --libs cairo x11` -lm
	# set link flags to static link cairo and X11
	# (you could ignore ld warnings, but the resulting binary is huge
	# (6.4Mb for x11 and cairo) NOT RECOMMENDED!!
	STATIC_LDFLAGS += -static `pkg-config --static --cflags --libs cairo ` \
	 -lX11 -ldl -lxcb -lpthread -lXau -lXdmcp
	# debug flags, use this to indicate a problem with libxputty
	LD_DBUGFLAGS += -g -D DEBUG \
	 `pkg-config --static --cflags --libs cairo` \
	 -L/usr/X11/lib -lX11 -lm


	LIB_SOURCES := $(wildcard $(LIB_DIR)*.c)
	LIB_OBJ := $(notdir $(patsubst %.c,%.o,$(LIB_SOURCES)))

	WIDGET_SOURCES := $(wildcard $(WIDGET_DIR)*.c)
	WIDGET_OBJ := $(notdir $(patsubst %.c,%.o,$(WIDGET_SOURCES)))

	RESOURCES := $(wildcard $(RESOURCES_DIR)*.png)
	RESOURCES_OBJ := $(notdir $(patsubst %.png,%.o,$(RESOURCES)))
	RESOURCE_EXTLD := $(notdir $(patsubst %.png,%_png,$(RESOURCES)))
	RESOURCEHEADER := $(RESOURCES_DIR)xresources.h

	DEP = $(LIB_OBJ:%.o=%.d) $(WIDGET_OBJ:%.o=%.d)

	EXAMPLE_SOURCES := $(wildcard *.c)
	EXAMPLE_OBJ := $(patsubst %.c,%.o,$(EXAMPLE_SOURCES))
	EXEC_FILES := $(patsubst %.c,%,$(EXAMPLE_SOURCES))

.PHONY : all build-static clean

all: $(LIB_SOURCES) $(WIDGET_SOURCES) $(EXAMPLE_SOURCES)
	$(CC) thema-example.c $(LIB_SOURCES) $(WIDGET_SOURCES) -o thema-example $(INCLUDES) $(LDFLAGS)
	$(CC) simple-example.c $(LIB_SOURCES) $(WIDGET_SOURCES) -o simple-example $(INCLUDES) $(LDFLAGS)
	$(CC) example.c $(LIB_SOURCES)  $(WIDGET_SOURCES) -o example $(INCLUDES) $(LDFLAGS)
	$(CC) example1.c $(LIB_SOURCES) $(WIDGET_SOURCES) -o example1 $(INCLUDES) $(LDFLAGS)
	$(CC) example2.c $(LIB_SOURCES)  $(WIDGET_SOURCES) -o example2 $(INCLUDES) $(LDFLAGS)

build-static:  $(RESOURCEHEADER) $(EXEC_FILES) 

clean:
	rm -f $(EXAMPLE_OBJ) $(EXEC_FILES)
	rm -f libxputty.a $(LIB_OBJ) $(WIDGET_OBJ) $(RESOURCES_OBJ) $(DEP) $(RESOURCEHEADER)

$(RESOURCEHEADER): $(RESOURCES_OBJ) 
	rm -f $(RESOURCEHEADER)
	for f in $(RESOURCE_EXTLD); do \
		echo 'EXTLD('$${f}')' >> $(RESOURCEHEADER) ; \
	done

$(RESOURCES_OBJ): $(RESOURCES)
	cd $(RESOURCES_DIR) && ld -r -b binary $(patsubst %.o,%.png,$@) -o ../../examples/$@

$(WIDGET_OBJ): $(WIDGET_SOURCES)
	$(CC) -MMD -c $(addprefix $(WIDGET_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

$(LIB_OBJ): $(LIB_SOURCES)
	$(CC) -MMD -c $(addprefix $(LIB_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

-include $(DEP)

libxputty.a:  $(RESOURCES_OBJ) $(LIB_OBJ) $(WIDGET_OBJ) 
	ar rcs libxputty.a $(LIB_OBJ) $(WIDGET_OBJ) $(RESOURCES_OBJ)

$(EXAMPLE_OBJ): $(EXAMPLE_SOURCES) libxputty.a
	$(CC) -c $(patsubst %.o,%.c,$@) -o $@  $(INCLUDES) $(LDFLAGS)

$(EXEC_FILES): $(EXAMPLE_OBJ)
	$(CC)  $(patsubst %,%.c,$@) -L. libxputty.a -o $@  $(INCLUDES) $(LDFLAGS)

doc:

