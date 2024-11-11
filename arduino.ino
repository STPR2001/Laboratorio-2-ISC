unsigned long lastMillis = 0;
unsigned long interval = 1000;

String incomingData;

int transitorLeftTop = 4;
int transitorLeftBottom = 13;
int transitorRightTop = 3;
int transitorRightBottom = 12;

void setup() {
  Serial.begin(9600);

  pinMode(transitorLeftTop, OUTPUT);
  pinMode(transitorLeftBottom, OUTPUT);
  pinMode(transitorRightTop, OUTPUT);
  pinMode(transitorRightBottom, OUTPUT);

  digitalWrite(transitorLeftTop, LOW);
  digitalWrite(transitorRightBottom, LOW);
  digitalWrite(transitorRightTop, HIGH);
  digitalWrite(transitorLeftBottom, HIGH);
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

    int actionStartIndex = incomingData.indexOf("\"action\":\"") + 10;
    int actionEndIndex = incomingData.indexOf("\"", actionStartIndex);

    String actionValue = incomingData.substring(actionStartIndex, actionEndIndex);

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