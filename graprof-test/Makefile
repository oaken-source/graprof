
## the default number of self profiling passes
all: pass2

## delete all trace and profile files
clean:
	rm -f gp.out.* graprof.out.*

## pass0 - profile the graprof --help call
pass0:
	GRAPROF_OUT=gp.out.0 graprof -FCMg -o graprof.out.pass0 -- /usr/local/bin/graprof-gp --help > /dev/null

## additional passes, copy and increment as needed.
## TODO: is there a better way for this?
pass1: pass0
	GRAPROF_OUT=gp.out.1 graprof -FCMg -o graprof.out.pass1 -- /usr/local/bin/graprof-gp -FCMg -t gp.out.0 -o /dev/null /usr/local/bin/graprof-gp

pass2: pass1
	GRAPROF_OUT=gp.out.2 graprof -FCMg -o graprof.out.pass2 -- /usr/local/bin/graprof-gp -FCMg -t gp.out.1 -o /dev/null /usr/local/bin/graprof-gp

pass3: pass2
	GRAPROF_OUT=gp.out.3 graprof -FCMg -o graprof.out.pass3 -- /usr/local/bin/graprof-gp -FCMg -t gp.out.2 -o /dev/null /usr/local/bin/graprof-gp

