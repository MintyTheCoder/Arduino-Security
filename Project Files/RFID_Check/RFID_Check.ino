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
  Serial.print("Initializing RFID...");
  SPI.begin();         // Initiate SPI bus
  mfrc522.PCD_Init();  // Initiate MFRC522
  Serial.print("RFID Initialized");
}

void loop() 
{
  // put your main code here, to run repeatedly:
  rfidInput();
}

void rfidInput() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }

  // Show UID on serial monitor
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();

  if (content != "")
  {
    Serial.print(content);
  }

  
  content = "";
  delay(2500);
}