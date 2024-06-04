#include <pitches.h>
#include <LiquidCrystal.h>
#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include <Arduino_GigaDisplay.h>

#include "lvgl.h"
#include "ui.h"

#define USBSerial Serial
#define RFIDSerial Serial1
#define CameraSerial Serial2

#define Password_Length 7

//Pin declaration and intialization
#define lockPin 42
#define buzzerPin 44
#define buttonPin 39

String content = "";

extern char input[6];
extern bool passInput;
extern bool canRFID;
extern int currentPos;

bool screenOn = true;

Arduino_H7_Video Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch Touch;
GigaDisplayBacklight backlight;
GigaDisplayRGB rgb;  //create rgb object

void setup() 
{
  setupSerial();
  while (!Serial1)
    ;
  USBSerial.println("System starting... \n");
  setupPins();
  setupScreen();
  USBSerial.println("System setup completed. \n");
}

void setupSerial() 
{
  USBSerial.begin(115200);   // Initiate a serial communication via USB
  RFIDSerial.begin(9600);  // Initiate a serial communication between RFID and GIGA (RX0/TX0)
  RFIDSerial.setTimeout(10);
  CameraSerial.begin(115200);  // Initiate a serial communication between camera and GIGA (RX1/TX1)
  if (!Serial) 
  {
    delay(1500);
  }
}

void setupScreen() 
{
  Display.begin();
  Touch.begin();
  backlight.begin();
  ui_init();
  resetScreen();
  lv_timer_handler();
  backlight.off();
  screenOn = false;
}
void setupPins() 
{
  USBSerial.println("Initializing Pins...");
  pinMode(lockPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  digitalWrite(lockPin, HIGH);
  rgb.begin();
  rgb.off();
  USBSerial.println("Pins Initialized \n");
}

void loop() 
{
  loopScreen();
  camCheck();
  serialOverride();
  inputRetrieval();
  buttonCheck();
  loopScreen();
}

void loopScreen()
{
  if(screenOn)
  {
    lv_timer_handler();
  }
  return;
}

void rfidInput() 
{
    String rfidInput = RFIDSerial.readString();
    if (canRFID)
    {
      if (rfidInput != "") 
      {
        if (rfidInput.substring(1) == "EA E3 78 82" || rfidInput.substring(1) == "1A 19 AB 81") 
        {
          USBSerial.println("RFID Access Approved");
          correctInput();
          USBSerial.println("RFID Cleared\n");
          clearData();
        }

        else if (checkForRFIDRestart(rfidInput.substring(0))) 
        {
          return;
        }

        else 
        {
          incorrectInput();
          clearData();
        }
      }
    }
  
}

void clearData() 
{
  content = "";
}

void correctInput() 
{
  ui_edit_color(lv_color_hex(0x0acc0a));
  ui_edit_label("Correct");
  delay(25);
  loopScreen();
  playBuzzer(buzzerPin, 0);
  digitalWrite(lockPin, LOW);
  rgb.on(0, 255, 0);
  USBSerial.println("Lock Unlocked - Correct Input");
  delay(5000);
  rgb.off();
  digitalWrite(lockPin, HIGH);
  ui_update_InputField();]
}

void incorrectInput() 
{
  ui_edit_color(lv_color_hex(0xFF0000));
  ui_edit_label("Incorrect");
  delay(25);
  loopScreen();
  playBuzzer(buzzerPin, 1);
  rgb.on(255, 0, 0);
  USBSerial.println("Access Denied - Incorrect Input");
  delay(5000);
  rgb.off();
  ui_update_InputField();
}

void playBuzzer(byte pin, byte element) 
{
  int note[] = { NOTE_A5, NOTE_E4 };
  tone(pin, note[element], 500);
}

void inputRetrieval() 
{
  loopScreen();
  rfidInput();
  char code[Password_Length] = "205614";
  //char emergencyCode[Password_Length] = "0*0*0*";
  if (passInput) 
  {
    if (strstr(input, code) != NULL) 
    {
      correctInput();
    } 

    else
    {
      incorrectInput();
    }

    memset(input, 0, sizeof(input));
    currentPos = 0;
    ui_update_InputField();
    passInput = false;
  }
}

void buttonCheck() {
  // Check if the button is pressed (active LOW)
  if (digitalRead(buttonPin) == LOW) {
    // If button is pressed, unlock the lock
    digitalWrite(lockPin, LOW);
    USBSerial.println("Button Pressed - Lock Unlocked");
    delay(2500);  // Delay to keep the lock unlocked for 2.5 seconds
  }
  digitalWrite(lockPin, HIGH);  // Lock the lock after delay
}

void camCheck()
{
  if(CameraSerial.available() > 0 )
  {
    String camInput = CameraSerial.readStringUntil('\n');
    USBSerial.println(camInput);
    if (camInput.indexOf("Detected") != -1) 
    {
      loopScreen();
      backlight.set(100);
      Touch.begin();
      screenOn = true;
      loopScreen();
    }

    else if (camInput.indexOf("cam_hal: EV-VSYNC-OVF") != -1)
    {
      camCheck();
    }

    else
    {
      resetScreen();
      delay(1000);
      loopScreen();
      Touch.end();
      backlight.off();
      screenOn = false;
    }
  }   
}


void serialOverride()
{
  if (USBSerial.available() > 0)
  {
    String serialInput = USBSerial.readStringUntil('\n');
    USBSerial.println(serialInput);
    if (serialInput.indexOf("RFID") != -1)
    {
      canRFID = true;
    }

    if (serialInput.indexOf("Not RFID") != -1)
    {
      canRFID = false;
    }

    else if (serialInput.indexOf("Detected") != -1)
    {
      loopScreen();
      backlight.set(100);
      Touch.begin();
      screenOn = true;
      loopScreen();
    }
  }
}
bool checkForRFIDRestart(String varToCheck) {
  if (varToCheck == "Initializing RFID...") {
    USBSerial.println(varToCheck);
    return true;
  }

  else if (varToCheck == "RFID Initialized") {
    USBSerial.println(varToCheck);
    USBSerial.println("RFID Board or Microcontroller restarted \n");
    return true;
  }

  return false;
}