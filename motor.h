/**************************************************************
 * Class: CSC-615-01 Fall 2022
 * Name: Justin Mao
 * Student ID: 918453242
 * Github ID: jmao12
 * Project: Term Project
 *
 * File: motor.h
 *
 * Description:
 * Holds functions and defintions for GPIO pins and registers
 **************************************************************/
#ifndef MOTOR_H
#define MOTOR_H

// #define OUTPUT 17
// #define INPUT 27



#define LED_ON_L 0x06
#define LED_ON_H 0x07
#define LED_OFF_L 0x08
#define LED_OFF_H 0x09


#define MOTORA_SPEED 0
#define MOTORA_IN1 1
#define MOTORA_IN2 2

#define MOTORB_SPEED 5
#define MOTORB_IN1 3
#define MOTORB_IN2 4


void writeToReg(uint8_t reg, uint8_t intvalue);
// void writeToReg(uint8_t handle, uint8_t reg, uint8_t intvalue);
void intHandler(int signum);
void setPWMFreq(uint16_t freq);
void* runMotorForward(void * input);
void* runMotorBackward(void * input);
void stopMotorA();
void stopMotorB();
void setPWMDutyCycle(uint8_t channel, uint16_t pulse);
void setPWM(uint8_t channel, uint16_t on, uint16_t off);




#endif