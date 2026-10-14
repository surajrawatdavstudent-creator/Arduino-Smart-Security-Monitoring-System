#include <LiquidCrystal.h>
#include <Keypad.h>
#include <DHT.h>

// ================= LCD =================

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);


// ================= DHT11 =================

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);


// ================= ULTRASONIC =================

#define TRIG_PIN 3
#define ECHO_PIN 4


// ================= BUZZER + LEDS =================

#define BUZZER 5
#define GREEN_LED 6
#define RED_LED 7


// ================= KEYPAD =================

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {8, 9, 10, 11};
byte colPins[COLS] = {12, 13, 0, 1};

Keypad keypad = Keypad(
  makeKeymap(keys),
  rowPins,
  colPins,
  ROWS,
  COLS
);


// ================= SECURITY =================

String correctPIN = "333";
String enteredPIN = "";

bool accessGranted = false;


// ================= SENSOR VALUES =================

float temperature = 0;
float humidity = 0;
float distance = 0;


// ================= TIMERS =================

unsigned long lastDHT = 0;
unsigned long lastDisplay = 0;

byte displayPage = 0;


// =================================================
// SETUP
// =================================================

void setup() {

  lcd.begin(16, 2);

  dht.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUZZER, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  noTone(BUZZER);

  showPINScreen();
}


// =================================================
// MAIN LOOP
// =================================================

void loop() {

  // =================================================
  // LOCKED MODE
  // =================================================

  if (!accessGranted) {

    char key = keypad.getKey();

    if (key) {

      // ================= CLEAR =================

      if (key == '*') {

        enteredPIN = "";

        digitalWrite(RED_LED, LOW);
        noTone(BUZZER);

        showPINScreen();
      }


      // ================= ENTER =================
      // 2 is being used as ENTER

      else if (key == '2') {

        checkPIN();
      }


      // ================= NUMBER =================

      else if (key >= '0' && key <= '9') {

        if (enteredPIN.length() < 8) {

          enteredPIN += key;

          showEnteredPIN();
        }
      }
    }

    return;
  }


  // =================================================
  // ACCESS GRANTED
  // =================================================

  char key = keypad.getKey();

  // * = LOCK AGAIN

  if (key == '*') {

    accessGranted = false;

    enteredPIN = "";

    digitalWrite(GREEN_LED, LOW);

    showPINScreen();

    return;
  }


  // =================================================
  // READ DHT11
  // =================================================

  if (millis() - lastDHT >= 2000) {

    lastDHT = millis();

    float newHumidity = dht.readHumidity();
    float newTemperature = dht.readTemperature();

    if (!isnan(newHumidity) && !isnan(newTemperature)) {

      humidity = newHumidity;
      temperature = newTemperature;
    }
  }


  // =================================================
  // READ ULTRASONIC
  // =================================================

  distance = getDistance();


  // =================================================
  // CHANGE SENSOR SCREEN
  // =================================================

  if (millis() - lastDisplay >= 2000) {

    lastDisplay = millis();

    displayPage++;

    if (displayPage > 2) {
      displayPage = 0;
    }

    showSensorScreen();
  }
}


// =================================================
// SHOW PIN SCREEN
// =================================================

void showPINScreen() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SMART SECURITY");

  lcd.setCursor(0, 1);
  lcd.print("PIN:");
}


// =================================================
// SHOW ENTERED PIN
// =================================================

void showEnteredPIN() {

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("ENTER PIN:");

  lcd.setCursor(0, 1);

  // Show the actual PIN
  lcd.print(enteredPIN);
}


// =================================================
// CHECK PIN
// =================================================

void checkPIN() {

  lcd.clear();


  // =================================================
  // CORRECT PIN
  // =================================================

  if (enteredPIN == correctPIN) {

    accessGranted = true;

    // GREEN ON
    digitalWrite(GREEN_LED, HIGH);

    // RED OFF
    digitalWrite(RED_LED, LOW);

    lcd.setCursor(0, 0);
    lcd.print("ACCESS GRANTED");

    lcd.setCursor(0, 1);
    lcd.print("WELCOME");

    // ONE SHORT BEEP
    tone(BUZZER, 1000);
    delay(250);
    noTone(BUZZER);

    delay(1500);

    // Start sensor display
    displayPage = 0;
    lastDisplay = millis();

    showSensorScreen();
  }


  // =================================================
  // WRONG PIN
  // =================================================

  else {

    accessGranted = false;

    // RED ON
    digitalWrite(RED_LED, HIGH);

    // GREEN OFF
    digitalWrite(GREEN_LED, LOW);

    lcd.setCursor(0, 0);
    lcd.print("ACCESS DENIED");

    lcd.setCursor(0, 1);
    lcd.print("WRONG PIN");

    // BUZZER CONTINUOUS FOR 3 SECONDS
    tone(BUZZER, 1000);

    delay(3000);

    noTone(BUZZER);

    digitalWrite(RED_LED, LOW);

    enteredPIN = "";

    showPINScreen();
  }
}


// =================================================
// ULTRASONIC DISTANCE
// =================================================

float getDistance() {

  float total = 0;
  byte validReadings = 0;


  // Take 5 readings
  for (byte i = 0; i < 5; i++) {

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG_PIN, LOW);

    unsigned long duration =
      pulseIn(ECHO_PIN, HIGH, 30000);


    if (duration > 0) {

      float d = duration * 0.0343 / 2.0;

      // Valid HC-SR04 range
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


// =================================================
// SENSOR DISPLAY
// =================================================

void showSensorScreen() {

  lcd.clear();


  // ================= PAGE 0 =================
  // TEMPERATURE

  if (displayPage == 0) {

    lcd.setCursor(0, 0);
    lcd.print("TEMPERATURE");

    lcd.setCursor(0, 1);

    lcd.print(temperature, 1);
    lcd.print((char)223);
    lcd.print("C");
  }


  // ================= PAGE 1 =================
  // HUMIDITY

  else if (displayPage == 1) {

    lcd.setCursor(0, 0);
    lcd.print("HUMIDITY");

    lcd.setCursor(0, 1);

    lcd.print(humidity, 1);
    lcd.print("%");
  }


  // ================= PAGE 2 =================
  // DISTANCE

  else {

    lcd.setCursor(0, 0);
    lcd.print("DISTANCE");

    lcd.setCursor(0, 1);

    if (distance < 0) {

      lcd.print("NO READING");
    }

    else {

      lcd.print(distance, 1);
      lcd.print(" cm");
    }
  }
}
