/**
  "Potion Placement" Puzzle
  This sketch is setup as an event handler to read specific tags and output different results based on tag read. 
*/

// DEFINES
// Provides debugging information over serial connection if defined
#define DEBUG

// LIBRARIES
#include <SPI.h>
#include <MFRC522.h>

// CONSTANTS
// The number of RFID readers
const byte numReaders = 1;
// Each reader has a unique Slave Select pin
const byte ssPins = 10;
// Reset pin
const byte resetPin = 9;
// Tag info read to trigger output
// ID strings are written to tags using MFRC522 library 
const String green = "1111111111111111";
const String red = "2222222222222222" ;
const String blue = "3333333333333333";

//outputs
int green_trigger = 2;
int red_trigger = 3;
int blue_trigger = 4;
int cipherLED = 5;
int blueLED = 6;
int magLock = 7;

// GLOBALS
// Initialise an array of MFRC522 instances representing each reader
MFRC522 mfrc522;
// The tag IDs currently detected by each reader
String currentIDs;
// specific block to be read from
byte blockAddr      = 4;
byte buffer[18]     ;
byte size          = sizeof(buffer);
MFRC522::StatusCode status;
MFRC522::MIFARE_Key key;

/**
   Initialisation
*/
void setup() {
  pinMode(green_trigger, OUTPUT);
  pinMode(red_trigger, OUTPUT);
  pinMode(blue_trigger, OUTPUT);
  pinMode(magLock, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(cipherLED, OUTPUT);

#ifdef DEBUG
  // Initialise serial communications channel with the PC
  Serial.begin(9600);
  Serial.println(F("Serial communication started"));
#endif

  
  // Initialise the SPI bus
  SPI.begin();

  // Initialise the reader
  // Documentation for pin setup can be found at https://github.com/miguelbalboa/rfid
  mfrc522.PCD_Init(ssPins, resetPin);

  // Set the gain to max 
  //mfrc522.PCD_SetAntennaGain(MFRC522::PCD_RxGain::RxGain_max);

#ifdef DEBUG
  // Dump some debug information to the serial monitor
  Serial.print(F("Reader #"));
  Serial.print(numReaders);
  Serial.print(F(" initialised on pin "));
  Serial.print(String(ssPins));
  Serial.print(F(". Antenna strength: "));
  Serial.print(mfrc522.PCD_GetAntennaGain());
  Serial.print(F(". Version : "));
  mfrc522.PCD_DumpVersionToSerial();
#endif


#ifdef DEBUG
  Serial.println(F("--- END SETUP ---"));
#endif


}

/**
   Main loop
*/
void loop() {


  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++) key.keyByte[i] = 0xFF;


  // Assume that the tags have not changed since last reading
  boolean changedValue = false;

  // String to hold the ID detected by sensor
  String readRFID;

  // Initialise the sensor
  mfrc522.PCD_Init();

  // If the sensor detects a tag and is able to read it
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
    if (status == MFRC522::STATUS_OK) {
      status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
      if (status == MFRC522::STATUS_OK) {
        readRFID = dump_byte_array(buffer, 16);
        Serial.println(readRFID);
      }
    }
  }


  // If the current reading is different from the last known reading
  if (readRFID != currentIDs) {
    changedValue = true;
    // Update the stored value for this sensor
    currentIDs = readRFID;
  }

  //Watch for potion switch
  if (changedValue = true) {

    if (currentIDs == green) {
      //Serial.println("The lights are green and other stuff");
      digitalWrite(green_trigger, HIGH);
      digitalWrite(red_trigger, LOW);
      digitalWrite(blue_trigger, LOW);
    }

    else if (currentIDs == red) {
      //Serial.println("The lights are red and mirror cipher is visible");
      digitalWrite(red_trigger, HIGH);
      digitalWrite(cipherLED, HIGH);
      digitalWrite(green_trigger,LOW);
      digitalWrite(blue_trigger, LOW);
    }

    else if (currentIDs == blue) {
      //Serial.println("The lights are blue and panel drops open");
      digitalWrite(blue_trigger, HIGH);
      digitalWrite(magLock, LOW);
      digitalWrite(blueLED, HIGH);
      digitalWrite(green_trigger, LOW);
      digitalWrite(red_trigger, LOW);
    }

    else {
      Serial.println("Yellow");
      digitalWrite(green_trigger, LOW);
      digitalWrite(red_trigger, LOW);
      digitalWrite(blue_trigger, LOW);
      digitalWrite(magLock, HIGH);
      digitalWrite(blueLED, LOW);
      digitalWrite(cipherLED, LOW);
    }
  }
}


/*
   Helper function to return a string ID from byte array
*/

String dump_byte_array( byte *buffer, byte bufferSize) {
  String read_rfid = "";
  for (byte i = 0; i < bufferSize; i++) {
    read_rfid = read_rfid + String(buffer[i], HEX);
  }
  return read_rfid;
}
