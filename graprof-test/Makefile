
all: vanilla gprof graprof graprof-static

vanilla:
	gcc -o test test.c -g -lgrapes
	echo "f\n" | ./test

gprof:
	gcc -o test test.c -g -pg -lgrapes
	echo "f\n" | ./test
	gprof test > gprof.out
	rm gmon.out

graprof:
	gcc -o test test.c -g -finstrument-functions -lgraprof -lgrapes
	echo "f\n" | GRAPROF_OUT=gp.out ./test
	graprof test gp.out -FCg -o graprof.out
	rm gp.out

graprof-static:
	gcc -o test test.c -g -finstrument-functions -lgraprof -Wl,-Bstatic -lgrapes-graprof -Wl,-Bdynamic
	echo "f\n" | GRAPROF_OUT=gp.out ./test
	graprof test gp.out -FCg -o graprof.out.static
	rm gp.out

clean:
	rm test gprof.out graprof.out*

rebuild: clean all
