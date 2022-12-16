// Include required libraries
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SPI.h>
// Create instances
LiquidCrystal lcd(7,8,9,10,11,12);
MFRC522 mfrc522(47, 49); // MFRC522 mfrc522(SS_PIN, RST_PIN)
// Initialize Pins for led's, servo and buzzer
// Blue LED is connected to 5V
constexpr uint8_t greenLed = 48;
constexpr uint8_t redLed = 53;
constexpr uint8_t lockPin = 42;
constexpr uint8_t buzzerPin = 44;
char initial_password[4] = {'1', '2', '3', '4'};  // Variable to store initial password
String tagUID = "EA E3 78 82";  // String to store UID of tag. Change it with your tag's UID
String tagUID2 = "1A 19 AB 81";
char password[4];   // Variable to store users password
boolean RFIDMode = true; // boolean to change modes
char key_pressed = 0; // Variable to store incoming keys
uint8_t i = 0;  // Variable used for counter
// defining how many rows and columns our keypad have
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
  // Arduino Pin configuration
  analogWrite(2, 60);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  lcd.begin(16,2);   // LCD screen
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  lcd.clear(); // Clear LCD screen
  Serial.begin(9600);
}
void loop() {
  // System will first look for mode
  if (RFIDMode == true) {
    lcd.setCursor(0, 0);
    lcd.print("Door Lock");
    lcd.setCursor(0, 1);
    lcd.print("Scan Your Tag");
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
      Serial.print("testo");
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
      
    }
    //Reading from the card
    String tag = "";
    for (byte j = 0; j < mfrc522.uid.size; j++)
    {
      tag.concat(String(mfrc522.uid.uidByte[j] < 0x10 ? " 0" : " "));
      tag.concat(String(mfrc522.uid.uidByte[j], HEX));
    }
    tag.toUpperCase();
    //Checking the card
    if (tag.substring(1) == tagUID || tag.substring(1) == tagUID2)
    {
      // If UID of tag is matched.
      lcd.clear();
      lcd.print("Tag Matched");
      digitalWrite(greenLed, HIGH);
      delay(3000);
      digitalWrite(greenLed, LOW);
      lcd.clear();
      //lcd.print("Enter Password:");
      //lcd.setCursor(0, 1);
      RFIDMode = false; // Make RFID mode false
    }
    else
    {
      // If UID of tag is not matched.
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wrong Tag Shown");
      lcd.setCursor(0, 1);
      lcd.print("Access Denied");
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(redLed, HIGH);
      delay(3000);
      digitalWrite(buzzerPin, LOW);
      digitalWrite(redLed, LOW);
      lcd.clear();
    }
  }
}