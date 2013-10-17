
SUBDIRS=analysisperf

all: $(SUBDIRS)

analysisperf: FORCE
	make -C $@

clean: 
	for i in $(SUBDIRS); do make -C $$i clean; done 

FORCE:
