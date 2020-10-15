#include<SoftwareSerial.h>

SoftwareSerial gps(3,2);
byte gpsdata = 0;
void setup()
{
//    Serial.begin(9600); //set the baud rate of serial port to 9600;
    gps.begin(9600);  //set the GPS baud rate to 9600;
}

void loop()
{
    gps.println("hello");
    delay(500); 
}
