#include <Servo.h>

#include <SPI.h>
#include <MFRC522.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

Servo iesire;   int poz_iesire = 0;
Servo intrare;  int poz_intrare = 0;

int nrpersoane = 0;

void setup() {
  // put your setup code here, to run once:

  intrare.write(0);
  iesire.write(0);

  iesire.attach(3);
  intrare.attach(5);

  pinMode(7, INPUT);
  
  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);

  lcd.init();
  lcd.backlight();  
}

void loop() {
  // put your main code here, to run repeatedly:

  // PARTEA DE CONTOR SI AFISAJ (ECRAN LCD)
  
  lcd.setCursor(0, 0);
  lcd.print("Persoane in");

  lcd.setCursor(0, 1);
  lcd.print("cladire:");

  if(nrpersoane < 0)
    nrpersoane = 0;
  lcd.setCursor(11, 1);
  lcd.print(nrpersoane);


  // PARTEA DE IESIRE DIN CLADIRE (SENZOR + SERVOMOTOR) 

  if( digitalRead(7) == 0 ){
    nrpersoane --;
    for(poz_iesire = 0; poz_iesire <= 90; poz_iesire ++){
      iesire.write(poz_iesire);
      delay(22);
    }

                  while( digitalRead(7) == 0 )
                    iesire.write(90);

    delay(3000);

                
    for(poz_iesire = 90; poz_iesire >= 0; poz_iesire --){
      iesire.write(poz_iesire);
      delay(22);
    }    
  }


  //PARTEA DE INTRARE IN CLADIRE (RFID + SERVOMOTOR)  

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
  
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  
  if (content.substring(1) == "D3 2D E8 1A"){ //change here the UID of the card/cards that you want to give access
    digitalWrite(0, HIGH);
    nrpersoane ++;
    for(poz_intrare = 0; poz_intrare <= 90; poz_intrare ++){
      intrare.write(poz_intrare);
      delay(22);
    }
    digitalWrite(0, LOW);
    
    delay(3000);

    for(poz_intrare = 90; poz_intrare >= 0; poz_intrare --){
      intrare.write(poz_intrare);
      delay(22);
    }
  } 
  else{
    digitalWrite(1, HIGH);
    delay(1000);
    digitalWrite(1, LOW);
  }
}
