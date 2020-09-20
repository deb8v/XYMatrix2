#include <FastLED.h>

#define LED_PIN  D4

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define BRIGHTNESS 64

const uint8_t kMatrixWidth = 16;
const uint8_t kMatrixHeight = 16;

const bool    kMatrixSerpentineLayout = true;

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }
  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      i = (y * kMatrixWidth) + x;
    }
  }
  
  return i;
}
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

uint16_t XYsafe( uint8_t x, uint8_t y)
{
  if( x >= kMatrixWidth) return -1;
  if( y >= kMatrixHeight) return -1;
  return XY(x,y);
}


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "4.18b";
const uint16_t port = 80;

ESP8266WiFiMulti WiFiMulti;

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
  delay(500);
}

char last,mode,numbas=0;
char pktlen=6;
byte buffer[5]={0,0,0,0,0};

void loop() {
  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
    delay(5000);
    return;
  }
  client.println("0x0");
  Serial.println(">>>");

  while (client.connected())
    {
      if (client.available()){
        
      last=client.read();
      if(last=='|'){FastLED.show();}
      if(last=='{')client.readBytesUntil('}', buffer, pktlen);  
      leds[ XY(buffer[0], buffer[1])]  = CRGB(buffer[2], buffer[3], buffer[4]);
      /*
      for(int i=0;i<=4;i++){
            Serial.print(buffer[i],DEC);
            Serial.print(',');
          
          }Serial.println();
      */
    }
    }
  
  

  Serial.println("closing connection");
  client.stop();

  Serial.println("wait 0.5 sec...");
  delay(500);
}

