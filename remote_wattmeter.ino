#include <ads1256.h>
#include <SignalProcessing.h>
#include "screens.h"
#include "keypad.h"
#include "wifiserver.h"
#include "gps.h"


int cs[2]={3,1};//5, 4
int rdy=16;

float constant1=0.00001, offset1=-50;
float constant2=0.00001, offset2=-28;
float constant3=1, offset3=0;

int samples[120][4];
char data[200];
int *punt_samples=&samples[0][0];
int last_sample=0;
int *punt_last_sample=&last_sample;

Ads1256 adread(2, cs, rdy, 0, 7680000);

SignalProcessing voltage(constant1, offset1);
SignalProcessing current(constant2, offset2);
SignalProcessing power(constant3, offset3);

SignalProcessing *punt_voltage=&voltage;
SignalProcessing *punt_current=&current;
SignalProcessing *punt_power=&power;
 
Screens pantalla = Screens(punt_voltage, punt_current, punt_power, punt_samples, punt_last_sample, punt_a, punt_long_a, punt_b ,punt_long_b);

  
float value[2] = {0,0};
int cp=0;
int cn=1;
float t, t_screen=micros();
char date[25];


void setup() {

  Serial.begin(115200);
  gps_init();
  initserver();
  keypad(5, 4);
  pantalla.init();
  
  adread.ads1256config(3, 0, 0, 0);
  
  adread.init();
}

void loop(void) {

  server.handleClient();
  
  adread.readchannel(value, cp, cn);
  
  time_t hour_time=now();
  t=(float)hour_time*1000000+nowMicros();


  voltage.Processing(value[0], t);
  current.Processing(value[1], t);
  power.Processing((voltage.instant_value)*(current.instant_value), t); 
  
  samples[last_sample][0]=voltage.instant_value;
  samples[last_sample][1]=current.instant_value;
  samples[last_sample][2]=power.instant_value;
  samples[last_sample][3]=t;
     
  last_sample++;
  if(last_sample>=120){
    last_sample=0;
  }
    
  if(micros()-t_screen > 1000000){
    
    adread.finish();  
    
    pantalla.refresh();  
     
    adread.init();
    
    set_time();

    time_now(hour_time, date);
    
    if(shipping_status==false){
      sprintf(data, "%s %010.2f %010.2f %010.2f %010.2f\n", date, voltage.average_quadratic_value, current.average_quadratic_value, power.average_value, power.average_negative_values_respect_average_value*p);
      sprintf(mesage1, "%s%s", mesage1, data);
    }
    else{
      sprintf(mesage1,"_________time ____date ___Voltage ___Current _____Power __Reactive\n");
      sprintf(data, "%s %010.2f %010.2f %010.2f %010.2f\n", date, voltage.average_quadratic_value, current.average_quadratic_value, power.average_value, power.average_negative_values_respect_average_value*p);
      sprintf(mesage1, "%s%s", mesage1, data);
      shipping_status=false;
    }
    
    t_screen=micros();
    
  }
}
