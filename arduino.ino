#include <Servo.h>

unsigned long lastMillis = 0;
unsigned long interval = 1000;

String incomingData;

int transitorLeftTop = 4;
int transitorLeftBottom = 13;
int transitorRightTop = 3;
int transitorRightBottom = 12;

Servo servomotor;
int servoPin = 11;

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
}

void loop() {
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

    if (actionValue == "reverse") {
      changeDCDirection();
    }

    incomingData = "";
  }
}

void emitirDatos() {
  // DC
  String sentido = digitalRead(transitorLeftTop) ? "horario" : "antihorario";
  Serial.println("{ \"type\": \"DC\", \"status\": \"sentido " + sentido + "\" }");
  // Servo
  Serial.println("{ \"type\": \"Servo\", \"status\": \"angle: " + String(servomotor.read()) + "\" }");
  // PaP
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

void changeServoAngle(String angle) {
  int val = map(angle.toInt(), 0, 180, 0, 180);
  servomotor.write(val);
}