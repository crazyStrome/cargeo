#include <SoftwareSerial.h>

//sim module
// D3: Rx, D4: Tx
SoftwareSerial sim(3, 4);

//GPS module
// D5: Rx, D6: Tx
SoftwareSerial ss(5, 6);

// Pin definition
int POWERKEY = 2;

/*********************TCP and UDP**********************/
char APN[] = "CMNET";
char aux_string[60];
char ServerIP[] = "47.92.139.92";
char Port[] = "8080";
char Message[100];
int i = 0, j;

/**************************Other functions**************************/
uint8_t sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout) {
//    Serial.println(ATcommand);

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialize the string
    
//    delay(100);
    
    while( sim.available() > 0) sim.read();    // Clean the input buffer
    
    sim.println(ATcommand);    // Send the AT command 
//    sim.write('\n');


    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(sim.available() != 0){    
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            response[x] = sim.read();      
//            sim.print(response[x]);
            x++;
            // check if the desired answer  is in the response of the module
            if (strstr(response, expected_answer) != NULL)    
            {
                answer = 1;
            }
        }
         // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));
    
//    sim.print("\n"); 
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

void setup() {
    Serial.begin(9600);
    PowerOn(POWERKEY);

    memset(aux_string, '\0', 30);
    sendATcommand("AT+CSOC=1,1,1", "OK", 1000);
    /*********************TCP client in command mode******************/
    snprintf(aux_string, sizeof(aux_string), "AT+CSOCON=0,%s,\"%s\"", Port, ServerIP);
//    delay(50);
    sendATcommand(aux_string, "OK", 2000);     //Setting tcp mode��server ip and port
                                    //  Serial.println("AT+CIPSEND=0,5");                 //Sending "Hello" to server.
//    sendATcommand("AT+CIPSEND=0,", ">", 2000);  
//If not sure the message number,write the command like this: AT+CIPSEND=0, (end with 1A(hex))
    sendTCPMessage("A", "38.88", "121.99");                                
    sendATcommand("AT+CSOCL=0", "OK", 15000);  //close by local
//    sendATcommand("AT+NETCLOSE", "+NETCLOSE: 0", 1000);     //Close network

}
void sendTCPMessage(char *Car, char *Lat, char *Lon) {
    memset(Message, '\0', 100);
    snprintf(Message, sizeof(Message), "AT+CSOSEND=0,0,\"Car:%s,Lat:%s,Lon:%s\"", Car, Lat, Lon);
    Serial.println(Message);
    if (sendATcommand(Message, "OK", 3000) == 1)   //End of sending with 26(HEX:1A)
    {
      printf("Send Message:%s Successfully!\n",Message);
    }
}


void loop() {

}
