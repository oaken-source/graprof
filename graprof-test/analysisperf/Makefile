
BIN = ../../graprof/graprof
BIN_GP = ../../graprof/.libs/graprof-gp

LD = LD_LIBRARY_PATH=../../libgraprof/.libs

## the default number of self profiling passes
all: pass3

## delete all trace and profile files
clean:
	rm -f gp.out.* graprof.out.*

## pass0 - profile the graprof --help call
pass0:
	$(LD) GRAPROF_OUT=gp.out.0 $(BIN) -FCMg -o graprof.out.pass0 -- $(BIN_GP) --help > /dev/null

## additional passes, copy and increment as needed.
## TODO: is there a better way for this?
pass1: pass0
	$(LD) GRAPROF_OUT=gp.out.1 $(BIN) -FCMg -o graprof.out.pass1 -- $(BIN_GP) -FCMg -t gp.out.0 -o /dev/null $(BIN_GP)

pass2: pass1
	$(LD) GRAPROF_OUT=gp.out.2 $(BIN) -FCMg -o graprof.out.pass2 -- $(BIN_GP) -FCMg -t gp.out.1 -o /dev/null $(BIN_GP)

pass3: pass2
	$(LD) GRAPROF_OUT=gp.out.3 $(BIN) -FCMg -o graprof.out.pass3 -- $(BIN_GP) -FCMg -t gp.out.2 -o /dev/null $(BIN_GP)

pass4: pass3
	$(LD) GRAPROF_OUT=gp.out.4 $(BIN) -FCMg -o graprof.out.pass4 -- $(BIN_GP) -FCMg -t gp.out.3 -o /dev/null $(BIN_GP)
