
#include <Wire.h>
#include <Servo.h>
#include "HUSKYLENS.h"
#include "SoftwareSerial.h"

const int motorA1 = 5; 
const int motorA2 = 4; 
const int motorB1 = 6; 
const int motorB2 = 7; 

int ID1 = 1;
HUSKYLENS huskylens;
Servo servo_9, servo_10;

int posPan = 90;
int oldPan = 90;
int posTilt = 90;
int oldTilt = 90;
float p = 0.05;
float d = 0.07;

int flip_i = 0;

void printResult(HUSKYLENSResult result);

void setup(){
  Serial.begin(115200);
  Wire.begin();
  servo_9.attach(9);
  servo_10.attach(10); 
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
    
  while (!huskylens.begin(Wire)){
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
    
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);        

  motorStop();
}

void loop(){
  if (!huskylens.request())
  Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
  else if(!huskylens.isLearned())
  {
  Serial.println(F("Object not learned!"));
  }
  else if(!huskylens.available())
  {
  Serial.println(F("Object disappeared!"));
  }
  else
  {
    
    while (huskylens.available())
    {
      HUSKYLENSResult result = huskylens.read();
   
      posPan -= int(float(result.xCenter-160)*p);
      posPan += int((posPan - oldPan)*d);
      
      if(posPan <= 40)        posPan = 40;
      else if(posPan >= 140) posPan = 140;
      servo_9.write(abs(posPan));
      Serial.print("posPan:");
      Serial.print(posPan);
      Serial.print(" center:");
      Serial.print((result.xCenter - 160));
      delay(50);

      posTilt += int(float(result.yCenter-160)*p);
      posTilt -= int((posTilt - oldTilt)*d);
      
      if(posTilt <= 40)        posTilt = 40;
      else if(posTilt >= 120) posTilt = 120;
      servo_10.write(abs(posTilt));
      Serial.print("posTilt:");
      Serial.print(posTilt);
      Serial.print(" center:");
      Serial.print((result.yCenter - 160));
      delay(50);      
      
      if(abs(result.xCenter -160 ) < 40 && abs(posPan-90) > 40 && flip_i){
        if(posPan >= 140){
          moveLeft(); 
          }else if(posPan <= 40){
            moveRight(); 
            }
            flip_i = 0;
            }
            else{
              motorStop();           
              flip_i = 1;
            }
            oldPan = posPan;
    }
  }
}

void printResult(HUSKYLENSResult result){
  if (result.command == COMMAND_RETURN_BLOCK){
    Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
  }

  else if (result.command == COMMAND_RETURN_ARROW){
    Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
  }
  else{
    Serial.println("Object unknown!");
  }   
}

void moveRight(){
  digitalWrite(motorA1, HIGH);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);
}

void moveLeft(){
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, HIGH);
  digitalWrite(motorB2, LOW);
}

void motorStop(){
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);  
}
