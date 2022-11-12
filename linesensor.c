/*
* Program to run multiple line sensor on thread
* @author Nusan Rana
*  
*/



#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>




//defining sensors gpio
#define LEFT_LINE_SENSOR 14
#define RIGHT_LINE_SENSOR 18
#define ON 1
#define OFF 0

//function prototypes

void *line_sensor();
void *line_sensor_control();

struct LineSensorData
{
	char * sensor_name;
	int gpioPin;
	int updated_data = 0;
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

	//creating threads, passing struct which has sensor name and sensor gpio pin #
	if(pthread_create(&left_line_sensor_thread, NULL, line_sensor_control, (void *)LeftLine) != 0){
		printf("Error creating thread");
	}
	if(pthread_create(&right_line_sensor_thread, NULL, line_sensor_control, (void *)RightLine) != 0){
		printf("Error creating thread");
	}

	while(1){
		printf("Left Sensor: %d\n", LeftLine->updated_data);
		printf("Right Sensor: %d\n", RightLine->updated_data);
		sleep(1);
	}


	
	//waiting for thread to end
	pthread_join(left_line_sensor_thread, NULL);
	pthread_join(right_line_sensor_thread, NULL);
    printf("Thread ended.\n"); 
	gpioTerminate(); //terminate the gpio
    return 0;
}

//this function take in a pointer to a struct LineSensorData
//and access the name and value of gpio from the struct
//TODO: add a way to stop the loop
void *line_sensor(void* input){

	struct LineSensorData *sensor_data = (struct LineSensorData *)input;

	char  *name = sensor_data->sensor_name;
	int gpioPin = sensor_data->gpioPin;
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

//TODO: check the value or line sensor and detect which way to turn
void *line_sensor_control(void* input){
	struct LineSensorData *sensor_data = (struct LineSensorData *)input;
	char  *name = sensor_data->sensor_name;
	int gpioPin = sensor_data->gpioPin;
	while(1){
		sensor_data->updated_data = gpioRead(gpioPin);
		// if(gpioPin == 1){
		// 	printf("%s on BLACK!\n", name);
		// 	sensor_data->updated_data=1;
		// }
		// if(gpioPin == 0) {
		// 	printf("%s on WHITE!\n", name);
		// }
		// RIGHT_LINE_SENSOR = 0;

	sleep(2);
	printf("\n");
	}

}

