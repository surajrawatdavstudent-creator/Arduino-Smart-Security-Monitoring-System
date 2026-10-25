#include <LiquidCrystal.h>
#include <Keypad.h>
#include <DHT.h>

// ================= PIN MAP =================

#define RED_LED 2
#define GREEN_LED 3
#define DHTPIN 4
#define BUZZER 5
#define ECHO_PIN 6
#define TRIG_PIN 7

// LCD: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ================= KEYPAD =================

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 0, 1};

Keypad keypad = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  ROWS,
  COLS
);

// ================= SECURITY =================

const String correctPIN = "111";
String enteredPIN = "";
bool accessGranted = false;

// ================= SENSOR VALUES =================

float temperature = 0;
float humidity = 0;
float distance = -1;

unsigned long lastDHT = 0;
unsigned long lastDistance = 0;
unsigned long lastDisplay = 0;
byte displayPage = 0;

// ================= SETUP =================

void setup() {
  lcd.begin(16, 2);
  dht.begin();

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  noTone(BUZZER);

  Serial.begin(9600);
  delay(500);
  Serial.println("SYSTEM_STARTED");

  showPINScreen();
}

// ================= MAIN LOOP =================

void loop() {
  if (!accessGranted) {
    handleLockedMode();
    return;
  }

  handleMonitoringMode();
}

// ================= LOCKED MODE =================

void handleLockedMode() {
  char key = keypad.getKey();

  if (!key) {
    return;
  }

  if (key == '*') {
    enteredPIN = "";
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    showPINScreen();
    return;
  }

  // Physical key 4 is used as Enter / Submit.
  if (key == '4') {
    checkPIN();
    return;
  }

  if (key >= '0' && key <= '9') {
    if (enteredPIN.length() < 8) {
      enteredPIN += key;
      showEnteredPIN();
    }
  }
}

// ================= MONITORING MODE =================

void handleMonitoringMode() {
  char key = keypad.getKey();

  // * locks the system again.
  if (key == '*') {
    accessGranted = false;
    enteredPIN = "";
    digitalWrite(GREEN_LED, LOW);
    showPINScreen();
    return;
  }

  if (millis() - lastDHT >= 2000) {
    lastDHT = millis();

    float newHumidity = dht.readHumidity();
    float newTemperature = dht.readTemperature();

    if (!isnan(newHumidity) && !isnan(newTemperature)) {
      humidity = newHumidity;
      temperature = newTemperature;

      Serial.print("TEMPERATURE:");
      Serial.println(temperature, 1);

      Serial.print("HUMIDITY:");
      Serial.println(humidity, 1);
    }
  }

  if (millis() - lastDistance >= 1000) {
    lastDistance = millis();
    distance = getDistance();

    if (distance >= 0) {
      Serial.print("DISTANCE:");
      Serial.println(distance, 1);
    } else {
      Serial.println("DISTANCE:ERROR");
    }
  }

  if (millis() - lastDisplay >= 2000) {
    lastDisplay = millis();
    displayPage++;

    if (displayPage > 2) {
      displayPage = 0;
    }

    showSensorScreen();
  }
}

// ================= PIN SCREEN =================

void showPINScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SMART SECURITY");
  lcd.setCursor(0, 1);
  lcd.print("PIN:");
}

void showEnteredPIN() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ENTER PIN:");
  lcd.setCursor(0, 1);
  lcd.print(enteredPIN);
}

// ================= CHECK PIN =================

void checkPIN() {
  lcd.clear();

  if (enteredPIN == correctPIN) {
    accessGranted = true;

    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);

    lcd.setCursor(0, 0);
    lcd.print("ACCESS GRANTED");
    lcd.setCursor(0, 1);
    lcd.print("WELCOME");

    tone(BUZZER, 1000);
    delay(2000);
    noTone(BUZZER);

    Serial.println("ACCESS_GRANTED");

    enteredPIN = "";
    displayPage = 0;
    lastDisplay = millis();
    showSensorScreen();
  } else {
    accessGranted = false;

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);

    lcd.setCursor(0, 0);
    lcd.print("ACCESS DENIED");
    lcd.setCursor(0, 1);
    lcd.print("WRONG PIN");

    tone(BUZZER, 1000);
    delay(5000);
    noTone(BUZZER);

    digitalWrite(RED_LED, LOW);
    enteredPIN = "";

    Serial.println("ACCESS_DENIED");

    showPINScreen();
  }
}

// ================= ULTRASONIC =================

float getDistance() {
  float total = 0;
  byte validReadings = 0;

  for (byte i = 0; i < 5; i++) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);

    if (duration > 0) {
      float d = duration * 0.0343 / 2.0;

      if (d >= 2 && d <= 400) {
        total += d;
        validReadings++;
      }
    }

    delay(10);
  }

  if (validReadings > 0) {
    return total / validReadings;
  }

  return -1;
}

// ================= LCD SENSOR DISPLAY =================

void showSensorScreen() {
  lcd.clear();

  if (displayPage == 0) {
    lcd.setCursor(0, 0);
    lcd.print("TEMPERATURE");

    lcd.setCursor(0, 1);
    lcd.print(temperature, 1);
    lcd.print((char)223);
    lcd.print("C");
  }
  else if (displayPage == 1) {
    lcd.setCursor(0, 0);
    lcd.print("HUMIDITY");

    lcd.setCursor(0, 1);
    lcd.print(humidity, 1);
    lcd.print("%");
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print("DISTANCE");

    lcd.setCursor(0, 1);

    if (distance < 0) {
      lcd.print("NO READING");
    } else {
      lcd.print(distance, 1);
      lcd.print(" cm");
    }
  }
}
