#include <TinyGPS++.h>
//#include <TinyGPS.h>
#include <SoftwareSerial.h>
#include <string.h>
#include <stdlib.h>

TinyGPSPlus gps;


/**
 * for gps module
 */
//TinyGPS gps;
// D5: Rx, D6: Tx
SoftwareSerial ss(5, 6);
/**
 * ddmm.mmmm format
 */
float latitude;
float longitude;
// flag show is encode successed ?
bool flag = false;

/**
 * for sim module
 * 
 */
//sim module
// D3: Rx, D4: Tx
SoftwareSerial sim(3, 4);
// Pin definition
// POWERKEY used to start the module
int POWERKEY = 2;

char aux_string[60];
char ServerIP[] = "47.92.139.92";
char Port[] = "8080";
char Message[100];

void setup()
{
    // Serial for debug
    Serial.begin(9600); //set the baud rate of serial port to 9600;
    Serial.println("by HSP");
    Serial.println();

    /**
     * gps init
     */
    ss.begin(9600); //set the GPS baud rate to 9600;
    ss.listen();

    /**
     * sim init
     */
    PowerOn(POWERKEY);
//    TCPinit();
//    TCPconnect();
}
/**
 * for gps decode
 */
// content contains the message to decode
String content = "";
// use the char input by serial port, to analysis the GPS data
// NEMA-0183 format
void seed(char c) {
    // start of a command
    // like: $GNGGA,082414.600,3853.0930,N,12131.3756,E,1,10,2.0,63.0,M,7.6,M,,0000*48
    if (c == '$') {
        content = "";
        flag = false;
    } else if (c == '\n' || c == '\r') {
        if (content.startsWith("GNGGA")) {
            Serial.println("seed: --> " + content);
            char *p;
            // GNGGA
            p = strtok(content.c_str(), ",");
            int count = 0;
            while(p) {
//                Serial.println(p);
                count ++;
                if (count == 3) {
                    Serial.print("seed: --> latitude: ");
                    Serial.println(p);
                    latitude = atof(p);
                } else if (count == 5) {
                    Serial.print("seed: --> longtitude: ");
                    Serial.println(p);
                    longitude = atof(p);
                }
                p = strtok(NULL, ",");
            }
            flag = true;
        } 
    } else {
        content += c;
    }
}
/**
 * sim functions 
 */
uint8_t sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout) {

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialize the string

    sim.listen();
    while( sim.available() > 0) sim.read();    // Clean the input buffer
    
    sim.println(ATcommand);    // Send the AT command 
    
    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(sim.available() != 0){    
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            response[x] = sim.read(); 
            x++;
            // check if the desired answer  is in the response of the module
            if (strstr(response, expected_answer) != NULL)    
            {
                answer = 1;
            }
        }
         // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));
    
    char *p = response;  
    while (*p == '\r' || *p == '\n') {
        p ++; 
    }
    Serial.print("sendATcommand --> command: \nsendATcommand --> ");
    Serial.println(ATcommand);
    Serial.print("sendATcommand --> expect_answer: \nsendATcommand --> ");
    Serial.println(expected_answer);
    Serial.print("sendATcommand --> response: \nsendATcommand --> ");
    Serial.println(p);
    if (answer > 0) {
        Serial.println("sendATcommand --> response successed");
    } else {
        Serial.println("sendATcommand --> response failed");
    }

    return answer;
}
/**************************Power on Sim7x00**************************/
void PowerOn(int PowerKey){
    uint8_t answer = 0;

    sim.begin(115200);
    sim.listen();

    // checks if the module is started
    
    while (sendATcommand("AT", "OK", 2000) == 0)
    {
        Serial.print("Starting up...\n");
    
        pinMode(PowerKey, OUTPUT);
        // power on pulse
        digitalWrite(PowerKey, HIGH);
        delay(1000);
        digitalWrite(PowerKey, LOW);
        delay(1000);
    }

    sendATcommand("ATE0", "OK", 1000);
//    delay(1000);
}
void TCPsendMessage(char *Car, float Lat, float Lon) {
    uint8_t x=0,  answer=0;
    char response[100];
    char expected_answer[] = "OK";
    unsigned long previous;
    unsigned long timeout = 2000;

    memset(response, '\0', 100);    // Initialize the string

    sim.listen();
    while( sim.available() > 0) sim.read();    // Clean the input buffer
    
//    sim.println(ATcommand);    // Send the AT command 
    sim.print("AT+CSOSEND=0,0,\"Car:");
    sim.print(Car);
    sim.print(",Lat:");
    sim.print(Lat, 4);
    sim.print(",Lon:");
    sim.print(Lon, 4);
    sim.println("\"");
    
    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(sim.available() != 0){    
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            response[x] = sim.read(); 
            x++;
            // check if the desired answer  is in the response of the module
            if (strstr(response, expected_answer) != NULL)    
            {
                answer = 1;
            }
        }
         // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));
    
    char *p = response;  
    while (*p == '\r' || *p == '\n') {
        p ++; 
    }
    Serial.print("TCPsendMessage --> command: \nTCPsendMessage --> ");
//    Serial.println(ATcommand);
    Serial.print("AT+CSOSEND=0,0,\"Car:");
    Serial.print(Car);
    Serial.print(",Lat:");
    Serial.print(Lat, 4);
    Serial.print(",Lon:");
    Serial.print(Lon, 4);
    Serial.println("\"");
    Serial.print("TCPsendMessage --> expect_answer: \nTCPsendMessage --> ");
    Serial.println(expected_answer);
    Serial.print("TCPsendMessage --> response: \nTCPsendMessage --> ");
    Serial.println(p);
    if (answer > 0) {
        Serial.println("TCPsendMessage --> response successed");
    } else {
        Serial.println("TCPsendMessage --> response failed");
    }
}
void TCPinit() {
    sendATcommand("AT+CSOC=1,1,1", "OK", 1000);
}
void TCPconnect() {
    memset(aux_string, '\0', 50);
    snprintf(aux_string, sizeof(aux_string), "AT+CSOCON=0,%s,\"%s\"", Port, ServerIP);
//    delay(50);
    sendATcommand(aux_string, "OK", 2000);  
}
void TCPclose() {
    sendATcommand("AT+CSOCL=0", "OK", 3000); 
}
void loop()
{
    ss.listen();
//    TCPinit();
//    TCPconnect();
    if (ss.available() > 0) {
        char c = ss.read();
        seed(c);
        if (flag) {
            Serial.println("this is a message");
            Serial.println(latitude);
            Serial.println(longitude);
            TCPinit();
            TCPconnect();
            TCPsendMessage("CARA", latitude, longitude);
//TCPsendMessage("CARA", 33.3333, 111.1111);
            TCPclose();
        }
    }
//    TCPclose();
//    delay(300);
}
