#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gateServo;

// פינים
const int trigPin = 2;
const int echoPin = 3;

const int greenLed = 6;
const int redLed = 7;
const int yellowLed = 8;

const int servoPin = 9;

const int ldrPin = A0;
const int tempPin = A1;
const int flamePin = A2;
bool fireDetected = false;
bool fireDisplayActive = false;

unsigned long fireStartTime = 0;

const unsigned long FIRE_DISPLAY_TIME = 60000;

// מצבים
bool gateOpen = false;
bool manualOpen = false;
bool manualClose = false;
bool nightMode = false;

unsigned long lastDetectionTime = 0;

// הבהוב צהוב
unsigned long previousBlink = 0;
bool yellowState = false;
const int blinkInterval = 300;

void showTemperature() {
  if (fireDisplayActive)
  return;
  int sensorValue = analogRead(tempPin);

  float voltage = sensorValue * (5.0 / 1023.0);

  float temperature = voltage * 100.0;

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature, 1);
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(0, 1);

  if (nightMode)
    lcd.print("Night: ON ");
  else
    lcd.print("Night: OFF");
}

void openGate() {

  gateServo.write(180);
  gateOpen = true;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gate Open");

  Serial.println("Gate Open");
}

void closeGate() {

  digitalWrite(yellowLed, LOW);

  // הבהוב אדום בזמן סגירה
  for (int i = 0; i < 6; i++) {

    digitalWrite(redLed, HIGH);
    delay(150);

    digitalWrite(redLed, LOW);
    delay(150);
  }

  gateServo.write(0);

  gateOpen = false;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gate Closed");

  Serial.println("Gate Closed");

  delay(1500);

  showTemperature();
}

void setup() {

  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(flamePin, INPUT);

  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, LOW);
  digitalWrite(yellowLed, LOW);

  gateServo.attach(servoPin);
  gateServo.write(0);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Smart Gate");

  lcd.setCursor(0, 1);
  lcd.print("Ready");

  delay(2000);

  showTemperature();
}

void loop() {

  // ======================
  // מצב לילה
  // ======================

  int lightValue = analogRead(ldrPin);

  if (lightValue < 400 && !nightMode) {

    nightMode = true;

    digitalWrite(greenLed, HIGH);

    Serial.println("Night Mode Enabled");

    if (!gateOpen && !fireDisplayActive)
      showTemperature();
  }

  else if (lightValue >= 400 && nightMode) {

    nightMode = false;

        digitalWrite(greenLed, LOW);

    Serial.println("Day Mode Enabled");

    if (!gateOpen && !fireDisplayActive)
      showTemperature();
  }
// ======================
// Flame Sensor
// ======================

int flameValue = analogRead(flamePin);

if (flameValue < 1000 && !fireDetected) {

  fireDetected = true;
  fireDisplayActive = true;

  fireStartTime = millis();

  digitalWrite(redLed, HIGH);

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("!!! FIRE !!!");

  lcd.setCursor(0, 1);
  lcd.print("Flame Found");

  Serial.println("************************");
  Serial.println("FIRE ALERT!");
  Serial.println("Fire Detected!");
  Serial.println("************************");
}

// השארת ההתראה למשך דקה

if (fireDisplayActive &&
    millis() - fireStartTime >= FIRE_DISPLAY_TIME &&
    flameValue >= 1000) {

  fireDisplayActive = false;
  fireDetected = false;

  digitalWrite(redLed, LOW);

  Serial.println("Fire Alert Ended");

  if (!gateOpen)
    showTemperature();
}

  // ======================
  // הבהוב צהוב כשהשער פתוח
  // ======================

  if (gateOpen) {

    if (millis() - previousBlink >= blinkInterval) {

      previousBlink = millis();

      yellowState = !yellowState;

      digitalWrite(yellowLed, yellowState);
    }
  }
  else {

    digitalWrite(yellowLed, LOW);
  }

  // ======================
  // Bluetooth
  // ======================

  if (Serial.available()) {

    String msg = Serial.readString();
    msg.trim();

    if (msg == "O") {

      manualOpen = true;
      manualClose = false;

      openGate();

      lcd.setCursor(0, 1);
      lcd.print("By Phone      ");

      Serial.println("Opened By Phone");
    }

    else if (msg == "C") {

      manualOpen = false;
      manualClose = true;

      closeGate();

      Serial.println("Closed By Phone");
    }

    else if (msg == "A") {

      manualOpen = false;
      manualClose = false;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Auto Mode");

      lcd.setCursor(0, 1);
      lcd.print("Enabled");

      Serial.println("Auto Mode Enabled");

      delay(1500);

      if (!gateOpen)
        showTemperature();
    }
  }

  // ======================
  // מצב אוטומטי
  // ======================

  if (!manualOpen && !manualClose) {

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);

    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH, 30000);

    if (duration > 0) {

      float distance = duration * 0.0343 / 2;

      if (distance <= 10) {

        lastDetectionTime = millis();

        if (!gateOpen) {

          openGate();

          lcd.setCursor(0, 1);
          lcd.print("By Sensor     ");

          Serial.println("Opened By Sensor");
        }
      }

      if (gateOpen &&
          (millis() - lastDetectionTime > 2000)) {

        closeGate();

        Serial.println("Closed Automatically");
      }
    }
  }
}