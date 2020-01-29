#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps ;

SoftwareSerial mySerial(3, 2) ;

void setup()
{
  Serial.begin(115200) ;
  mySerial.begin(9600) ;
  
  Serial.println("running") ;
  
}

void loop()
{
  while (mySerial.available() > 0)
  {
    gps.encode(mySerial.read()) ;
    
  const double tower_lat = 48.85826 ;
  const double tower_lng = 2.294516 ;
 // double courseto = TinyGPSPlus.courseTo(gps.location.lat(), gps.location.lng(), tower_lat, tower_lng) ;
  Serial.print(gps.location.lng()) ;
  Serial.print(" ");
  Serial.println(gps.location.lat());
  }
}
