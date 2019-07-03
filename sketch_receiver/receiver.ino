#include <RH_RF95.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>
#include <Keyboard.h>


#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

#define RF95_FREQ 868.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

Speck myCipher;   
RHEncryptedDriver myDriver(rf95, myCipher); 

float frequency = 868.0; 
unsigned char encryptkey[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}; // The very secret key


void setup() {
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  delay(100);

  // manual reset of lora module
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    while (1);
  }

  
  if (!rf95.setFrequency(RF95_FREQ)) {
    while (1);
  }
  
  rf95.setTxPower(23, false);
  
  myCipher.setKey(encryptkey, sizeof(encryptkey));
  
  delay(1000);
  
   Keyboard.begin();
}

void loop() {
  if (myDriver.available()) {
  
    uint8_t buf[myDriver.maxMessageLength()];
    uint8_t len = sizeof(buf);
    if (myDriver.recv(buf, &len)) {  
      Keyboard.write(buf[0]);
  
    }
  }
}
