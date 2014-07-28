
SUBDIRS=analysisperf guitest

all: $(SUBDIRS)

analysisperf: FORCE
	make -C $@

guitest: FORCE
	make -C $@

clean:
	for i in $(SUBDIRS); do make -C $$i clean; done

FORCE:
