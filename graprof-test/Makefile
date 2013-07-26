
all: gprof graprof graprof-static

gprof:
	gcc -o test test.c -g -pg -lgrapes
	./test
	gprof test > gprof.out
	rm gmon.out

graprof:
	gcc -o test test.c -g -finstrument-functions -lgraprof -lgrapes
	GRAPROF_OUT=gp.out ./test
	graprof test gp.out -FCg -o graprof.out
	rm gp.out

graprof-static:
	gcc -o test test.c -g -finstrument-functions -lgraprof -Wl,-Bstatic -lgrapes-graprof -Wl,-Bdynamic
	GRAPROF_OUT=gp.out ./test
	graprof test gp.out -FCg -o graprof.out.static
	rm gp.out

clean:
	rm test gprof.out graprof.out*

rebuild: clean all
