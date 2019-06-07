#include "ads1256.h"
#include "SignalProcessing.h"
#include "gps.h"
#include "wifiserver.h"

#define PI 3.1415926

double value[2] = {0,0};

char data[200];
int lines_mensage=0;
int cp=0;
int cn=1;
int disp=2;
int cs[2]={2, 0};
int rdy[2]={16, 5};
int rst=NULL;
unsigned long spispeed=7680000;

int gain[2]={2,4};
float gainAmplifier=0.125;

float gainDivResVol=3001;
float gainVoltage=gainAmplifier*gainDivResVol;

float gainDivResCur=1/0.01;
float gainCurrent=gainAmplifier*gainDivResCur;

float offsetVoltage=0;
float offsetCurrent=0;
  
float maxFrecuencies=50;

char date[25];
double t, t_before, t_before2, t_gps=micros(), t_data=micros();

time_t hour_time;

Ads1256 adread(disp, cs, rdy, 0, 7680000);

SignalProcessing voltage(gainVoltage, offsetVoltage, maxFrecuencies);
SignalProcessing current(gainCurrent, offsetCurrent, maxFrecuencies);
SignalProcessing power(1, 0, maxFrecuencies);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  adread.ads1256config(0, gain, 0, 0, 0, 1, 0, 1);
  initserver();
}

void loop() {
  // put your main code here, to run repeatedly:
  delayMicroseconds(2);
  server.handleClient();
  delayMicroseconds(2);
  adread.standbyOff();
  adread.voltageReadChannel(value, cp, cn);
  //adread.readchannel(value, cp, cn);
  adread.standbyOn();
  
  hour_time=now();    
  t=(double)hour_time*1000000+nowMicros();
  if(t_before-t_before2<500){
    voltage.Processing(value[0], t_before/1000000);
    current.Processing(value[1], t_before/1000000);
    power.Processing(voltage.instant_value*current.instant_value, t_before/1000000); 
  }
  
  t_before2=t_before; 
  t_before=t; 

  if((micros()-t_data)> 1000000){ 
    t_data=micros();
    time_now(hour_time, date);  
    if(shipping_status==false){
      if(lines_mensage<8){
        sprintf(data, "%s %010.2f %010.2f %010.2f %010.2f\n", date, voltage.average_quadratic_value, current.average_quadratic_value, power.average_value, power.average_negative_values_respect_average_value*PI);
        sprintf(mesage1, "%s%s", mesage1, data);
        lines_mensage++;
      }
    }
    else{
      sprintf(mesage1,"___________time ______date ___Voltage ___Current _____Power __Reactive\n");
      sprintf(data, "%s %010.2f %010.2f %010.2f %010.2f\n", date, voltage.average_quadratic_value, current.average_quadratic_value, power.average_value, power.average_negative_values_respect_average_value*PI);
      sprintf(mesage1, "%s%s", mesage1, data);
      shipping_status=false;
      lines_mensage=0;
    }
    if(micros()-t_gps > 900000000 || timeStatus() == timeNotSet){
      reset_statusset();
      set_time();
    }
    
    Serial.print("voltage = ");
    Serial.println(voltage.average_quadratic_value);
    Serial.print("current = ");
    Serial.println(current.average_quadratic_value);
    Serial.print("voltage_ = ");
    Serial.println(voltage.average_value);
    Serial.print("current_ = ");
    Serial.println(current.average_value);
    Serial.print("power_ = ");
    Serial.println(power.average_value);
    Serial.print("reactive_ = ");
    Serial.println(power.average_negative_values_respect_average_value*PI);    
    Serial.print("voltage loosen signal = ");
    Serial.println(voltage.loosen_signal, 6);
    Serial.print("current loosen signal = ");
    Serial.println(current.loosen_signal, 6);
    Serial.print("voltage frequency = ");
    Serial.println(voltage.frequency, 6);
    Serial.print("current frequency = ");
    Serial.println(current.frequency, 6);
    Serial.print("ROCOF = ");
    Serial.println(voltage.ROCOF, 6);
    
    
  }

  if(micros()-t_data < 0){
    t_data=micros();
  }
  if(micros()-t_gps < 0){
    t_gps=micros();
  }
}
