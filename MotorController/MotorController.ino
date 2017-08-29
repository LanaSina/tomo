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
//ref https://learn.sparkfun.com/tutorials/xbee-shield-hookup-guide/example-communication-test


// This is the library for the TB6612 that contains the class Motor and all the
// functions
#include <SparkFun_TB6612.h>
#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX

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


//from xbee
String data = "";

void setup(){
  Serial.begin(9600);
  Serial.println("Hello, world");
  // Set up both ports at 9600 baud. This value is most important
  // for the XBee. Make sure the baud rate matches the config
  // setting of your XBee.
  XBee.begin(9600);
}


void loop(){

  //receive command from XBee
  if (XBee.available()){ 
    
    char character = XBee.read(); // Receive a single character from the software serial port
        data.concat(character); // Add the received character to the receive buffer
        if (character == '\n'){
            Serial.print("Received: ");
            Serial.println(data);

            //parse
            parseCommand(data);

            // Clear receive buffer so we're ready to receive the next line
            data = "";
        }
  }
  
delay(200);
}


//https://arduino.stackexchange.com/questions/1013/how-do-i-split-an-incoming-string
//format: motor_id, speed
//motorname = right, left, both
void parseCommand(String xdata){
  int motor_id = getValue(xdata, ',', 0).toInt();
  int speed = getValue(xdata, ',', 1).toInt();
  switch (motor_id) {
    case 0:{
      motor1.drive(speed);
      break;
    }
    case 1:{
      motor2.drive(speed);
      break;
    }
    case -1:{
     forward(motor1, motor2, speed);
      break;
    }
  }
}


String getValue(String data, char separator, int index){
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
