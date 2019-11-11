
SUBDIR := $(sort $(wildcard */))
SUBDIR := $(filter-out  xputty/, $(SUBDIR))
SUBDIR := $(filter-out  header/, $(SUBDIR))
SUBDIR := $(filter-out  Xgain.lv2/, $(SUBDIR))
SUBDIR := $(filter-out  examples/, $(SUBDIR))
SUBDIR := $(filter-out  docs/, $(SUBDIR))
SUBDIR := $(filter-out  libxputty/, $(SUBDIR))

.PHONY: $(SUBDIR) recurse

$(MAKECMDGOALS) recurse: $(SUBDIR)

$(SUBDIR):
	@exec $(MAKE) -C $@ $(MAKECMDGOALS)

doc:
	@rm -rf ./docs
	doxygen Doxyfile
	cp ./examples/index.html ./docs/
