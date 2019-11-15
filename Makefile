
SUBDIR := examples Xgain.lv2

.PHONY: $(SUBDIR) libxputty recurse

$(MAKECMDGOALS) recurse: $(SUBDIR)

libxputty:
	@exec $(MAKE) -j 1 -C $@ $(MAKECMDGOALS)

$(SUBDIR): libxputty
	@exec $(MAKE) -C $@ $(MAKECMDGOALS)

doc:
	@rm -rf ./docs
	doxygen doxyfile
	cp ./examples/index.html ./docs/
