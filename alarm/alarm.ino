#include <LiquidCrystal.h> // includes the LiquidCrystal Library 
#include <Keypad.h>

#define buzzer 8
#define trigPin 9
#define echoPin 10

// Global variables declaration
long duration;
int distance, initialDistance, currentDistance, i;
int screenOffMsg = 0;
int Contrast = 50;
String password = "1234";
String tempPassword;
boolean activated = false; // State of the alarm
boolean isActivated;
boolean activateAlarm = false;
boolean alarmActivated = false;
boolean enteredPassword; // State of the entered password to stop the alarm
boolean passChangeMode = false;
boolean passChanged = false;

// Keypad setup
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keypressed;
// Define the symbols on the buttons of the keypads
char keyMap[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3}; // Rows
byte colPins[COLS] = {A4, A5, 12, 11}; // Columns

Keypad myKeypad = Keypad( makeKeymap(keyMap), rowPins, colPins, ROWS, COLS);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

void setup() {
  analogWrite(13, Contrast); // Set contrast for LCD
  lcd.begin(16, 2); // Initialize LCD
  pinMode(buzzer, OUTPUT); // Set buzzer as an output
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Start serial communication
}

void loop() {
  keypressed = myKeypad.getKey(); // Update keypressed value
  
  // Activate the alarm if requested
  if (activateAlarm) {
    // Display countdown before activating the alarm
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarm will be");
    lcd.setCursor(0, 1);
    lcd.print("activated in");

    int countdown = 9; // 9 seconds count down before activating the alarm
    while (countdown != 0) {
      lcd.setCursor(13, 1);
      lcd.print(countdown);
      countdown--;
      tone(buzzer, 700, 100);
      delay(1000);
    }
    // Activate the alarm
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarm Activated!");
    initialDistance = getDistance();
    activateAlarm = false;
    alarmActivated = true;
  }

  // Check for alarm activation and trigger if an object is detected
  if (alarmActivated == true) {
    currentDistance = getDistance() + 10;
    if ( currentDistance < initialDistance) {
      tone(buzzer, 1000); // Send 1KHz sound signal
      lcd.clear();
      enterPassword(); // Ask for password
    }
  }

  // Display main screen if alarm is not activated
  if (!alarmActivated) {
    if (screenOffMsg == 0 ) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("A - Activate");
      lcd.setCursor(0, 1);
      lcd.print("B - Change Pass");
      screenOffMsg = 1;
      Serial.println(keypressed);
    }

    // Check for keypad inputs
    if (keypressed == 'A') {      //If A is pressed, activate the alarm
      tone(buzzer, 1000, 200);
      activateAlarm = true;
    }
    else if (keypressed == 'B') {
      // Change password mode
      lcd.clear();
      int i = 1;
      tone(buzzer, 2000, 100);
      tempPassword = "";
      lcd.setCursor(0, 0);
      lcd.print("Current Password");
      lcd.setCursor(0, 1);
      lcd.print(">");
      passChangeMode = true;
      passChanged = true;
      // Enter new password
      while (passChanged) {
        keypressed = myKeypad.getKey();
        if (keypressed != NO_KEY) {
          if (keypressed >= '0' && keypressed <= '9') {
            tempPassword += keypressed;
            lcd.setCursor(i, 1);
            lcd.print("*");
            i++;
            tone(buzzer, 2000, 100);
          }
        }
        // Reset if more than 5 digits entered or # is pressed
        if (i > 5 || keypressed == '#') {
          tempPassword = "";
          i = 1;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Current Password");
          lcd.setCursor(0, 1);
          lcd.print(">");
        }
        // Store new password if * is pressed
        if ( keypressed == '*') {
          i = 1;
          tone(buzzer, 2000, 100);
          if (password == tempPassword) {
            tempPassword = "";
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Set New Password");
            lcd.setCursor(0, 1);
            lcd.print(">");
            // Enter new password
            while (passChangeMode) {
              keypressed = myKeypad.getKey();
              if (keypressed != NO_KEY) {
                if (keypressed >= '0' && keypressed <= '9') {
                  tempPassword += keypressed;
                  lcd.setCursor(i, 1);
                  lcd.print("*");
                  i++;
                  tone(buzzer, 2000, 100);
                }
              }
              // Reset if more than 5 digits entered or # is pressed
              if (i > 5 || keypressed == '#') {
                tempPassword = "";
                i = 1;
                tone(buzzer, 2000, 100);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Set New Password");
                lcd.setCursor(0, 1);
                lcd.print(">");
              }
              // Store new password if * is pressed
              if ( keypressed == '*') {
                i = 1;
                tone(buzzer, 2000, 100);
                password = tempPassword;
                passChangeMode = false;
                passChanged = false;
                screenOffMsg = 0;
              }
            }
          }
        }
      }
    }
  }
}

// Function to enter the password
void enterPassword() {
  int k = 5;
  tempPassword = "";
  activated = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" *** ALARM *** ");
  lcd.setCursor(0, 1);
  lcd.print("Pass>");
  while (activated) {
    keypressed = myKeypad.getKey();
    if (keypressed != NO_KEY) {
      if (keypressed >= '0' && keypressed <= '9') {
        tempPassword += keypressed;
        lcd.setCursor(k, 1);
        lcd.print("*");
        k++;
      }
    }
    // Reset if more than 5 digits entered or # is pressed
    if (k > 9 || keypressed == '#') {
      tempPassword = "";
      k = 5;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" *** ALARM *** ");
      lcd.setCursor(0, 1);
      lcd.print("Pass>");
    }
    // Deactivate alarm if correct password entered
    if ( keypressed == '*') {
      if ( tempPassword == password ) {
        activated = false;
        alarmActivated = false;
        noTone(buzzer);
        screenOffMsg = 0;
      }
      // Display error message if wrong password entered
      else if (tempPassword != password) {
        lcd.setCursor(0, 1);
        lcd.print("Wrong! Try Again");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" *** ALARM *** ");
        lcd.setCursor(0, 1);
        lcd.print("Pass>");
      }
    }
  }
}

// Custom function for the Ultrasonic sensor to measure distance
long getDistance() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;
  return distance;
}
