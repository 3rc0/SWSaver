/*
    ::  ::  ::  ::  ::  ::  ::  ::  ::  ::  ::  ::  ::  ::
    ::  Name          :: Prototype SWSaver Source Code  ::
    ::  Version       :: 0.03                           ::
    ::  Build:        :: Alpha                          ::
    ::  Codename:     :: AZDDS                          ::
    ::  Author:       :: Diyar Hussein                  ::
    ::  Last updated  :: Monday, May 20, 2019           ::
    ::                                                  ::
    ::               `  ©  2016 - 2019                  ::
    ::  ::  ::  ::  ::  ::  ::  ::  ::  ::  ::  ::  ::  ::

    + References:

    - Waveshare Motor Control Shield - Created by Waveshare https://www.waveshare.com/wiki/Motor_Control_Shield
    - Keypad 4x4 https://playground.arduino.cc/code/keypad
    - Ethernet Shield W5100 https://www.arduino.cc/en/Reference/Ethernet
    - LiquidCrystal I2C https://github.com/johnrickman/LiquidCrystal_I2C
    - MySQL Connector Arduino - Created by: Dr. Charles A. Bell https://github.com/ChuckBell/MySQL_Connector_Arduino
    - RFID - RC522 https://github.com/miguelbalboa/rfid
    - http://aconcaguasci.blogspot.com/2017/01/sprintf-function-in-arduino.html

    +List of components:

    - Arduino Mega 2560 R3 ( Original ).
    - Stepper Motor 28BYJ-48 v5.
    - Hybrid Stepper Motor MT-1704HS180A - NEMA 17 Stepper motor.
    - Ethernet Shield Network Module W5100 (From Aliexpress, China's clone).
    - Liquid Crystal Display 16x2 (From Aliexpress, China's clone)( 2 pieces).
    - Motor Control Shield Waveshare SKU 9724
    - Carriage MGW12H.
    - Mini Rail MGW12.
    - LED ( 4 pieces).
*/


//********************************************//
//######::  ::   Library Part     ::  ::######//
//********************************************//

#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Ethernet.h>
#include <SPI.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include "DEV_Config.h"
#include "Motor.h"
#include <MFRC522.h>


//********************************************//
//######::  ::     lcd1 Part       ::  ::######//
//********************************************//

LiquidCrystal_I2C lcd0(0x27, 20, 4);
LiquidCrystal_I2C lcd1(0x3f, 20, 4);


//********************************************//
//######::  ::    Keypad Part     ::  ::######//
//********************************************//

const byte rowsKeypad = 4;
const byte columnsKeypad = 4;
char keys[rowsKeypad][columnsKeypad] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowKeypadPins[rowsKeypad] = {25, 24, 23, 22};
byte columnKeypadPins[columnsKeypad] = {29, 28, 27, 26};
char customKey;

Keypad customKeypad(makeKeymap(keys), rowKeypadPins, columnKeypadPins, rowsKeypad, columnsKeypad);

//********************************************//
//######::  ::    ID Key Part     ::  ::######//
//********************************************//

#define idLength 5
#define rfidLength 2
char tempId[idLength];
char rfidOption[rfidLength] = "A";
byte idCounter = 0;

char testIdKey [idLength] = "1234";  //  For debug purpose only.

//********************************************//
//######::  ::  Networking Part   ::  ::######//
//********************************************//

byte physicalAddress[] = {0xAA, 0xBB, 0xEE, 0xFF, 0xAA};
IPAddress serverAddress(192, 168, 0, 14);
EthernetClient ethernetClient;

MySQL_Connection MySQLConnection((Client *)&ethernetClient);
const char MySQLUser[] = "root";
const char MySQLPassword = "123456";

//********************************************//
//######::  ::  MySQL Query Part  ::  ::######//
//********************************************//

char insertData[] = "INSERT INTO `hanger`.`hanger` ( hangerNumber, hangerStatus, idKey) VALUES (%d, %d, %d)"; //  For debug purpose only.
char hangerStatus[] = "SELECT hangerStatus FROM hanger.hanger WHERE hangerStatus ='1' LIMIT 1";
char hangerNumber[] = "SELECT hangerNumber FROM hanger.hanger WHERE hangerStatus = '1' LIMIT 1";
char updateHanger[] = "UPDATE hanger.hanger SET hangerStatus = %d, idKey = %d WHERE hangerNumber = %d";
char updateHangerB[] = "UPDATE hanger.hanger SET hangerStatus = %u, idKey = %u WHERE hangerNumber = %u"; //  Backup Update Statement
char idSelector[] = "SELECT idKey FROM hanger.hanger WHERE idKey = %d";
const char unloadHa[] = "SELECT hangerNumber FROM hanger.hanger WHERE idKey =%d";
char query[128];

//********************************************//
//######::  ::   RFID-RC522 Part  ::  ::######//
//********************************************//

#define SS_RFID 53
#define RST_RFID 49
MFRC522 rfid_Read(SS_RFID, RST_RFID);
MFRC522 mfrc522(SS_RFID, RST_RFID);



//********************************************//
//######::  ::     Other Parts    ::  ::######//
//********************************************//

#define sensorSwitch  30      // Assigned Pin Number D40 to Sensor Switch.
#define led0  31              //  For debug purpose only.
#define led1  32              //  For debug purpose only.
#define led2  33              //  For debug purpose only.
#define led3  34              //  For debug purpose only.
int sensorSwitchStatus = 0;   //  Define the Sensor Switch Status in zero value assuming is not pressed.


void setup() {
  Serial.begin(115200);
  Serial.println(F("  #sis000  Initialize and confirm Serial connected."));
  while (!Serial);
  Serial.println(F("  #sip000  Initialize Pins."));
  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(sensorSwitch, INPUT);
  Serial.println(F("  #sil000  Initialize Liquid Crystal Display."));
  lcd0.init();
  lcd0.backlight();
  lcd0.setCursor(0, 0);
  lcd0.print("SWSaver v0.03");
  lcd1.init();
  lcd1.backlight();
  lcd1.setCursor(3, 0);
  lcd1.print("Welcome...");
  lcd1.setCursor(2, 1);
  lcd1.print("2016 - 2019");
  Serial.println(F("  #sie000  Initialize Ethernet Shield Network Module"));
  ethernetHardwareChecker();
  Serial.println(F("  #sim000  Initialize MySQL Server connection"));
  mysqlServerConnection();
  lcd0.clear();
  lcd1.clear();
  Serial.println(F("  #sir000  Initialize RFID-RC522."));
  mfrc522.PCD_Init();
}

void loop() {
  sensorSwitchStatus = digitalRead(sensorSwitch);
  customKey = customKeypad.getKey();
  lcd0.setCursor(0, 0);
  lcd0.print("SWSaver v0.03");
  lcd1.setCursor(4, 0);
  lcd1.print("Enter ID Key");
  lcd1.setCursor(2, 1);
  lcd1.print("2016 - 2019");


  if ( sensorSwitchStatus == 0) {
    unloadHanger();
  } else {
    loadHanger();
  }
  digitalWrite(RST_RFID, HIGH);
  digitalWrite(SS_RFID, HIGH);
  rfid_Read.PCD_SoftReset;
  rfid_Read.PCD_Init();
  //rfid_Read.PCD_DumpVersionToSerial(); // For debug purpose only.
  if (rfid_Read.PICC_IsNewCardPresent() && rfid_Read.PICC_ReadCardSerial()) {
    readDataFromRFIDTag();
  }
}

void ethernetHardwareChecker () {
  Ethernet.begin(physicalAddress);
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println(F("  #sie001 Ethernet shield was not found.  Sorry, can't run without hardware. :("));
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println(F("Ethernet cable is not connected."));
  }
  Serial.print(F("  #sie002 Assign a local IP Address for Ethernet Shield Network Module: "));
  Serial.println(Ethernet.localIP());
}

void mysqlServerConnection() {
  lcd1.clear();
  lcd1.setCursor(1, 0);
  lcd1.print("Connecting to");
  lcd1.setCursor(2, 1);
  lcd1.print("MySQL Server");
  if (MySQLConnection.connect(serverAddress, 3306, MySQLUser, MySQLPassword))
  {
    delay(500);
    lcd1.clear();
  }
  else
  {
    lcd1.clear();
    Serial.println(F("  #sim001 Failed to connect, try again!"));
    lcd1.setCursor(1, 0);
    lcd1.print("Error connect");
    lcd1.setCursor(0, 1);
    lcd1.print("  Try again...");
    delay(10000);
    lcd1.clear();
  }
}

void loadHanger() {
  Serial.println(F("  #ll000 Detected Sensor Switch a new coat on Hanger.\n\r #llr000 Software turn off RFID device."));

  rfid_Read.PCD_SoftPowerDown();
  lcd1.clear();
  lcd1.setCursor(1, 0);
  lcd1.print(" Processing...");
  Serial.println(F("  #llm000 Running MySQL Connector operation for hangerStatus statement."));

  int hangerStatusCount = 0;
  MySQL_Cursor *mysqlCursor = new MySQL_Cursor(&MySQLConnection);
  mysqlCursor->execute(hangerStatus);
  // char hangerStatus[] = "SELECT hangerStatus FROM hanger.hanger WHERE hangerStatus ='1' LIMIT 1";
  column_names *hangerStatusColumn = mysqlCursor->get_columns();
  row_values *row = NULL;
  do {
    row = mysqlCursor->get_next_row();
    if (row != NULL) {
      hangerStatusCount = atol(row->values[0]);
    }
  } while (row != NULL);

  Serial.println(String("") + "  #llm001 MySQL Connector operation is finished and the Hanger Status available is: " + hangerStatusCount);
  if (hangerStatusCount == 1) {

    Serial.println(F("  #llm002 MySQL Connector operation for hangerNumber statement to select that first top available."));

    row_values *row = 0;
    int numHa = 0;
    mysqlCursor->execute(hangerNumber);
    // char hangerNumber[] = "SELECT hangerNumber FROM hanger.hanger WHERE hangerStatus = '1' LIMIT 1";
    column_names *hangerNumberColumn = mysqlCursor->get_columns();
    do {
      row = mysqlCursor->get_next_row();
      if (row != NULL) {
        numHa = atol(row->values[0]);
      }
    } while (row != NULL);

    Serial.println(String("") + "  #llm003 MySQL Connector operation for hangerNumber statement is finished, and the Hanger Number is: " + numHa + "\n\r  #llm004 MySQL Connector operation for hangerUpdate statement to set a new value for selected Hanger.\n\r  #llm005 Generating a new 4 digits number as ID Key for the selected Hanger");
    randomSeed(analogRead(A0));
    unsigned int firstGenerator = random(1000, 8999);
    unsigned int secondGenerator = random(100, 899);
    unsigned int thirdGenerator = random(99);
    unsigned int fourthGenerator = random(3);
    unsigned int idKey = firstGenerator + secondGenerator + thirdGenerator + fourthGenerator;
    Serial.println(String("") + "  #ll001 Generator of ID Key is finished, the ID Key is: " + idKey);
    int reserved = 2;
    sprintf(query, updateHanger, reserved, idKey, numHa);
    // updateHanger[] = "UPDATE hanger.hanger SET hangerStatus = %d, idKey = %d WHERE hangerNumber = %d";
    mysqlCursor->execute(query);
    Serial.println(F("  #llm006 MySQL Connector operation for hangerUpdate statement is finishedd, run SELECT statement for MySQL Server view the changes"));

    ///  Loop statement of Countdown Timer for User Selection

    for (int timer = 0; timer <= 15;) {

      customKey = customKeypad.getKey();
      lcd0.clear();
      lcd1.clear();
      lcd0.setCursor(0, 0);
      lcd0.print("SWSaver v0.03");
      lcd0.setCursor(14, 0);
      lcd0.print(timer);
      lcd0.setCursor(timer, 1);
      lcd0.print("################");
      lcd1.setCursor(0, 0);
      lcd1.print("Press A for NFC");
      lcd1.setCursor(0, 1);
      lcd1.print("or C to continue");

      ///   Waiting for decision by Keypad input or conitune after deadline

      if  (customKey) {
        Serial.println(F("  #llk000 Detected input from Keypad.\n\r  #llk001  Recording the types from Keypad."));
        tempId[idCounter] = customKey;
        lcd0.setCursor(14, 0);
        lcd0.print(tempId[idCounter]);
        idCounter++;
      }
      if (idCounter == rfidLength - 1) {
        lcd0.clear();
        lcd1.clear();
        if (strcmp(tempId, rfidOption)) {
          Serial.println(F("  #llk0002  Continue by display ID Key on LCD."));
          lcd0.setCursor(0, 0);
          lcd0.print("SWSaver v0.03");
          lcd1.setCursor(1, 0);
          lcd1.print("ID Key is:");
          lcd1.setCursor(12, 0);
          lcd1.print(idKey);
          lcd1.setCursor(3, 1);
          lcd1.print("2016 - 2019");

          break;
        }
        else {
          Serial.println(F("  #llk003  Select A option for write ID Key on RFID Tag."));
          lcd0.setCursor(0, 0);
          lcd0.print("SWSaver v0.03");
          lcd1.setCursor(0, 0);
          lcd1.print("ID Key is:");
          lcd1.setCursor(12, 0);
          lcd1.print(idKey);
          lcd1.setCursor(0, 1);
          lcd1.print("Nearby NFC Tag");

          //********************************************//
          //######::  ::  RFID-RC522 Write  ::  ::######//
          //********************************************//

          Serial.println(F("  #llw000 Software turn on RFID device."));
          mfrc522.PCD_SoftPowerUp();
          // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
          MFRC522::MIFARE_Key key;
          for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

          // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
          if ( ! mfrc522.PICC_IsNewCardPresent()) {
            return;
          }

          // Select one of the cards
          if ( ! mfrc522.PICC_ReadCardSerial()) {
            return;
          }

          Serial.print(F("  #llw001 Card UID:"));    //Dump UID
          for (byte i = 0; i < mfrc522.uid.size; i++) {
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX);
          }
          MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
          Serial.println(String("") + " #llr002 PICC type: " + mfrc522.PICC_GetTypeName(piccType));

          byte buffer[34];
          byte block;
          MFRC522::StatusCode status;
          byte len;

          Serial.setTimeout(20000L) ;     // wait until 20 seconds for input from serial
          sprintf(buffer, "%d", idKey);
          block = 1;
          //Serial.println(F("Authenticating using key A..."));
          status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
          if (status != MFRC522::STATUS_OK) {
            Serial.print(String("") + " #llw003 PCD_Authenticate() failed: " + mfrc522.GetStatusCodeName(status));
            return;
          }
          else Serial.println(String("") + "  #llw004 PCD_Authenticate() success: ");

          // Write block
          status = mfrc522.MIFARE_Write(block, buffer, 16);
          if (status != MFRC522::STATUS_OK) {
            Serial.print(String("") + " #llw004 MIFARE_Write() failed: " + mfrc522.GetStatusCodeName(status));
            return;
          }
          else Serial.println(String("") + "  #llw005 MIFARE_Write() success: ");

          block = 2;
          Serial.println(F("  #llw006 Authenticating using key A..."));
          status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
          if (status != MFRC522::STATUS_OK) {
            Serial.print(String("") + " #llw007 PCD_Authenticate() failed: " + mfrc522.GetStatusCodeName(status));
            return;
          }

          // Write block
          status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
          if (status != MFRC522::STATUS_OK) {
            Serial.print(String("") + " #llw008 MIFARE_Write() failed: " + mfrc522.GetStatusCodeName(status));
            return;
          }
          else Serial.println(F(" #llw009 MIFARE_Write() success: "));

          Serial.println(" ");
          mfrc522.PICC_HaltA(); // Halt PICC
          mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

          break;
        }
      }


      delay(800);
      timer++;
    }

    ///  End Loop statement of Countdown Timer
    lcd0.clear();
    lcd1.clear();
    lcd0.setCursor(1, 0);
    lcd0.print("SWSaver v0.03");
    lcd1.setCursor(1, 0);
    lcd1.print("ID Key is:");
    lcd1.setCursor(12, 0);
    lcd1.print(idKey);
    lcd1.setCursor(3, 1);
    lcd1.print("2016 - 2019");

    Serial.println(F("  #llo000 Passing the Hanger Number to the Stepper Motors"));
    if (numHa == 1) {
      Serial.println(F("  #llo001 Motor Controller operating the Hanger number 1"));
      baseMotor1();
      linearMotor1();
      linearMotorReturnToHome1();
      baseMotorReturnToHome1();
      delay(500);
    }
    else if (numHa == 2) {
      Serial.println(F("  #llo002 Motor Controller operating the Hanger number 2"));
      baseMotor2();
      linearMotor2();
      linearMotorReturnToHome2();
      baseMotorReturnToHome2();
      delay(500);
    }
    Serial.println(F("  #ll002 Operation for checking Hanger, number Hanger, and ID Key generating is finished."));
    delay(2000);
    lcd1.clear();

  } else {
    Serial.println(F("  #ll003 No place availalble, Out put the message on Display about the status!"));
    lcd1.clear();
    lcd1.setCursor(3, 0);
    lcd1.print("It Is Full");
    lcd1.setCursor(3, 1);
    lcd1.print("Try later");
    delay(5000);
  }
  delay(5000);
  Serial.println(String("") + "  #llm007 Memory usage before deleting the MySQL Cursor / Packets: " + memoryUsage());
  delete mysqlCursor;
  Serial.println(String("") + "  #llm008 Memory usage after deteling the MySQL Cursor / Packets: " + memoryUsage());
  lcd0.clear();
  lcd1.clear();
  clearTemp();
}
void unloadHanger() {
  if  (customKey) {
    Serial.println(F("  #luk000 Detected input from Keypad.\n\r  #lux0001  Recording the types from Keypad."));
    tempId[idCounter] = customKey;
    lcd1.setCursor(idCounter, 0);
    lcd1.print(tempId[idCounter]);
    idCounter++;
  }
  if (idCounter == idLength - 1) {
    int idTyped = atol(tempId);
    if (idTyped != 0) {
      Serial.println(String("") + "  #luk001 The ID Key recorded ( " + idTyped + " )" + "\n\r  #lum000  Running MySQL Connector operation for idSelector Statement.");
      MySQL_Cursor *mysqlCursor = new MySQL_Cursor(&MySQLConnection);

      row_values *row = NULL;
      int idDB = 0;
      sprintf(query, idSelector, idTyped);
      mysqlCursor->execute(query);
      column_names *columns = mysqlCursor->get_columns();
      do {
        row = mysqlCursor->get_next_row();
        if (row != NULL) {
          idDB = atol(row->values[0]);
        }
      } while (row != NULL);
      Serial.println(F("  #lum001  MySQL Connector operation is finished.\n\r  #lum002 Verifying the typed ID Key with Database."));

      if (!strcmp(idTyped, idDB)) {
        Serial.println(F("  #luk002  Entered correct ID Key."));
        lcd1.clear();
        lcd1.setCursor(0, 0);
        lcd1.print("Please wait...");

        Serial.println(String("") + "  #lum003   Running MySQL Connector operation for unloadHa statement.\n\r ID of Database: ." + idDB + " and ID Key Typed:" + idTyped);
        int numHa = 0;
        row_values *row = NULL;
        sprintf(query, unloadHa, idTyped);
        mysqlCursor->execute(query);
        column_names *columns = mysqlCursor->get_columns();
        do {
          row = mysqlCursor->get_next_row();
          if (row != NULL) {
            numHa = atol(row->values[0]);
          }
        } while (row != NULL);
        Serial.println(String("") + " #lum004 The Hanger Number from unloadHa statement is: " + numHa + "\n\r  #lum005 The ID Key belong to the Hanger number: ");
        if (numHa == 1) {
          Serial.println(numHa);
          baseMotor1();
          linearMotor1();
          linearMotorReturnToHome1();
          baseMotorReturnToHome1();

        } else if (numHa == 2 ) {
          Serial.println(numHa);
          baseMotor2();
          linearMotor2();
          linearMotorReturnToHome2();
          baseMotorReturnToHome2();
        }
        lcd1.setCursor(0, 1);
        lcd1.print("Check the hanger");
        delay(2500);
        lcd1.clear();
        Serial.println("  #lum006  MySQL Connector operation for hangerNumber statement is finished.\n\r  #lum007  Running MySQL Connector ooperation for updateHanger statement.");
        int unreserved = 1;
        int idKey = 0;
        sprintf(query, updateHanger, unreserved, idKey, numHa);
        // updateHanger[] = "UPDATE hanger.hanger SET hangerStatus = %d, idKey = %d WHERE hangerNumber = %d";
        mysqlCursor->execute(query);
        Serial.println(F("  #lum008 MySQL Connector operation is finished."));
      }
      else {
        Serial.println(F("  #luk003  Entered incorrect ID Key."));
        lcd1.clear();
        lcd1.setCursor(0, 0);
        lcd1.print("Incorrect ID Key");
        lcd1.setCursor(1, 1);
        lcd1.print("Enter it again");
        delay(5000);
        lcd1.clear();

      }
      Serial.println(String("") + "  #lum009 Memory usage before deleting the MySQL Cursor / Packets: " + memoryUsage());
      delete mysqlCursor;
      Serial.println(String("") + "  #lum010 Memory usage after deteling the MySQL Cursor / Packets: " + memoryUsage());
    } else {
      lcd1.clear();
      lcd1.print("ID Key doesnot");
      lcd1.setCursor(0, 1);
      lcd1.print("start with/or 0");
      delay(5000);
      lcd1.clear();

    } clearTemp();
  }
}

//********************************************//
//######::  ::   RFID-RC522 Read  ::  ::######//
//********************************************//

void readDataFromRFIDTag() {
  MFRC522::MIFARE_Key KeyR;
  MFRC522::StatusCode statusR;
  byte bufferR[18];
  byte len = 18;
  byte Block = 1;

  Serial.println(F("  #luR0001 Read data from RFID Tag"));
  for (uint8_t i = 0; i < 6; i++)KeyR.keyByte[i] = 0xFF;
  statusR = rfid_Read.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, Block, &KeyR, &(rfid_Read.uid));
  if (statusR != MFRC522::STATUS_OK) {
    statusR = rfid_Read.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, Block, &KeyR, &(rfid_Read.uid));
    if (statusR != MFRC522::STATUS_OK) {
      Serial.println(String("") + " #luR0002  Authentication failed: " + rfid_Read.GetStatusCodeName(statusR));
      return;
    }
  }
  statusR = rfid_Read.MIFARE_Read(Block, bufferR, &len);
  if (statusR != MFRC522::STATUS_OK) {
    Serial.println(String("") + " #luR0003  Reading failed: " + rfid_Read.GetStatusCodeName(statusR));
    return;
  }
  for (uint8_t i = 0; i < 16; i++) {
    if (bufferR[i] != 32) {
      //Passing the value to out of loop statement
    }
  }
  Serial.println(F("  #luR0004  Convert the Byte array to unsigned Interger number with operation \'atol\'."));
  unsigned int rfid_IDKey = atol(bufferR);
  Serial.println(String("") + "\n\r #luR0005  ID Key stored on Tag is: " + rfid_IDKey + (".\n\r #luR0006  Running MySQL Connector operation for idSelector Statement."));
  MySQL_Cursor *mysqlCursor = new MySQL_Cursor(&MySQLConnection);

  row_values *row = NULL;
  int idDB = 0;
  sprintf(query, idSelector, rfid_IDKey);
  mysqlCursor->execute(query);
  column_names *columns = mysqlCursor->get_columns();
  do {
    row = mysqlCursor->get_next_row();
    if (row != NULL) {
      idDB = atol(row->values[0]);
    }
  } while (row != NULL);
  Serial.println(F("  #luR0007  MySQL Connector operation is finished.\n\r #luR0008 Verifying the typed ID Key with Database."));

  if (!strcmp(rfid_IDKey, idDB)) {
    Serial.println(F("  #luR0009  Entered correct ID Key."));
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("Please wait...");

    Serial.println(F("  #luR0010   Running MySQL Connector operation for unloadHa statement."));
    Serial.println(idDB);
    Serial.println(rfid_IDKey);
    int numHa = 0;
    row_values *row = NULL;
    sprintf(query, unloadHa, rfid_IDKey);
    mysqlCursor->execute(query);
    column_names *columns = mysqlCursor->get_columns();
    do {
      row = mysqlCursor->get_next_row();
      if (row != NULL) {
        numHa = atol(row->values[0]);
      }
    } while (row != NULL);
    Serial.println(String("") + ("  #luR0011 The Hanger Number from unloadHa statement is: ") + numHa + ("\n\r  #luR0012 The ID Key belong to the Hanger number: "));
    if (numHa == 1) {
      Serial.println(numHa);
      baseMotor1();
      linearMotor1();
      linearMotorReturnToHome1();
      baseMotorReturnToHome1();

    } else if (numHa == 2 ) {
      Serial.println(numHa);
      baseMotor2();
      linearMotor2();
      linearMotorReturnToHome2();
      baseMotorReturnToHome2();
    }
    lcd1.setCursor(0, 1);
    lcd1.print("Check the hanger");
    delay(200);
    lcd1.clear();
    Serial.println("  #luR0013  MySQL Connector operation for hangerNumber statement is finished.\n\r #luR0014  Running MySQL Connector ooperation for updateHanger statement.");
    int unreserved = 1;
    int idKey = 0;
    sprintf(query, updateHanger, unreserved, idKey, numHa);
    // updateHanger[] = "UPDATE hanger.hanger SET hangerStatus = %d, idKey = %d WHERE hangerNumber = %d";
    mysqlCursor->execute(query);
    Serial.println(F("  #luR0015 MySQL Connector operation is finished."));
  }
  else {
    Serial.println(F("  #luR0016  Entered incorrect ID Key."));
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("Incorrect ID Key");
    lcd1.setCursor(1, 1);
    lcd1.print("Enter it again");
    delay(5000);
  }
  Serial.println(String("") + ("  #luR0017 Memory usage before deleting the MySQL Cursor / Packets: ") + memoryUsage());
  delete mysqlCursor;
  Serial.println(String("") + ("  #luR0018 Memory usage after deteling the MySQL Cursor / Packets: ") + memoryUsage());

  rfid_Read.PICC_HaltA();
  rfid_Read.PCD_StopCrypto1();
  rfid_Read.PCD_Idle;
  digitalWrite(RST_RFID, LOW);
  digitalWrite(SS_RFID, LOW);
  lcd1.clear();
}

//********************************************//
//#####   Motor Control for Position 1  ######//
//********************************************//

void baseMotor1()
{
  digitalWrite(led0, HIGH);
  delay(500);
  Motor_Init(MOTOR_DEV_1, MOTOR_DEV_2);
  Motor_Trun(MOTOR_DEV_1, 7); //750
  digitalWrite(led0, LOW);
}

void baseMotorReturnToHome1()
{
  digitalWrite(led1, HIGH);
  delay(500);
  Motor_Init0(MOTOR_DEV_1, MOTOR_DEV_2);
  Motor_Trun0(MOTOR_DEV_1, 7);  //750
  digitalWrite(led1, LOW);
}

void linearMotor1()
{
  digitalWrite(led2, HIGH);
  delay(500);
  Motor_Init(MOTOR_DEV_1, MOTOR_DEV_2);
  Motor_Trun(MOTOR_DEV_2, 9);   //90
  digitalWrite(led2, LOW);
}

void linearMotorReturnToHome1()
{
  digitalWrite(led3, HIGH);
  delay(500);
  Motor_Init0(MOTOR_DEV_1, MOTOR_DEV_2);
  Motor_Trun0(MOTOR_DEV_2, 9);  //90
  digitalWrite(led3, LOW);
}


//********************************************//
//#####   Motor Control for Position 2  ######//
//********************************************//

void baseMotor2()
{
  digitalWrite(led0, HIGH);
  delay(500);
  Motor_Init(MOTOR_DEV_1, MOTOR_DEV_2);
  Motor_Trun(MOTOR_DEV_1, 1);  //1600
  digitalWrite(led0, LOW);
}

void baseMotorReturnToHome2()
{
  digitalWrite(led1, HIGH);
  delay(500);
  Motor_Init0(MOTOR_DEV_1, MOTOR_DEV_2);
  Motor_Trun0(MOTOR_DEV_1, 1); //1600
  digitalWrite(led1, LOW);
}

void linearMotor2()
{
  digitalWrite(led2, HIGH);
  delay(500);
  Motor_Init(MOTOR_DEV_1, MOTOR_DEV_2);
  Motor_Trun(MOTOR_DEV_2, 9);  //90
  digitalWrite(led2, LOW);
}

void linearMotorReturnToHome2()
{
  digitalWrite(led3, HIGH);
  delay(500);
  Motor_Init0(MOTOR_DEV_1, MOTOR_DEV_2);
  Motor_Trun0(MOTOR_DEV_2, 90);
  digitalWrite(led3, LOW);
}


void clearTemp() {
  while (idCounter != 0) {
    tempId[idCounter--] = 0;
  }
  return;
}
int memoryUsage()
{
  extern char __bss_end;
  extern char *__brkval;
  int free_memory;
  if ((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);
  return free_memory;
}
