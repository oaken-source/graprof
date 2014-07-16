
BIN_GP = ../../graprof/.libs/graprof-gp
BIN = ../../graprof/graprof
ENV = LD_LIBRARY_PATH=../../libgraprof/.libs GRAPROF_OUT=gp.out.

## the default number of self profiling passes
all: pass4

## delete all trace and profile files
clean:
	rm -f gp.out.* graprof.out.*

## pass0 - profile the graprof --help call
pass0:
	$(ENV)0 $(BIN) -o graprof.out.pass0 -- $(BIN_GP) --help > /dev/null

## additional passes, copy and increment as needed.
## TODO: is there a better way for this?
pass1: pass0
	$(ENV)1 $(BIN) -o graprof.out.pass1 -- $(BIN_GP) -t gp.out.0 -o /dev/null $(BIN_GP)

pass2: pass1
	$(ENV)2 $(BIN) -o graprof.out.pass2 -- $(BIN_GP) -t gp.out.1 -o /dev/null $(BIN_GP)

pass3: pass2
	$(ENV)3 $(BIN) -o graprof.out.pass3 -- $(BIN_GP) -t gp.out.2 -o /dev/null $(BIN_GP)

pass4: pass3
	$(ENV)4 $(BIN) -o graprof.out.pass4 -- $(BIN_GP) -t gp.out.3 -o /dev/null $(BIN_GP)

pass5: pass4
	$(ENV)5 $(BIN) -o graprof.out.pass5 -- $(BIN_GP) -t gp.out.4 -o /dev/null $(BIN_GP)
