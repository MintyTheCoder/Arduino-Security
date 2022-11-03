#include <LiquidCrystal.h>
#include <Keypad.h>
//#include <SPI.h>
#include <MFRC522.h>
#include <DS3231.h>

#define Password_Length 7 
#define SS_PIN 47
#define RST_PIN 49
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

char userInput[Password_Length]; 
char codeBlock2[Password_Length] = "B2#205"; 
char codeBlock3[Password_Length] = "B3#205";
char toggleCode[Password_Length];
byte screenPosition = 0;
char customKey;

DS3231 clock;
RTCDateTime dt;

byte redPin = 52;
byte greenPin = 48;
byte lockPin = 42;

const byte ROWS = 4;
const byte COLS = 4;

String content= "";
byte letter;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {22, 25, 26, 29};
byte colPins[COLS] = {30, 33, 34, 37};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);  

void setup()
{
  Serial.begin(9600);   // Initiate a serial communication
  clock.begin();
  clock.setDateTime(__DATE__, __TIME__); 
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  lcd.begin(16,2);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(lockPin, OUTPUT);
  digitalWrite(lockPin, HIGH);
}

void loop()
{
  dt = clock.getDateTime();
  
  rfidInput();
  lcd.setCursor(0,0);
  lcd.print("Enter Password:");
    
  customKey = customKeypad.getKey();
  if (customKey)
  {
    userInput[screenPosition] = customKey; 
    lcd.setCursor(screenPosition,1); 
    lcd.print(userInput[screenPosition]); 
    screenPosition++; 
  }
  if(screenPosition == Password_Length-1)
  {
    lcd.clear();
    if (dt.hour >= 10 && dt.hour <= 12)
    {
      if (dt.hour == 10)
      {
        if(dt.minute >=41)
        {
          checkKeyIn();  
        }        
      }

      else if (dt.hour == 11)
      {
        if(dt.minute >= 0)
        {
          checkKeyIn();      
        }  
      }
    }
    
    while (dt.hour >= 12 && dt.hour <= 14)
    {
    }

    lcd.clear();
    clearData();  
  }
}

void clearData()
{
  while(screenPosition !=0)
  {
    userInput[screenPosition--] = 0; 
  }
  return;
}
void rfidInput()
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
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
  if (content.substring(1) == "A7 BE CB B5" || content.substring(1) == "27 C4 C9 B4") //change here the UID of the card/cards that you want to give access
  {
    clearData();
    lcd.clear();
    lcd.print("Enter New Code:");
    delay(3000);
    lcd.setCursor(screenPosition,1);    
    lcd.print(userInput[screenPosition]); 
    screenPosition++; 
    Serial.println(userInput[screenPosition]);
    //Serial.println(customKey);
    delay(3000);
    lcd.clear();
  }
  
  else if (content.substring(1) == "EA E3 78 82" || content.substring(1) == "1A 19 AB 81")
  {
    clearData();
    lcd.clear();
    lcd.print("Access Approved");
    Serial.println();
    correctInput();
    delay(3000);
    lcd.clear();
  }
 
  else   {
    clearData();
    lcd.clear();
    lcd.print("Access denied");
    delay(3000);
    incorrectInput();
    lcd.clear();
  }
}
void correctInput()
{
  digitalWrite(lockPin, LOW);
  digitalWrite(greenPin, HIGH); 
  delay(5000);
  digitalWrite(greenPin, LOW);
  digitalWrite(lockPin, HIGH);
  //return;
  lcd.clear();
  clearData();
}

void incorrectInput()
{
  lcd.print("Incorrect");
  digitalWrite(redPin, HIGH);
  delay(5000);
  digitalWrite(redPin, LOW);
  //return;
  lcd.clear();
  clearData();
}

void checkKeyIn()
{
  if(!strcmp(userInput, codeBlock2))
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
