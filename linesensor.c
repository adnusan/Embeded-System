#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>
#include <pthread.h>



//hi//defining sensors gpio
#define LEFT_LINE_SENSOR 14
#define RIGHT_LINE_SENSOR 18
#define ON 1
#define OFF 0

//function prototypes

void *line_sensor();

int left;
int main(){
	if(gpioInitialise() < 0){
		return 1;
	}
	
	gpioSetMode(LEFT_LINE_SENSOR, PI_INPUT);
	gpioSetMode(RIGHT_LINE_SENSOR, PI_INPUT);
	
	//waiting for gpio to  calibrate
	gpioDelay(5);
	
	//creating thread for line sensor and obstacle sensor
	pthread_t left_line_sensor_thread, right_line_sensor_thread;
	if(pthread_create(&left_line_sensor_thread, NULL, line_sensor(LEFT_LINE_SENSOR), NULL)){
		printf("error \n");
	}

//	if(pthread_create(&right_line_sensor_thread, NULL, line_sensor(RIGHT_LINE_SENSOR), NULL)){
//		printf("error \n");
//	}


//	for (int i = 0; i < 10; ++i){
	while(1){
	//	if(left == 0){
	//		printf("WHITE: %d", left);
	//	}
	//	if(left == 1){
	//		printf("BLACK: %d", left);
	//	}
	//	printf("\n");
	//	sleep(1);
		line_sensor(LEFT_LINE_SENSOR);
		sleep(1);
//		line_sensor(RIGHT_LINE_SENSOR);
	}
	pthread_join(left_line_sensor_thread, NULL);
//	pthread_join(right_line_sensor_thread, NULL);
    	printf("Thread ended.\n"); 
	gpioTerminate(); //terminate the gpio
    	return 0;
}


void *line_sensor(int sensor_gpio){
//	for(int i = 0; i < 10; i++){	
while(1){
//		left = gpioRead(sensor_gpio);
	//	printf("Value: %d", temp);
	int temp =  gpioRead(sensor_gpio);
		if(temp == 1){
			printf("BLACK LINE DETECTED!\n");
		}
		if(temp == 0) {
			printf("WHITE LINE DETECTED\n");
		}
	sleep(1);
	}
}


