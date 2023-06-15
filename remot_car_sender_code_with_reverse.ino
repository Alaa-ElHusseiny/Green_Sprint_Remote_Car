/*
the sender code for remote car control for asmr compition 
author :john remon 21/4/2023 
code describtion :
this code is for sending data to the car using M177 NRF24L01 2.4GHz Antenna Wireless Transceiver Module
this module is a rf module using radio waves to send data 
the control will bs as follows 
you will have two speeds and turning back and of course stop the vehicle 

changing the speeds will be done by an interup pin you need to puch that botton before you change the speed 

the change in the speed will be done by pipe 0 and the movement will be done by pipe 1

*/
//you must conect mosi to pin 11 and iso to pin 12 and sck to pin 13  (important)
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10
#define interupt_pin_1 2
#define interupt_pin_2 3 //not that this is not a botton but a switch that is either on  and the car is moving or stay off and the car is not moving
#define low_speed_pin 4
#define reverse_pin 5




uint8_t speed =0;
bool send_number=0;
RF24 radio(CE_PIN,CSN_PIN);//those are the cr_pin and the csn_pin connected to the arduino

const uint64_t pipes[2] = {0xABADACAEA1LL, 0xABADACAEA2LL};//The adresse that must be in the reciever code 

int turning_mechanism=0;
void setup() {
Serial.begin(9600);
radio.begin();
radio.setPALevel(RF24_PA_HIGH);
pinMode(A3,INPUT);
radio.setChannel(76); // Set the channel to 76
radio.setDataRate(RF24_250KBPS); // Set the data rate to 250 kbps
radio.openWritingPipe(pipes[0]); // Open pipe 0 for writing
radio.openWritingPipe(pipes[1]); // Open pipe 1 for writing
pinMode(interupt_pin_1,INPUT);
pinMode(interupt_pin_2,INPUT);
pinMode(reverse_pin,INPUT);
attachInterrupt(digitalPinToInterrupt(interupt_pin_1),speed_selector,RISING);
attachInterrupt(digitalPinToInterrupt(interupt_pin_2),stop_the_car,RISING);//note that this is not a botton but a switch that is either on  and the car is moving or stay off and the car is not moving



}

void loop() {

turning_mechanism=analogRead(A3);
 radio.openWritingPipe(pipes[1]); // Set the writing pipe to pipe 1
 radio.write(&turning_mechanism, sizeof(turning_mechanism)); // Send the number on pipe 1
 Serial.println(turning_mechanism);
if (send_number==1)
{
   radio.openWritingPipe(pipes[0]);   // Set the writing pipe to pipe 0
   radio.write(&speed, sizeof(speed)); // Send the speed on pipe 0  
   send_number=0;
   Serial.print("we sent the speed value and = ");
   Serial.println(speed);   
}
}



//the normal mode that you are oving with your highest speed unless the switch is on 
void speed_selector()
{
bool speed_botton =digitalRead(low_speed_pin);
if (speed_botton==HIGH)
{
  send_number=1;
   speed=1;
   //radio.openWritingPipe(pipes[0]);
   // Set the writing pipe to pipe 0
  //radio.write(&speed, sizeof(speed)); // Send the speed on pipe 0
  Serial.println("moving with low speed");

}
else if (digitalRead(reverse_pin)==HIGH)
{
   send_number=1;
    speed=3;
   //radio.openWritingPipe(pipes[0]);
   // Set the writing pipe to pipe 0
  //radio.write(&speed, sizeof(speed)); // Send the speed on pipe 0
  Serial.println("reverse moving"); 
}
else
{
   speed=2;
     send_number=1;
  // radio.openWritingPipe(pipes[0]); // Set the writing pipe to pipe 0
  //radio.write(&speed, sizeof(speed)); // Send the speed on pipe 0
  Serial.println("moving with high speed");

}

}

void stop_the_car()//not that this is not a botton but a switch that is either on and the car is moving or stay off and the car is not moving
{ 
 speed =0;
  send_number=1;
 //radio.openWritingPipe(pipes[0]); // Set the writing pipe to pipe 0
 //radio.write(&speed, sizeof(speed)); // Send the speed on pipe 0
 Serial.println("stop the car");

}







