#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN         8          // Configurable, see typical pin layout above
#define SS_PIN          9         // Configurable, see typical pin layout above

byte readCard[4];
String realTag = "43acd6c";       //sets the right card's UID
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
String myScore = "";
Servo myservo;    //create servo object
int pos = 0;      //this is the variable to store the servo position
bool isGateOpen = false;
const int buzzerPin = 4;

void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC

  //rfid
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus

	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan for access"));
  //servo
  myservo.write(0);
  myservo.attach(3);    //assigning digital pin 3 to servo

  //passive buzzer
  pinMode(buzzerPin, OUTPUT);

}
//logic: scan the right card, door opens, scan the right card again, door closes
void loop() {
	if (getID()) {
    if (myScore == realTag){      //if the right card is scanned
      Serial.println("Access granted!");  //print that access is granted

      if (isGateOpen == false){
        Serial.println("Doors closing");
        for (pos = 0; pos <=90; pos +=1){   //turn the servo 90°
        myservo.write(pos);   //tell servo to go to position in the pos variable
        delay(15);    //wait 15 ms for the servo to catch up
        }
        isGateOpen = true;
      }
      else{
        Serial.println("Doors opening");
        for (pos = 90; pos >=0; pos -=1){
        myservo.write(pos);
        delay(15);
        }
        isGateOpen = false;
      }
    }
    else{     //if the card isn't read
      Serial.println("Access denied");    //print that they are denied access
      tone(buzzerPin, 250, 250);
      delay(1000);

    }
    delay (500);     //delay 1 seconds for the rfid to catch up
  }

}
  bool getID(){                 //Define the getID function (which I stole from the dumpinfo sketch :))
    if ( ! mfrc522.PICC_IsNewCardPresent()) {     //if there's no card detected
		return false;     //exit
	}

	if ( ! mfrc522.PICC_ReadCardSerial()) {       //if there's no serial number read
		return false;     //exit
	}
  
  myScore = "";     //clear the variable from previous scans

  for (byte i = 0; i < mfrc522.uid.size; i++) {                  // I want to store the UID of the card
        myScore = myScore + String(mfrc522.uid.uidByte[i], HEX);
      }
  return true;
  }
