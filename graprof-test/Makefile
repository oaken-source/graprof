
all: vanilla gprof graprof graprof-static graprof-meta

gp.out:
	gcc -o test test.c -g -finstrument-functions -lgraprof -lgrapes
	echo "f\n" | GRAPROF_OUT=gp.out ./test

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
	graprof test gp.out -FCMg -o graprof.out
	rm gp.out

graprof-static:
	gcc -o test test.c -g -finstrument-functions -lgraprof -Wl,-Bstatic -lgrapes-graprof -Wl,-Bdynamic
	echo "f\n" | GRAPROF_OUT=gp.out ./test
	graprof test gp.out -FCMg -o graprof.out.static
	rm gp.out

graprof-meta:
	gcc -o test test.c -g -finstrument-functions -lgraprof -Wl,-Bstatic -lgrapes-graprof -Wl,-Bdynamic
	echo "f\n" | GRAPROF_OUT=gp.out ./test
	GRAPROF_OUT=gp.out.outer graprof-gp test gp.out -FCMg -o graprof.out.static.inner
	graprof /usr/local/bin/graprof-gp gp.out.outer -FCMg -o graprof.out.static.outer
	rm gp.out*

clean:
	rm test gprof.out graprof.out*

rebuild: clean all
