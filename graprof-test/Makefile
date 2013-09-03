
BIN_GP=graprof-gp
BIN_PG=graprof-pg
BIN_PATH=/usr/local/bin/

all: pass3

pass0:
	GRAPROF_OUT=gp.out.0 $(BIN_GP) --help &> /dev/null
	graprof $(BIN_PATH)$(BIN_GP) gp.out.0 -FCMg -o graprof.out.pass0
	$(BIN_PG) --help &> /dev/null
	gprof $(BIN_PATH)$(BIN_PG) > gprof.out.pass0

pass1: pass0
	GRAPROF_OUT=gp.out.1 $(BIN_GP) $(BIN_PATH)$(BIN_GP) gp.out.0 -FCMg &> /dev/null
	graprof $(BIN_PATH)$(BIN_GP) gp.out.1 -FCMg -o graprof.out.pass1
	$(BIN_PG) $(BIN_PATH)$(BIN_GP) gp.out.0 -FCMg &> /dev/null
	gprof $(BIN_PATH)$(BIN_PG) > gprof.out.pass1

pass2: pass1
	GRAPROF_OUT=gp.out.2 $(BIN_GP) $(BIN_PATH)$(BIN_GP) gp.out.1 -FCMg &> /dev/null
	graprof $(BIN_PATH)$(BIN_GP) gp.out.2 -FCMg -o graprof.out.pass2
	$(BIN_PG) $(BIN_PATH)$(BIN_GP) gp.out.1 -FCMg &> /dev/null
	gprof $(BIN_PATH)$(BIN_PG) > gprof.out.pass2
	
pass3: pass2
	GRAPROF_OUT=gp.out.3 $(BIN_GP) $(BIN_PATH)$(BIN_GP) gp.out.2 -FCMg &> /dev/null
	graprof $(BIN_PATH)$(BIN_GP) gp.out.3 -FCMg -o graprof.out.pass3
	$(BIN_PG) $(BIN_PATH)$(BIN_GP) gp.out.2 -FCMg &> /dev/null
	gprof $(BIN_PATH)$(BIN_PG) > gprof.out.pass3
	
clean:
	rm gp.out.* graprof.out.* gmon.out gprof.out.*

