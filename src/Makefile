SUBDIRS = \
 object \
 tools \
 unit

all:
	for i in $(SUBDIRS); do make -C $$i; done

clean:
	for i in $(SUBDIRS); do make -C $$i clean; done

.PHONY: clean
