/*@author Nusan Rana
*Program to run multiple line sensor on thread
*Added logics to handle turns.
*Left sensor must connect to gpio #14 and right gpio #18
*Line Sensor value: 0 == white line, 1 == black line 
*gcc -Wall -o linesensor linesensor.c -lpigpio -lpthread
*/
#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

//defining sensors gpio
#define LEFT_LINE_SENSOR 14
#define RIGHT_LINE_SENSOR 18
#define BLACK 1
#define WHITE 0

//function prototypes
void *line_sensor_control();

struct LineSensorData
{
	char * sensor_name;
	int gpioPin;
	int updated_data;
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
		printf("Error creating thread\n");
	}
	if(pthread_create(&right_line_sensor_thread, NULL, line_sensor_control, (void *)RightLine) != 0){
		printf("Error creating thread\n");
	}


	//checking the updated value of sensor
	while(1){
		//sensor value 0 == white line, 1 == black line
		//if both sensor is on black line then go straight
		if (LeftLine->updated_data == BLACK && RightLine->updated_data == BLACK){
			printf("Both Line Sensor on Black line: R:%d  L:%d\n Go straight: %d\n", RightLine->updated_data, LeftLine->updated_data);
		}

		//if left sensor detect white then left side of car is out of lane, turn right to stay on black lane
		if(LeftLine->updated_data == WHITE && RightLine->updated_data == BLACK){
			printf("Left Line Sensor on White line: R:%d  L:%d\n TURN RIGHT!: %d\n", RightLine->updated_data, LeftLine->updated_data);
		}
		//if right sensor detect white then right side of car is out of lane, turn left to stay on black lane
		if(LeftLine->updated_data == BLACK && RightLine->updated_data == WHITE){
			printf("Right Line Sensor on White line: R:%d  L:%d\n TURN LEFT!: %d\n", RightLine->updated_data, LeftLine->updated_data);
		}
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
//updates the value of updated_data from gpioRead(gpio)
//could modify this function to get data from any sensor
void *line_sensor_control(void* input){
	struct LineSensorData *sensor_data = (struct LineSensorData *)input;
	//char  *name = sensor_data->sensor_name;
	int gpioPin = sensor_data->gpioPin;
	while(1){
		sensor_data->updated_data = gpioRead(gpioPin);
	sleep(1);
	printf("\n");
	}
}