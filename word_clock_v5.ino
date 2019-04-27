#define FASTLED_ESP8266_D1_PIN_ORDER
#include "FastLED.h"
#include "TimeLib.h"
#include <SoftwareSerial.h>
#include "ctype.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <math.h>

// DEBUG FLAGS

// increase clock speed to see transitions faster
bool debugClockSpeed = false;

// WIFI SETUP

const char *ssid = "WLAN-124257";
const char *password = "97325530254611032945";

WiFiUDP ntpUDP;

ESP8266WebServer webServer(80);
WiFiServer server(80);
IPAddress ip(192, 168, 0, 99);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);

NTPClient timeClient(ntpUDP, "0.europe.pool.ntp.org", 3600, 60000);

#define NUM_LEDS 228
#define NUM_LETTERS 110
#define brightnessSensor A0

#define DATA_PIN 6

CRGB leds[NUM_LEDS];
CRGB leds_copy[NUM_LEDS];

//Save the offset of the first letter from 0

//Length 1:
#define OBENLINKS 110
#define OBENRECHTS 111
#define UNTENRECHTS 112
#define UNTENLINKS 113

//Length 2:
#define ES 0

//Length 3:
#define VOR 33
#define EIN 55
#define ELF 49
#define UHR 107
#define IST 3

//Length 4:
#define FUENFMINUTEN 7
#define ZEHNMINUTEN 11
#define FUENFSTUNDEN 51
#define ZEHNSTUNDEN 99
#define HALB 44
#define NACH 40
#define EINS 55
#define ZWEI 62
#define DREI 66
#define VIER 73
#define ACHT 84
#define NEUN 102

//Length 5:
#define SECHS 77
#define ZWOELF 94

//Length 6:
#define SIEBEN 88

//Length 7:
#define ZWANZIG 15
#define VIERTEL 26

uint8_t ledTransformationMatrix[114][2] = {
    //First Row (Top Row)
    {219, 218},
    {217, 216},
    {215, 214},
    {213, 212},
    {211, 210},
    {209, 208},
    {207, 206},
    {205, 204},
    {203, 202},
    {201, 200},
    {199, 198},
    // {204,205},{206,207},{208,209},{210,211},{212,213},{214,215},{216,217},{218,219},{220,221},{222,223},{224,225}, values from v4

    //Second Row
    {176, 177},
    {178, 179},
    {180, 181},
    {182, 183},
    {184, 185},
    {186, 187},
    {188, 189},
    {190, 191},
    {192, 193},
    {194, 195},
    {196, 197},
    // {200,201},{198,199},{196,197},{194,195},{192,193},{190,191},{188,189},{186,187},{184,185},{182,183},{180,181},

    //Third Row
    {175, 174},
    {173, 172},
    {171, 170},
    {169, 168},
    {167, 166},
    {165, 164},
    {163, 162},
    {161, 160},
    {159, 158},
    {157, 156},
    {155, 154},
    // {158,159},{160,161},{162,163},{164,165},{166,167},{168,169},{170,171},{172,173},{174,175},{176,177},{178,179},

    //Fourth Row
    {132, 133},
    {134, 135},
    {136, 137},
    {138, 139},
    {140, 141},
    {142, 143},
    {144, 145},
    {146, 147},
    {148, 149},
    {150, 151},
    {152, 153},
    // {156,157},{154,155},{152,153},{150,151},{148,149},{146,147},{144,145},{142,143},{140,141},{138,139},{136,137},

    //Fifth Row
    {131, 130},
    {129, 128},
    {127, 126},
    {125, 124},
    {123, 122},
    {121, 120},
    {119, 118},
    {117, 116},
    {115, 114},
    {113, 112},
    {111, 110},
    // {114,115},{116,117},{118,119},{120,121},{122,123},{124,125},{126,127},{128,129},{130,131},{132,133},{134,135},

    //Sixth Row
    {88, 89},
    {90, 91},
    {92, 93},
    {94, 95},
    {96, 97},
    {98, 99},
    {100, 101},
    {102, 103},
    {104, 105},
    {106, 107},
    {108, 109},
    // {112,113},{110,111},{108,109},{106,107},{104,105},{102,103},{100,101},{98,99},{96,97},{94,95},{92,93},

    //Seventh Row
    {87, 86},
    {85, 84},
    {83, 82},
    {81, 80},
    {79, 78},
    {77, 76},
    {75, 74},
    {73, 72},
    {71, 70},
    {69, 68},
    {67, 66},
    // {70,71},{72,73},{74,75},{76,77},{78,79},{80,81},{82,83},{84,85},{86,87},{88,89},{90,91},

    //Eight Row
    {44, 45},
    {46, 47},
    {48, 49},
    {50, 51},
    {52, 53},
    {54, 55},
    {56, 57},
    {58, 59},
    {60, 61},
    {62, 63},
    {64, 65},
    // {68,69},{66,67},{64,65},{62,63},{60,61},{58,59},{56,57},{54,55},{52,53},{50,51},{48,49},

    //Ninth Row
    {43, 42},
    {41, 40},
    {39, 38},
    {37, 36},
    {35, 34},
    {33, 32},
    {31, 30},
    {29, 28},
    {27, 26},
    {25, 24},
    {23, 22},
    // {26,27},{28,29},{30,31},{32,33},{34,35},{36,37},{38,39},{40,41},{42,43},{44,45},{46,47},

    //Tenth Row (Bottom Row)
    {0, 1},
    {2, 3},
    {4, 5},
    {6, 7},
    {8, 9},
    {10, 11},
    {12, 13},
    {14, 15},
    {16, 17},
    {18, 19},
    {20, 21},
    // {22,23},{20,21},{18,19},{16,17},{14,15},{12,13},{10,11},{8,9},{6,7},{4,5},{2,3},

    //Upper Left Corner
    {220, 221},
    // {202,203},

    //Upper Right Corner
    {222, 223},
    // {226,227},

    //Lower Right Corner
    {224, 225},
    // {0,1},

    //Lower Left Corner
    {226, 227}
    // {24,25}

};

uint8_t brightness = 130;
uint8_t oldBrightness = 0;
int brightnessTimesTen = 0;
bool setBrightness = false;
uint8_t globalR = 0;
uint8_t globalG = 255;
uint8_t globalB = 128;
uint8_t minBrightness = 10;
uint8_t maxBrightness = 200;
uint32_t brightnessAcc = 0;
bool colorGradient = false;
bool randomColorsEnabled = false;
uint8_t n_randomColors = 19;
uint8_t edgeR = 255;
uint8_t edgeG = 255;
uint8_t edgeB = 255;

uint8_t oldMinute = 61;

bool keepTransientColors = true;

//Random Colors based on Googles Material Design
uint8_t randomColors[19][3] = {
    {244, 67, 54},   //#ed8880 Soft Red
    {233, 30, 99},   //#ff96cf Rosa
    {156, 39, 176},  //#be9ad1 Flieder
    {130, 120, 163}, //#a8a8a8 Lila-Grau
    {107, 153, 255}, //#6b99ff Blau
    {95, 227, 244},  //#5fe3f4 Türkis
    {211, 250, 255}, //#d3faff Hellblau
    {255, 255, 255}, //#ffffff Weiß
    {45, 76, 50},    //#2d4c32 Dunkelgrün
    {0, 150, 136},   //#009688 Seegrün
    {76, 175, 80},   //#4CAF50 Olive
    {139, 195, 74},  //#8BC34A Limone
    {205, 220, 57},  //#CDDC39 Gelb
    {255, 235, 59},  //#FFEB3B Gold
    {255, 193, 7},   //#FFC107 Orange
    {255, 152, 0},   //#FF9800 Orangerot
    {121, 85, 72},   //#795548 Braun
    {168, 168, 168}, //#a8a8a8 Grau
    {252, 196, 179}  //#user - can be customized by user
};
uint8_t selectedRandomColors[19][3] = {0, 0, 0};
uint8_t randomColorCounter = 0;
bool randomColorsUsed[19] = {true};
uint8_t fixedColorValues[3][3] = {
    {255, 255, 255}, {255, 255, 255}, {255, 255, 255}};

//Event Management
// 0 = show Time
// 1 = show effect
uint8_t state = 0;
bool ledState[NUM_LEDS];

//state Managment for communication
bool multipleColorSelectionEnabled = false;
bool setHighlightColor = false;
bool setLetterColor = false;
bool setR = false;
bool setG = false;
bool setB = false;
bool fourColorSelectionEnabled = false;
uint8_t letterColorCounter = 0;
bool setLetterColor1 = false;
bool setLetterColor2 = false;
bool setLetterColor3 = false;

bool summerTime = false;

#if defined(ESP8266)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

int counter = 0;
int relay = 0;
char HTML_String[5000];

int redVal = 10;
int redTemp = 15;
int bufLength;
char colorBuff[4];

// local clock variables
unsigned long local_milliseconds = 0;
struct
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    String weekday;
    uint8_t month;
    uint16_t year;
} currentTime;

bool effectShown = false;

// boot flag for first boot
bool bootup = false;

void setup()
{

    Serial.begin(112500);
    Serial.println();
    Serial.print("Startup reason:");
    Serial.println(ESP.getResetReason());

    Serial.println("Enter Setup");
    initWifi();
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);

    FastLED.setBrightness(analogRead(brightnessSensor) / 4);

    // Summer / Winter time button
    pinMode(D8, INPUT);

    //Create new random numbers with each restart
    randomSeed(millis());

    // set initial brightness value
    for (int i = 0; i < 100; i++)
    {
        brightnessAcc += analogRead(brightnessSensor);
    }

    brightness = brightnessAcc / 100;

    // do stuff once in the loop
    bootup = true;


        timeClient.update();
        Serial.println(" -- Time Updated from Clock Server -- ");
        Serial.println(timeClient.getFormattedTime());

        if (!summerTime)
        {
            currentTime.hour = timeClient.getHours();
        }
        else
        {
            currentTime.hour = timeClient.getHours() + 1;
        }

        currentTime.minute = timeClient.getMinutes();
        currentTime.second = timeClient.getSeconds();
        currentTime.day = timeClient.getDay();


        updateClockState();
        FastLED.setBrightness(brightness);
        FastLED.show();

        // color transient
        showEffects(1);
        

    Serial.println("Setup Complete");
}

void loop()
{

   

    if (digitalRead(D8))
    {
        while (digitalRead(D8))
            ;
        //toggle summertime
        summerTime = !summerTime;
        if (summerTime)
        {
            currentTime.hour++;
        }
        else
        {
            currentTime.hour--;
        }
        Serial.println("Summer Time toggled");
        updateClockState();
    }

    webServer.handleClient();
    webServer.send(200, "text/html", getPage());

    if (counter % 10000 == 0)
    {
        timeClient.update();
        Serial.println(" -- Time Updated from Clock Server -- ");
        Serial.println(timeClient.getFormattedTime());

        if (!summerTime)
        {
            currentTime.hour = timeClient.getHours();
        }
        else
        {
            currentTime.hour = timeClient.getHours() + 1;
        }

        currentTime.minute = timeClient.getMinutes();
        currentTime.second = timeClient.getSeconds();
        currentTime.day = timeClient.getDay();
    }
    else
    {
        if (millis() - local_milliseconds >= 1000)
        {
            local_milliseconds = millis();
            currentTime.second++;
        }

        if (currentTime.second > 59)
        {
            currentTime.minute++;
            currentTime.second = 0;
        }
        if (currentTime.minute > 59)
        {
            currentTime.minute = 0;
            currentTime.hour++;
        }
        if (currentTime.hour > 23)
        {
            currentTime.hour = 0;
        }
    }

    //print time
    if (counter % 100 == 0)
    {
        Serial.println(timeClient.getFormattedTime());
    }

    //show effect every 15 minutes

    if (currentTime.minute % 15 == 0 && !effectShown)
    {
      //color transient
        showEffects(1);
    }
    else if(currentTime.minute % 5 == 0 && !effectShown)
    {
      //fade in and out
      showEffects(2);
    }
   
    if (currentTime.minute % 15 != 0 && currentTime.minute %5 != 0)
    {
        effectShown = false;
    }
   

    //show other effect every 5 minutes

    if (oldMinute != currentTime.minute && currentTime.minute != 0)
    {
        updateClockState();
        FastLED.show();
        oldMinute = currentTime.minute;
    }

    // BRIGHTNESS MANAGEMENT
    if (counter % 5 == 0)
    {
        brightness = analogRead(brightnessSensor);

        // Serial.print("current Sensor value : ");
        // Serial.print(brightness);

        //Lowpass filtering

        brightnessAcc = 0;
        for (int i = 0; i < 200; i++)
        {
            brightnessAcc += analogRead(brightnessSensor);
        }

        brightness = brightnessAcc / 200;


        if (brightness <= 15)
        {
            brightness = 15;
        }
        if (brightness > maxBrightness)
        {
            brightness = maxBrightness;
        }

        if (brightness > oldBrightness + 3)
        {
            oldBrightness++;
        }
        if (brightness < oldBrightness - 3)
        {
            oldBrightness--;
        }

        FastLED.setBrightness(oldBrightness);
    }

    FastLED.show();

    delay(3); // prevent wifi disconnect

    counter++;
  
  // increase speed of clock for debugging
  if(debugClockSpeed)
  {
    if (counter % 2 == 0 || true)
    {
        currentTime.second++;
    }
  }

  
}

void showEffects(int i)
{
    effectShown = true;
   
    //randomly select effect
    int NUM_EFFECTS = 2;
    int r = random(NUM_EFFECTS);

    switch (i)
    {

    case 0:
        dropTheLetters();
        enterTheMatrix(100);
        fadeOut(300);
        updateClockState();
        fadeIn();
        break;
    case 1:
        colorTransientOn();
        saveLEDs();
        colorTransientOff();
        if (!keepTransientColors)
        {
            delay(1000);
            fadeOut(300);
            FastLED.clear();
            updateClockState();
            FastLED.show();
            fadeIn();
        }
        Serial.println("update clock state after effects");
        updateClockState();
        break;
      case 2:
        fadeInAndOut();
    }
}

void fadeInAndOut()
{
    bool oldClockState[114] = {false};
    bool newClockState[114] = {false};

    CRGB local_led_copy_old[114];

     for (int i = 0; i < 114; i++)
    {
     local_led_copy_old[i].r = 0;
     local_led_copy_old[i].g = 0;
     local_led_copy_old[i].b = 0;
    }
    
    for (int i = 0; i < 114; i++)
    {
        if (leds[ledTransformationMatrix[i][0]].r != 0 || leds[ledTransformationMatrix[i][0]].g != 0 || leds[ledTransformationMatrix[i][0]].b != 0)
        {
            oldClockState[i] = true;

            local_led_copy_old[i].r = leds[ledTransformationMatrix[i][0]].r ;
            local_led_copy_old[i].g = leds[ledTransformationMatrix[i][0]].g ;
            local_led_copy_old[i].b = leds[ledTransformationMatrix[i][0]].b ;
        }
    }
   
    updateClockState();

    CRGB local_led_copy[114] = {0};
 
    for (int i = 0; i < 114; i++)
    {
        if (leds[ledTransformationMatrix[i][0]].r != 0 || leds[ledTransformationMatrix[i][0]].g != 0 || leds[ledTransformationMatrix[i][0]].b != 0)
        {
            newClockState[i] = true;
            
            local_led_copy[i].r = leds[ledTransformationMatrix[i][0]].r ;
            local_led_copy[i].g = leds[ledTransformationMatrix[i][0]].g ;
            local_led_copy[i].b = leds[ledTransformationMatrix[i][0]].b ;
        }
    }

    for (int i = 0; i < 114; i++)
    {

        leds[ledTransformationMatrix[i][0]].r = local_led_copy_old[i].r;
        leds[ledTransformationMatrix[i][0]].g = local_led_copy_old[i].g;
        leds[ledTransformationMatrix[i][0]].b = local_led_copy_old[i].b;
    
    }


    float fadeFactorDec = 0.95;
    float fadeFactorInc = 1/fadeFactorDec;

    for(int k = 0;k<ceil(log(brightness)/log(fadeFactorInc));k++)
    {
      for (int i = 0; i < 114; i++)
      {
          if(oldClockState[i] && !newClockState[i])
          {
            uint8_t r = leds[ledTransformationMatrix[i][0]].r;
            uint8_t g = leds[ledTransformationMatrix[i][0]].g;
            uint8_t b = leds[ledTransformationMatrix[i][0]].b;
            
            if(r != 0)
            {
              leds[ledTransformationMatrix[i][0]].r = floor(fadeFactorDec*r);
              leds[ledTransformationMatrix[i][1]].r = floor(fadeFactorDec*r);
            }

            if(g != 0)
            {
              leds[ledTransformationMatrix[i][0]].g = floor(fadeFactorDec*g);
              leds[ledTransformationMatrix[i][1]].g = floor(fadeFactorDec*g);
            }

            if(b != 0)
            {
              leds[ledTransformationMatrix[i][0]].b = floor(fadeFactorDec*b);
              leds[ledTransformationMatrix[i][1]].b = floor(fadeFactorDec*b);
            }
            
          }
          
          if(newClockState[i] && !oldClockState[i])
          {

            if(ceil(leds[ledTransformationMatrix[i][0]].r*fadeFactorInc) >= local_led_copy[i].r)
            {
              leds[ledTransformationMatrix[i][0]].r = local_led_copy[i].r;
              leds[ledTransformationMatrix[i][1]].r = local_led_copy[i].r;
            }
            else
            {
              leds[ledTransformationMatrix[i][0]].r = ceil(leds[ledTransformationMatrix[i][0]].r*fadeFactorInc);
              leds[ledTransformationMatrix[i][1]].r = ceil(leds[ledTransformationMatrix[i][0]].r*fadeFactorInc);
            }

            if(ceil(leds[ledTransformationMatrix[i][0]].r*fadeFactorInc) == 0)
            {
              leds[ledTransformationMatrix[i][0]].r = 2;
              leds[ledTransformationMatrix[i][1]].r = 2;
            }

            if(leds[ledTransformationMatrix[i][0]].g*fadeFactorInc >= local_led_copy[i].g)
            {
              leds[ledTransformationMatrix[i][0]].g = local_led_copy[i].g;
              leds[ledTransformationMatrix[i][1]].g = local_led_copy[i].g;
            }
            else
            {
              leds[ledTransformationMatrix[i][0]].g = ceil(leds[ledTransformationMatrix[i][0]].g*fadeFactorInc);
              leds[ledTransformationMatrix[i][1]].g = ceil(leds[ledTransformationMatrix[i][0]].g*fadeFactorInc);
            }

            
            if(ceil(leds[ledTransformationMatrix[i][0]].g*fadeFactorInc) == 0)
            {
              leds[ledTransformationMatrix[i][0]].g = 2;
              leds[ledTransformationMatrix[i][1]].g = 2;
            }

            if(leds[ledTransformationMatrix[i][0]].b*fadeFactorInc >= local_led_copy[i].b)
            {
              leds[ledTransformationMatrix[i][0]].b = local_led_copy[i].b;
              leds[ledTransformationMatrix[i][1]].b = local_led_copy[i].b;
            }
            else
            {
              leds[ledTransformationMatrix[i][0]].b = ceil(leds[ledTransformationMatrix[i][0]].b*fadeFactorInc);
              leds[ledTransformationMatrix[i][1]].b = ceil(leds[ledTransformationMatrix[i][0]].b*fadeFactorInc);
            }

            
            if(ceil(leds[ledTransformationMatrix[i][0]].b*fadeFactorInc) == 0)
            {
              leds[ledTransformationMatrix[i][0]].b = 2;
              leds[ledTransformationMatrix[i][1]].b = 2;
            }

          }
          
      }
      FastLED.show();
      delay(30);
    }
}

void enterTheMatrix(int time)
{

    bool oldClockState[110] = {false};
    saveLEDs();
    updateClockState();

    for (int i = 0; i < 110; i++)
    {
        if (leds[ledTransformationMatrix[i][0]].r != 0 || leds[ledTransformationMatrix[i][0]].g != 0 || leds[ledTransformationMatrix[i][0]].b != 0)
        {
            oldClockState[i] = true;
        }
    }

    int r = random(11);
    int r1 = random(11);
    uint8_t maxMatrixLength = 7;
    float fadingFactor = 1.1;
    //do this a while
    for (int t = 0; t < time; t++)
    {
        for (int tt = 0; tt < 3; tt++)
        {
            if (leds[ledTransformationMatrix[r][0]].r == 0 && leds[ledTransformationMatrix[r][0]].g == 0 && leds[ledTransformationMatrix[r][0]].b == 0)
            {
                leds[ledTransformationMatrix[r][0]].setRGB(0, 180, 17);
                leds[ledTransformationMatrix[r][1]].setRGB(0, 180, 17);
                //FastLED.show();
            }
            else
            {
                setLetter(r, 0, 0, 0);
            }
            r = random(11);
        }

        // color fading
        for (int a = 0; a < 11; a++)
        {
            int tempCounter = 0;
            for (int b = 11; b <= 109; b += 11)
            {
                if (leds[ledTransformationMatrix[(b + a) - 11][0]].g != 0)
                {
                    if (leds[ledTransformationMatrix[(b + a)][0]].g != 0)
                    {
                        leds[ledTransformationMatrix[(b + a) - 11][0]].setRGB(leds[ledTransformationMatrix[(b + a)][0]].r / fadingFactor, leds[ledTransformationMatrix[(b + a)][0]].g / fadingFactor, leds[ledTransformationMatrix[(b + a)][0]].b / fadingFactor);
                        leds[ledTransformationMatrix[(b + a) - 11][1]].setRGB(leds[ledTransformationMatrix[(b + a)][0]].r / fadingFactor, leds[ledTransformationMatrix[(b + a)][0]].g / fadingFactor, leds[ledTransformationMatrix[(b + a)][0]].b / fadingFactor);
                    }
                }
            }
        }

        for (int a = 0; a < 11; a++)
        {
            int tempCounter = 0;
            for (int b = 0; b <= 109; b += 11)
            {
                if (leds[ledTransformationMatrix[b + a][0]].g != 0)
                {
                    tempCounter++;
                    if (tempCounter > maxMatrixLength)
                    {
                        setLetter(a, 0, 0, 0);
                    }
                }
            }
        }

        r = random(11);
        r1 = random(11);

        for (int i = 109; i >= 11; i--)
        {
            leds[ledTransformationMatrix[i][0]] = leds[ledTransformationMatrix[(i - 11)][0]];
            leds[ledTransformationMatrix[i][1]] = leds[ledTransformationMatrix[(i - 11)][1]];
        }

        FastLED.show();
        delay(80);
    }

    for (int i = 0; i < 10; i++)
    {
        moveOneRowDown();
        delay(80);
        FastLED.show();
    }
}

void colorTransientOn()
{

  fadeOut(1000);
  FastLED.clear();

  uint8_t temp_r1, temp_g1, temp_b1, temp_r2, temp_g2, temp_b2;
  FastLED.setBrightness(oldBrightness);
    
  // Algorithm to ensure different colors for the gradient

  // higher -> less similar (max = 255);
  int minDelta = 70;
  
  int timestamp = millis();

  float delta_e = 0;
  
  while(delta_e < minDelta)
  {

    temp_r1 = random(256);
    temp_g1 = random(256);
    temp_b1 = random(256);

    temp_r2 = random(256);
    temp_g2 = random(256);
    temp_b2 = random(256);
    
    float var_R1 = ( (float) temp_r1 / 255 );
    float var_G1 = ( (float) temp_g1 / 255 );
    float var_B1 = ( (float) temp_b1 / 255 );
    
    float var_R2 = ( (float) temp_r2 / 255 );
    float var_G2 = ( (float) temp_g2 / 255 );
    float var_B2 = ( (float) temp_b2 / 255 );
    
    
    
    
    if ( var_R1 > 0.04045 ) var_R1 = pow(( ( var_R1 + 0.055 ) / 1.055 ), 2.4);
    else                   var_R1 = var_R1 / 12.92;
    if ( var_G1 > 0.04045 ) var_G1 = pow(( ( var_G1 + 0.055 ) / 1.055 ) , 2.4);
    else                   var_G1 = var_G1 / 12.92;
    if ( var_B1 > 0.04045 ) var_B1 = pow(( ( var_B1 + 0.055 ) / 1.055 ) , 2.4);
    else                   var_B1 = var_B1 / 12.92;
    
    if ( var_R2 > 0.04045 ) var_R2 = pow(( ( var_R2 + 0.055 ) / 1.055 ) , 2.4);
    else                   var_R2 = var_R2 / 12.92;
    if ( var_G2 > 0.04045 ) var_G2 = pow(( ( var_G2 + 0.055 ) / 1.055 ) , 2.4);
    else                   var_G2 = var_G2 / 12.92;
    if ( var_B2 > 0.04045 ) var_B2 = pow(( ( var_B2 + 0.055 ) / 1.055 ) , 2.4);
    else                   var_B2 = var_B2 / 12.92;
    
    
    // save computation time as scaling factor is 100 later on
    /*
    var_R1 = var_R1 * 100;
    var_G1 = var_G1 * 100;
    var_B1 = var_B1 * 100;
    
    var_R2 = var_R2 * 100;
    var_G2 = var_G2 * 100;
    var_B2 = var_B2 * 100;
    
    */
    float var_X1 = var_R1 * 0.4124 + var_G1 * 0.3576 + var_B1 * 0.1805;
    float var_Y1 = var_R1 * 0.2126 + var_G1 * 0.7152 + var_B1 * 0.0722;
    float var_Z1 = var_R1 * 0.0193 + var_G1 * 0.1192 + var_B1 * 0.9505;
    
    
    float var_X2 = var_R2 * 0.4124 + var_G2 * 0.3576 + var_B2 * 0.1805;
    float var_Y2 = var_R2 * 0.2126 + var_G2 * 0.7152 + var_B2 * 0.0722;
    float var_Z2 = var_R2 * 0.0193 + var_G2 * 0.1192 + var_B2 * 0.9505;
    
    
    
    // save computation time as scaling factor is 100
    /*
    var_X1 = X1 / 100;
    var_Y1 = Y1 / 100;
    var_Z1 = Z1 / 100;
    
    var_X2 = X2 / 100;
    var_Y2 = Y2 / 100;
    var_Z2 = Z2 / 100;
    */
    
    
    
    if ( var_X1 > 0.008856 ) var_X1 = pow(var_X1 , ( 1.0/3.0 ));
    else                    var_X1 = ( 7.787 * var_X1 ) + ( 16.0 / 116.0 );
    if ( var_Y1 > 0.008856 ) var_Y1 = pow(var_Y1 , ( 1.0/3.0 ));
    else                    var_Y1 = ( 7.787 * var_Y1 ) + ( 16.0 / 116.0 );
    if ( var_Z1 > 0.008856 ) var_Z1 = pow(var_Z1 , ( 1.0/3.0 ));
    else                    var_Z1 = ( 7.787 * var_Z1 ) + ( 16.0 / 116.0 );
    
    if ( var_X2 > 0.008856 ) var_X2 = pow(var_X2 , ( 1.0/3.0 ));
    else                    var_X2 = ( 7.787 * var_X2 ) + ( 16.0 / 116.0 );
    if ( var_Y2 > 0.008856 ) var_Y2 = pow(var_Y2 , ( 1.0/3.0 ));
    else                    var_Y2 = ( 7.787 * var_Y2 ) + ( 16.0 / 116.0 );
    if ( var_Z2 > 0.008856 ) var_Z2 = pow(var_Z2 , ( 1.0/3.0 ));
    else                    var_Z2 = ( 7.787 * var_Z2 ) + ( 16.0 / 116.0 );
    
    
    Serial.print("X1: ");
    Serial.print(var_X1);
    
    
    Serial.print(" Y1: ");
    Serial.print(var_Y1);
    
    
    Serial.print(" Z1: ");
    Serial.println(var_Z1);
    
    
    float CIE_L1 = ( 116 * var_Y1 ) - 16;
    float CIE_a1 = 500 * ( var_X1 - var_Y1 );
    float CIE_b1 = 200 * ( var_Y1 - var_Z1 );
    
    float CIE_L2 = ( 116 * var_Y2 ) - 16;
    float CIE_a2 = 500 * ( var_X2 - var_Y2 );
    float CIE_b2 = 200 * ( var_Y2 - var_Z2 );
    
    
    delta_e = sqrt( ( pow(( CIE_L1 - CIE_L2 ) , 2) )
                   + ( pow(( CIE_a1 - CIE_a2 ) , 2) )
                   + ( pow(( CIE_b1 - CIE_b2 ) , 2) ) );
    
  }
    
  Serial.print("RESULTING DELTA: ");
  Serial.println(delta_e);
    
  Serial.print("Choosing color took ");
  Serial.print((millis()-timestamp));
  Serial.println(" ms");




    // run once to save colors
    // rows
    for (int a = 0; a < 20; a++)
    {
        for (int i = a; i <= 11 * a; i += 10)
        {

            // set all others
            if (i < 110)
            {

                uint8_t gradient_r = (uint8_t)(temp_r1 * i / 109.0) + (uint8_t)(temp_r2 * (109.0 - i) / 109.0);
                uint8_t gradient_g = (uint8_t)(temp_g1 * i / 109.0) + (uint8_t)(temp_g2 * (109.0 - i) / 109.0);
                uint8_t gradient_b = (uint8_t)(temp_b1 * i / 109.0) + (uint8_t)(temp_b2 * (109.0 - i) / 109.0);

                leds[ledTransformationMatrix[i][0]].setRGB(gradient_r, gradient_g, gradient_b);
                leds[ledTransformationMatrix[i][1]].setRGB(gradient_r, gradient_g, gradient_b);
            }
        }
    }

    CRGB local_led_copy[NUM_LEDS];

    // save current clock state locally
    for (int i = 0; i < NUM_LEDS; i++)
    {
        local_led_copy[i].r = leds_copy[i].r;
        local_led_copy[i].g = leds_copy[i].g;
        local_led_copy[i].b = leds_copy[i].b;
    }

    // save created gradient
    saveLEDs();

    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i].r = local_led_copy[i].r;
        leds[i].g = local_led_copy[i].g;
        leds[i].b = local_led_copy[i].b;
    }

    // With every iteration the value of the led is set to max(2*<previous_value>,<gradient_value>)
    // This exponential increase in brightness is perceived as a linear increase by the human eye

    // run algorithm 20 times regularly + 8 times to achieve maximum brightness of 2^8=256

    // get required runs for max value = log_b_factor(256)

    // even for the last two leds

    //============================  TIMING SETTINGS =========================================

    uint8_t start_value_led = 5;//oldBrightness / 4;

    // scaling factor for brightness (lower -> slower transition)
    float b_factor = 1.35;

    // speed of the animation (delay -> higher is slower) [ms]
    uint16_t delayPerRound = 70;

    // keep the transtion after it is done [ms]
    uint16_t delayAfterTransitionDone = 550;

    //============================ END TIMING SETTINGS =========================================

  FastLED.clear();

    for (int ii = 0; ii < NUM_LEDS; ii++)
    {
        local_led_copy[ii].r = start_value_led;
        local_led_copy[ii].g = start_value_led;
        local_led_copy[ii].b = start_value_led;
    }

    for (int k = 0; k < 20+30/b_factor ; k++)
    {

        for (int a = 0; a < k; a++)
        {
            // rows
            for (int i = a; i <= 11 * a; i += 10)
            {

                // set all others
                if (i < 110)
                {
                  
                // retrieve previously calculated values
                uint8_t gradient_r = leds_copy[ledTransformationMatrix[i][0]].r;
                uint8_t gradient_g = leds_copy[ledTransformationMatrix[i][0]].g;
                uint8_t gradient_b = leds_copy[ledTransformationMatrix[i][0]].b;

                // calculate new values
                uint8_t gradient_r_new = local_led_copy[ledTransformationMatrix[i][0]].r * b_factor < gradient_r ? local_led_copy[ledTransformationMatrix[i][0]].r * b_factor : gradient_r;
                uint8_t gradient_g_new = local_led_copy[ledTransformationMatrix[i][0]].g * b_factor < gradient_g ? local_led_copy[ledTransformationMatrix[i][0]].g * b_factor : gradient_g;
                uint8_t gradient_b_new = local_led_copy[ledTransformationMatrix[i][0]].b * b_factor < gradient_b ? local_led_copy[ledTransformationMatrix[i][0]].b * b_factor : gradient_b;

                // note.. min is not working correctly, hence using
                // local_led_copy[ledTransformationMatrix[i][0]].r*b_factor < gradient_r ? local_led_copy[ledTransformationMatrix[i][0]].r*b_factor : gradient_r
                // to get the min

                    leds[ledTransformationMatrix[i][0]].setRGB(
                        gradient_r_new,
                        gradient_g_new,
                        gradient_b_new);

                    leds[ledTransformationMatrix[i][1]].setRGB(
                        gradient_r_new,
                        gradient_g_new,
                        gradient_b_new);

                    // save local copy of leds for intensity calculations
                    // custom max() implementation
                    local_led_copy[ledTransformationMatrix[i][0]].r = local_led_copy[ledTransformationMatrix[i][0]].r < (255 / b_factor) ? local_led_copy[ledTransformationMatrix[i][0]].r * b_factor : leds[ledTransformationMatrix[i][0]].r;
                    local_led_copy[ledTransformationMatrix[i][0]].g = local_led_copy[ledTransformationMatrix[i][0]].g < (255 / b_factor) ? local_led_copy[ledTransformationMatrix[i][0]].g * b_factor : leds[ledTransformationMatrix[i][0]].g;
                    local_led_copy[ledTransformationMatrix[i][0]].b = local_led_copy[ledTransformationMatrix[i][0]].b < (255 / b_factor) ? local_led_copy[ledTransformationMatrix[i][0]].b * b_factor : leds[ledTransformationMatrix[i][0]].b;

                } // i < 110

            } // for (int i = a; i <= 11 * a ; i += 10)

           
             

        } // for (int a = 0; a < k; a++)

           FastLED.show();
           delay(delayPerRound);
    }
      delay(delayAfterTransitionDone);
}

void colorTransientOff()
{

    bool oldClockState[110] = {false};
    updateClockState();

    for (int i = 0; i < 110; i++)
    {
        if (leds[ledTransformationMatrix[i][0]].r != 0 || leds[ledTransformationMatrix[i][0]].g != 0 || leds[ledTransformationMatrix[i][0]].b != 0)
        {
            oldClockState[i] = true;
        }
    }
    restoreLEDs();
    FastLED.show();

    saveLEDs();
    CRGB local_led_copy[NUM_LEDS];

    // save current clock state locally
    for (int i = 0; i < NUM_LEDS; i++)
    {
        local_led_copy[i].r = leds_copy[i].r;
        local_led_copy[i].g = leds_copy[i].g;
        local_led_copy[i].b = leds_copy[i].b;
    }

    // With every iteration the value of the led is set to min(<previous_value>*b_factor,<brightness>)
    // This exponential increase in brightness is perceived as a linear increase by the human eye

    //============================  TIMING SETTINGS =========================================

    // scaling factor for brightness (lower -> slower transition)
    float b_factor = 0.75;

    // speed of the animation (delay -> higher is slower) [ms]
    uint16_t delayPerRound = 70;

    // keep the transtion after it is done [ms]
    uint16_t delayAfterTransitionDone = 0;


    //============================ END TIMING SETTINGS =========================================


    // diagonally

    for (int k = 0; k < 30; k++)
    {
        //columns
        for (int a = 0; a < k; a++)
        {
            // rows
            for (int i = a; i <= 11 * a; i += 10)
            {
                if (i < 110)
                {
                  
                    if (!oldClockState[i])
                    {

                        leds[ledTransformationMatrix[i][0]].setRGB(
                            local_led_copy[ledTransformationMatrix[i][0]].r * b_factor > 2 ? local_led_copy[ledTransformationMatrix[i][0]].r * b_factor : 0,
                            local_led_copy[ledTransformationMatrix[i][0]].g * b_factor > 2 ? local_led_copy[ledTransformationMatrix[i][0]].g * b_factor : 0,
                            local_led_copy[ledTransformationMatrix[i][0]].b * b_factor > 2 ? local_led_copy[ledTransformationMatrix[i][0]].b * b_factor : 0);

                        leds[ledTransformationMatrix[i][1]].setRGB(
                            local_led_copy[ledTransformationMatrix[i][0]].r * b_factor > 2 ? local_led_copy[ledTransformationMatrix[i][0]].r * b_factor : 0,
                            local_led_copy[ledTransformationMatrix[i][0]].g * b_factor > 2 ? local_led_copy[ledTransformationMatrix[i][0]].g * b_factor : 0,
                            local_led_copy[ledTransformationMatrix[i][0]].b * b_factor > 2 ? local_led_copy[ledTransformationMatrix[i][0]].b * b_factor : 0);

                        // save local copy of leds for intensity calculations
                        // custom max() implementation
                        local_led_copy[ledTransformationMatrix[i][0]].r = local_led_copy[ledTransformationMatrix[i][0]].r > 2 ? local_led_copy[ledTransformationMatrix[i][0]].r * b_factor : 0;
                        local_led_copy[ledTransformationMatrix[i][0]].g = local_led_copy[ledTransformationMatrix[i][0]].g > 2 ? local_led_copy[ledTransformationMatrix[i][0]].g * b_factor : 0;
                        local_led_copy[ledTransformationMatrix[i][0]].b = local_led_copy[ledTransformationMatrix[i][0]].b > 2 ? local_led_copy[ledTransformationMatrix[i][0]].b * b_factor : 0;

                    } // !oldClockState[i]
                }     // i < 110
            }         // for (int i = a; i <= 11 * a ; i += 10)

        } // for (int a = 0; a < 20; a++)

        FastLED.show();
        delay(delayPerRound);

    } // for(int k = 0;k<20;k++)

    delay(delayAfterTransitionDone);

} // colorTransientOff()

void dropTheLetters()
{

    for (int i = 0; i < 11; i++)
    {
        moveOneRowDown();
        FastLED.show();
        delay(50);
    }
}

void moveOneRowDown()
{

    // Move letters down one row
    for (int i = 109; i >= 11; i--)
    {
        leds[ledTransformationMatrix[i][0]] = leds[ledTransformationMatrix[(i - 11)][0]];
        leds[ledTransformationMatrix[i][1]] = leds[ledTransformationMatrix[(i - 11)][1]];
    }
    for (int i = 0; i < 11; i++)
    {
        leds[ledTransformationMatrix[i][0]].setRGB(0, 0, 0);
        leds[ledTransformationMatrix[i][1]].setRGB(0, 0, 0);
    }
}

void moveOneRowUp()
{
}

void setLetter(int index, int r, int g, int b)
{

    leds[ledTransformationMatrix[index][0]].setRGB(r, g, b);
    leds[ledTransformationMatrix[index][1]].setRGB(r, g, b);
}

void setMultipleLEDs(char start, int size)
{

    int r = globalR;
    int g = globalG;
    int b = globalB;

    if (randomColorsEnabled)
    {
        if (randomColorCounter > n_randomColors - 1)
        {
            randomColorCounter = randomColorCounter - n_randomColors;
        }

        r = selectedRandomColors[randomColorCounter][0];
        g = selectedRandomColors[randomColorCounter][1];
        b = selectedRandomColors[randomColorCounter][2];

        randomColorCounter = randomColorCounter + random(n_randomColors);

        fourColorSelectionEnabled = false;
    }

    if (fourColorSelectionEnabled)
    {

        if (start >= 110)
        {
            r = edgeR;
            g = edgeG;
            b = edgeB;
        }
        else
        {
            r = fixedColorValues[letterColorCounter][0];
            g = fixedColorValues[letterColorCounter][1];
            b = fixedColorValues[letterColorCounter][2];

            letterColorCounter++;
            if (letterColorCounter > 2)
            {
                letterColorCounter = 0;
            }
        }
    }

    for (int i = 0; i < size; i++)
    {

        if (keepTransientColors)
        {
            if (start < 110)
            {
                setLetter(start + i, leds_copy[ledTransformationMatrix[(start + i)][0]].r, leds_copy[ledTransformationMatrix[(start + i)][0]].g, leds_copy[ledTransformationMatrix[(start + i)][0]].b);
            }
            else
            {
                switch ((int)start)
                {

                // use color in upper left
                case 110:
                    setLetter(start, leds_copy[ledTransformationMatrix[0][0]].r, leds_copy[ledTransformationMatrix[0][0]].g, leds_copy[ledTransformationMatrix[0][0]].b);
                    break;
                // use color in upper right:
                case 111:
                    setLetter(start, leds_copy[ledTransformationMatrix[10][0]].r, leds_copy[ledTransformationMatrix[10][0]].g, leds_copy[ledTransformationMatrix[10][0]].b);
                    break;
                // use color in lower right
                case 112:
                    setLetter(start, leds_copy[ledTransformationMatrix[109][0]].r, leds_copy[ledTransformationMatrix[109][0]].g, leds_copy[ledTransformationMatrix[109][0]].b);
                    break;
                    // use color in lower left
                case 113:
                    setLetter(start, leds_copy[ledTransformationMatrix[98][0]].r, leds_copy[ledTransformationMatrix[98][0]].g, leds_copy[ledTransformationMatrix[98][0]].b);
                    break;
                default:
                    setLetter(start, globalR, globalG, globalB);
                }
            }
        }
        else
        {
            setLetter(start + i, r, g, b);
        }
    }

    
    if(bootup)
    {
       for (int i = 0; i < size; i++)
       {
           setLetter(start+i, globalR, globalG, globalB);
       }
      bootup = false;
    }
}

void updateClockState()
{

    FastLED.clear();

    uint8_t hour = currentTime.hour;
    uint8_t minute = currentTime.minute;
    uint8_t second = currentTime.second;

    if ((minute >= 0) && (minute < 5) || (minute > 29) && (minute < 35))
    {
        setMultipleLEDs(ES, 2);
        setMultipleLEDs(IST, 3);
    }

    if ((minute % 5 != 0))
    {

        if ((minute % 5 == 1))
        {
            setMultipleLEDs(OBENLINKS, 1);
        }

        if ((minute % 5 == 2))
        {
            setMultipleLEDs(OBENLINKS, 1);

            setMultipleLEDs(OBENRECHTS, 1);
        }

        if ((minute % 5 == 3))
        {
            setMultipleLEDs(OBENLINKS, 1);

            setMultipleLEDs(OBENRECHTS, 1);

            setMultipleLEDs(UNTENRECHTS, 1);
        }

        if ((minute % 5 == 4))
        {
            setMultipleLEDs(OBENLINKS, 1);

            setMultipleLEDs(OBENRECHTS, 1);

            setMultipleLEDs(UNTENRECHTS, 1);

            setMultipleLEDs(UNTENLINKS, 1);
        }
    }

    if ((minute > 4) && (minute < 10))
    {
        setMultipleLEDs(FUENFMINUTEN, 4);
        setMultipleLEDs(NACH, 4);
    }
    if ((minute > 9) && (minute < 15))
    {
        setMultipleLEDs(ZEHNMINUTEN, 4);
        setMultipleLEDs(NACH, 4);
    }
    if ((minute > 14) && (minute < 20))
    {
        setMultipleLEDs(VIERTEL, 7);
        setMultipleLEDs(NACH, 4);
    }
    if ((minute > 19) && (minute < 25))
    {
        setMultipleLEDs(ZWANZIG, 7);
        setMultipleLEDs(NACH, 4);
    }
    if ((minute > 24) && (minute < 30))
    {
        setMultipleLEDs(FUENFMINUTEN, 4);
        setMultipleLEDs(VOR, 3);
        setMultipleLEDs(HALB, 4);
    }
    if ((minute > 29) && (minute < 35))
    {
        setMultipleLEDs(HALB, 4);
    }
    if ((minute > 34) && (minute < 40))
    {
        setMultipleLEDs(FUENFMINUTEN, 4);
        setMultipleLEDs(NACH, 4);
        setMultipleLEDs(HALB, 4);
    }
    if ((minute > 39) && (minute < 45))
    {
        setMultipleLEDs(ZWANZIG, 7);
        setMultipleLEDs(VOR, 3);
    }
    if ((minute > 44) && (minute < 50))
    {
        setMultipleLEDs(VIERTEL, 7);
        setMultipleLEDs(VOR, 3);
    }
    if ((minute > 49) && (minute < 55))
    {
        setMultipleLEDs(ZEHNMINUTEN, 4);
        setMultipleLEDs(VOR, 3);
    }
    if (minute > 54)
    {
        setMultipleLEDs(FUENFMINUTEN, 4);
        setMultipleLEDs(VOR, 3);
    }

    switch (hour)
    {
    case 1:
    case 13:
        if (minute > 24)
        {
            setMultipleLEDs(ZWEI, 4);
        }
        else
        {

            if (minute < 5)
            {
                setMultipleLEDs(EIN, 3);
            }
            else
            {
                setMultipleLEDs(EINS, 4);
            }
        }

        break;
    case 2:
    case 14:
        if (minute > 24)
        {
            setMultipleLEDs(DREI, 4);
        }
        else
        {
            setMultipleLEDs(ZWEI, 4);
        }
        break;
    case 3:
    case 15:
        if (minute > 24)
        {
            setMultipleLEDs(VIER, 4);
        }
        else
        {
            setMultipleLEDs(DREI, 4);
        }
        break;

    case 4:
    case 16:
        if (minute > 24)
        {
            setMultipleLEDs(FUENFSTUNDEN, 4);
        }
        else
        {
            setMultipleLEDs(VIER, 4);
        }
        break;
    case 5:
    case 17:
        if (minute > 24)
        {
            setMultipleLEDs(SECHS, 5);
        }
        else
        {

            setMultipleLEDs(FUENFSTUNDEN, 4);
        }
        break;
    case 6:
    case 18:
        if (minute > 24)
        {
            setMultipleLEDs(SIEBEN, 6);
        }
        else
        {
            setMultipleLEDs(SECHS, 5);
        }
        break;
    case 7:
    case 19:
        if (minute > 24)
        {
            setMultipleLEDs(ACHT, 4);
        }
        else
        {
            setMultipleLEDs(SIEBEN, 6);
        }
        break;
    case 8:
    case 20:
        if (minute > 24)
        {
            setMultipleLEDs(NEUN, 4);
        }
        else
        {
            setMultipleLEDs(ACHT, 4);
        }
        break;
    case 9:
    case 21:
        if (minute > 24)
        {
            setMultipleLEDs(ZEHNSTUNDEN, 4);
        }
        else
        {
            setMultipleLEDs(NEUN, 4);
        }
        break;
    case 10:
    case 22:
        if (minute > 24)
        {
            setMultipleLEDs(ELF, 3);
        }
        else
        {
            setMultipleLEDs(ZEHNSTUNDEN, 4);
        }
        break;
    case 11:
    case 23:
        if (minute > 24)
        {
            setMultipleLEDs(ZWOELF, 5);
        }
        else
        {
            setMultipleLEDs(ELF, 3);
        }
        break;
    case 12:
    case 0:
        if (minute > 24)
        {
            setMultipleLEDs(EINS, 4);
        }
        else
        {
            setMultipleLEDs(ZWOELF, 5);
        }
        break;
    }

    if ((minute < 5))
    {
        setMultipleLEDs(UHR, 3);
    }
}

int char2int(char val)
{
    int res = (int)val - 48;
    return res;
}

void fadeIn()
{
    //slowly fade in
    int brightnessTemp = 0;
    while (brightnessTemp != brightness)
    {
        delay(10);
        FastLED.setBrightness(brightnessTemp);
        brightnessTemp++;
        delay(10);
        FastLED.show();
    }
}

void fadeOut(uint16_t duration)
{
    int brightnessTemp = oldBrightness;

    //slowly fade out
    while (brightnessTemp != 0)
    {
        brightnessTemp--;
        FastLED.setBrightness(brightnessTemp);
        delay(duration / brightness);
        FastLED.show();
    }
}

// WIFI

void initWifi()
{
    Serial.print("Connecting to Wifi");
    //WiFi.config(ip, gateway, subnet);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.print("\nWiFi connected, IP address: ");
    Serial.println(WiFi.localIP());

    //Serial.println("Starting Time Client");
    //timeClient.begin();
    //Serial.println("Getting current Time");
    //timeClient.update();
    //Serial.println("Starting Web Server");
    //server.begin();

    webServer.on("/", handleRoot);

    webServer.begin();
    Serial.println("HTTP server started");
}
/*  all changeable parameters

  uint8_t brightness = 130;
  bool setBrightness = false;
  uint8_t globalR = 0;
  uint8_t globalG = 255;
  uint8_t globalB = 128;
  uint8_t minBrightness = 15;
  uint8_t maxBrightness = 130;
  bool colorGradient = false;
  bool randomColorsEnabled = false;

  uint8_t edgeR = 255;
  uint8_t edgeG = 255;
  uint8_t edgeB = 255;

  uint8_t randomColorCounter = 0;
  uint8_t fixedColorValues [3][3] = {
  {255, 255, 255}, {255, 255, 255}, {255, 255, 255}
  };

  //Event Management
  // 0 = show Time
  // 1 = show effect
  uint8_t state = 0;
  bool ledState[NUM_LEDS];


  //state Managment for communication
  bool multipleColorSelectionEnabled = false;
  bool setHighlightColor = false;
  bool setLetterColor = false;
  bool setR = false;
  bool setG = false;
  bool setB = false;
  bool fourColorSelectionEnabled = false;
  uint8_t letterColorCounter = 0;
  bool setLetterColor1 = false;
  bool setLetterColor2 = false;
  bool setLetterColor3 = false;




*/

void handleRoot()
{

    // get custom color
    if (webServer.hasArg("customR") && webServer.arg("customR").length() > 0)
    {
        uint8_t customR = webServer.arg("customR").toInt();
        Serial.print("customR: ");
        Serial.println(customR);
        randomColors[18][0] = customR;
    }

    if (webServer.hasArg("customG") && webServer.arg("customG").length() > 0)
    {
        uint8_t customG = webServer.arg("customG").toInt();
        Serial.print("customG: ");
        Serial.println(customG);
        randomColors[18][1] = customG;
    }

    if (webServer.hasArg("customB") && webServer.arg("customB").length() > 0)
    {
        uint8_t customB = webServer.arg("customB").toInt();
        Serial.print("customB: ");
        Serial.println(customB);
        randomColors[18][2] = customB;
    }

    if (webServer.hasArg("ENABLE_RANDOM_COLORS"))
    {
        Serial.println("Enable Random Colors");
        if (webServer.arg("ENABLE_RANDOM_COLORS") == "RC")
        {
            n_randomColors = 0;
            randomColorsEnabled = true;
            fourColorSelectionEnabled = false;
            for (int i = 0; i < 19; i++)
            {
                randomColorsUsed[i] = false;
            }
        }
        else
        {
            Serial.println("Disable Random Colors");
            n_randomColors = 0;
            randomColorsEnabled = false;
            for (int i = 0; i < 19; i++)
            {
                randomColorsUsed[i] = false;
            }

            Serial.println("Use fixed Colors");
            fourColorSelectionEnabled = true;
            fixedColorValues[0][0] = randomColors[webServer.arg("fixedColor1").toInt()][0];
            fixedColorValues[0][1] = randomColors[webServer.arg("fixedColor1").toInt()][1];
            fixedColorValues[0][2] = randomColors[webServer.arg("fixedColor1").toInt()][2];

            fixedColorValues[1][0] = randomColors[webServer.arg("fixedColor2").toInt()][0];
            fixedColorValues[1][1] = randomColors[webServer.arg("fixedColor2").toInt()][1];
            fixedColorValues[1][2] = randomColors[webServer.arg("fixedColor2").toInt()][2];

            fixedColorValues[2][0] = randomColors[webServer.arg("fixedColor3").toInt()][0];
            fixedColorValues[2][1] = randomColors[webServer.arg("fixedColor3").toInt()][1];
            fixedColorValues[2][2] = randomColors[webServer.arg("fixedColor3").toInt()][2];

            edgeR = randomColors[webServer.arg("fixedColorE").toInt()][0];
            edgeG = randomColors[webServer.arg("fixedColorE").toInt()][1];
            edgeB = randomColors[webServer.arg("fixedColorE").toInt()][2];

            updateClockState();
            FastLED.show();
        }
    }
    if (webServer.hasArg("randomColors") && randomColorsEnabled)
    {
        for (int i = 0; i < webServer.args(); i++)
        {
            if (webServer.argName(i) == "randomColors")
            {
                Serial.print("Name: ");
                Serial.println(webServer.argName(i));
                Serial.print("Value: ");
                Serial.println(webServer.arg(i));
                randomColorsUsed[webServer.arg(i).toInt()] = true;
                n_randomColors++;
            }
        }
    }
    if (randomColorsEnabled)
    {
        uint8_t k = 0;
        for (int i = 0; i < 19; i++)
        {
            if (randomColorsUsed[i])
            {
                Serial.print("index #");
                Serial.print(i);
                Serial.println(" is used!");
                selectedRandomColors[k][0] = randomColors[i][0];
                selectedRandomColors[k][1] = randomColors[i][1];
                selectedRandomColors[k][2] = randomColors[i][2];
                k++;
            }
        }
        updateClockState();
        FastLED.show();
    }
    else
    {
        webServer.send(200, "text/html", getPage());
    }
}

String getPage()
{
    String html = "<!-- // ALL CAPS VARIABLES ARE DEFINED IN ARDUINO SOURCE CODE ! --> <!DOCTYPE HTML> <html> <head> <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css\" integrity=\"sha384-MCw98/SFnGE8fJT3GXwEOngsV7Zt27NXFoaoApmYm81iuXoPkFOJwJ8ERdknLPMO\" crossorigin=\"anonymous\"> <style type=\"text/css\"> body{ background-color:#c1c1c1; } </style> </head> <body> <form action=\"?sCmd\" id=\"myForm\"> <div class=\"container\" style=\"background-color:#e2e2e2; padding:40px\"> <div class=\"row\"> <div class=\"col-12\"> <center> <h2> Farben</h2> <br> </center> </div> </div> <div class=\"row\"> <input type=\"button\" value=\"Soft Red\" class=\"col-2\" name=\"1\" onclick=\"submitForm(1)\" style=\"background-color:#ed8880; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Rosa\" class=\"col-2\" name=\"2\" onclick=\"submitForm(2)\" style=\"background-color:#ff96cf; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Flieder\" class=\"col-2\" name=\"3\" onclick=\"submitForm(3)\" style=\"background-color:#be9ad1; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Lila-Grau\" class=\"col-2\" name=\"4\" onclick=\"submitForm(4)\" style=\"background-color:#b0a4d8; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Blau\" class=\"col-2\" name=\"5\" onclick=\"submitForm(5)\" style=\"background-color:#6b99ff; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"T&uuml;rkis\" class=\"col-2\" name=\"6\" onclick=\"submitForm(6)\" style=\"background-color:#5fe3f4; padding:10px; padding-top:15px\"> </div> <div class=\"row\"> <input type=\"button\" value=\"Hellblau\" class=\"col-2\" name=\"7\" onclick=\"submitForm(7)\" style=\"background-color:#d3faff; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Wei&szlig;\" class=\"col-2\" name=\"8\" onclick=\"submitForm(8)\" style=\"background-color:#ffffff; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Dunkelgr&uuml;n\" class=\"col-2\" name=\"9\" onclick=\"submitForm(9)\" style=\"background-color:#2d4c32; color:white; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Seegr&uuml;n\" class=\"col-2\" name=\"10\" onclick=\"submitForm(10)\" style=\"background-color:#009688; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Olive\" class=\"col-2\" name=\"11\" onclick=\"submitForm(11)\" style=\"background-color:#4CAF50; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Limone\" class=\"col-2\" name=\"12\" onclick=\"submitForm(12)\" style=\"background-color:#8BC34A; padding:10px; padding-top:15px\"> </div> <div class=\"row\"> <input type=\"button\" value=\"Gelbgr&uuml;n\" class=\"col-2\" name=\"13\" onclick=\"submitForm(13)\" style=\"background-color:#CDDC39; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Gold\" class=\"col-2\" name=\"14\" onclick=\"submitForm(14)\" style=\"background-color:#FFEB3B; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Orange\" class=\"col-2\" name=\"15\" onclick=\"submitForm(15)\" style=\"background-color:#FFC107; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Orangerot\" class=\"col-2\" name=\"16\" onclick=\"submitForm(16)\" style=\"background-color:#FF9800; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Braun\" class=\"col-2\" name=\"17\" onclick=\"submitForm(17)\" style=\"background-color:#795548; padding:10px; padding-top:15px\"> <input type=\"button\" value=\"Grau\" class=\"col-2\" name=\"18\" onclick=\"submitForm(18)\" style=\"background-color:#a8a8a8; padding:10px; padding-top:15px\"> </div> <div class=\"row\"> <div class=\"col-12\" style=\"background-color:#";
    html += rgbToHex(randomColors[18][0], randomColors[18][1], randomColors[18][2]); //change user color in web browser
    html += "; padding:10px; padding-top:15px\" id=\"customColorBG\"> <center> <input type=\"button\" value=\"Eigene\" class=\"col-2\" name=\"19\" onclick=\"submitForm(19)\" style=\"background-color:#a1a1a1; padding:10px; padding-top:15px\"> R: <input type=\"number\" min=\"0\" max=\"255\" id=\"customR\" name=\"customR\" onChange=\"customColorChangedR(this)\"> G: <input type=\"number\" min=\"0\" max=\"255\" id=\"customG\" name=\"customG\" onChange=\"customColorChangedG(this)\"> B: <input type=\"number\" min=\"0\" max=\"255\" id=\"customB\" name=\"customB\" onChange=\"customColorChangedB(this)\"> </center> </div> </div> <br> <div class=\"row\"> <div class=\"col-12\"> <center> <input type=\"radio\" name=\"ENABLE_RANDOM_COLORS\" value=\"FC\" id=\"fc_enabled\" checked onChange=\"toggleCSView()\"> Farbwerte beibehalten<br> <input type=\"radio\" name=\"ENABLE_RANDOM_COLORS\" value=\"RC\" id=\"rc_enabled\" onChange=\"toggleCSView()\"> Zuf&auml;llige Farben aktivieren<br><br> </center> </div> </div> <center> <h4>Folgende Farben werden verwendet:</h4><br> <div id=\"rc_selection\"> <center><h5>Zuf&auml;llig wechseln zwischen:</h5></center> <select name=\"randomColors\" multiple> <option value=\"0\">Soft Red</option> <option value=\"1\">Rosa</option> <option value=\"2\">Flieder</option> <option value=\"3\">Lila-Grau</option> <option value=\"4\">Blau</option> <option value=\"5\">T&uuml;rkis</option> <option value=\"6\">Hellblau</option> <option value=\"7\">Wei&szlig;</option> <option value=\"8\">Dunkelgr&uuml;n</option> <option value=\"9\">Seegr&uuml;n</option> <option value=\"10\">Olive</option> <option value=\"11\">Limone</option> <option value=\"12\">Gelb</option> <option value=\"13\">Gold</option> <option value=\"14\">Orange</option> <option value=\"15\">Orangerot</option> <option value=\"16\">Braun</option> <option value=\"17\">Grau</option> <option value=\"18\">Eigene</option> </select> </div> </center> <br> <div id=\"fc_selection\"> <div class=\"row\"> <div class=\"col-3\"> <center><h5>Farbe #1</h5> <select name=\"fixedColor1\" id=\"fixedColorOption1\"> <option value=\"0\">Soft Red</option> <option value=\"1\">Rosa</option> <option value=\"2\">Flieder</option> <option value=\"3\">Lila-Grau</option> <option value=\"4\">Blau</option> <option value=\"5\">T&uuml;rkis</option> <option value=\"6\">Hellblau</option> <option value=\"7\">Wei&szlig;</option> <option value=\"8\">Dunkelgr&uuml;n</option> <option value=\"9\">Seegr&uuml;n</option> <option value=\"10\">Olive</option> <option value=\"11\">Limone</option> <option value=\"12\">Gelb</option> <option value=\"13\">Gold</option> <option value=\"14\">Orange</option> <option value=\"15\">Orangerot</option> <option value=\"16\">Braun</option> <option value=\"17\">Grau</option> <option value=\"18\">Eigene</option> </select> </center> </div> <div class=\"col-3\"> <center><h5>Farbe #2</h5> <select name=\"fixedColor2\" id=\"fixedColorOption2\"> <option value=\"0\">Soft Red</option> <option value=\"1\">Rosa</option> <option value=\"2\">Flieder</option> <option value=\"3\">Lila-Grau</option> <option value=\"4\">Blau</option> <option value=\"5\">T&uuml;rkis</option> <option value=\"6\">Hellblau</option> <option value=\"7\">Wei&szlig;</option> <option value=\"8\">Dunkelgr&uuml;n</option> <option value=\"9\">Seegr&uuml;n</option> <option value=\"10\">Olive</option> <option value=\"11\">Limone</option> <option value=\"12\">Gelb</option> <option value=\"13\">Gold</option> <option value=\"14\">Orange</option> <option value=\"15\">Orangerot</option> <option value=\"16\">Braun</option> <option value=\"17\">Grau</option> <option value=\"18\">Eigene</option> </select> </center> </div> <div class=\"col-3\"> <center><h5>Farbe #3</h5> <select name=\"fixedColor3\" id=\"fixedColorOption3\"> <option value=\"0\">Soft Red</option> <option value=\"1\">Rosa</option> <option value=\"2\">Flieder</option> <option value=\"3\">Lila-Grau</option> <option value=\"4\">Blau</option> <option value=\"5\">T&uuml;rkis</option> <option value=\"6\">Hellblau</option> <option value=\"7\">Wei&szlig;</option> <option value=\"8\">Dunkelgr&uuml;n</option> <option value=\"9\">Seegr&uuml;n</option> <option value=\"10\">Olive</option> <option value=\"11\">Limone</option> <option value=\"12\">Gelb</option> <option value=\"13\">Gold</option> <option value=\"14\">Orange</option> <option value=\"15\">Orangerot</option> <option value=\"16\">Braun</option> <option value=\"17\">Grau</option> <option value=\"18\">Eigene</option> </select> </center> </div> <div class=\"col-3\"> <center><h5>Eckfarbe</h5> <select name=\"fixedColorE\" id=\"fixedColorOptionE\"> <option value=\"0\">Soft Red</option> <option value=\"1\">Rosa</option> <option value=\"2\">Flieder</option> <option value=\"3\">Lila-Grau</option> <option value=\"4\">Blau</option> <option value=\"5\">T&uuml;rkis</option> <option value=\"6\">Hellblau</option> <option value=\"7\">Wei&szlig;</option> <option value=\"8\">Dunkelgr&uuml;n</option> <option value=\"9\">Seegr&uuml;n</option> <option value=\"10\">Olive</option> <option value=\"11\">Limone</option> <option value=\"12\">Gelb</option> <option value=\"13\">Gold</option> <option value=\"14\">Orange</option> <option value=\"15\">Orangerot</option> <option value=\"16\">Braun</option> <option value=\"17\">Grau</option> <option value=\"18\">Eigene</option> </select> </center> </div> </div> <br> </div> <br> <center> <input type=\"submit\" value=\"Einstellungen speichern\" > </center> </div> <span id=\"debug\">debug</span> </form> </body> </html> <script> var debug = document.getElementById(\"debug\"); var customColorBG = document.getElementById(\"customColorBG\"); var customR = document.getElementById(\"customR\").value; var customG = 0; var customB = 0; var rc_enabled = document.getElementById(\"rc_enabled\"); var fc_enabled = document.getElementById(\"fc_enabled\"); var fc_selection = document.getElementById(\"fc_selection\"); var rc_selection = document.getElementById(\"rc_selection\"); function submitForm(val){ document.getElementById(\"fixedColorOption1\").value=val-1; document.getElementById(\"fixedColorOption2\").value=val-1; document.getElementById(\"fixedColorOption3\").value=val-1; document.getElementById(\"fixedColorOptionE\").value=val-1; rc_selection.style.display = 'none'; fc_selection.style.display = 'block'; document.getElementById(\"myForm\").submit();} function toggleCSView(){ if(rc_selection.style.display =='none'){ fc_selection.style.display = 'none'; rc_selection.style.display = 'block'; debug.innerHTML = \"showRC\"; } else{ rc_selection.style.display = 'none'; fc_selection.style.display = 'block'; debug.innerHTML = \"showFC\"; } } function rgb(r, g, b){ return \"rgb(\"+r+\",\"+g+\",\"+b+\")\"; } function customColorChangedR(val){ customR = val.value; customColorBG.style.backgroundColor = rgb(customR,customG,customB); customColorButton.style.backgroundColor = rgb(customR,customG,customB); } function customColorChangedG(val){ customG = val.value; customColorBG.style.backgroundColor = rgb(customR,customG,customB); customColorButton.style.backgroundColor = rgb(customR,customG,customB); } function customColorChangedB(val){ customB = val.value; customColorBG.style.backgroundColor = rgb(customR,customG,customB); customColorButton.style.backgroundColor = rgb(customR,customG,customB); } window.onload = function() { fc_selection.style.display = 'block'; rc_selection.style.display = 'none'; } </script> </script> <script src=\"http://code.jquery.com/jquery-1.10.1.min.js\"></script> <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js\" integrity=\"sha384-ZMP7rVo3mIykV+2+9J3UJ46jBk0WLaUAdn689aCwoqbBJiSnjAK/l8WvCWPIPm49\" crossorigin=\"anonymous\"> </script> <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js\" integrity=\"sha384-ChfqqxuZUCnJSK3+MXmPNIyE6ZbWh2IMqE241rYiqJxyMiZ6OW/JmZQ5stwEULTy\" crossorigin=\"anonymous\"> </script> </body> </html>";
    return html;
}

// Helper functions

String rgbToHex(uint8_t r, uint8_t g, uint8_t b)
{
    long RGB = (r << 16L) | (g << 8) | b;
    String s = String(RGB, HEX);
    while (s.length() < 6)
    {
        s = "0" + s;
    }
    return s;
}

void printLocalTime()
{
    Serial.print("Local time: ");
    if (currentTime.hour < 10)
        Serial.print(0);
    Serial.print(currentTime.hour);
    Serial.print(":");
    if (currentTime.minute < 10)
        Serial.print(0);
    Serial.print(currentTime.minute);
    Serial.print(":");
    if (currentTime.second < 10)
        Serial.print(0);
    Serial.println(currentTime.second);
}

void saveLEDs()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds_copy[i].r = leds[i].r;
        leds_copy[i].g = leds[i].g;
        leds_copy[i].b = leds[i].b;
    }
}

void restoreLEDs()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i].r = leds_copy[i].r;
        leds[i].g = leds_copy[i].g;
        leds[i].b = leds_copy[i].b;
    }
}


