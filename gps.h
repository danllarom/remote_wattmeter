#include "TimeLib.h"
#include <TinyGPS.h> 
#include <SoftwareSerial.h>


SoftwareSerial SerialGPS = SoftwareSerial(2, NULL, false, 256);
TinyGPS gps; 

const int offset = 1;

void gps_init(){
  SerialGPS.begin(9600);
}

void set_time(){
  
  while (SerialGPS.available() && timeStatus() == timeNotSet) {
    
    if (gps.encode(SerialGPS.read())) {
      unsigned long age;
      int Year;
      byte Month, Day, Hour, Minute, Second;
      gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
      if (age < 500) {
        setTime(Hour, Minute, Second, Day, Month, Year);
        resetNow();
        adjustTime(offset * SECS_PER_HOUR);
      }
    }
  }
}

void time_now(time_t hour_time, char date[25]){
  sprintf(date, "%02i%02i%02i.%06i %02i%02i%04i",hour(hour_time), minute(hour_time), second(hour_time), nowMicros(), day(hour_time), month(hour_time), year(hour_time)); 
}
