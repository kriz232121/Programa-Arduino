
//VELOCIDAD DE BANDA NIVEL 3
//VELOCIDAD APLICADOR NIVEL 5
//MOVISTAR MOTOR 3: P=405 / E=1250
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
#define MICROSTEPS 4
#define DIR 2//2
#define STEP 3//3
#define SLEEP 4//4

//MOTOR2 CORRRECTO
#define MICROSTEPS2 4
#define DIR2 5
#define STEP2 6
#define SLEEP2 7

//MOTOR3
#define MICROSTEPS3 4
#define DIR3 8 //8
#define STEP3 9 //9
#define SLEEP3 10 //10

//MOTOR4
#define MICROSTEPS4 4
#define DIR4 11 //11
#define STEP4 12 //12
#define SLEEP4 13 //13

const int sensorPin1= 20;
const int sensorPin2= 19;
const int sensorPin3= 18;
const int sensorEtiqPin= 21;

int value = 0;
//Uncomment line to use enable/disable functionality

const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;
int totalChars=0;

bool stopMotor1 = true;
bool stepMotor1 = true;
int timeMotor1 = 5000;
int contTimeMotor1 = 0;

int timeMotor2 = 700;
int contTimeMotor2 = 0;
bool stopMotor2 = false;
bool stepMotor2 = true;

bool moveMotor3 = false;
int timeStartMotor3 = 101;
int stepsMotor3 = 215;
int contTimeMotor3 = 0;
bool sensorEtiqueta = false;
int contEtiquetas = 0;
int totalEtiquetas = 2;
bool onEtiqueta = false;

bool moveMotor4 = false;
int timeStartMotor4 = 500;
int stepsMotor4 = 150;
int contTimeMotor4 = 0;
bool disableMotor4 = false;

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
    pinMode(sensorEtiqPin,INPUT);
    
    attachInterrupt(3,sensor1,FALLING); //PIN 20
    attachInterrupt(4,sensor2,FALLING); //PIN 19
    attachInterrupt(5,sensor3,FALLING); //PIN 18
    // attachInterrupt(2,sensorEtiq,FALLING); //PIN 21

    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    stepper1.setEnableActiveState(HIGH);
    stepper2.setEnableActiveState(LOW);
    stepper3.setEnableActiveState(LOW);
    stepper4.setEnableActiveState(LOW);

//    Serial.println("Inicio programa!!");

}

void loop() {
  motor1();
  motor2();
  motor3();
  motor4();
  recvWithStartEndMarkers();
  serialComOptions();
  delayMicroseconds(500);
}

void sensor1(){
    //PARA MOTOR 1 INMEDIATAMENTE
    int cont = 0;
    int val = 0;
    while(val == 0 && cont <= 2)
    {
      val = digitalRead(sensorPin1);  //lectura digital de pin
      cont++;
    }
    if(cont == 3){
//      Serial.println("PARAR MOTOR 1");
      stepper1.stop();
      stopMotor1 = true;
      Serial.write(15);
    }
}

void sensor2(){
    //MUEVE MOTOR 3
    int cont = 0;
    int val = 0;
    while(val == 0 && cont <= 2)
    {
      val = digitalRead(sensorPin2);  //lectura digital de pin
      cont++; 
    }
    if(cont == 3){
    // Serial.println("MOVER MOTOR 3");
    //! MUEVO ESTE SERIAL WRITE A OTRO DE LOS SENSORES (3)
    // Serial.write(18);
      moveMotor3 = true;
      contEtiquetas=0;
    }
}

// void sensorEtiq(){

//     //Serial.println("entra");
//     //onEtiqueta
//     //SISTEMA ANTIREBOTES
//     int cont = 0;
//     int val = 0;
//     //!ASUMIENDO QUE EL SENSOR DE ETIQUETAS ME DA BAJO CUANDO NO HAY ETIQUETA
//     while(val == 0 && cont <= 2)
//     {
//       val = digitalRead(sensorEtiqPin);  //lectura digital de pin
//       cont++; 
//     }
//     if(cont == 3){
//         //AUMENTO CONTADOR 
// //          int val = digitalRead(sensorEtiqPin);
// //          Serial.println(val);
// //          Serial.println(onEtiqueta);
// //          if(!onEtiqueta && val == 0){
// //            onEtiqueta = true;
// //          }else if(onEtiqueta && val == 1){
// //            onEtiqueta = false;
//             contEtiquetas++;
//             Serial.println("cuenta");
// //          }
//         //!CUANDO SE TERMINA DE COLOCAR ETIQUETAS SE PARA EL MOTOR 3 Y SE REINICIA SU CONTADOR
//         if(contEtiquetas == totalEtiquetas){
//             Serial.println("para");
//             contTimeMotor3 = 0;
//             contEtiquetas = 0;
//             moveMotor3 = false;
//         }
//     } 
// }

void sensor3(){
    //MUEVE MOTOR 4
    int cont = 0;
    int val = 0;
    while(val == 0 && cont <= 2)
    {
      val = digitalRead(sensorPin3);  //lectura digital de pin
      cont++;
    }
    if(cont == 3 && !disableMotor4){
//      Serial.println("MOVER MOTOR 4");
      Serial.write(18);
      moveMotor4 = true;
    }
}

void motor1(){
//INICIA A MOVERSE DESPUÉS DE UN TIEMPO
  if(contTimeMotor1 == timeMotor1){
    // DA UN PASO CADA 2 LOOPS
      if(!stopMotor1 && stepMotor1){
        stepMotor1=false;
        stepper1.move(1);
      }else{
        stepMotor1=true;
      }
  }else{
      contTimeMotor1++;
  }
}

void motor2(){
  if((!stopMotor1 || contTimeMotor2 <= timeMotor2) && !stopMotor2 && stepMotor2 ){
    stepMotor2=false;
    stepper2.move(1);
    if(stopMotor1){
      contTimeMotor2++;
    }
  }else{
    stepMotor2=true;
  }
}

void motor3(){

    //! MOTOR 3 YA NO SE MUEVE POR TIEMPO SINO HASTA QUE SE LE INDIQUE LO CONTRARIO
 if(moveMotor3 && contEtiquetas < totalEtiquetas){
     //MANTENGO LA VELOCIDAD POR LO QUE USO EL CONTTIMEMOTOR PARA QUE DE 1 PASO CADA 2 LOOPS
     //TAMBIÉN MANTENGO EL TIEMPO DE ESPERA PARA INICIO DE ETIQUETADO
    if(contTimeMotor3 > timeStartMotor3 && contTimeMotor3%2 == 0){
      stepper3.move(1);
      
      //DESPUÉS DE DAR UN PASO VERIFICO SI EL SENSOR DETECTA CAMBIO (PASO DE ESPACIO A ETIQUETA
      //O DE ETIQUETA A ESPACIO)
      int val = digitalRead(sensorEtiqPin);
      if(!onEtiqueta && val == 1){
          onEtiqueta = true;
      }else if(onEtiqueta && val == 0){
          onEtiqueta = false;
          contEtiquetas++;
      }
    }
     contTimeMotor3++; 
  }else if(contEtiquetas == totalEtiquetas){
      Serial.println("para");
      contTimeMotor3 = 0;
      contEtiquetas = 0;
      moveMotor3 = false;
  }
}

void motor4(){
 if(moveMotor4 && contTimeMotor4 <= (stepsMotor4+timeStartMotor4) && !disableMotor4){
    if(contTimeMotor4 > timeStartMotor4 && contTimeMotor4%2 == 0){
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
      if(receivedChars[0] == 'I'){
//        Serial.println("Inicio");
            stopMotor1 = false;
            stopMotor2 = false;
            contTimeMotor2 = 0;
            contTimeMotor1 = 0;
            totalChars = 0;
      }else if(receivedChars[0] == 'S'){
//        Serial.println("PARO GENERAL");
            stopMotor1 = true;
            stopMotor2 = true;
            moveMotor3 = false;
            moveMotor4 = false;
            totalChars = 0;
      }else if(receivedChars[0] == 'P'){
        String nuevosPasos = "";
            if(receivedChars[1] == '3'){
//              Serial.println("MOD PASOS 3");
              for(int i = 2; i <= totalChars; i++){
                nuevosPasos.concat(receivedChars[i]);
              }
              stepsMotor3 = nuevosPasos.toInt();
              contTimeMotor3 = 0;
            }else if(receivedChars[1] == '4'){
//              Serial.println("MOD PASOS 4");
              for(int i = 2; i <= totalChars; i++){
                nuevosPasos.concat(receivedChars[i]);
              }
              stepsMotor4 = nuevosPasos.toInt();
              contTimeMotor4 = 0;
            }
            totalChars = 0;
      }else if(receivedChars[0] == 'E'){
        String nuevaEspera = "";
            if(receivedChars[1] == '3'){
//              Serial.println("MOD ESPERA 3");
              for(int i = 2; i <= totalChars; i++){
                nuevaEspera.concat(receivedChars[i]);
              }
              timeStartMotor3 = nuevaEspera.toInt();
              contTimeMotor3 = 0;
            }else if(receivedChars[1] == '4'){
//              Serial.println("MOD ESPERA 4");
              for(int i = 2; i <= totalChars; i++){
                nuevaEspera.concat(receivedChars[i]);
              }
              timeStartMotor4 = nuevaEspera.toInt();
              contTimeMotor4 = 0;
            }
            totalChars = 0;
            
      }else if(receivedChars[0] == 'D'){
            // MOTOR DESHABILITAR 4
            disableMotor4 = true ;
            
      }else if(receivedChars[0] == 'H'){
            // MOTOR HABILITAR 4
            disableMotor4 = false ;
      }else if(receivedChars[0] == 'T'){
            // SETEAR TOTAL ETIQUETAS
            totalEtiquetas = String(receivedChars[1]).toInt() ;
      }

      newData = false;
     }
}
