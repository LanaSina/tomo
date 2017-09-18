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
// Include the RFM69 and SPI libraries:
#include <RFM69.h>
#include <SPI.h>

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


// Addresses for this node. CHANGE THESE FOR EACH NODE!
#define NETWORKID     0   // Must be the same for all nodes
#define MYNODEID      2   // My node ID
#define TONODEID      1   // Destination node ID

// RFM69 frequency, uncomment the frequency of your module:
#define FREQUENCY   RF69_433MHZ
//#define FREQUENCY     RF69_915MHZ

// AES encryption (or not):
#define ENCRYPT       false // Set to "true" to use encryption
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):
#define USEACK        false // Request ACKs or not

// Packet sent/received indicator LED (optional):
#define LED           9 // LED positive pin
#define GND           8 // LED ground pin


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

int current_motor = 2;
int current_speed = 100;
//from xbee
String data = "";


// Create a library object for our RFM69HCW module:
RFM69 radio;
int prev_rssi = -1000;


void setup(){
  Serial.begin(9600);
  Serial.println("Hello, world");
  // Set up both ports at 9600 baud. This value is most important
  // for the XBee. Make sure the baud rate matches the config
  // setting of your XBee.
  XBee.begin(9600);

  Serial.print("Node ");
  Serial.print(MYNODEID,DEC);
  Serial.println(" ready");  

  // Set up the indicator LED (optional):
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  pinMode(GND,OUTPUT);
  digitalWrite(GND,LOW);

  // Initialize the RFM69HCW:
  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower(); // Always use this for RFM69HCW

  // Turn on encryption if desired:
  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);
}


void loop(){

  //receive command from XBee
  /*if (XBee.available()){ 
    
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
  }*/

  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:

    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(", message [");

    // The actual message is contained in the DATA array,
    // and is DATALEN bytes in size:

    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);

    // RSSI is the "Receive Signal Strength Indicator",
    // smaller numbers mean higher power.

    Serial.print("], RSSI ");
    int cur_rssi = radio.RSSI;
    Serial.println(radio.RSSI);

    // Send an ACK if requested.
    // (You don't need this code if you're not using ACKs.)

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("ACK sent");
    }
    Blink(LED,10);


    //now use RSSI to estimate optimal motion
    if(cur_rssi>20){
      //do nothing, or test directivity
    } else {
      if(cur_rssi<prev_rssi){
        //go back
        Serial.println("Go back!");
        motors_go(-current_speed, 2000);
      } else {
        //go forward
        Serial.println("Go forward!");
        motors_go(current_speed, 2000);
      }
    }
  }
  
  
delay(1000);
}

void move(){
   /*switch (current_motor) {
    case 1:{
      motor2.drive(current_speed);
    }
    break;
    case 2:{
      motor1.drive(current_speed);
    }
    break;
    case -1:{
      motor1.drive(current_speed);
      motor2.drive(current_speed);
     //forward(motor1, motor2, speed);
    }
    break;
  }*/

  if(current_motor==1){
    motor1.drive(current_speed);
  }else if(current_motor==2){
    motor2.drive(current_speed);
  }else if(current_motor==-1){
    motors_go(current_speed);
  }
}

void motors_go(int m_speed, int duration){
  if(duration==0){
    motor1.drive(m_speed);
    motor2.drive(m_speed);
  }else{
    motor1.drive(m_speed, duration);
    motor2.drive(m_speed, duration);
  }
}

//https://arduino.stackexchange.com/questions/1013/how-do-i-split-an-incoming-string
//format: motor_id, speed
//motorname = right, left, both
void parseCommand(String xdata){
  current_motor = getValue(xdata, ',', 0).toInt();
  current_speed = getValue(xdata, ',', 1).toInt();

   Serial.print("motor ");
   Serial.print(current_motor);
   Serial.print(" speed ");
   Serial.print(current_speed);
   Serial.print("\n");
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
