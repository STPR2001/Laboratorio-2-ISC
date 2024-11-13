#include <Servo.h>
#include <Stepper.h>

unsigned long lastMillis = 0;
unsigned long interval = 1000;

String incomingData;

int transitorLeftTop = 4;
int transitorLeftBottom = 13;
int transitorRightTop = 3;
int transitorRightBottom = 12;

const int stepPin = 5;      // Pin para el motor paso a paso
const int dirPin = 6;       // Pin para la dirección del motor paso a paso

Servo servomotor;
int servoPin = 11;

const int pasosPorRevolucion = 2048;
Stepper steppermotor(pasosPorRevolucion, 5,6,7,8);

bool motorState = true; // Variable para controlar el estado del motor

void setup() {
  Serial.begin(9600);

  pinMode(transitorLeftTop, OUTPUT);
  pinMode(transitorLeftBottom, OUTPUT);
  pinMode(transitorRightTop, OUTPUT);
  pinMode(transitorRightBottom, OUTPUT);

  servomotor.attach(servoPin);

  digitalWrite(transitorLeftTop, LOW);
  digitalWrite(transitorRightBottom, LOW);
  digitalWrite(transitorRightTop, HIGH);
  digitalWrite(transitorLeftBottom, HIGH);

  servomotor.write(0); 
  steppermotor.setSpeed(15);   
}

void loop() {
  //steppermotor.setSpeed(pasosPorRevolucion);  

  unsigned long currentMillis = millis();

  // Los datos se envían a la scada cada 1 segundo
  if (currentMillis - lastMillis >= interval) {
    lastMillis = currentMillis;
    emitirDatos();
  }

  if (Serial.available()) {
    incomingData = Serial.readStringUntil('\n');

    int typeStartIndex = incomingData.indexOf("\"type\":\"") + 8;
    int typeEndIndex = incomingData.indexOf("\"", typeStartIndex);

    String typeValue = incomingData.substring(typeStartIndex, typeEndIndex);

    int actionStartIndex = incomingData.indexOf("\"action\":\"") + 10;
    int actionEndIndex = incomingData.indexOf("\"", actionStartIndex);

    String actionValue = incomingData.substring(actionStartIndex, actionEndIndex);

    

    if (typeValue == "Servo") {
      if (actionValue == "setAngle"){
        int angleStartIndex = incomingData.indexOf("\"angle\":\"") + 9;
        int angleEndIndex = incomingData.indexOf("\"", angleStartIndex);

        String angleValue = incomingData.substring(angleStartIndex, angleEndIndex);
        changeServoAngle(angleValue);
      }
    }

    if (typeValue == "DC") {
      
      if (actionValue == "reverse") {
        changeDCDirection();
      }
      
      if (actionValue == "toggle") {
        if ( motorState ){
          Serial.println("pasa por apagar");
          turnOffMotor();
        }else {
          Serial.println("pasa por prender");
          turnOnMotor();
        }        
      }
    }

    if (typeValue == "PaP"){
      int stepStartIndex = incomingData.indexOf("\"steps\":") + 8;
      int stepEndIndex = incomingData.indexOf(",", stepStartIndex);
      String pasosPorRevolucion = incomingData.substring(stepStartIndex, stepEndIndex);
      const char* cpasosPorRevolucion = pasosPorRevolucion.c_str();
      int pasosRevol = atoi(cpasosPorRevolucion);
      
      int dirStartIndex = incomingData.indexOf("\"direction\":\"") + 13;
      int dirEndIndex = incomingData.indexOf("\"", dirStartIndex);
      String direccionValue = incomingData.substring(dirStartIndex, dirEndIndex);
      Serial.println(incomingData);
      Serial.println("step: "+ String(dirStartIndex));
      Serial.println("step: "+ String(dirEndIndex));
      Serial.println("pasos: "+ pasosPorRevolucion);
      Serial.println("direccion: "+direccionValue);

      if ( direccionValue == "clockwise"){
        Serial.println("entra en horario");
        steppermotor.step(pasosRevol);    // Establecer dirección
      }else{
        Serial.println("entra en NO horario"+String(-pasosRevol));
        steppermotor.step( pasosRevol * -1);     // Establecer dirección
      }
      // Control del motor paso a paso
      
      /* for (int i = 0; i < stepsValue ; i++) { // Hacer 200 pasos
          digitalWrite(stepPin, HIGH);
          delayMicroseconds(1000);     // Esperar un microsegundo
          digitalWrite(stepPin, LOW);
          delayMicroseconds(1000);
      } */
    }
    incomingData = "";
  }
}

void emitirDatos() {
  // DC
  // Estado del motor
  String motorStatus = motorState ? "encendido" : "apagado";

  String sentido = digitalRead(transitorLeftTop) ? "horario" : "antihorario";
  Serial.println("{ \"type\": \"DC\", \"status\": \"" + sentido + " | " + motorStatus + "\" }");
  // Servo
  Serial.println("{ \"type\": \"Servo\", \"status\": \"angle: " + String(servomotor.read()) + "\" }");
  // PaP
  
  //Serial.println("{ \"type\": \"DC\", \"motorStatus\": \"" + motorStatus + "\" }");
}

void changeDCDirection() {
  if (digitalRead(transitorLeftTop)) {
    digitalWrite(transitorLeftTop, LOW);
    digitalWrite(transitorRightBottom, LOW);
    digitalWrite(transitorRightTop, HIGH);
    digitalWrite(transitorLeftBottom, HIGH);
  } else {
    digitalWrite(transitorLeftTop, HIGH);
    digitalWrite(transitorRightBottom, HIGH);
    digitalWrite(transitorRightTop, LOW);
    digitalWrite(transitorLeftBottom, LOW);
  }
}

void turnOnMotor() {
  // Encender el motor
  digitalWrite(transitorLeftTop, HIGH);
  digitalWrite(transitorRightBottom, HIGH);
  digitalWrite(transitorRightTop, LOW);
  digitalWrite(transitorLeftBottom, LOW);
  
  motorState = true; // Actualizar estado del motor
}

void turnOffMotor() {
  // Apagar el motor
  digitalWrite(transitorLeftTop, LOW);
  digitalWrite(transitorRightBottom, LOW);
  
  // Mantener la dirección anterior si es necesario
  motorState = false; // Actualizar estado del motor
}

void changeServoAngle(String angle) {
  int val = map(angle.toInt(), 0, 180, 0, 180);
  servomotor.write(val);
}