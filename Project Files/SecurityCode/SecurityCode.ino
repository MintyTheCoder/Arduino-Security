#include <Keypad.h>
#include <pitches.h>
#include <LiquidCrystal.h>
//#include <EEPROM.h>

/* Pin layout used:
 * ---------------------------------
 *             MFRC522      Arduino
 *             Reader/PCD   Mega
 * Signal      Pin          Pin
 * ---------------------------------
 * RST/Reset   RST          49
 * SPI SS      SDA(SS)      47
 * SPI MOSI    MOSI         51
 * SPI MISO    MISO         50
 * SPI SCK     SCK          52
 */

#define Password_Length 7

//Pin declaration and intialization
#define redPin 23
#define greenPin 48
#define lockPin 42
#define buzzerPin 44
#define buttonPin 39


//for fun
#define BI_LED_RED 86
#define BI_LED_GREEN 87
#define BI_LED_BLUE 88

String content = "";

char userInput[Password_Length];
int screenPosition = 0;


LiquidCrystal lcd(7, 8, 9, 10, 11, 12);


const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 22, 25, 26, 29 };
byte colPins[COLS] = { 30, 33, 34, 37 };

// creates keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() 
{
  setupSerial();
  while (!Serial1);
  Serial.println("System starting... \n");
  setupPins();
  setupLCD();
  Serial.println("System setup completed. \n");
}

void setupSerial()
{
  Serial.begin(9600);  // Initiate a serial communication via USB
  Serial1.begin(9600); // Initiate a serial communication between boards
  Serial1.setTimeout(10);
  if(!Serial)
  {
    delay(1500);
  }
}

void setupLCD() 
{
  Serial.println("Initializing LCD...");
  // set brightness of LCD
  analogWrite(2, 60);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  Serial.println("LCD Initialized \n");
}

void setupPins() 
{
  Serial.println("Initializing Pins...");
  pinMode(greenPin, OUTPUT);
  digitalWrite(greenPin, LOW);
  pinMode(redPin, OUTPUT);
  digitalWrite(redPin, LOW);
  pinMode(lockPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(lockPin, HIGH);
  //digitalWrite(BI_LED_GREEN, LOW);
  //digitalWrite(BI_LED_BLUE, LOW);

  Serial.println("Pins Initialized \n");
}

void loop() 
{
  inputRetrieval();
  buttonCheck();
}

void resetLCD() 
{
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  Serial.println("LCD Reset");
}

void rfidInput() 
{
  String rfidInput = Serial1.readString();

  if (rfidInput != "")
  {
    if (rfidInput.substring(1) == "EA E3 78 82" || rfidInput.substring(1) == "1A 19 AB 81") 
    {
      lcd.clear();
      lcd.print("Access Approved");
      Serial.println("RFID Access Approved");
      correctInput();
      Serial.println("RFID Cleared\n");
      clearData();
    } 
    
    else if(checkForRFIDRestart(rfidInput.substring(0)))
    {
      return;
    }
    
    else 
    {
      lcd.clear();
      lcd.print("Access Denied");
      //Serial.println("RFID Access Denied: \n" + rfidInput.substring(1));
      incorrectInput();
      clearData();
    }
  }
}

void clearData() 
{
  while (screenPosition != 0) 
  {
    userInput[screenPosition--] = NULL;
  }
  content = "";
}

void correctInput() 
{
  playBuzzer(buzzerPin, 0);
  digitalWrite(lockPin, LOW);
  digitalWrite(greenPin, HIGH);
  Serial.println("Lock Unlocked - Correct Input");
  delay(5000);
  digitalWrite(greenPin, LOW);
  digitalWrite(lockPin, HIGH);
  resetLCD();
}

void incorrectInput() 
{
  playBuzzer(buzzerPin, 1);
  digitalWrite(redPin, HIGH);
  Serial.println("Access Denied - Incorrect Input");
  delay(5000);
  digitalWrite(redPin, LOW);
  resetLCD();
}

void playBuzzer(byte pin, byte element) 
{
  int note[] = { NOTE_A5, NOTE_E4 };
  tone(pin, note[element], 500);
}

void inputRetrieval() 
{
  rfidInput();
  char customKey = customKeypad.getKey();

  if (customKey) 
  {
    userInput[screenPosition] = customKey;
    lcd.setCursor(screenPosition, 1);
    lcd.print(userInput[screenPosition]);
    screenPosition++;
    Serial.print("Keypad Input: ");
    Serial.println(customKey);
  }

  while (screenPosition == Password_Length - 1) 
  {
    lcd.clear();
    checkKeypadInput();
    rfidInput();
    clearData();
  }
}

void checkKeypadInput() 
{
  char codeBlock1[Password_Length] = "B1#205";
  char emergencyCode[Password_Length] = "0*0*0*";
  Serial.print("Password Inputted By User: ");
  Serial.println(userInput);

  if (!strcmp(userInput, emergencyCode) || !strcmp(userInput, codeBlock1)) 
  {
    lcd.print("Correct");
    correctInput();
  } 
  else 
  {
    lcd.print("Incorrect");
    incorrectInput();
  }
}

void buttonCheck() 
{
  // Check if the button is pressed (active LOW)
  if (digitalRead(buttonPin) == LOW) 
  {
    // If button is pressed, unlock the lock
    digitalWrite(lockPin, LOW);
    Serial.println("Button Pressed - Lock Unlocked");
    delay(2500); // Delay to keep the lock unlocked for 2.5 seconds
  }
  digitalWrite(lockPin, HIGH); // Lock the lock after delay
}

bool checkForRFIDRestart(String varToCheck)
{
  if (varToCheck == "Initializing RFID...")
  {
    Serial.println(varToCheck);
    return true;
  }

  else if (varToCheck == "RFID Initialized")
  {
    Serial.println(varToCheck);
    Serial.println("RFID Board or Microcontroller restarted \n");
    return true;
  }

  return false;
}
