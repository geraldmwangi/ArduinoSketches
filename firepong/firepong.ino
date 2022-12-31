/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     6
#define TFT_RST    8  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     7

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 4   // set these to be whatever pins you like!
#define TFT_MOSI 5   // set these to be whatever pins you like!
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

int radius=3;
float p = 3.1415926;
int len=0;
int lastpos=0;
struct ballClass{
  uint16_t lasttime;
  float lastPosx;
  float lastPosy;
  float momx;
  float momy;
  float posx;
  float posy;
  float mass;
} ball;

struct batClass
{
  uint16_t lasttime;
  int len;
  float posx;
  float posy;
  float lastPosx;
  float momx;
  int color;
  int poti;
};

batClass bat1, bat2;

void setup(void) {
  Serial.begin(9600);
  Serial.print("Hello! ST7735 TFT Test");

  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

  // Use this initializer (uncomment) if you're using a 1.44" TFT
  //tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab

  Serial.println("Initialized");

  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  bat1.poti=0;
  bat1.len=tft.width()/4.0;
    float val=analogRead(bat1.poti);
  bat1.lastPosx=tft.width()*(1.0-val/1024.0);
  bat1.posy=tft.height()*0.9;
  for(int i=(bat1.lastPosx);i<(bat1.lastPosx+bat1.len);i++)
      tft.drawPixel(i, bat1.posy, ST7735_GREEN);
  
  bat1.lasttime=millis();
  bat1.color=ST7735_GREEN;

  bat2.poti=1;
  bat2.len=tft.width()/4.0;
    float val2=analogRead(bat2.poti);
  bat2.lastPosx=tft.width()*(1.0-val2/1024.0);
  bat2.posy=tft.height()*0.1;
  for(int i=(bat2.lastPosx);i<(bat2.lastPosx+bat2.len);i++)
      tft.drawPixel(i, bat2.posy, ST7735_GREEN);
  
  bat2.lasttime=millis();
  bat2.color=ST7735_GREEN;


  ball.lasttime=millis();
  ball.mass=1.0f;
  ball.lastPosx=tft.width()/2.0;
  ball.lastPosy=tft.height()/2.0;
  ball.momx=100.0f; //Ball has unit mass
  ball.momy=100.0f; 
}
void updateBall()
{

  uint16_t currtime=millis();
  float timediff=(currtime-ball.lasttime)/1000.0;

  //tft.fillCircle(ball.lastPosx, ball.lastPosy, radius, ST7735_BLACK);
  //if(ball.momx)
  {
    
    ball.posx=ball.lastPosx+ball.momx*timediff;
    if(ball.posx+radius>tft.width()||ball.posx-radius<0)
    {
      ball.momx=-ball.momx;
      ball.posx=ball.lastPosx+ball.momx*timediff;
    }
    
  }
  //if(ball.momy)
  {
    
    ball.posy=ball.lastPosy+ball.momy*timediff;
    //char msg[100];
    //sprintf(msg,"posy: %d, timediff %d\0",ball.posy,currtime-ball.lasttime);
    //Serial.println(msg);

    updateBat1();
    updateBat2();
    
    //Rebound the ball at the boundaries
    if(ball.posy+radius>tft.height()||ball.posy-radius<0)
    {
      float signy=fabs(ball.momy)/ball.momy;
      float signx=fabs(ball.momx)/ball.momx;
      ball.momy+=signy*5;
      ball.momx+=signx*5;
      ball.momy=-ball.momy;
      ball.posy=ball.lastPosy+ball.momy*timediff;
    }

    //Bounce off bat1
    if((fabs(ball.posy-bat1.posy)<=(radius+1)&&
        (ball.posx>=bat1.posx&&ball.posx<=(bat1.posx+bat1.len)))||
        sqrt((ball.posx-bat1.posx)*(ball.posx-bat1.posx)+(ball.posy-bat1.posy)*(ball.posy-bat1.posy))<radius||
        sqrt((ball.posx-bat1.posx-bat1.len)*(ball.posx-bat1.posx-bat1.len)+(ball.posy-bat1.posy)*(ball.posy-bat1.posy))<radius
        )
    {
      ball.momy=-ball.momy;
      //ball.momx+=bat1.momx;
      ball.posy=ball.lastPosy+ball.momy*timediff;
    }


    //Bounce off bat2
    if((fabs(ball.posy-bat2.posy)<=(radius+1)&&
        (ball.posx>=bat2.posx&&ball.posx<=(bat2.posx+bat2.len)))||
        sqrt((ball.posx-bat2.posx)*(ball.posx-bat2.posx)+(ball.posy-bat2.posy)*(ball.posy-bat2.posy))<radius||
        sqrt((ball.posx-bat2.posx-bat2.len)*(ball.posx-bat2.posx-bat2.len)+(ball.posy-bat2.posy)*(ball.posy-bat2.posy))<radius
        )
    {
      ball.momy=-ball.momy;
      //ball.momx+=bat2.momx;
      ball.posy=ball.lastPosy+ball.momy*timediff;
    }
    
  }


  tft.fillCircle(ball.lastPosx, ball.lastPosy, radius, ST7735_BLACK);
    ball.lastPosx=ball.posx;
  ball.lastPosy=ball.posy;
  ball.lasttime=currtime;
  tft.fillCircle(ball.posx, ball.posy, radius, ST7735_WHITE);

}
void updateBat1()
{
    //Update color
  if(ball.posy+radius>tft.height())
  {
    bat1.color=ST7735_RED;
    bat1.len-=2;
  }
  else if(ball.posy<bat1.posy&&bat1.color==ST7735_RED)
    bat1.color=ST7735_GREEN;
  float val=analogRead(bat1.poti);
  bat1.posx=(tft.width()-bat1.len)*(1.0-val/1024.0);
//  for(int i=0;i<pos;i++)
//    tft.drawPixel(i, tft.height()/2, ST7735_BLACK);
//  for(int i=(pos+len);i<tft.width();i++)
//    tft.drawPixel(i, tft.height()/2, ST7735_BLACK);
  //if(bat1.posx!=bat1.lastPosx)
  {


    if(bat1.posx+bat1.len>tft.width())
      bat1.posx=tft.width()-bat1.len;
    if(bat1.posx>bat1.lastPosx)
    {
      for(int i=(bat1.lastPosx);i<(bat1.posx);i++)
        tft.drawPixel(i, bat1.posy, ST7735_BLACK);
      for(int i=(bat1.posx);i<(bat1.posx+bat1.len);i++)
        tft.drawPixel(i, bat1.posy, bat1.color);
    }
    else
    {
      for(int i=(bat1.posx);i<(bat1.posx+bat1.len);i++)
        tft.drawPixel(i, bat1.posy, bat1.color);
      for(int i=(bat1.posx+bat1.len);i<(bat1.lastPosx+bat1.len);i++)
        tft.drawPixel(i, bat1.posy, ST7735_BLACK);
    }
    
     uint16_t currtime=millis();
     float timediff=-(bat1.lasttime-currtime)/1000.0f;
     bat1.momx=(bat1.posx-bat1.lastPosx)/timediff;
     bat1.lasttime=currtime;
     bat1.lastPosx=bat1.posx;
  }
}


void updateBat2()
{
    //Update color
  if(ball.posy-radius<=0)
  {
    bat2.color=ST7735_RED;
    bat2.len-=2;
  }
  else if(ball.posy<bat2.posy&&bat2.color==ST7735_RED)
    bat2.color=ST7735_GREEN;
  float val=analogRead(bat2.poti);
  bat2.posx=(tft.width()-bat2.len)*(1.0-val/1024.0);
//  for(int i=0;i<pos;i++)
//    tft.drawPixel(i, tft.height()/2, ST7735_BLACK);
//  for(int i=(pos+len);i<tft.width();i++)
//    tft.drawPixel(i, tft.height()/2, ST7735_BLACK);
  //if(bat2.posx!=bat2.lastPosx)
  {


    if(bat2.posx+bat2.len>tft.width())
      bat2.posx=tft.width()-bat2.len;
    if(bat2.posx>bat2.lastPosx)
    {
      for(int i=(bat2.lastPosx);i<(bat2.posx);i++)
        tft.drawPixel(i, bat2.posy, ST7735_BLACK);
      for(int i=(bat2.posx);i<(bat2.posx+bat2.len);i++)
        tft.drawPixel(i, bat2.posy, bat2.color);
    }
    else
    {
      for(int i=(bat2.posx);i<(bat2.posx+bat2.len);i++)
        tft.drawPixel(i, bat2.posy, bat2.color);
      for(int i=(bat2.posx+bat2.len);i<(bat2.lastPosx+bat2.len);i++)
        tft.drawPixel(i, bat2.posy, ST7735_BLACK);
    }
    
     uint16_t currtime=millis();
     float timediff=-(bat2.lasttime-currtime)/1000.0f;
     bat2.momx=(bat2.posx-bat2.lastPosx)/timediff;
     bat2.lasttime=currtime;
     bat2.lastPosx=bat2.posx;
  }
}
void loop() {
  //updateBat1();

  if(bat1.len>3&&bat2.len>3)
    updateBall();

  else
  {
    tft.setTextColor(ST77XX_RED);
    tft.println("Game Over");
    tft.setTextColor(ST77XX_GREEN);
    if(bat1.len<=3)
      tft.println("Lower player lost!");
    else
      tft.println("Upper player lost!");

    return;
  }
  


}

