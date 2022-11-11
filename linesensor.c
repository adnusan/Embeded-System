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
	//name of sensor for more readability
	char l_sensor[] = "LEFT SENSOR";
	char r_sensor[] = "RIGHT SENSOR";
	if(gpioInitialise() < 0){
		return 1;
	}
	
	gpioSetMode(LEFT_LINE_SENSOR, PI_INPUT);
	gpioSetMode(RIGHT_LINE_SENSOR, PI_INPUT);
	
	//waiting for gpio to  calibrate
	gpioDelay(5);
	
	//creating thread for line sensor and obstacle sensor
	pthread_t left_line_sensor_thread, right_line_sensor_thread;
	if(pthread_create(&left_line_sensor_thread, NULL, line_sensor(LEFT_LINE_SENSOR,l_sensor), NULL)){
		printf("error \n");
	}
//TODO: run two line sensor in thread and detect which way to turn

	if(pthread_create(&right_line_sensor_thread, NULL, line_sensor(RIGHT_LINE_SENSOR,r_sensor), NULL)){
		printf("error \n");
	}


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
		line_sensor(LEFT_LINE_SENSOR, l_sensor);
		printf("after left line");
		line_sensor(RIGHT_LINE_SENSOR, r_sensor);
		sleep(1);
	}
	pthread_join(left_line_sensor_thread, NULL);
	pthread_join(right_line_sensor_thread, NULL);
    	printf("Thread ended.\n"); 
	gpioTerminate(); //terminate the gpio
    	return 0;
}


void *line_sensor(int sensor_gpio, const char *name){
//	for(int i = 0; i < 10; i++){	
while(1){
//		left = gpioRead(sensor_gpio);
	//	printf("Value: %d", temp);
		if(gpioRead(sensor_gpio) == 1){
			printf("%s on BLACK!\n", name);
		}
		if(gpioRead(sensor_gpio) == 0) {
			printf("%s on WHITE!\n", name);
		}
	sleep(1);
	}
}


