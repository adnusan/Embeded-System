/*@author Nusan Rana
*Program to run multiple line sensor on thread
*Added logics to handle turns.
*Left sensor must connect to gpio #14 and right gpio #18
*Line Sensor value: 0 == white line, 1 == black line 
*gcc -I. -o linesensor linesensor.c motor.c -lpigpio -lrt -lpthread -lm
*/
#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include "motor.h"

//defining sensors gpio
#define LEFT_LINE_SENSOR 14
#define RIGHT_LINE_SENSOR 18
#define BLACK 1
#define WHITE 0

//function prototypes
void *line_sensor_control();

static int i2cHandle;

//data for line sensor
struct LineSensorData
{
	char * sensor_name;
	int gpioPin;
	int updated_data;
};

//data for motor
struct motorData
{
    char * name;
    uint8_t speed;
    uint16_t channel1;
    uint16_t channel2;
    uint16_t power;
};




int main(int argc, char *argv[]){
	//name of sensor for more readability
	char l_sensor[] = "LEFT SENSOR";
	char r_sensor[] = "RIGHT SENSOR";

	//line sensor data
	struct LineSensorData *LeftLine = (struct LineSensorData *)malloc(sizeof(struct LineSensorData));
	struct LineSensorData *RightLine = (struct LineSensorData *)malloc(sizeof(struct LineSensorData));

	//motors struct
	struct motorData *motorA = (struct motorData *)malloc(sizeof(struct motorData));
    struct motorData *motorB = (struct motorData *)malloc(sizeof(struct motorData));


	//setting sensor data name and var for easy readability
	LeftLine->sensor_name = l_sensor;
	LeftLine->gpioPin = LEFT_LINE_SENSOR;
	RightLine->sensor_name = r_sensor;
	RightLine->gpioPin = RIGHT_LINE_SENSOR;

	///setting data of each motor
    motorA->name = "Motor A";
    motorA->speed = MOTORA_SPEED;
    motorA->channel1 = MOTORA_IN1;
    motorA->channel2 = MOTORA_IN2;
    motorA->power = 50;

    motorB->name = "Motor B";
    motorB->speed = MOTORB_SPEED;
    motorB->channel1 = MOTORB_IN1;
    motorB->channel2 = MOTORB_IN2;
    motorB->power = 50;

	//initialize pigpio
	if (gpioInitialise() < 0)
    {
        fprintf(stderr, "pigpio initialization failed\n");
        return 1;
    }
	//setting gpio mode to input
	gpioSetMode(LEFT_LINE_SENSOR, PI_INPUT);
	gpioSetMode(RIGHT_LINE_SENSOR, PI_INPUT);
	//waiting for gpio to  calibrate
	gpioDelay(5);


	//i2c handler
	i2cHandle = i2cOpen(1, 0x40, 0);
    // printf("i2c: %d\n", i2cHandle);
    if (i2cHandle < 0)
    {
        printf("handle not valid: %d\n", i2cHandle);
        return 2;
    }

	//sigaction interrupt handler
    struct sigaction newSigAction;
	//struct sigaction *newSigAction = (struct sigaction *)malloc(sizeof(struct sigaction));
    newSigAction.sa_handler = intHandler;
    newSigAction.sa_flags = 0;
    sigemptyset(&newSigAction.sa_mask);
    if (sigaction(SIGINT, &newSigAction, NULL) != 0)
    {
        fprintf(stderr, "sigaction initialization failed\n");
        return 3;
    }

	//setting PWM frequency
	int pow = 100;
    setPWMFreq(pow);

	//threads for line sensor
	pthread_t left_line_sensor_thread, right_line_sensor_thread;

	//creating threads, passing struct which has sensor name and sensor gpio pin #
	if(pthread_create(&left_line_sensor_thread, NULL, line_sensor_control, (void *)LeftLine) != 0){
		printf("Error creating thread\n");
	}
	if(pthread_create(&right_line_sensor_thread, NULL, line_sensor_control, (void *)RightLine) != 0){
		printf("Error creating thread\n");
	}

	//TURN LOGICS
	//checking the updated value of sensor
	while(1){
		//sensor value 0 == white line, 1 == black line
		//if both sensor is on black line then go straight
		if (LeftLine->updated_data == WHITE && RightLine->updated_data == WHITE){
<<<<<<< HEAD
			printf("Both Line Sensor on Black line: R:%d  L:%d\n Go straight: \n", RightLine->updated_data, LeftLine->updated_data);
			//call runmotorforward function
			runMotorForward(motorA);
			runMotorForward(motorB);
=======
			printf("Both Line Sensor on Black line: R:%d  L:%d\n Go straight: %d\n", RightLine->updated_data, LeftLine->updated_data);
>>>>>>> ba6474b9214cb7f3627e98a58806bbc66e17052b
		}

		//if left sensor detect white then left side of car is out of lane, turn right to stay on black lane
		if(LeftLine->updated_data == BLACK && RightLine->updated_data == WHITE){
<<<<<<< HEAD
			printf("Left Line Sensor on Black line: R:%d  L:%d\n TURN LEFT!: \n", RightLine->updated_data, LeftLine->updated_data);
			runMotorForward(motorB);
			stopMotorA();
		}
		//if right sensor detect white then right side of car is out of lane, turn left to stay on black lane
		if(RightLine->updated_data == BLACK && LeftLine->updated_data == WHITE){
			printf("Right Line Sensor on Black line: R:%d  L:%d\n TURN RIGHT!: \n", RightLine->updated_data, LeftLine->updated_data);
			runMotorForward(motorA);
			stopMotorB();
=======
			printf("Left Line Sensor on Black line: R:%d  L:%d\n TURN LEFT!: %d\n", RightLine->updated_data, LeftLine->updated_data);
		}
		//if right sensor detect white then right side of car is out of lane, turn left to stay on black lane
		if(RightLine->updated_data == BLACK && LeftLine->updated_data == WHITE){
			printf("Right Line Sensor on Black line: R:%d  L:%d\n TURN RIGHT!: %d\n", RightLine->updated_data, LeftLine->updated_data);
>>>>>>> ba6474b9214cb7f3627e98a58806bbc66e17052b
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
