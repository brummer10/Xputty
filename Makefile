
SUBDIR := $(wildcard */)
SUBDIR := $(filter-out  libxputty/, $(SUBDIR))
SUBDIR := $(filter-out  header/, $(SUBDIR))

.PHONY: $(SUBDIR) recurse

$(MAKECMDGOALS) recurse: $(SUBDIR)

$(SUBDIR):
	@exec $(MAKE) -C $@ $(MAKECMDGOALS)
