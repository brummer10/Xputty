
SUBDIR := $(wildcard */)
SUBDIR := $(filter-out  libxputty/, $(SUBDIR))
SUBDIR := $(filter-out  header/, $(SUBDIR))
SUBDIR := $(filter-out  docs/, $(SUBDIR))

.PHONY: $(SUBDIR) recurse

$(MAKECMDGOALS) recurse: $(SUBDIR)

$(SUBDIR):
	@exec $(MAKE) -C $@ $(MAKECMDGOALS)

doc:
	@rm -rf ./docs
	doxygen Doxyfile
	cp ./examples/index.html ./docs/
