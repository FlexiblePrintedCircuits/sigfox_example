#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_MMA8451.h>
#include <SIGFOX.h>

#define TRG 0
#define ECH 1 
#define HIGHTIME 10

Adafruit_BME280 bme;
Adafruit_MMA8451 mma = Adafruit_MMA8451();

// IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.
static const String device = "";  //  Set this to your device name if you're using UnaBiz Emulator.
static const bool useEmulator = false;  //  Set to true if using UnaBiz Emulator.
static const bool echo = true;  //  Set to true if the SIGFOX library should display the executed commands.
static const Country country = COUNTRY_JP;  //  Set this to your country to configure the SIGFOX transmission frequencies.
static UnaShieldV2S transceiver(country, useEmulator, device, echo);  //  Uncomment this for UnaBiz UnaShield V2S Dev Kit
static String response;  //  Will store the downlink response from SIGFOX.
// IMPORTANT: Check these settings with UnaBiz to use the SIGFOX library correctly.

void setup() {
  Serial.begin(9600);
  if (!bme.begin(0x76)) stop("BME280 is missing.");
  if (!mma.begin(0x1c)) stop("MMA8451 is missing.");
  mma.setRange(MMA8451_RANGE_2_G);
  if (!transceiver.begin()) stop("Unable to init SIGFOX module.");  //Run SigFox module
  pinMode(TRG, OUTPUT);
  pinMode(ECH, INPUT);
}

void loop() {
  
  sensors_event_t event; 
  mma.getEvent(&event);
  static int counter = 0;

  int diff;
  float dis;
  digitalWrite(TRG, HIGH);
  delayMicroseconds(HIGHTIME);
  digitalWrite(TRG, LOW);
  diff = pulseIn(ECH, HIGH);
  dis = (float)diff * 0.01715;
  Serial.print(dis);
  Serial.println("cm");
  delay(1000);

  word sDis = dis * 1.0;

  Serial.print("sDis=");  Serial.println(sDis);

  // format:
  //  temp::int:16:little-endian humid::int:16:little-endian \
  //  press::int:16:little-endian x::int:16:little-endian \
  //  y::int:16:little-endian z::int:16:little-endian
  String msg = transceiver.toHex(sDis);
  Serial.println(msg);
  if (!transceiver.sendMessage(msg))  Serial.println("Send failed.");
  counter++;
  Serial.println("Waiting 15 minutes...");  delay(900000);
}
