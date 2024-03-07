#include <Keypad.h>
#include <MFRC522.h>
#include <pitches.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

/*Pin layout used:
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
#define RST_PIN 49
#define SS_PIN 47

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

char userInput[Password_Length];
char codeBlock2[Password_Length] = "B2#205";
char codeBlock3[Password_Length] = "B3#205";
char emergencyCode[Password_Length] = "0*0*0*";
int screenPosition = 0;
char customKey;

int note[] = { NOTE_A5, NOTE_E4 };
int duration = 500;  // 500 miliseconds

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
const byte redPin = 23;
const byte greenPin = 48;
const byte lockPin = 42;
const byte buzzerPin = 44;
const byte buttonPin = 50;

const byte ROWS = 4;
const byte COLS = 4;

String content = "";
byte letter;

char hexaKeys[ROWS][COLS] = 
{
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 22, 25, 26, 29 };
byte colPins[COLS] = { 30, 33, 34, 37 };

//creates keypad object
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() 
{
  Serial.begin(9600);  // Initiate a serial communication
  analogWrite(2, 60);
  Serial.write(13);
  SPI.begin();         // Initiate  SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(lockPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  //digitalWrite(redPin, LOW);
  digitalWrite(lockPin, HIGH);
}

void loop() 
{
  //rfidInput();
  digitalWrite(lockPin, HIGH);

  delay(50);
  customKey = customKeypad.getKey();
  
  if (customKey) 
  {
    userInput[screenPosition] = customKey;
    lcd.setCursor(screenPosition, 1);
    lcd.print(userInput[screenPosition]);
    screenPosition++;
  }

  while (screenPosition == Password_Length - 1) 
  {
    lcd.clear();
    checkKeyIn();
    clearData();
  }

  while (digitalRead(buttonPin) == LOW)
  {
    digitalWrite(lockPin, LOW);
  }

  delay(15);
}

void getInput() 
{
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  customKey = customKeypad.getKey();
  Serial.println("LCD Reset");
}

void rfidInput() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) 
  {
    Serial.println("RFID No Card");
    return;
  }

  //Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    Serial.println("RFID Loop Done");
    return;
  }

  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  if (content.substring(1) == "EA E3 78 82" || content.substring(1) == "1A 19 AB 81" ) 
  {
    //clearData();
    lcd.clear();
    lcd.print("Access Approved");
    Serial.println("RFID Recieved");
    correctInput();
    Serial.println("RFID Cleared");
    //delay(3000);
    //lcd.clear();
    //clearData();
    //mfrc522.PCD_Reset();
  }

  else 
  {
    //clearData();
    lcd.clear();
    lcd.print("Access Denied");
    //delay(500);
    incorrectInput();
    //lcd.clear();
    //clearData();
  }

  return;
}

void clearData() 
{
  while (screenPosition != 0) 
  {
    userInput[screenPosition--] = NULL;
  }
}

void correctInput() 
{
  tone(buzzerPin, note[0], duration);
  digitalWrite(lockPin, LOW);
  digitalWrite(greenPin, HIGH);
  delay(5000);
  digitalWrite(greenPin, LOW);
  digitalWrite(lockPin, HIGH);
  delay(1000);
  getInput();
}

void incorrectInput() 
{
  tone(buzzerPin, note[1], duration);
  digitalWrite(redPin, HIGH);
  delay(5000);
  digitalWrite(redPin, LOW);
  delay(1000);
  getInput();
}

void checkKeyIn() 
{
  if (!strcmp(userInput, emergencyCode) || !strcmp(userInput, codeBlock2) || !strcmp(userInput, codeBlock3)) 
  {
    lcd.print("Correct");
    correctInput();
  }

  else 
  {
    lcd.print("Incorrect");
    incorrectInput();
  }

  delay(100);
}