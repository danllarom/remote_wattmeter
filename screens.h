#include <Arduino.h>
#include <SPI.h>
#include <SignalProcessing.h>
#include <iostream>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#define TFT_CS        15//NULL //2
#define TFT_RST        -1 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC          3

float p = 3.1415926;

class Screens{
    public:
      //atributos publicos:
      Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
      SignalProcessing *punt_voltage;
      SignalProcessing *punt_current;
      SignalProcessing *punt_power;
      int *punt_a;
      int *punt_long_a;
      int *punt_b;
      int *punt_long_b;
      int *punt_samples;
      int *punt_last_sample;
      int screennumbert=0;
      
      //constructores: 
      Screens(SignalProcessing *punt_voltage, SignalProcessing *punt_current, SignalProcessing *punt_power, int* punt_samples, int *punt_last_sample, int *punt_a, int *punt_long_a ,int *punt_b ,int *punt_long_b);
                              
      //metodos:
      void init();

      void refresh();
      
      void screen1();
      void screen2();

      private:
      
};

Screens::Screens(SignalProcessing *punt_vol, SignalProcessing *punt_curr, SignalProcessing *punt_powe, int *punt_mu, int *punt_last, int *punte_a, int *punte_long_a ,int *punte_b ,int *punte_long_b){
      punt_voltage=punt_vol;
      punt_current=punt_curr;
      punt_power=punt_powe;
      punt_a=punte_a;
      punt_long_a=punte_long_a;
      punt_b=punte_b;
      punt_long_b=punte_long_b;
      punt_samples=punt_mu;
      punt_last_sample=punt_last;
}

void Screens::init(){
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(1);
}

void Screens::refresh(){
  if(screennumbert==0){
    screen1();
  }
  if(screennumbert==1){
    screen2();
  }
}

void Screens::screen1(){
  *punt_long_b=2;  
  screennumbert=*punt_b;
   
  tft.fillScreen(ST77XX_BLACK);  
  tft.setTextSize(2);
  tft.setTextWrap(true);
  

  if(*punt_a == 0){
      tft.setTextColor(ST77XX_BLUE);
  }
  else{
    tft.setTextColor(ST77XX_WHITE);
  }   
  tft.setCursor(10, 10);  
  tft.print("V = ");
  tft.print(punt_voltage->average_quadratic_value);
  tft.println(" V");
  tft.println();
  
  if(*punt_a == 1){
      tft.setTextColor(ST77XX_BLUE);
  }
  else{
    tft.setTextColor(ST77XX_WHITE);
  } 
  tft.setCursor(10, 40);
  tft.print("I = ");
  tft.print(punt_current->average_quadratic_value);
  tft.println(" A");
  tft.println();

  if(*punt_a == 2){
      tft.setTextColor(ST77XX_BLUE);
  }
  else{
    tft.setTextColor(ST77XX_WHITE);
  } 
  tft.setCursor(10, 70);
  tft.print("P = ");
  tft.print(punt_power->average_value);
  tft.println(" W");
  tft.println();

  if(*punt_a == 3){
      tft.setTextColor(ST77XX_BLUE);
  }
  else{
    tft.setTextColor(ST77XX_WHITE);
  }
  tft.setCursor(10, 100);
  tft.print("Q = ");
  tft.print((punt_power->average_negative_values_respect_average_value)*p);
  tft.println(" VA");
}

void Screens::screen2(){
  *punt_long_b=2;
  screennumbert=*punt_b;
  int x,y=*punt_last_sample;
  tft.fillScreen(ST77XX_BLACK);  
  for(x=0; x<120; x++){
    if(y >=120 ){
     y=0;
    }
    
    if(*punt_a == 0 || *punt_a == 3){
      tft.drawPixel(x*4, 60-*(punt_samples+4*y+0)/2, ST77XX_BLUE);
    }
    if(*punt_a == 1 || *punt_a == 3){
      tft.drawPixel(x*4, 60-*(punt_samples+4*y+1)/2, ST77XX_RED);  
    }
    if(*punt_a == 2 || *punt_a == 3){
      tft.drawPixel(x*4, 60-*(punt_samples+4*y+2)/100, ST77XX_GREEN);  
    }
    y++;
  }

  tft.setTextSize(1);
  tft.setTextWrap(true);
  tft.setCursor(10, 110);
  if(*punt_a == 0 || *punt_a == 3){
      tft.setTextColor(ST77XX_BLUE);
      tft.print("V  ");
  }
  if(*punt_a == 1 || *punt_a == 3){
      tft.setTextColor(ST77XX_RED); 
      tft.print("I  ");  
  }
  if(*punt_a == 2 || *punt_a == 3){
      tft.setTextColor(ST77XX_GREEN); 
      tft.print("P  ");  
  }
}
