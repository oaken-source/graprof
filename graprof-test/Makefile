
BIN=graprof-test
PATH=/usr/local/bin/

all: pass0 pass1 pass2 pass3

pass0:
	GRAPROF_OUT=gp.out.0 $(BIN) --help &> /dev/null

pass1:
	GRAPROF_OUT=gp.out.1 $(BIN) $(PATH)$(BIN) gp.out.0 -FCMg -o graprof.out.pass1

pass2:
	GRAPROF_OUT=gp.out.2 $(BIN) $(PATH)$(BIN) gp.out.1 -FCMg -o graprof.out.pass2

pass3:
	GRAPROF_OUT=gp.out.3 $(BIN) $(PATH)$(BIN) gp.out.2 -FCMg -o graprof.out.pass3

clean:
	rm gp.out.* graprof.out.*

rebuild: clean all
