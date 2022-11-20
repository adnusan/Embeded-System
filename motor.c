/**************************************************************
* Motor Controller
* functions to control the motors
* can run both motors forward and backward at the same time using thread 
* TODO: turn logics.. write function to turn right / left using data from linesensor
* TODO: implement turining function in linesensor.c file
*
*INFO TO CONTROL INDIVIDUAL MOTOR
* motorA (left motor)
* channel1 = 1 
* channel2=  2
* 0 = pin number of motorA for setPWMDutyCycle
*
* motorB (right motor)
* channel1 = 3 
* channel2 = 4  
* 5 = pin number of motorB for setPWMDutyCycle
*
* TO COMPILE without main function: use -c flag
* example: gcc -I. -c motor.c -lpigpio -lrt -lpthread -lm
*
 **************************************************************/


#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include "motor.h"



int i2cHandle;

struct motorData
{
    char * name;
    uint8_t speed;
    uint16_t channel1;
    uint16_t channel2;
    uint16_t power;
};


// run motor forward
void* runMotorForward(void * input)
{
    // limit speed to 100
    struct motorData *inputData = (struct motorData *)input;
    uint16_t speed = inputData->power;
    if (speed > 100)
    {
        speed = 100;
    }
    printf("%s Forward \nspeedPin:%d \tchannel1: %d\tchannel2: %d\tspeed: %d\n",inputData->name,inputData->speed, inputData->channel1, inputData->channel2, speed);
    // write to registers 6,7,8,9
    setPWMDutyCycle(inputData->speed, speed);
    // write to registers 10,11,12,13
    setPWM(inputData->channel1, 0, 0);
    // write to registers 14,15,16,17
    setPWM(inputData->channel2, 0, 4095);
}


// run motor backward
void* runMotorBackward(void * input)
{
    struct motorData *backData = (struct motorData *)input;
    uint16_t speed = backData->power;
    // limit speed to 100
    if (speed > 100)
    {
        speed = 100;
    }
    printf("%s Backward \n speedPin: %d \tchannel1: %d\tchannel2: %d\tspeed: %d\n",backData->name,backData->speed, backData->channel1, backData->channel2, speed);
    // write to registers 6,7,8,9
    setPWMDutyCycle(backData->speed, speed);
    // write to registers 10,11,12,13
    setPWM(backData->channel1, 0, 4095);
    // write to registers 14,15,16,17
    setPWM(backData->channel2, 0, 0);
}

// set speed to 0 for motor
void stopMotorA()
{
    setPWMDutyCycle(0, 0);
}

// set speed to 0 for motor
void stopMotorB()
{
    setPWMDutyCycle(5, 0);
}


// function to run i2cWriteByteData and print out error messages
void writeToReg(uint8_t reg, uint8_t intValue)
{
    // printf("reg: %d\tval: %d\n", reg, intValue);
    int value = i2cWriteByteData(i2cHandle, reg, intValue);
    if (value != 0)
    {
        printf("Error: %d\n", value);
    }
}
// handle interrupt when Ctrl+c
void intHandler(int signum)
{
    printf("Stopping Program\n");
    // stop motor
    stopMotorA();
    stopMotorB();
    i2cClose(i2cHandle);
    gpioTerminate();
    exit(0);
}
// adjust PWM pulse widths
void setPWMFreq(uint16_t freq)
{
    uint8_t test = i2cReadByteData(i2cHandle, 0x00);
    // printf("%d %d\n", test, i2cReadByte(i2cHandle));
    freq *= 0.9;
    double prescaleval = 25000000.0;
    prescaleval /= 4096.0;
    prescaleval /= freq;
    prescaleval -= 1;
    // printf("prescaleveal = %lf\n", prescaleval);
    uint8_t prescale = floor(prescaleval + 0.5);
    // printf("prescaleveal = %lf\n", prescaleval);

    //  uint8_t oldmode = i2cReadByteData(i2cHandle, 0x00);
    //set value instead of reading from register 0
    //reads 16 at register 0 when running program immediately after turning on pi
    //while PCA9685 always reads a 0 
    uint8_t oldmode = 0x00;
    uint8_t newmode = (oldmode & 0x7F) | 0x10;

    // printf("oldmode: %d\tnewmode:%d\tprescale:%d\n", oldmode, newmode, prescale);

    //writes to registers 0 and 254
    //set sleep bit to mode register 1 to turn off internal osscillator
    writeToReg(0x00, newmode);  
    //set prescale register
    writeToReg(0xFE, prescale);

    writeToReg(0x00, oldmode);
    time_sleep(0.005);
    //set to turn on auto-increment
    writeToReg(0x00, oldmode | 0x80);

    //write hardcoded values to registers
    // writeToReg(0, 16);
    // writeToReg(254, 67);
    // writeToReg(0, 0);
    // time_sleep(0.005);
    // writeToReg(0, 128);
}


//set amount of time digital signal is active base on pulse value
void setPWMDutyCycle(uint8_t channel, uint16_t pulse)
{
    setPWM(channel, 0, pulse * (4096 / 100) - 1);
}

// write to registers channel with 4 registers
//  6/7/8/9 + 4*channel
void setPWM(uint8_t channel, uint16_t on, uint16_t off)
{
    // printf("\n");
    //value & 0xFF will only carry over first 8 bits that
    //are true in both the value and 0xFF
    writeToReg(LED_ON_L + 4 * channel, on & 0xFF);
    //shift bits to the right 8 times
    writeToReg(LED_ON_H + 4 * channel, on >> 8);
    writeToReg(LED_OFF_L + 4 * channel, off & 0xFF);
    writeToReg(LED_OFF_H + 4 * channel, off >> 8);
}
