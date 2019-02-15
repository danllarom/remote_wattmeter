#include <ads1256.h>
#include <SignalProcessing.h>
#include "screens.h"
#include "keypad.h"
#include <SPI.h>

int cs[2]={3,1};//5, 4
int rdy=16;

float constant1=0.00001, offset1=-50;
float constant2=0.00001, offset2=-28;
float constant3=1, offset3=0;

Ads1256 adread(2, cs, rdy, 0, 7680000);

SignalProcessing voltage(constant1, offset1);
SignalProcessing current(constant2, offset2);
SignalProcessing power(constant3, offset3);

SignalProcessing *punt_voltage=&voltage;
SignalProcessing *punt_current=&current;
SignalProcessing *punt_power=&power;

int muestras[120][4];
int *punt_muestras=&muestras[0][0];
  
Screens pantalla = Screens(punt_voltage, punt_current, punt_power, punt_muestras, punt_a, punt_long_a, punt_b ,punt_long_b);
  
void setup() {

  
  Serial.begin(115200);
  
  keypad(5, 4);
  pantalla.init();
  adread.ads1256config(3, 0, 0, 0);
}

void loop(void) {
  int i;
  float value[2] = {0,0};
  int cp=0;
  int cn=1;
  float t, t1, t2;   

    
  t1=millis();
  adread.init();
  for(i=0 ; i<1000 ; i++){  
    adread.readchannel(value, cp, cn);
    t=micros(); 
    voltage.Processing(value[0], t);
    current.Processing(value[1], t);
    power.Processing((voltage.instant_value)*(current.instant_value), t);
    if(i<120){   
      muestras[i][0]=voltage.instant_value;
      muestras[i][1]=current.instant_value;
      muestras[i][2]=power.instant_value;
      muestras[i][3]=t;
    }
  }
  adread.finish();
  
  //pantalla.screen1();
  //pantalla.screen2();
  pantalla.refresh();
  
  while((millis()-t1) < 1000){
  }
  
}
