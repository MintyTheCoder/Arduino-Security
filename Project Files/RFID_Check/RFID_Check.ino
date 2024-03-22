#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

String content = "";

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);  // Initiate a serial communication
  while (!Serial);
  Serial.println("Initializing RFID...");
  SPI.begin();         // Initiate SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  Serial.println("RFID Initialized \n");
}

void loop() {
  // put your main code here, to run repeatedly:
  rfidInput();

}

void rfidInput() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) 
  {
    //Serial.println("RFID No Card");
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) 
  {
    //Serial.println("RFID Checked");
    return;
  }

  // Show UID on serial monitor
  //Serial.println("UID tag :");
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  //Serial.println();
  //Serial.print("Message : ");
  content.toUpperCase();


  Serial.print(content + "\n");
  content = "";

  // if (content.substring(1) == "EA E3 78 82" || content.substring(1) == "1A 19 AB 81") 
  // {
  //   //lcd.clear();
  //   //lcd.print("Access Approved");
  //   Serial.println("RFID Access Approved");
  //   //correctInput();
  //   Serial.println("RFID Cleared");
  //   clearData();
  // } 
  // else 
  // {
  //   //lcd.clear();
  //   //lcd.print("Access Denied");
  //   Serial.println("RFID Access Denied");
  //   //incorrectInput();
  //   clearData();
  // }
}

void clearData() 
{
  content = "";
}