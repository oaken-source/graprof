
all: pass0 pass1 pass2 pass3

pass0:
	GRAPROF_OUT=gp.out.0 graprof-test --help &> /dev/null

pass1:
	GRAPROF_OUT=gp.out.1 graprof-test /usr/local/bin/graprof-test gp.out.0 -FCMg -o graprof.out.pass1

pass2:
	GRAPROF_OUT=gp.out.2 graprof-test /usr/local/bin/graprof-test gp.out.1 -FCMg -o graprof.out.pass2

pass3:
	GRAPROF_OUT=gp.out.3 graprof-test /usr/local/bin/graprof-test gp.out.2 -FCMg -o graprof.out.pass3

clean:
	rm gp.out.* graprof.out.*

rebuild: clean all
