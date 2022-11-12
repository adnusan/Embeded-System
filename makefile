linesensor:
	gcc -Wall -o linesensor linesensor.c -lpigpio -lpthread

clean:
	rm linesensor
run: linesensor
	./linesensor
