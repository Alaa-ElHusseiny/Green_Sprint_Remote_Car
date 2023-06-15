/* 
the reciever code that is attached to the remot car 
author : john remon 21/4/2023
code describtion :this code if for controlling the car speed and the turning all the car movement 
this code recieves from nrf24l01 module that have 6 pipes of sendin or resieving 
this code uses only two pipes pipe 1 for the speed of the car (high_speed(default),low_speed,stop)
and pipe 2 that is have 90% of totla load on the module as it sends the degree of movement of the remote car ;
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>



#define CE_PIN 9
#define CSN_PIN 10
#define left_motor_1 7
#define left_motor_2 8
#define right_motor_1 4
#define right_motor_2 3
#define pwm_right_motor 5
#define pwm_left_motor 6

//if you changed the high speed you must changd right_stage_three_high_speed and left_stage_three_high_speed with the same value
#define high_speed 255
#define left_stage_one_highest_speed 70
#define left_stage_two_highest_speed 150
#define left_stage_three_highest_speed 255//must be 255 so the left wheel will not move or equal to high speed
#define right_stage_one_highest_speed 70
#define right_stage_two_highest_speed 150
#define right_stage_three_highest_speed 255//must be 255 so the left wheel will not move


//if you changed the low speed you must changd right_stage_three_low_speed and left_stage_three_low_speed with the same value
#define low_speed 180
#define left_stage_one_low_speed 20
#define left_stage_two_low_speed 100
#define left_stage_three_low_speed 180
#define right_stage_one_low_speed 20
#define right_stage_two_low_speed 100
#define right_stage_three_low_speed 180


#define very_small_amount_of_drift 0


RF24 radio(CE_PIN,CSN_PIN);//those are the cr_pin and the csn_pin connected to the arduino
const uint64_t pipes[2] = {0xABADACAEA1LL, 0xABADACAEA2LL};//The adresse that must be in the reciever code 
uint8_t speed=0;//this variable is for regulating speed and ranges from 0 to 255 as 255 is the hightest speed
int recieved_number=0;//this number is made only for map function that is used in turn mechanism functions
int num;
uint8_t pipeNum = 0;

void setup() {

Serial.begin(9600);
radio.begin();
radio.setPALevel(RF24_PA_HIGH);
radio.setChannel(76); // Set the channel to 76
radio.setDataRate(RF24_250KBPS); // Set the data rate to 250 kbps
radio.openReadingPipe(1, pipes[0]); // Open pipe 0 for reading
radio.openReadingPipe(2, pipes[1]); // Open pipe 1 for reading
radio.startListening(); // Start listening for incoming messages

 pinMode(left_motor_1,OUTPUT);
 pinMode(left_motor_2,OUTPUT);
 pinMode(right_motor_1,OUTPUT);
 pinMode(right_motor_2,OUTPUT);
 pinMode(pwm_right_motor,OUTPUT);
 pinMode(pwm_left_motor,OUTPUT);
speed=2;
}

void loop() {

 
 if (radio.available()) 
 {// Check if there is data available
     // Define a variable to store the received number
     // Define a variable to store the pipe number
     
 while (radio.available(&pipeNum)) // Loop through all available pipes
 {  
      radio.read(&num, sizeof(num)); // Read the data from the pipe
      if (pipeNum==1)// this means that the sending number is for regulating the speed not the direction
      {
         if (num==2)//this means that we are still moving with our hightest speed 
         {
           speed=2;
           Serial.print("movint with our highest speed");
         }
         if(num==1)//this means that we are moving with our low speed
         {
           speed=1;
           Serial.print("moving with our lowest speed");
         }
         if (num==0)//this means that the car must stop
         {
           speed=0;
            digitalWrite(left_motor_1,LOW);
           digitalWrite(left_motor_2,LOW);
           digitalWrite(right_motor_1,LOW);
           digitalWrite(right_motor_2,LOW);
           Serial.println("stop the car");
            }
            if (num==3)
            {
             speed=3;
            Serial.println("Reverse the car");
            }
            }
         }
      }
      else if (pipeNum==2)//this means that the sending number is for regulating the direcrtion 
      {
         recieved_number= map(num, 0, 1023, 0, 255);
         

        if (speed==2)//moving with our highest speed 
        {
          digitalWrite(left_motor_1,HIGH);
          digitalWrite(left_motor_2,LOW);
          digitalWrite(right_motor_1,HIGH);
          digitalWrite(right_motor_2,LOW);
          if ((recieved_number>0)&&(recieved_number<36))//
          {
            analogWrite(pwm_left_motor,high_speed-left_stage_three_highest_speed);//left_stage_three_highest_speed  = 255
            analogWrite(pwm_right_motor,high_speed-0);
             Serial.println("sharp left highest speed");
          }
          if ((recieved_number>36)&&(recieved_number<72))
          {
            analogWrite(pwm_left_motor,high_speed-left_stage_two_highest_speed);//left_stage_two_highest_speed 70
            analogWrite(pwm_right_motor,high_speed-0);
             Serial.println("soft left highest speed");
          }
          if ((recieved_number>72)&&(recieved_number<108))
          {
            analogWrite(pwm_left_motor,high_speed-left_stage_one_highest_speed);//left_stage_one_highest_speed 20
            analogWrite(pwm_right_motor,high_speed-0);
             Serial.println("soft soft left highest speed");
          }
          if ((recieved_number>108)&&(recieved_number<120))//this is only for be totally stright on the road if he slipt with a very small amount to the right
          {
            analogWrite(pwm_left_motor,high_speed-very_small_amount_of_drift);//very_small_amount_of_drift 5
            analogWrite(pwm_right_motor,high_speed-0);
             Serial.println("forward with high speed but trying to go back to road with small amount of drift left");
          }
          if ((recieved_number>120)&&(recieved_number<132))
          {
            analogWrite(pwm_left_motor,high_speed-0);
            analogWrite(pwm_right_motor,high_speed-0);
             Serial.println("forward with highest speed");
          }
          if ((recieved_number>132)&&(recieved_number<144))//this is only for be totally stright on the road if he slipt with a very small amount to the left
          {
            analogWrite(pwm_left_motor,high_speed-0);
            analogWrite(pwm_right_motor,high_speed-very_small_amount_of_drift);//very_small_amount_of_drift 5
             Serial.println("forward with high speed but trying to go back to road with small amount of drift right");
          }          
          if ((recieved_number>144)&&(recieved_number<180))
          {
            analogWrite(pwm_left_motor,high_speed-0);
            analogWrite(pwm_right_motor,high_speed- right_stage_one_highest_speed);//righ_stage_one_highest_speed 20
             Serial.println("soft soft right highest speed");
          }
          if ((recieved_number>180)&&(recieved_number<216))
          {
            analogWrite(pwm_left_motor,high_speed-0);
            analogWrite(pwm_right_motor,high_speed- right_stage_two_highest_speed);//righ_stage_two_highest_speed 70
             Serial.println("soft right highest speed");
          }
          if ((recieved_number>216)&&(recieved_number<255))
          {
            analogWrite(pwm_left_motor,high_speed-0);
            analogWrite(pwm_right_motor,high_speed- right_stage_three_highest_speed);//righ_stage_three_highest_speed 255
             Serial.println("sharp right highest speed");
          }
        }
        else if (speed==1)//we are moving on low speed low speed =180;
        {
           digitalWrite(left_motor_1,HIGH);
           digitalWrite(left_motor_2,LOW);
           digitalWrite(right_motor_1,HIGH);
           digitalWrite(right_motor_2,LOW);
            if ((recieved_number>0)&&(recieved_number<36))
          {
            analogWrite(pwm_left_motor,low_speed-left_stage_three_low_speed);//left_stage_three_low_speed  = 180
            analogWrite(pwm_right_motor,low_speed-0);
             Serial.println("sharp left low speed");
          }
           if ((recieved_number>36)&&(recieved_number<72))
          {
            analogWrite(pwm_left_motor,low_speed-left_stage_two_low_speed);//left_stage_two_highest_speed 70
            analogWrite(pwm_right_motor,low_speed-0);
             Serial.println("soft left low speed");
          }
          if ((recieved_number>72)&&(recieved_number<108))
          {
            analogWrite(pwm_left_motor,low_speed-left_stage_one_low_speed);//left_stage_one_low_speed 20
            analogWrite(pwm_right_motor,low_speed-0);
             Serial.println("soft soft left low speed");
          }
          if ((recieved_number>108)&&(recieved_number<120))//this is only for be totally stright on the road if he slipt with a very small amount to the right
          {
            analogWrite(pwm_left_motor,low_speed-very_small_amount_of_drift);//very_small_amount_of_drift 5
            analogWrite(pwm_right_motor,low_speed-0);
             Serial.println("forward with high speed but trying to go back to road with small amount of drift left");
          }
           if ((recieved_number>120)&&(recieved_number<132))
          {
            analogWrite(pwm_left_motor,low_speed-0);//forward with low speed 180
            analogWrite(pwm_right_motor,low_speed-0);
             Serial.println("forward with low speed");
          }
          if ((recieved_number>132)&&(recieved_number<144))//this is only for be totally stright on the road if he slipt with a very small amount to the left
          {
            analogWrite(pwm_left_motor,low_speed-0);
            analogWrite(pwm_right_motor,low_speed-very_small_amount_of_drift);//very_small_amount_of_drift 5
             Serial.println("forward with low speed but trying to go back to road with small amount of drift right");
          }
          if ((recieved_number>144)&&(recieved_number<180))
          {
            analogWrite(pwm_left_motor,low_speed-0);
            analogWrite(pwm_right_motor,low_speed- right_stage_one_low_speed);//righ_stage_one_low_speed 20
             Serial.println("soft soft right low speed");
          }
          if ((recieved_number>180)&&(recieved_number<216))
          {
            analogWrite(pwm_left_motor,low_speed-0);
            analogWrite(pwm_right_motor,low_speed- right_stage_two_low_speed);//righ_stage_two_low_speed 70
             Serial.println("soft right low speed");
          }
          if ((recieved_number>216)&&(recieved_number<=255))
          {
            analogWrite(pwm_left_motor,low_speed-0);
            analogWrite(pwm_right_motor,low_speed- right_stage_three_low_speed);//righ_stage_three_low_speed 180
             Serial.println("sharp right low speed");
          }
        
        }
        else if(speed==0)
        {
          digitalWrite(left_motor_1,LOW);
          digitalWrite(left_motor_2,LOW);
          digitalWrite(right_motor_1,LOW);
          digitalWrite(right_motor_2,LOW);
          Serial.println("stop the car");
        }
        else if (speed==3)
        {
          digitalWrite(left_motor_1,LOW);
          digitalWrite(left_motor_2,HIGH);
          digitalWrite(right_motor_1,LOW);
          digitalWrite(right_motor_2,HIGH);
          Serial.println("Reverse the car");
            if ((recieved_number>0)&&(recieved_number<36))
          {
            analogWrite(pwm_left_motor,low_speed-left_stage_three_low_speed);//left_stage_three_low_speed  = 180
            analogWrite(pwm_right_motor,low_speed-0);
             Serial.println("sharp left low speed");
          }
           if ((recieved_number>36)&&(recieved_number<72))
          {
            analogWrite(pwm_left_motor,low_speed-left_stage_two_low_speed);//left_stage_two_highest_speed 70
            analogWrite(pwm_right_motor,low_speed-0);
             Serial.println("soft left low speed");
          }
          if ((recieved_number>72)&&(recieved_number<108))
          {
            analogWrite(pwm_left_motor,low_speed-left_stage_one_low_speed);//left_stage_one_low_speed 20
            analogWrite(pwm_right_motor,low_speed-0);
             Serial.println("soft soft left low speed");
          }
          if ((recieved_number>108)&&(recieved_number<120))//this is only for be totally stright on the road if he slipt with a very small amount to the right
          {
            analogWrite(pwm_left_motor,low_speed-very_small_amount_of_drift);//very_small_amount_of_drift 5
            analogWrite(pwm_right_motor,low_speed-0);
             Serial.println("forward with high speed but trying to go back to road with small amount of drift left");
          }
           if ((recieved_number>120)&&(recieved_number<132))
          {
            analogWrite(pwm_left_motor,low_speed-0);//forward with low speed 180
            analogWrite(pwm_right_motor,low_speed-0);
             Serial.println("forward with low speed");
          }
          if ((recieved_number>132)&&(recieved_number<144))//this is only for be totally stright on the road if he slipt with a very small amount to the left
          {
            analogWrite(pwm_left_motor,low_speed-0);
            analogWrite(pwm_right_motor,low_speed-very_small_amount_of_drift);//very_small_amount_of_drift 5
             Serial.println("forward with low speed but trying to go back to road with small amount of drift right");
          }
          if ((recieved_number>144)&&(recieved_number<180))
          {
            analogWrite(pwm_left_motor,low_speed-0);
            analogWrite(pwm_right_motor,low_speed- right_stage_one_low_speed);//righ_stage_one_low_speed 20
             Serial.println("soft soft right low speed");
          }
          if ((recieved_number>180)&&(recieved_number<216))
          {
            analogWrite(pwm_left_motor,low_speed-0);
            analogWrite(pwm_right_motor,low_speed- right_stage_two_low_speed);//righ_stage_two_low_speed 70
             Serial.println("soft right low speed");
          }
          if ((recieved_number>216)&&(recieved_number<=255))
          {
            analogWrite(pwm_left_motor,low_speed-0);
            analogWrite(pwm_right_motor,low_speed- right_stage_three_low_speed);//righ_stage_three_low_speed 180
             Serial.println("sharp right low speed");
          }
        
          
        }
      }
    
}


/*
analogWrite(pwm_left_motor,normal_speed);
analogWrite(pwm_right_motor,normal_speed);

 digitalWrite(left_motor_1,HIGH);
 digitalWrite(left_motor_2,LOW);
 digitalWrite(right_motor_1,HIGH);
 digitalWrite(right_motor_2,LOW);
 Serial.println("forward");
 */
