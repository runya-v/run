# author: Velichko R.N.; rostislav.vel@gmail.com; (c) 2006 - ...
# Makefile for "test"

-include ../../platform

ftrace:
	$(OS_CC) ftrace.cpp -finstrument-functions

bfdtrace:
	$(OS_CC) bfdtrace.cpp -finstrument-functions -lbfd -liberty

extmergsort:
	$(OS_CC) extmergsort.cpp -D_FILE_OFFSET_BITS=64 -lpthread


#all: $(MODULES)

#$(MODULES): msg
#	@list='$(TARGETS)'; \
#	for target in $$list; do \
#	echo ""; \
#	echo "================================================================================"; \
#	echo "[ Create \"$$target\""; \
#	echo "================================================================================"; \
#	(make -sC $(SRC_DIR)/$$target $(EXPORTS) $(MAKE_KEY)) || exit 1; \
#	echo "================================================================================"; \
#	done