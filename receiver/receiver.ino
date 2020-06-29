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
unsigned char encryptkey[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; // The very secret key

// setting up phys buttons
struct
{
  int8_t pin; // Button is wired between this pin and GND
  uint8_t key;
  bool prevState;
  uint32_t lastChangeTime;
} button[] = {
    {A1, 32},  // Button 1 Space
    {A2, 'j'}, // Button 2 next (j)
               //{ 11, h },       // Button 3 previous (h)
};
#define N_BUTTONS (sizeof(button) / sizeof(button[0]))
#define DEBOUNCE_US 600000 // Button debounce time, microseconds
uint8_t state = 0;

void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  delay(100);

  // manual reset of lora module
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init())
  {
    while (1)
      ;
  }

  if (!rf95.setFrequency(RF95_FREQ))
  {
    while (1)
      ;
  }

  rf95.setTxPower(23, false);

  myCipher.setKey(encryptkey, sizeof(encryptkey));

  delay(1000);

  Keyboard.begin();

  //setup for phys buttons
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);

  uint8_t i;

  for (i = 0; i < N_BUTTONS; i++)
  {
    pinMode(button[i].pin, INPUT_PULLUP);
    button[i].prevState = digitalRead(button[i].pin);
    button[i].lastChangeTime = micros();
  }
}

void loop()
{
  if (myDriver.available())
  {
    uint8_t buf[myDriver.maxMessageLength()];
    uint8_t len = sizeof(buf);
    if (myDriver.recv(buf, &len))
    {
      Keyboard.write(buf[0]);
    }
  }
  uint32_t t;
  bool s;
  int i, value, dx, dy;
  // float a;
  // uint16_t *buf;

  for (i = 0; i < N_BUTTONS; i++)
  {
    s = digitalRead(button[i].pin);
    if (s != button[i].prevState)
    {
      t = micros();
      if ((t - button[i].lastChangeTime) >= DEBOUNCE_US)
      {
        if (s)
        {
        }
        else
        {
          uint8_t data[1] = {0};
          data[0] = button[i].key;
          Keyboard.write(data[0]);
        }
        button[i].prevState = s;
        button[i].lastChangeTime = micros();
      }
    }
  }
}
