/******************************************************************************
TestRun.ino
TB6612FNG H-Bridge Motor Driver Example code
Michelle @ SparkFun Electronics
8/20/16
https://github.com/sparkfun/SparkFun_TB6612FNG_Arduino_Library

Uses 2 motors to show examples of the functions in the library.  This causes
a robot to do a little 'jig'.  Each movement has an equal and opposite movement
so assuming your motors are balanced the bot should end up at the same place it
started.

Resources:
TB6612 SparkFun Library

Development environment specifics:
Developed on Arduino 1.6.4
Developed with ROB-9457
******************************************************************************/

// This is the library for the TB6612 that contains the class Motor and all the
// functions
#include <SparkFun_TB6612.h>

// Pins for all inputs, keep in mind the PWM defines must be on PWM pins
// the default pins listed are the ones used on the Redbot (ROB-12097) with
// the exception of STBY which the Redbot controls with a physical switch
#define AIN1 7
#define AIN2 6
#define BIN1 9
#define BIN2 10
#define PWMA 5
#define PWMB 11
#define STBY 8

// these constants are used to allow you to make your motor configuration 
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = -1;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

void setup(){
  Serial.begin(9600);
  Serial.println("Hello, world");
}


void loop(){
  
  Serial.println("Hi");
delay(10000);

  forward(motor1, motor2, -255);
      delay(5000);
/*brake(motor1, motor2);
delay(2000);
left(motor1, motor2, 100);
delay(2000);
forward(motor1, motor2, 255);
      delay(3000);
      brake(motor1, motor2);
      delay(2000);
right(motor1, motor2, 100);
delay(2000);*/
      
  /*backNForth(255,3000);
  backNForth(255,7000);
  backNForth(255,13000);?*/


   /*backNForth(100,1000);
   backNForth(100,3000);
   backNForth(100,7000);
   backNForth(100,13000);*/
}

/** goes forward, brake, wait, goes backwards*/
void backNForth(int speed, int time){
   //Use of the drive function which takes as arguements the speed
   //and optional duration.  A negative speed will cause it to go
   //backwards.  Speed can be from -255 to 255.  Also use of the 
   //brake function which takes no arguements.
   motor1.drive(speed,time);
   motor1.brake();
   delay(3000);
   motor1.drive(-speed,time);
   motor1.brake();
   delay(3000);
}

