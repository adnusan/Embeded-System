all: lineSensor motorTest

lineSensor:
	gcc -I. -o linesensor linesensor.c motor.c -lpigpio -lrt -lpthread -lm
motorTest:
	gcc -I. -c -o motorTest motor.c -lpigpio -lrt -lpthread -lm
run:
	sudo ./linesensor
clean:
	rm motorTest linesensor
