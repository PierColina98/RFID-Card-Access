#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <stdbool.h>
#include <Servo.h>

#define OPEN_POS 0
#define CLOSE_POS 80
#define RST_PIN 9
#define SS_PIN 10
typedef unsigned long tTime;

Servo myServo;
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
tTime lastOn;
byte servoState;

// put function declarations here:
tTime timePassed(tTime since);
void offLcd();
void waitCard();
void verifyCredential();
void changeServoState();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200); // Iniciar comunicación serial
  SPI.begin();          // Iniciar comunicación SPI
  mfrc522.PCD_Init();   // Iniciar el lector RC522
  mfrc522.PCD_DumpVersionToSerial();
  myServo.attach(6);
  servoState = 0;
  myServo.write(CLOSE_POS);
  lcd.init();
  lcd.backlight();
  lastOn = millis();
}

void loop()
{
  offLcd();
  waitCard();
  verifyCredential();
}

// put function definitions here:

tTime timePassed(tTime since)
{
  tTime now = millis();
  return now - since;
}

void offLcd()
{
  if (timePassed(lastOn) > 10000)
  {
    lcd.noBacklight();
  }
}

void waitCard()
{
  lcd.setCursor(0, 1);
  if(servoState == 0){
    lcd.print("State: CLOSED");
  }else{
    lcd.print("State: OPEN");
  }
  lcd.setCursor(0, 0);
  lcd.print("Reading RFIDs");
  for (int i = 0; i < 3; i++)
  {
    lcd.print('.');
    delay(300);
  }
  lcd.clear();
}

void verifyCredential()
{
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
  {
    lcd.backlight();
    lastOn = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card Found!");
    lcd.setCursor(0, 1);
    lcd.print("Verifying...");
    // Obtener UID de la tarjeta
    String cardUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      cardUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println(cardUID);
    delay(1000);
    if (cardUID.compareTo("a1b8e11d") == 0)
    {
      lcd.clear();
      lcd.print("Granted! c:");
      delay(1000);
      changeServoState();
    }
    else
    {
      lcd.clear();
      lcd.print("Denied.");
      delay(1000);
    }
  }
}

void changeServoState(){
  if(servoState == 0){
    myServo.write(CLOSE_POS);
    servoState = 1;
  }else{
    myServo.write(OPEN_POS);
    servoState = 0;
  }
}