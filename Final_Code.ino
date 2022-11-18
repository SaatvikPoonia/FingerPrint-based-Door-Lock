#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
#include <Adafruit_I2CDevice.h>

#define LED_G 6  // define green LED pin
#define LED_R 7  // define red LED
#define RELAY 8  // relay pin
#define BUZZER 9 // buzzer pibn
#define ACCESS_DELAY 1000
#define DENIED_DELAY 2000


SoftwareSerial mySerial(2, 3);
LiquidCrystal_I2C lcd(0x27, 16, 2);

uint32_t syncTime = 0; // time of last sync()
RTC_DS1307 RTC;            // define the Real Time Clock object
const int chipSelect = 10; // for the data logging shield, we use digital pin 10 for the SD cs line
File myfile;  

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

bool lastState = -1;

void setup()

{
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH);
  lcd.begin();
  lcd.backlight();

  Serial.begin(9600);
  SPI.begin();        // Initiate  SPI bus
 
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);

  delay(100);

  Serial.println("\n\nAdafruit finger detect test");

  finger.begin(57600);

  if (finger.verifyPassword())
  {
    Serial.println("Found fingerprint sensor!");
  }
  else
  {

    Serial.println("Did not find fingerprint sensor :(");

    while (1)
    {
      delay(1);
    }
  }

  finger.getTemplateCount();

  Serial.print("Sensor contains ");
  Serial.print(finger.templateCount);
  Serial.println(" templates");

  Serial.println("Waiting for valid finger...");

  displayWaitFinger();

  initSDcard();
  createFile();
  initRTC();
  myfile.println("millis, stamp, date, time, security type, ID, name");
}

void loop() // run over and over again
{

  fingerprint();
}

void displayWaitFinger()
{

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PUT YOUR FINGER ");
  lcd.setCursor(0, 1);
  lcd.print("On the Sensor");
}

void fingerprint()
{
  int id;

  id = getFingerprintIDez();

  if (id == 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WELCOME Saatvik");
    lcd.setCursor(0, 1);
    lcd.print("DOOR UNLOCKED");

    data_log("FINGER PRINT", "Saatvik","1");
    displayFingerOK();
    delay(2000);
    displayWaitFinger(); 
  }

  if (id == 2)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WELCOME Omkar");
    lcd.setCursor(0, 1);
    lcd.print("DOOR UNLOCKED");

    data_log("FINGER PRINT","Omkar", "2" );
    displayFingerOK();
    delay(2000);
    displayWaitFinger(); 
  }

  if (id == 3)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WELCOME Saksham");
    lcd.setCursor(0, 1);
    lcd.print("DOOR UNLOCKED");

    data_log("FINGER PRINT","Saksham", "3" );
    displayFingerOK();
    delay(2000);
    displayWaitFinger(); 
  }
  if (id == 4)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WELCOME Prabhat");
    lcd.setCursor(0, 1);
    lcd.print("DOOR UNLOCKED");

    data_log("FINGER PRINT", "Prabhat","4");
    displayFingerOK();
    delay(2000);
    displayWaitFinger(); 
  }

  if (id == 5)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WELCOME Gaurish");
    lcd.setCursor(0, 1);
    lcd.print("DOOR UNLOCKED");
    data_log("FINGER PRINT","Gaurish", "5");
    displayFingerOK();
    delay(2000);
    displayWaitFinger(); 
  }

  else 
  {
      displayInvalidFinger();
      delay(2000);
      displayWaitFinger();
  }
}

void displayInvalidFinger()

{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("FINGER");
  lcd.setCursor(0, 1);
  lcd.print("NOT FOUND!!!");
  digitalWrite(LED_R, HIGH);
  tone(BUZZER, 300);
  delay(DENIED_DELAY);
  digitalWrite(LED_R, LOW);
  noTone(BUZZER);
}

void displayFingerOK()
{
  digitalWrite(RELAY, LOW);
  digitalWrite(LED_G, HIGH);
  tone(BUZZER, 5);
  delay(ACCESS_DELAY);
  digitalWrite(LED_G, LOW);
  noTone(BUZZER);
  digitalWrite(RELAY, HIGH);
}


int getFingerprintIDez()
{

  uint8_t p = finger.getImage();

  Serial.println(p);

  if (p == 0)
  {

    lastState = 0;
  }

  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.image2Tz();

  if (p != FINGERPRINT_OK)
    return -1;

  p = finger.fingerFastSearch();

  if (p != FINGERPRINT_OK)
    return -1;

  lastState = 1;

  // found a match!

  Serial.print("Found ID #");
  Serial.print(finger.fingerID);

  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);

  return finger.fingerID;
}
void initSDcard()
{
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect))
  {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}

void createFile()
{
  // file name must be in 8.3 format (name length at most 8 characters, follwed by a '.' and then a three character extension.
  char filename[] = "AIAPB00.CSV";
  for (uint8_t i = 0; i < 100; i++)
  {
    filename[5] = i / 10 + '0';
    filename[6] = i % 10 + '0';
    if (!SD.exists(filename))
    {
      // only open a new file if it doesn't exist
      myfile = SD.open(filename, FILE_WRITE);
      break; // leave the loop!
    }
  }

  Serial.print("Logging to: ");
  Serial.println(filename);
}

void initRTC()
{
  Wire.begin();
  if (!RTC.begin())
  {
    myfile.println("RTC failed");
  }
}

void data_log(String securityType, String Name, String ID)
{
  DateTime n = RTC.now();
  delay((100 - 1) - (millis() % 100));
  uint32_t m = millis();
  myfile.print(m);
  myfile.print(", ");
  n = RTC.now();
  myfile.print(n.unixtime());
  myfile.print(", ");
  myfile.print(n.day(), DEC);
  myfile.print("/ ");
  myfile.print(n.month(), DEC);
  myfile.print("/ ");
  myfile.print(n.year(), DEC);
  myfile.print(" ");
  myfile.print(", ");
  myfile.print(n.hour(), DEC);
  myfile.print(":");
  myfile.print(n.minute(), DEC);
  myfile.print(":");
  myfile.print(n.second(), DEC);
  myfile.print(", ");
  myfile.print(securityType);
  myfile.print(", ");
  myfile.print(ID);
  myfile.print(", ");
  myfile.println(Name);

  if ((millis() - syncTime) < 100)
    return;
  syncTime = millis();

  myfile.flush();
}
