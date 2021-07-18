#include <Arduino.h>
#include "BasicStepperDriver.h"
/// PASOS DEL MOTOR SI EL DE 1.8 GRADOS SON 200
#define MOTOR_STEPS 200
// VEL MOTOR 1
#define RPM 150
#define RPM3 120
#define RPM4 80


//// VALOR DEFINIDO POR EL CONTROLADRO DEL MOTOR A PASOS
// MOTOR 1 PINES
#define MICROSTEPS 1
#define DIR 2//2
#define STEP 3//3
#define SLEEP 4//4

//MOTOR2 CORRRECTO
#define MICROSTEPS2 1
#define DIR2 5
#define STEP2 6
#define SLEEP2 7

//MOTOR3
#define MICROSTEPS3 1
#define DIR3 8 //8
#define STEP3 9 //9
#define SLEEP3 10 //10

//MOTOR4
#define MICROSTEPS4 1
#define DIR4 11 //11
#define STEP4 12 //12
#define SLEEP4 13 //13

const int sensorPin1= 20;
const int sensorPin2= 19;
const int sensorPin3= 18;

int value = 0;
//Uncomment line to use enable/disable functionality

const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;
int totalChars=0;

bool stopMotor1 = true;

int timeMotor2 = 900;
int contTimeMotor2 = 0;
bool stopMotor2 = false;

bool moveMotor3 = false;
int timeStartMotor3 = 250;
int stepsMotor3 = 215;
int contTimeMotor3 = 0;

bool moveMotor4 = false;
int timeStartMotor4 = 300;
int stepsMotor4 = 150;
int contTimeMotor4 = 0;

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper1(MOTOR_STEPS, DIR, STEP, SLEEP);
BasicStepperDriver stepper2(MOTOR_STEPS, DIR2, STEP2, SLEEP2);
BasicStepperDriver stepper3(MOTOR_STEPS, DIR3, STEP3, SLEEP3);
BasicStepperDriver stepper4(MOTOR_STEPS, DIR4, STEP4, SLEEP4);

void setup() {
    Serial.begin(9600);
    
    stepper1.begin(RPM, MICROSTEPS);
    stepper2.begin(RPM, MICROSTEPS2);
    stepper3.begin(RPM3, MICROSTEPS3);
    stepper4.begin(RPM4, MICROSTEPS4);
    
    pinMode(sensorPin1,INPUT);
    pinMode(sensorPin2,INPUT);
    pinMode(sensorPin3,INPUT);
    
    attachInterrupt(3,sensor1,FALLING); //PIN 21
    attachInterrupt(4,sensor2,FALLING); //PIN 20
    attachInterrupt(5,sensor3,FALLING); //PIN 19

    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    stepper1.setEnableActiveState(LOW);
    stepper2.setEnableActiveState(LOW);
    stepper3.setEnableActiveState(LOW);
    stepper4.setEnableActiveState(LOW);

    Serial.println("Inicio programa!!");

}

void loop() {
  motor1();
  motor2();
  motor3();
  motor4();
  recvWithStartEndMarkers();
  serialComOptions();
  delay(2);
}

void sensor1(){
    //PARA MOTOR 1 INMEDIATAMENTE
    value = digitalRead(sensorPin1);  //lectura digital de pin
    Serial.println("PARAR MOTOR 1");
    stepper1.stop();
    stopMotor1 = true;
}

void sensor2(){
    //MUEVE MOTOR 3
    value = digitalRead(sensorPin2);  //lectura digital de pin
    Serial.println("MOVER MOTOR 3");
    moveMotor3 = true;
}

void sensor3(){
    //MUEVE MOTOR 4
    value = digitalRead(sensorPin3);  //lectura digital de pin
    Serial.println("MOVER MOTOR 4");
    moveMotor4 = true;
}

void motor1(){
  if(!stopMotor1){
    stepper1.move(1);
  }
}

void motor2(){
  if((!stopMotor1 || contTimeMotor2 <= timeMotor2) && !stopMotor2 ){
    stepper2.move(1);
    if(stopMotor1){
      contTimeMotor2++;
    }
  }
}

void motor3(){
  
 if(moveMotor3 && contTimeMotor3 <= (stepsMotor3+timeStartMotor3)){
    if(contTimeMotor3 > timeStartMotor3){
      stepper3.move(1);
    }
    if(contTimeMotor3 == (stepsMotor3+timeStartMotor3)){
      contTimeMotor3 = 0;
      moveMotor3 = false;
    }else{
     contTimeMotor3++; 
    }
  }
}

void motor4(){
 if(moveMotor4 && contTimeMotor4 <= (stepsMotor4+timeStartMotor4)){
    if(contTimeMotor4 > timeStartMotor4){
      stepper4.move(1);
    }
    if(contTimeMotor4 == (stepsMotor4+timeStartMotor4)){
      contTimeMotor4 = 0;
      moveMotor4 = false;
    }else{
     contTimeMotor4++; 
    }
  }
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '*';
    char endMarker = '*';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                totalChars++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}
void serialComOptions() {
    if (newData == true) {
        //Serial.print("This just in ... ");
        switch(receivedChars[0]){
          case 'I':
            Serial.println("Inicio");
            stopMotor1 = false;
            contTimeMotor2 = 0;
            totalChars = 0;
            break;
          case 'S':
            Serial.println("PARO GENERAL");
            stopMotor1 = true;
            stopMotor2 = true;
            moveMotor3 = false;
            moveMotor4 = false;
            totalChars = 0;
            break;
          case 'E':
            String nuevaEspera = "";
            if(receivedChars[1] == '3'){
              Serial.println("MOD ESPERA 3");
              for(int i = 2; i <= totalChars; i++){
                nuevaEspera.concat(receivedChars[i]);
              }
              timeStartMotor3 = nuevaEspera.toInt();
              contTimeMotor3 = 0;
            }else if(receivedChars[1] == '4'){
              Serial.println("MOD ESPERA 4");
              for(int i = 2; i <= totalChars; i++){
                nuevaEspera.concat(receivedChars[i]);
              }
              timeStartMotor4 = nuevaEspera.toInt();
              contTimeMotor4 = 0;
            }
            totalChars = 0;
            break;
          case 'P':
            String nuevosPasos = "";
            if(receivedChars[1] == '3'){
              Serial.println("MOD PASOS 3");
              for(int i = 2; i <= totalChars; i++){
                nuevosPasos.concat(receivedChars[i]);
              }
              stepsMotor3 = nuevosPasos.toInt();
              contTimeMotor3 = 0;
            }else if(receivedChars[1] == '4'){
              Serial.println("MOD PASOS 4");
              for(int i = 2; i <= totalChars; i++){
                nuevosPasos.concat(receivedChars[i]);
              }
              stepsMotor4 = nuevosPasos.toInt();
              contTimeMotor4 = 0;
            }
            totalChars = 0;
            break;
          default:
            Serial.println("default");
            totalChars = 0;
            break;
        }
        newData = false;
    }
}