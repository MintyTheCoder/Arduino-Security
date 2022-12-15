#include <Keypad.h>
#include <MFRC522.h>
//#include <DS3231.h>
#include <pitches.h>
#include <LiquidCrystal.h>

#define Password_Length 7
#define SS_PIN 47
#define RST_PIN 49
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

char userInput[Password_Length];
char codeBlock2[Password_Length] = "B2#205";
char codeBlock3[Password_Length] = "B3#205";
char emergencyCode[Password_Length] = "0*0*0*";
int screenPosition = 0;
char customKey;

int note[] = { NOTE_A5, NOTE_E4 };
int duration = 500;  // 500 miliseconds

//DS3231 clock;
//RTCDateTime dt;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
byte redPin = 53;
byte greenPin = 48;
byte lockPin = 42;
byte buzzerPin = 44;

const byte ROWS = 4;
const byte COLS = 4;

String content = "";
byte letter;

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 22, 25, 26, 29 };
byte colPins[COLS] = { 30, 33, 34, 37 };

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);  // Initiate a serial communication
  analogWrite(2, 60);
  //clock.begin();
  //clock.setDateTime(__DATE__, __TIME__);
  SPI.begin();         // Initiate  SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(lockPin, OUTPUT);
  digitalWrite(redPin, LOW);
  digitalWrite(lockPin, HIGH);
}

void loop() {
  //dt = clock.getDateTime();
  rfidInput();
  getInput();
  rfidInput();

  if (customKey) {
    userInput[screenPosition] = customKey;
    lcd.setCursor(screenPosition, 1);
    lcd.print(userInput[screenPosition]);
    screenPosition++;
  }

  while (screenPosition == Password_Length - 1) {

    lcd.clear();
    rfidInput();
    checkKeyIn();

    lcd.clear();
    //clearData();
  }
}

void clearData() {
  while (screenPosition != 0) {
    userInput[screenPosition--] = 0;
  }
  return;
}

void rfidInput() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  if (content.substring(1) == "EA E3 78 82") {
    clearData();
    lcd.clear();
    lcd.print("Access Approved");
    Serial.println();
    correctInput();
    //delay(3000);
    //lcd.clear();
    //clearData();
  }

  else if (content.substring(1) == "1A 19 AB 81") {
    clearData();
    lcd.clear();
    lcd.print("Access Approved");
    Serial.println();
    correctInput();
    //delay(3000);
    //lcd.clear();
    //clearData();
  }

  else {
    clearData();
    lcd.clear();
    lcd.print("Access denied");
    delay(500);
    incorrectInput();
    //lcd.clear();
    //clearData();
  }
}

void correctInput() {
  tone(44, note[0], duration);
  digitalWrite(lockPin, LOW);
  digitalWrite(greenPin, HIGH);
  delay(5000);
  digitalWrite(greenPin, LOW);
  digitalWrite(lockPin, HIGH);
  //return;
  lcd.clear();
  clearData();
  //getInput();
}

void incorrectInput() {
  tone(44, note[1], duration);
  digitalWrite(redPin, HIGH);
  delay(5000);
  digitalWrite(redPin, LOW);
  //return;
  lcd.clear();
  clearData();
  //getInput();
}

void getInput() {
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  customKey = customKeypad.getKey();
  rfidInput();
}

void checkKeyIn() {
  if (!strcmp(userInput, emergencyCode)) {

    lcd.print("Correct");
    correctInput();
  }

  else {
    lcd.print("Incorrect");
    incorrectInput();
  }
}

/*void checkKeyIn2() {
  if (!strcmp(userInput, emergencyCode)) {
    lcd.print("Correct");
    correctInput();
  }

  else {
    lcd.print("Incorrect");
    incorrectInput();
  }
}*/