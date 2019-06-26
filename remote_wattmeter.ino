#include "ads1256.h"
#include "SignalProcessing.h"
#include "gps.h"
#include "wifiserver.h"
#define PI 3.1415926

void sampling();

double value[2] = {0,0};
char data[200];
char device[10]="wat1";
int lines_mensage=0;
int cp=0;
int cn=1;
int disp=2;
int cs[2]={2, 4};
int rdy[2]={16, 5};
int rst=NULL;
unsigned long spispeed=7680000;

int gain[2]={2,2};
float gainAmplifier=0.125;

float gainDivResVol=3001;
float gainVoltage=gainAmplifier*gainDivResVol;

float gainDivResCur=1/0.01;
float gainCurrent=gainAmplifier*gainDivResCur;

float offsetVoltage=-0.9;
float offsetCurrent=0;
  
float maxFrecuencies=60;
double loosen;

char date[25];
double t, t_before, t_before2, t_gps=micros(), t_data=micros(),pre_nowmicros=0;

time_t hour_time;

Ads1256 adread(disp, cs, rdy, 0, 7680000);

SignalProcessing voltage(gainVoltage, offsetVoltage, maxFrecuencies);
SignalProcessing current(gainCurrent, offsetCurrent, maxFrecuencies);
SignalProcessing power(1, 0, maxFrecuencies);

void setup() {
// put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(0, INPUT);
  attachInterrupt(digitalPinToInterrupt(0), handler, FALLING);
  adread.ads1256config(1, gain, 0, 0, 0, 0, 0, 1);
  initserver();
  unsigned int val[2]={0x44AC08, 0x44AC08};
  adread.setOffset(val);
}

void loop() {
  if (gps.encode(SerialGPS.read())==0) {
    sampling();
  }
  delayMicroseconds(2);
  server.handleClient();
  delayMicroseconds(2);
  
  if(pre_nowmicros>890000){
    pre_nowmicros=0;
  }
  //if((nowMicros()-pre_nowmicros)>99000 && (nowMicros()-pre_nowmicros)<101000){
  if((micros()-t_data)> 1000000){
    /*
    if(nowMicros()>990000 || nowMicros()<10000){
      if(nowMicros()>90000){
        loosen=((voltage.time_loosen_signal-((double)nowMicros()-1000000)/1000000)); 
      }
      else{
        loosen=((voltage.time_loosen_signal-((double)nowMicros())/1000000));   
      }
      loosen=loosen/0.02;
      loosen=loosen-(unsigned int)loosen;
      loosen=loosen*360;
      if(loosen<0){
        loosen=360+loosen;
      }
    }
    if(nowMicros()>90000 || nowMicros()<110000){
      loosen=((voltage.time_loosen_signal-((double)nowMicros()-100000)/1000000));
      loosen=loosen/0.02;
      loosen=loosen-(unsigned int)loosen;
      loosen=loosen*360;
      if(loosen<0){
        loosen=360+loosen;
      }
    }
    if(nowMicros()>190000 || nowMicros()<210000){
      loosen=((voltage.time_loosen_signal-((double)nowMicros()-200000)/1000000));
      loosen=loosen/0.02;
      loosen=loosen-(unsigned int)loosen;
      loosen=loosen*360;
      if(loosen<0){
        loosen=360+loosen;
      }
    }
    if(nowMicros()>290000 || nowMicros()<310000){
      loosen=((voltage.time_loosen_signal-((double)nowMicros()-300000)/1000000));
      loosen=loosen/0.02;
      loosen=loosen-(unsigned int)loosen;
      loosen=loosen*360;
      if(loosen<0){
        loosen=360+loosen;
      }
    }
    if(nowMicros()>390000 || nowMicros()<410000){
      loosen=((voltage.time_loosen_signal-((double)nowMicros()-400000)/1000000));
      loosen=loosen/0.02;
      loosen=loosen-(unsigned int)loosen;
      loosen=loosen*360;
      if(loosen<0){
        loosen=360+loosen;
      }
    }
    if(nowMicros()>490000 || nowMicros()<510000){
      loosen=((voltage.time_loosen_signal-((double)nowMicros()-500000)/1000000));
      loosen=loosen/0.02;
      loosen=loosen-(unsigned int)loosen;
      loosen=loosen*360;
      if(loosen<0){
        loosen=360+loosen;
      }
    }
    if(nowMicros()>590000 || nowMicros()<610000){
      loosen=((voltage.time_loosen_signal-((double)nowMicros()-600000)/1000000));
      loosen=loosen/0.02;
      loosen=loosen-(unsigned int)loosen;
      loosen=loosen*360;
      if(loosen<0){
        loosen=360+loosen;
      }
    }
    if(nowMicros()>690000 || nowMicros()<710000){
      loosen=((voltage.time_loosen_signal-((double)nowMicros()-700000)/1000000));
      loosen=loosen/0.02;
      loosen=loosen-(unsigned int)loosen;
      loosen=loosen*360;
      if(loosen<0){
        loosen=360+loosen;
      }
    }
    if(nowMicros()>790000 || nowMicros()<810000){
      loosen=((voltage.time_loosen_signal-((double)nowMicros()-800000)/1000000));
      loosen=loosen/0.02;
      loosen=loosen-(unsigned int)loosen;
      loosen=loosen*360;
      if(loosen<0){
        loosen=360+loosen;
      }
    }
    if(nowMicros()>890000 || nowMicros()<910000){
      loosen=((voltage.time_loosen_signal-((double)nowMicros()-900000)/1000000));
      loosen=loosen/0.02;
      loosen=loosen-(unsigned int)loosen;
      loosen=loosen*360;
      if(loosen<0){
        loosen=360+loosen;
      }
    }
    */
    pre_nowmicros=nowMicros();
    
    t_data=micros();
    time_now(hour_time, date);  
    if(shipping_status==false && lines_mensage<50){
      sprintf(data, "% 10s %s %010.2f %010.2f %010.2f %010.2f\n", device, date, voltage.average_quadratic_value, current.average_quadratic_value, power.average_value, power.average_negative_values_respect_average_value);//loosen);
      sprintf(mesage1, "%s%s", mesage1, data);
      lines_mensage++;
    }
    if(shipping_status==true){
      sprintf(mesage1,"____device ___________time ______date ___Voltage ___Current _____Power __Reactive\n");
      sprintf(data, "% 10s %s %010.2f %010.2f %010.2f %010.2f\n", device, date, voltage.average_quadratic_value, current.average_quadratic_value, power.average_value, power.average_negative_values_respect_average_value);//loosen);
      sprintf(mesage1, "%s%s", mesage1, data);
      shipping_status=false;
      lines_mensage=0;
    }
    if(micros()-t_gps > 900000000 || timeStatus() == timeNotSet){
      reset_statusset();
      set_time();
    } 
    //Serial.println(voltage.average_value, 10);  
    //Serial.println(current.average_value, 10);
    //Serial.println(current.sampling_frequency, 10);
    //Serial.println((voltage.average_quadratic_value*current.average_quadratic_value)/sqrt(pow(power.average_value, 2)+pow(power.average_quadratic_value-power.average_value-power.average_negative_values_respect_average_value, 2)+pow(power.average_negative_values_respect_average_value, 2)));
    //Serial.println((((voltage.positive_average_value*(1-voltage.sign_factor)-voltage.negative_average_value*voltage.sign_factor))*((current.positive_average_value*(1-current.sign_factor)-current.negative_average_value*current.sign_factor)))/((2*power.average_negative_values_respect_average_value*power.sign_factor)/3.1415926+power.average_value*(1-2*power.sign_factor)));
    pre_nowmicros=nowMicros();  
    
    if(reset_status==true){
      ESP.restart();
      //ESP.reset();
    } 
  }
  if(micros()-t_data < 0){
    t_data=micros();
  }
  if(micros()-t_gps < 0){
    t_gps=micros();
  }
  Serial.print(power.instant_value);
  Serial.print(",");
  Serial.print(current.instant_value*100);
  Serial.print(",");
  Serial.println(voltage.instant_value);
  //Serial.print(current.average_value*100);
  //Serial.print(",");
  //Serial.println(voltage.average_value*10);
}

void handler(){
  initPA();
}

void sampling() {
  //adread.standbyOff();
  adread.voltageReadChannel(value, cp, cn);
  //adread.standbyOn();
  
  hour_time=now();    
  t=hour_time*1000000+nowMicros();
  if(t_before-t_before2<2000){
    voltage.Processing(value[0], (double)t_before/1000000);
    current.Processing(value[1], (double)t_before/1000000);
    power.Processing((voltage.instant_value)*(current.instant_value), (double)t_before/1000000); 
  }
  else{
    voltage.time_now=(double)t_before/1000000;
    current.time_now=(double)t_before/1000000;
    power.time_now=(double)t_before/1000000;
  }
  t_before2=t_before; 
  t_before=t; 
}
