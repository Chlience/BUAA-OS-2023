fibo: fibo.o main.o
	gcc code/fibo.o code/main.o -o fibo

fibo.o:
	gcc -c code/fibo.c -o code/fibo.o -I include

main.o:
	gcc -c code/main.c -o code/main.o -I include

clean:
	rm -rf code/*.o
