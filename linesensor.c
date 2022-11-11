#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>




//hi//defining sensors gpio
#define LEFT_LINE_SENSOR 14
#define RIGHT_LINE_SENSOR 18
#define ON 1
#define OFF 0

//function prototypes

void *line_sensor();

struct LineSensorData
{
	char * sensor_name;
	int gpioPin;
};




int main(){
	//name of sensor for more readability
	char l_sensor[] = "LEFT SENSOR";
	char r_sensor[] = "RIGHT SENSOR";

	//line sensor data
	struct LineSensorData *LeftLine = (struct LineSensorData *)malloc(sizeof(struct LineSensorData));
    struct LineSensorData *RightLine = (struct LineSensorData *)malloc(sizeof(struct LineSensorData));

	//setting sensor data name and var for easy readability
	LeftLine->sensor_name = l_sensor;
	LeftLine->gpioPin = LEFT_LINE_SENSOR;
	RightLine->sensor_name = r_sensor;
	RightLine->gpioPin = RIGHT_LINE_SENSOR;

	//initialize pigpio
	if(gpioInitialise() < 0){
		return 1;
	}
	
	//setting gpio mode to input
	gpioSetMode(LEFT_LINE_SENSOR, PI_INPUT);
	gpioSetMode(RIGHT_LINE_SENSOR, PI_INPUT);
	
	//waiting for gpio to  calibrate
	gpioDelay(5);


	//threads for line sensor
	pthread_t left_line_sensor_thread, right_line_sensor_thread;
	if(pthread_create(&left_line_sensor_thread, NULL, line_sensor, (void *)LeftLine) != 0){
		printf("Error creating thread");
	}
	if(pthread_create(&right_line_sensor_thread, NULL, line_sensor, (void *)RightLine) != 0){
		printf("Error creating thread");
	}



	
	//waiting for thread to end
	pthread_join(left_line_sensor_thread, NULL);
	pthread_join(right_line_sensor_thread, NULL);
    printf("Thread ended.\n"); 
	gpioTerminate(); //terminate the gpio
    return 0;
}


void *line_sensor(void* input){

	char  *name = ((struct sensorInfo*)input)->name;
	int gpioPin = ((struct sensorInfo*)input)->sensorGpio;
	while(1){
		if(gpioRead(gpioPin) == 1){
			printf("%s on BLACK!\n", name);
		}
		if(gpioRead(gpioPin) == 0) {
			printf("%s on WHITE!\n", name);
		}
	sleep(1);
	}
}


